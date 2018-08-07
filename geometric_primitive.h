#pragma once
#include <d3d11.h>
#include<DirectXMath.h>

class geometric_primitive{

protected:
	ID3D11VertexShader*     Vertex;
	ID3D11PixelShader*      PixelShader;
	ID3D11InputLayout*      Layout;
	ID3D11Buffer*           VBuffer;
	ID3D11Buffer*			IBuffer;
	ID3D11Buffer*			CBuffer;
	ID3D11RasterizerState*  WRasState;
	ID3D11RasterizerState*  FRasState;
	ID3D11DepthStencilState*    DepthState;

public:
	geometric_primitive(ID3D11Device*);

	virtual ~geometric_primitive();

	struct vertex
	{
		DirectX::XMFLOAT3 position;		//位置
		DirectX::XMFLOAT3 normal;		//法線
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;		//ワールド・ビュー・プロジェクション合成行列
		DirectX::XMFLOAT4X4 world;						//ワールド変換行列
		DirectX::XMFLOAT4 material_color;				//材質色
		DirectX::XMFLOAT4 light_direction;				//ライト進行方向
	};

	void CreateBuffer(ID3D11Device* Device, vertex* vertices, u_int* indices, int numIndex, int numVertex);

	void render(ID3D11DeviceContext* Context,
				const DirectX::XMFLOAT4X4& word_view,
				const DirectX::XMFLOAT4X4& wordM,
				const DirectX::XMFLOAT4& light,
				const DirectX::XMFLOAT4& Material_color,
				bool bWireframe);

};

class geometric_cube : public geometric_primitive {
public:
	geometric_cube(ID3D11Device* Device);
	~geometric_cube() {};
};

class geometric_cylinder : public geometric_primitive{
public:
	const int NUM;
	geometric_cylinder(ID3D11Device* Device, int num = 6);		//numは何角形で近似するか
	~geometric_cylinder() {};
};