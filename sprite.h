#pragma once
#include <d3d11.h>
#include<DirectXMath.h>



class sprite {

private:
	ID3D11VertexShader*     Vertex = NULL;
	ID3D11PixelShader*      PixelShader = NULL;
	ID3D11InputLayout*      Layout = NULL;
	ID3D11Buffer*           Buffer = NULL;
	ID3D11RasterizerState*  RasState = NULL;
	ID3D11ShaderResourceView* SRView = NULL;
	D3D11_TEXTURE2D_DESC     Tex2d_desc;
	ID3D11SamplerState*       SamplerState = NULL;
	ID3D11DepthStencilState*    DepthState = NULL;
	//ID3D11BlendState*           BlendState = NULL;

public:
	sprite(ID3D11Device*,const wchar_t*);

	~sprite();

	void render(ID3D11DeviceContext * Context, float dx, float dy, float dw, float dh,float sx,float sy,float sw,float sh, float angle, float r, float g, float b, float a);
	



	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};