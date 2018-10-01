#include "Blend.h"
bool  Blend::bLoad = false;
ID3D11BlendState*  Blend::BlendState[MODE_MAX];
Blend::BLEND_MODE Blend::enumMode;

struct BLEND_DATA
{
	D3D11_BLEND SrcBlend;
	D3D11_BLEND DestBlend;
	D3D11_BLEND_OP BlendOp;
	D3D11_BLEND SrcBlendAlpha;
	D3D11_BLEND DestBlendAlpha;
	D3D11_BLEND_OP BlendOpAlpha;
}

BlendData[Blend::MODE_MAX] = 
{

	{//NONE
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD
	},
	{//ALPHA
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD
	},
	{//ADD
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD
	},
	{//SUB
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_ONE,
	D3D11_BLEND_OP_REV_SUBTRACT,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD
	},
	{//REPLACE
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD
	},
	{//MULYPLY
	D3D11_BLEND_ZERO,
	D3D11_BLEND_SRC_COLOR,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_DEST_ALPHA,
	D3D11_BLEND_ZERO,
	D3D11_BLEND_OP_ADD
	},
	{//LIGHTEN
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
	D3D11_BLEND_OP_MAX,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
	D3D11_BLEND_OP_MAX
	},
	{//DARKEN
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
	D3D11_BLEND_OP_MIN,
	D3D11_BLEND_ONE,
	D3D11_BLEND_ONE,
	D3D11_BLEND_OP_MIN
	},
	{//SCREEN
	D3D11_BLEND_SRC_ALPHA,
	D3D11_BLEND_INV_SRC_COLOR,
	D3D11_BLEND_OP_ADD,
	D3D11_BLEND_ONE,
	D3D11_BLEND_INV_SRC_ALPHA,
	D3D11_BLEND_OP_ADD
	},
};
 


bool Blend::Initialize(ID3D11Device * Device)
{
	HRESULT hr;
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));

	for (BLEND_MODE mode = NONE; mode < MODE_MAX; mode = (BLEND_MODE)(mode + 1))
	{
		
		BlendDesc.AlphaToCoverageEnable = FALSE;
		BlendDesc.IndependentBlendEnable = FALSE;
		BlendDesc.RenderTarget[0].BlendEnable = TRUE;
		BlendDesc.RenderTarget[0].SrcBlend = BlendData[mode].SrcBlend;
		BlendDesc.RenderTarget[0].DestBlend = BlendData[mode].DestBlend;
		BlendDesc.RenderTarget[0].BlendOp = BlendData[mode].BlendOp;
		BlendDesc.RenderTarget[0].SrcBlendAlpha = BlendData[mode].SrcBlendAlpha;
		BlendDesc.RenderTarget[0].DestBlendAlpha = BlendData[mode].DestBlendAlpha;
		BlendDesc.RenderTarget[0].BlendOpAlpha = BlendData[mode].BlendOpAlpha;
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = Device->CreateBlendState(&BlendDesc, &BlendState[mode]);
		if (FAILED(hr))return false;
	}
	bLoad = true;
	enumMode = MODE_MAX;

	return TRUE;
}
