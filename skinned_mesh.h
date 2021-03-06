#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>


class skinned_mesh {

protected:
	ID3D11VertexShader*     Vertex;
	ID3D11PixelShader*      PixelShader;
	ID3D11InputLayout*      Layout;
	ID3D11Buffer*			CBuffer;
	ID3D11RasterizerState*  WRasState;
	ID3D11RasterizerState*  FRasState;
	ID3D11DepthStencilState*    DepthState;
	ID3D11SamplerState* SamplerDesc;
	D3D11_TEXTURE2D_DESC     Tex2d_desc;

	//Y軸とZ軸を入れ替えて座標軸変換
	DirectX::XMFLOAT4X4 coordinate_conversion = {
		1,0,0,0,
		0,0,1,0,
		0,1,0,0,
		0,0,0,1
	};

public:

	skinned_mesh(ID3D11Device *Device, const char *fbx_filename);
	virtual ~skinned_mesh();

	struct bone_influence
	{
		int index;		//ボーン番号
		float weight;	//ボーンの重み
	};

	struct bone
	{
		DirectX::XMFLOAT4X4 transform;
	};

	typedef std::vector<bone_influence> bone_influences_per_control_point;
	typedef std::vector<bone> skeletal;

	struct skeletal_animation :public std::vector<skeletal>
	{
		float sampling_time = 1 / 24.0f;
		float animation_tick = 0.0f;
	};

#define MAX_BONE_INFLUENCES 4
	struct vertex
	{
		DirectX::XMFLOAT3 position;		//位置
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 normal;		//法線
		DirectX::XMFLOAT2 texcoord;
		FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 1,0,0,0};
		INT bone_indices[MAX_BONE_INFLUENCES] = {};
	};

#define MAX_BONES 256
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;		//ワールド・ビュー・プロジェクション合成行列
		DirectX::XMFLOAT4X4 world;						//ワールド変換行列
		DirectX::XMFLOAT4 material_color;				//材質色
		DirectX::XMFLOAT4 light_direction;				//ライト進行方向
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES];
	};

	struct material
	{
		DirectX::XMFLOAT4 color = { 0.8f,0.8f,0.8f,1.0f };
		ID3D11ShaderResourceView *shader_resource_view;
	};
	//material diffuse;
	struct subset
	{
		u_int index_start = 0; //start number of index buffer
		u_int index_count = 0; //number of vertices (indices)
		material diffuse;
	};
	//std::vector<subset>subsets;

	struct mesh
	{
		ID3D11Buffer* vertex_buffer;
		ID3D11Buffer* index_buffer;
		std::vector<subset> subsets;
		DirectX::XMFLOAT4X4 global_transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		//std::vector<skinned_mesh::bone> skeletal;
		skeletal_animation skeletal_animation;
		void CreateBuffer(ID3D11Device* Device, vertex* vertices, u_int* indices, int numIndex, int numVertex);

	};

	std::vector<mesh> meshes;


	void render(ID3D11DeviceContext* Context,
		const DirectX::XMFLOAT4X4& word_view,
		const DirectX::XMFLOAT4X4& wordM,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& Material_color,
		bool bWireframe,
		float elapsed_time);

};