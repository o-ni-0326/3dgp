#include "framework.h"
#include "geometric_primitive.h"
#include "ResourceManager.h"


geometric_primitive::geometric_primitive(ID3D11Device* Device) {

	HRESULT hr;

	BYTE*data = NULL;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },

	};
	UINT numElements = ARRAYSIZE(layout);


	ResourceManager::LoadVertexShader(Device, "geometric_primitive_vs.cso", layout, numElements, &Vertex, &Layout);

	ResourceManager::LoadPixelShaders(Device, "geometric_primitive_ps.cso", &PixelShader);


	D3D11_RASTERIZER_DESC rsDesc;
	//wire
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	hr = Device->CreateRasterizerState(&rsDesc, &WRasState);
	if (FAILED(hr))
		return;

	//fill
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
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

	//vertex vertices[24] = {};		//position8通り×normal3通り
	//u_int indices[36] = {};
	//int face = 0;

	//// top-side
	//// 0-------1
	//// |	   |
	//// |	   |
	//// |	   |
	//// 2-------3

	//vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	//vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	//vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	//vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);

	//vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	//vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	//vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	//vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);

	//indices[face * 6 + 0] = face * 4 + 0;
	//indices[face * 6 + 1] = face * 4 + 1;
	//indices[face * 6 + 2] = face * 4 + 2;
	//indices[face * 6 + 3] = face * 4 + 1;
	//indices[face * 6 + 4] = face * 4 + 3;
	//indices[face * 6 + 5] = face * 4 + 2;			//時計回りが表向き

	//// bottom-side
	//// 0-------1
	//// |	   |
	//// |	   |
	//// |	   |
	//// 2-------3

	//face++;
	//vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	//vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	//vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	//vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);

	//vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	//vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	//vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	//vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);

	//indices[face * 6 + 0] = face * 4 + 0;
	//indices[face * 6 + 1] = face * 4 + 2;
	//indices[face * 6 + 2] = face * 4 + 1;
	//indices[face * 6 + 3] = face * 4 + 1;
	//indices[face * 6 + 4] = face * 4 + 2;
	//indices[face * 6 + 5] = face * 4 + 3;

	//// front-side
	//// 0-------1
	//// |	   |
	//// |	   |
	//// |	   |
	//// 2-------3

	//face++;
	//vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	//vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	//vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	//vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);

	//vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	//vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	//vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	//vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);

	//indices[face * 6 + 0] = face * 4 + 0;
	//indices[face * 6 + 1] = face * 4 + 1;
	//indices[face * 6 + 2] = face * 4 + 2;
	//indices[face * 6 + 3] = face * 4 + 1;
	//indices[face * 6 + 4] = face * 4 + 3;
	//indices[face * 6 + 5] = face * 4 + 2;

	//// back-side
	//// 0-------1
	//// |	   |
	//// |	   |
	//// |	   |
	//// 2-------3

	//face++;
	//vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	//vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	//vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	//vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);

	//vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);
	//vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);
	//vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);
	//vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);

	//indices[face * 6 + 0] = face * 4 + 0;
	//indices[face * 6 + 1] = face * 4 + 2;
	//indices[face * 6 + 2] = face * 4 + 1;
	//indices[face * 6 + 3] = face * 4 + 1;
	//indices[face * 6 + 4] = face * 4 + 2;
	//indices[face * 6 + 5] = face * 4 + 3;

	//// right-side
	//// 0-------1
	//// |	   |
	//// |	   |
	//// |	   |
	//// 2-------3

	//face++;
	//vertices[face * 4 + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	//vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	//vertices[face * 4 + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	//vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);

	//vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);
	//vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);
	//vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);
	//vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+!.0f, 0.0f, 0.0f);

	//indices[face * 6 + 0] = face * 4 + 0;
	//indices[face * 6 + 1] = face * 4 + 1;
	//indices[face * 6 + 2] = face * 4 + 2;
	//indices[face * 6 + 3] = face * 4 + 1;
	//indices[face * 6 + 4] = face * 4 + 3;
	//indices[face * 6 + 5] = face * 4 + 2;

	//// left-side
	//// 0-------1
	//// |	   |
	//// |	   |
	//// |	   |
	//// 2-------3

	//face++;
	//vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	//vertices[face * 4 + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	//vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	//vertices[face * 4 + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);

	//vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);
	//vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);
	//vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);
	//vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);

	//indices[face * 6 + 0] = face * 4 + 0;
	//indices[face * 6 + 1] = face * 4 + 2;
	//indices[face * 6 + 2] = face * 4 + 1;
	//indices[face * 6 + 3] = face * 4 + 1;
	//indices[face * 6 + 4] = face * 4 + 2;
	//indices[face * 6 + 5] = face * 4 + 3;

}


void geometric_primitive::CreateBuffer(ID3D11Device* Device, vertex* vertices, u_int* indices, int numIndex, int numVertex) {

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
	hr = Device->CreateBuffer(&bd, &InitData, &VBuffer);
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
	hr = Device->CreateBuffer(&bd, &InitData, &IBuffer);
	if (FAILED(hr))
		return;

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

void geometric_primitive::render(ID3D11DeviceContext * Context,
							const DirectX::XMFLOAT4X4 & world_view,
							const DirectX::XMFLOAT4X4 & worldM,
							const DirectX::XMFLOAT4 & light,
							const DirectX::XMFLOAT4 & Material_color ,bool bWireframe) {
	

	cbuffer cb;
	cb.world_view_projection = world_view;
	cb.world = worldM;
	cb.material_color = Material_color;
	cb.light_direction = light;
	Context->UpdateSubresource(CBuffer, 0, NULL, &cb, 0, 0);
	Context->VSSetConstantBuffers(0, 1, &CBuffer);


	// Set vertex buffer
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	Context->IASetVertexBuffers(0, 1, &VBuffer, &stride, &offset);

	// Set index buffer
	Context->IASetIndexBuffer(IBuffer, DXGI_FORMAT_R32_UINT, 0);

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

	//depth
	Context->OMSetDepthStencilState(DepthState, 0);

	//index付き描画
	D3D11_BUFFER_DESC buffer_desc;
	IBuffer->GetDesc(&buffer_desc);
	Context->DrawIndexed(buffer_desc.ByteWidth / sizeof(u_int), 0, 0);

	//Context->DrawIndexed(numindices, 0, 0);


}


geometric_primitive::~geometric_primitive() 
{
	if (DepthState)DepthState->Release();
	if (FRasState)FRasState->Release();
	if (WRasState)WRasState->Release();
	if (CBuffer)CBuffer->Release();
	if (IBuffer)IBuffer->Release();
	if (VBuffer)VBuffer->Release();
	ResourceManager::ReleasePixelShaders(PixelShader);
	ResourceManager::ReleaseVertexShaders(Vertex, Layout);
}

geometric_cube::geometric_cube(ID3D11Device* Device):geometric_primitive(Device) {

	vertex vertices[24] = {};		//position8通り×normal3通り
	u_int indices[36] = {};
	int face = 0;

	// top-side
	// 0-------1
	// |	   |
	// |	   |
	// |	   |
	// 2-------3

	vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);

	vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);

	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	//時計回りが表向き												
	// bottom-side
													
	// 0-------1												
	// |	   |												
	// |	   |												
	// |	   |												
	// 2-------3

	face++;
	vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);

	vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);

	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	// front-side
	// 0-------1
	// |	   |
	// |	   |
	// |	   |
	// 2-------3

	face++;
	vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);

	vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);

	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// back-side
	// 0-------1
	// |	   |
	// |	   |
	// |	   |
	// 2-------3

	face++;
	vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);

	vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);
	vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);
	vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);
	vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);

	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	// right-side
	// 0-------1
	// |	   |
	// |	   |
	// |	   |
	// 2-------3

	face++;
	vertices[face * 4 + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);

	vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);
	vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);
	vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);
	vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+!.0f, 0.0f, 0.0f);

	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// left-side
	// 0-------1
	// |	   |
	// |	   |
	// |	   |
	// 2-------3

	face++;
	vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);

	vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);
	vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);
	vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);
	vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, +0.0f);

	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	CreateBuffer(Device, vertices, indices, 36, 24);
}

geometric_cylinder::geometric_cylinder(ID3D11Device* Device, int num):geometric_primitive(Device),NUM(num) {
	u_int indices[24*3];
	vertex vertices[24+2];
	int ofsV = 0, ofsI = 0;

	//上面
	vertices[ofsV + 0].position = DirectX::XMFLOAT3(0, 0.5f, 0);		//中心点

	const float DANGLE = DirectX::XMConvertToRadians(360.0f / 6);
	for (int n = 0; n < 6; n++) {
		vertices[ofsV + 1 + n].position = DirectX::XMFLOAT3(cos(DANGLE * n), 0.5f, sin(DANGLE * n));
	}

	for (int n = 0; n < 7; n++) {
		vertices[ofsV + n].normal = DirectX::XMFLOAT3(0, 1.0f, 0);
	}

	for (int n = 0; n < 5; n++) {
		indices[ofsI + 0 + 3 * n] = ofsV + 0;
		indices[ofsI + 1 + 3 * n] = ofsV + n + 1;
		indices[ofsI + 2 + 3 * n] = ofsV + n + 2;
	}
	indices[ofsI+0+3*5] = ofsV+0; indices[ofsI+1+3*5] = ofsV+6; indices[ofsI+2+3*5] = ofsV+1;

	ofsV += 7;
	ofsI += 18;

	//下面
	vertices[ofsV + 0].position = DirectX::XMFLOAT3(0, -0.5f, 0);		//中心点

	for (int n = 0; n < 6; n++) {
		vertices[ofsV + 1 + n].position = DirectX::XMFLOAT3(sin(DANGLE * n), -0.5f, cos(DANGLE * n));
	}

	for (int n = 0; n < 7; n++) {
		vertices[ofsV + n].normal = DirectX::XMFLOAT3(0, -1.0f, 0);
	}

	for (int n = 0; n < 5; n++) {
		indices[ofsI + 0 + 3 * n] = ofsV + 0;
		indices[ofsI + 1 + 3 * n] = ofsV + n + 2;
		indices[ofsI + 2 + 3 * n] = ofsV + n + 1;
	}
	indices[ofsI + 0 + 3 * 5] = ofsV + 0; indices[ofsI + 1 + 3 * 5] = ofsV + 1; indices[ofsI + 2 + 3 * 5] = ofsV + 6;

	ofsV += 7;
	ofsI += 18;

	//側面
	vertices[ofsV + 0].position = vertices[0 + 1].position;
	vertices[ofsV + 1].position = vertices[0 + 2].position;
	vertices[ofsV + 2].position = vertices[7 + 1].position;
	vertices[ofsV + 3].position = vertices[7 + 2].position;

	vertices[ofsV + 4].position = vertices[0 + 2].position;
	vertices[ofsV + 5].position = vertices[0 + 3].position;
	vertices[ofsV + 6].position = vertices[7 + 2].position;
	vertices[ofsV + 7].position = vertices[7 + 3].position;


	for (int n = 0; n < 8; n++) {
		vertices[ofsV + n].normal = DirectX::XMFLOAT3(vertices[ofsV + n].position.x, 0.0f, vertices[ofsV + n].position.z);
	}

	indices[ofsI + 0] = ofsV + 0; indices[ofsI + 1] = ofsV + 2; indices[ofsI + 2] = ofsV+1;
	indices[ofsI + 3] = ofsV + 1; indices[ofsI + 4] = ofsV + 2; indices[ofsI + 5] = ofsV + 3;

	indices[ofsI + 0 + 6] = ofsV +4+ 0; indices[ofsI + 1+6] = ofsV +4+ 2; indices[ofsI + 2+6] = ofsV +4+ 1;
	indices[ofsI + 3+6] = ofsV +4+ 1; indices[ofsI + 4+6] = ofsV +4+ 2; indices[ofsI + 5+6] = ofsV +4+ 3;

	ofsV += 4; ofsI += 3 * 2;


	CreateBuffer(Device, vertices, indices, ofsI, ofsV);

}