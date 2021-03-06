﻿#include "skinned_mesh.h"
#include <fbxsdk.h>
using namespace fbxsdk;
#include <vector>
#include <functional>
#include "misc.h"
#include "ResourceManager.h"


//ボーン行列をFBXデータから取得する
void fetch_bone_matrices(FbxMesh *fbx_mesh, std::vector<skinned_mesh::bone> &skeletal, FbxTime time)
{
	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin *skin = static_cast<FbxSkin *>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
		const int number_of_clusters = skin->GetClusterCount();
		skeletal.resize(number_of_clusters);

		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			skinned_mesh::bone &bone = skeletal.at(index_of_cluster);
			FbxCluster *cluster = skin->GetCluster(index_of_cluster);

			// this matrix trnasforms coordinates of the initial pose from mesh space to global space
			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			// this matrix trnasforms coordinates of the initial pose from bone space to global space
			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			// this matrix trnasforms coordinates of the current pose from bone space to global space
			FbxAMatrix cluster_global_current_position;
			cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(time);

			// this matrix trnasforms coordinates of the current pose from mesh space to global space
			FbxAMatrix reference_global_current_position;
			reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(time);

			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
			// (translation, rotation and scale), the last row of the matrix represents the translation part of the
			// transformation.
			FbxAMatrix transform = reference_global_current_position.Inverse() * cluster_global_current_position
				* cluster_global_init_position.Inverse() * reference_global_init_position;

			// convert FbxAMatrix(transform) to XMDLOAT4X4(bone.transform)
			for (int row = 0, column = 0; row < 4; ++row) {
				for (column = 0; column < 4; ++column) {
					bone.transform(row, column) = static_cast<float>(transform[row][column]);
				}
			}
		}
	}
}

//ボーン影響度をFBXデータから取得する
void fetch_bone_influences(const FbxMesh *fbx_mesh, std::vector<skinned_mesh::bone_influences_per_control_point> &influences) {

	const int number_of_control_points = fbx_mesh->GetControlPointsCount();
	influences.resize(number_of_control_points);

	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer) {
		FbxSkin *skin = static_cast<FbxSkin *>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

		const int number_of_clusters = skin->GetClusterCount();
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster) {
			FbxCluster* cluster = skin->GetCluster(index_of_cluster);

			const int number_of_control_point_indices = cluster->GetControlPointIndicesCount();
			const int *array_of_control_point_indices = cluster->GetControlPointIndices();
			const double *array_of_control_point_weights = cluster->GetControlPointWeights();

			for (int i = 0; i < number_of_control_point_indices; ++i) {
				skinned_mesh::bone_influences_per_control_point &influences_per_control_point = influences.at(array_of_control_point_indices[i]);
				skinned_mesh::bone_influence influence;
				influence.index = index_of_cluster;
				influence.weight = static_cast<float>(array_of_control_point_weights[i]);
				influences_per_control_point.push_back(influence);
			}
		}
	}
}
void fetch_animations(FbxMesh *fbx_mesh, skinned_mesh::skeletal_animation &skeletal_animation,
	u_int sampling_rate = 0)
{
	// Get the list of all the animation stack.
	FbxArray<FbxString *> array_of_animation_stack_names;
	fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);
	// Get the number of animations.
	int number_of_animations = array_of_animation_stack_names.Size();
	if (number_of_animations > 0)
	{
		// Get the FbxTime per animation's frame.
		FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		FbxTime frame_time;
		frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
		sampling_rate = sampling_rate > 0 ? sampling_rate : frame_time.GetFrameRate(time_mode);
		float sampling_time = 1.0f / sampling_rate;
		skeletal_animation.sampling_time = sampling_time;
		skeletal_animation.animation_tick = 0.0f;
		FbxString *animation_stack_name = array_of_animation_stack_names.GetAt(0);
		FbxAnimStack * current_animation_stack
			= fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
		fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);
		FbxTakeInfo *take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
		FbxTime start_time = take_info->mLocalTimeSpan.GetStart();
		FbxTime end_time = take_info->mLocalTimeSpan.GetStop();
		FbxTime smapling_step;
		smapling_step.SetTime(0, 0, 1, 0, 0, time_mode);
		smapling_step = static_cast<FbxLongLong>(smapling_step.Get() * sampling_time);
		for (FbxTime current_time = start_time; current_time < end_time; current_time += smapling_step)
		{
			skinned_mesh::skeletal skeletal;
			fetch_bone_matrices(fbx_mesh, skeletal, current_time);
			skeletal_animation.push_back(skeletal);
		}
	}
	for (int i = 0; i < number_of_animations; i++)
	{
		delete array_of_animation_stack_names[i];
	}
}

HRESULT	MakeDummyShaderResourceView(ID3D11Device *Device, ID3D11ShaderResourceView** shaderResourceView)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2d_desc = {};
	texture2d_desc.Width = 1;
	texture2d_desc.Height = 1;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresource_data = {};
	u_int color = 0xFFFFFFFF;
	subresource_data.pSysMem = &color;
	subresource_data.SysMemPitch = 4;
	subresource_data.SysMemSlicePitch = 4;

	ID3D11Texture2D *texture2d;
	hr = Device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
	if (hr)		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	shader_resource_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;

	hr = Device->CreateShaderResourceView(texture2d, &shader_resource_view_desc, shaderResourceView);
	if (hr)		return hr;

	texture2d->Release();

	return hr;
}

skinned_mesh::~skinned_mesh() {

	for (mesh &mesh : meshes) {
		if (mesh.vertex_buffer)mesh.vertex_buffer->Release();
		if (mesh.index_buffer)mesh.index_buffer->Release();
		/*for (subset &subset : mesh.subsets) {
			if (subset.diffuse.shader_resource_view) subset.diffuse.shader_resource_view->Release();
		}*/
	}

	if (CBuffer)CBuffer->Release();
	if (SamplerDesc)SamplerDesc->Release();
	if (DepthState)DepthState->Release();
	if (FRasState)FRasState->Release();
	if (WRasState)WRasState->Release();

	ResourceManager::ReleasePixelShaders(PixelShader);
	ResourceManager::ReleaseVertexShaders(Vertex, Layout);
}

skinned_mesh::skinned_mesh(ID3D11Device *Device, const char *fbx_filename)
{

	//Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	//Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	//Create an importer
	FbxImporter* importer = FbxImporter::Create(manager, "");

	//Initialize the importer
	bool import_status = false;
	import_status = importer->Initialize(fbx_filename, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	//Create a new scene so it can be populated by the imported file
	FbxScene* scene = FbxScene::Create(manager, "");

	//Import the contents of the file into the scene
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	//Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*teplace*/true);

	//Fetch node attributes and materials under this node recursively. Currently only mesh
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*) > traverse = [&](FbxNode* node) {
		if (node) {
			FbxNodeAttribute *fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute) {
				switch (fbx_node_attribute->GetAttributeType()) {
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
				traverse(node->GetChild(i));
		}
	};
	traverse(scene->GetRootNode());



	//Fetch mesh data
	std::vector<vertex> vertices;	//Vertex buffer
	std::vector<u_int> indices;		//Index buffer
	u_int vertex_count = 0;

	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();	//Currently only one mesh
	meshes.resize(fetched_meshes.size());
	for (size_t i = 0; i < fetched_meshes.size(); i++)
	{
		FbxMesh *fbx_mesh = fetched_meshes.at(i)->GetMesh();
		mesh &mesh = meshes.at(i);

		//Fetch bone influences
		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);

		//Fetch Bone transform
		//FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		//FbxTime frame_time;
		//frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
		//fetch_bone_matrices(fbx_mesh, mesh.skeletal, frame_time * 20); // pose at frame 20

		//Fetch Animations
		fetch_animations(fbx_mesh, mesh.skeletal_animation);

		//Fetch material properties
		const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
		mesh.subsets.resize(number_of_materials);

		if (number_of_materials == 0) {
			mesh.subsets.resize(1);
			MakeDummyShaderResourceView(Device, &mesh.subsets.at(0).diffuse.shader_resource_view);
		}

		for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
		{
			bool exist_tex = false;
			subset &subset = mesh.subsets.at(index_of_material);
			const FbxSurfaceMaterial *surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);

			const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);

			if (property.IsValid() && factor.IsValid())
			{
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();
				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
				if (number_of_textures)
				{
					const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
					if (file_texture)
					{
						const char *filename = file_texture->GetRelativeFileName();
						//Create "diffuse.shader_resource_view" from "filename".
						wchar_t wfilename[256];
						size_t stringSize = 0;
						mbstowcs_s(&stringSize, wfilename, filename, strlen(filename));
						ResourceManager::LoadShaderResourceView(Device, wfilename, &subset.diffuse.shader_resource_view, &Tex2d_desc);
						exist_tex = true;
					}
				}
			}
			if (!exist_tex)
				MakeDummyShaderResourceView(Device, &subset.diffuse.shader_resource_view);

		}

		//Count the Polygon count of each material
		if (number_of_materials > 0)
		{
			//Count the faces of each material
			const int number_of_polygons = fbx_mesh->GetPolygonCount();
			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
			{
				const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
				mesh.subsets.at(material_index).index_count += 3;
			}
			//Record the offset(how many vertex)
			int offset = 0;
			for (subset &subset : mesh.subsets)
			{
				subset.index_start = offset;
				offset += subset.index_count;
				//This will be used as counter in the following procedures,reset to zero
				subset.index_count = 0;
			}
		}
		else {
			int offset = 0;
			for (subset &subset : mesh.subsets)
			{
				subset.index_start = offset;
				offset += subset.index_count;
				//This will be used as counter in the following procedures, reset to zero
				subset.index_count = 0;

			}
		}



		const FbxVector4 *array_of_control_points = fbx_mesh->GetControlPoints();
		const int number_of_polygons = fbx_mesh->GetPolygonCount();
		indices.resize(number_of_polygons * 3); //UNIT.18
		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);

		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++) {
			//UNIT.18
			//The material for current face
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}
			//Where should I save the vertex attribute index, accroding to the material
			subset &subset = mesh.subsets.at(index_of_material);
			const int index_offset = subset.index_start + subset.index_count;

			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {
				vertex vertex;
				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

				if (fbx_mesh->GetElementNormalCount() > 0) {
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
					//MakeDummyShaderResourceView(Device, &subset.diffuse.shader_resource_view);
				}
				if (fbx_mesh->GetElementUVCount() > 0) {
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
					//MakeDummyShaderResourceView(Device, &subset.diffuse.shader_resource_view);
				}
				
				for (int i = 0,max = bone_influences[index_of_control_point].size(); i < max && i<MAX_BONE_INFLUENCES; ++i) {
					vertex.bone_weights[i] = bone_influences[index_of_control_point][i].weight;
					vertex.bone_indices[i] = bone_influences[index_of_control_point][i].index;
				}

				vertices.push_back(vertex);

				//UNIT.18
				//indices.push_back(vertex_count);
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;
			}
			subset.index_count += 3;	//UNIT.18
		}

		FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
		for (int row = 0; row < 4; row++)
		{
			for (int column = 0; column < 4; column++)
			{
				mesh.global_transform(row, column) = static_cast<float>(global_transform[row][column]);
			}
		}
		mesh.CreateBuffer(Device, vertices.data(), indices.data(), indices.size(), vertices.size());
	}

	manager->Destroy();

	indices.clear();
	vertices.clear();

	HRESULT hr;

	BYTE*data = NULL;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,		0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",	 0,DXGI_FORMAT_R32G32B32A32_FLOAT,	0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",	 0,DXGI_FORMAT_R32G32B32_FLOAT,		0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,		0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "WEIGHTS", 0,DXGI_FORMAT_R32G32B32A32_FLOAT,	0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "BONES",	 0,DXGI_FORMAT_R32G32B32A32_SINT,	0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	UINT numElements = ARRAYSIZE(layout);


	ResourceManager::LoadVertexShader(Device, "geometric_primitive_vs.cso", layout, numElements, &Vertex, &Layout);

	ResourceManager::LoadPixelShaders(Device, "geometric_primitive_ps.cso", &PixelShader);



	D3D11_RASTERIZER_DESC rsDesc;
	//wire
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	hr = Device->CreateRasterizerState(&rsDesc, &WRasState);
	if (FAILED(hr))
		return;

	//fill
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	hr = Device->CreateRasterizerState(&rsDesc, &FRasState);
	if (FAILED(hr))
		return;

	//Create the depth stencil state
	D3D11_DEPTH_STENCIL_DESC  depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = FALSE;
	depthDesc.StencilReadMask = 0xff;
	depthDesc.StencilWriteMask = 0xff;
	hr = Device->CreateDepthStencilState(&depthDesc, &DepthState);
	if (FAILED(hr))return;

	//サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Device->CreateSamplerState(&samplerDesc, &SamplerDesc);
	if (FAILED(hr))return;

	D3D11_BUFFER_DESC bd;
	//コンスタント
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(cbuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	hr = Device->CreateBuffer(&bd, nullptr, &CBuffer);
	if (FAILED(hr))
		return;

}

void skinned_mesh::mesh::CreateBuffer(ID3D11Device* Device, vertex* vertices, u_int* indices, int numIndex, int numVertex) {

	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;

	//頂点
	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&InitData, sizeof(InitData));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	//bd.ByteWidth = sizeof(vertices);
	bd.ByteWidth = numVertex * sizeof(vertex);//送りたい情報を換える
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;  //コンスタントバッファーに換える
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	InitData.pSysMem = vertices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	hr = Device->CreateBuffer(&bd, &InitData, &vertex_buffer);
	if (FAILED(hr))
		return;

	//インデックス
	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&InitData, sizeof(InitData));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	//bd.ByteWidth = sizeof(indices);
	bd.ByteWidth = numIndex * sizeof(u_int);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	InitData.pSysMem = indices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	UINT numindices = numIndex;
	hr = Device->CreateBuffer(&bd, &InitData, &index_buffer);
	if (FAILED(hr))
		return;

	
}

void skinned_mesh::render(ID3D11DeviceContext * Context,
	const DirectX::XMFLOAT4X4 & world_view,
	const DirectX::XMFLOAT4X4 & worldM,
	const DirectX::XMFLOAT4 & light,
	const DirectX::XMFLOAT4 & Material_color, 
	bool bWireframe,
	float elapsed_time) {


	cbuffer cb;
	//cb.world_view_projection = world_view;
	//cb.world = worldM;

	cb.light_direction = light;
	//Context->UpdateSubresource(CBuffer, 0, NULL, &cb, 0, 0);
	//Context->VSSetConstantBuffers(0, 1, &CBuffer);


	for (mesh &mesh : meshes)
	{
		//頂点バッファ||インデックスバッファが無ければ終了
		if (!mesh.vertex_buffer || !mesh.index_buffer)return;

		if (mesh.skeletal_animation.size() > 0)
		{
			int frame = mesh.skeletal_animation.animation_tick / mesh.skeletal_animation.sampling_time;
			if (frame > mesh.skeletal_animation.size() - 1)
			{
				frame = 0;
				mesh.skeletal_animation.animation_tick = 0;
			}
			std::vector<bone> &skeletal = mesh.skeletal_animation.at(frame);
			size_t number_of_bones = skeletal.size();
			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
			for (size_t i = 0; i < number_of_bones; i++)
			{
				XMStoreFloat4x4(&cb.bone_transforms[i], XMLoadFloat4x4(&skeletal.at(i).transform));
			}
			mesh.skeletal_animation.animation_tick += elapsed_time;
		}
		else {
			DirectX::XMStoreFloat4x4(&cb.bone_transforms[0], DirectX::XMMatrixIdentity());
			DirectX::XMStoreFloat4x4(&cb.bone_transforms[1], DirectX::XMMatrixIdentity());
			DirectX::XMStoreFloat4x4(&cb.bone_transforms[2], DirectX::XMMatrixIdentity());
		}


		for (subset &subset : mesh.subsets) {

			DirectX::XMStoreFloat4x4(&cb.world_view_projection,
				DirectX::XMLoadFloat4x4(&mesh.global_transform) *
				DirectX::XMLoadFloat4x4(&coordinate_conversion) *
				DirectX::XMLoadFloat4x4(&world_view));
			DirectX::XMStoreFloat4x4(&cb.world,
				DirectX::XMLoadFloat4x4(&mesh.global_transform) *
				DirectX::XMLoadFloat4x4(&coordinate_conversion) *
				DirectX::XMLoadFloat4x4(&worldM));

			cb.material_color.x = subset.diffuse.color.x * Material_color.x;
			cb.material_color.y = subset.diffuse.color.y * Material_color.y;
			cb.material_color.z = subset.diffuse.color.z * Material_color.z;
			cb.material_color.w = Material_color.w;

			

			// Set vertex buffer
			UINT stride = sizeof(vertex);
			UINT offset = 0;

			// Set vertex buffer
			Context->IASetVertexBuffers(0, 1, &mesh.vertex_buffer, &stride, &offset);

			// Set index buffer
			Context->IASetIndexBuffer(mesh.index_buffer, DXGI_FORMAT_R32_UINT, 0);

			// Set primitive topology
			Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Set the input layout
			Context->IASetInputLayout(Layout);

			//state
			if (bWireframe) Context->RSSetState(WRasState);
			else Context->RSSetState(FRasState);

			// Set Shaders
			Context->VSSetShader(Vertex, NULL, 0);
			Context->PSSetShader(PixelShader, NULL, 0);

			Context->UpdateSubresource(CBuffer, 0, NULL, &cb, 0, 0);
			Context->VSSetConstantBuffers(0, 1, &CBuffer);

			Context->PSSetSamplers(0, 1, &SamplerDesc);

			//depth
			Context->OMSetDepthStencilState(DepthState, 0);

			//index付き描画
			D3D11_BUFFER_DESC buffer_desc;
			mesh.index_buffer->GetDesc(&buffer_desc);

			Context->PSSetShaderResources(0, 1, &subset.diffuse.shader_resource_view);
			Context->DrawIndexed(subset.index_start + subset.index_count, 0, 0);

		}
	}
}
	