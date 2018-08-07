#pragma once
#include <d3d11.h>
#include<string>


class Blend
{
public:
	enum BLEND_MODE
	{
		NONE = 0,		//合成無し(デフォルト)
	    ALPHA,			//α合成
		ADD,			//加算合成
		SUB,			//減算合成
		REPLACE,		//置き換え
		MULTIPLY,		//乗算
		LIGHTEN,		//比較(明)
		DARKEN,			//比較(暗)
		SCREEN,			//スクリーン

		MODE_MAX
	};

private:
	static ID3D11BlendState* BlendState[MODE_MAX];			//ブレンド設定配列
	static bool bLoad;										//true:設定配列作成済み	
	static BLEND_MODE   enumMode;							//現在使用しているブレンドモード

	Blend() {};
	~Blend() { Release(); };

public:
	static bool Initialize(ID3D11Device*Device);								//ブレンド設定配列初期化
	static void Release() {
		for (BLEND_MODE mode = NONE; mode < MODE_MAX; mode = (BLEND_MODE)(mode + 1)) {
			BlendState[mode]->Release();
		}
		bLoad = false;
	}
	static void Set(ID3D11DeviceContext*Context, BLEND_MODE mode = NONE) {  	//ブレンド設定用関数
	
		if (!bLoad)						    return;
		if (mode < 0 || mode >= MODE_MAX)   return;
		if (mode == enumMode)				return;

		
		Context->OMSetBlendState(BlendState[mode], nullptr, 0xffffffff);
		enumMode=mode;
	
	}

	
};

