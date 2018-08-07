#pragma once
#include "framework.h"

//class ResourceManager
//{
//private:
//	static const int RESOURCE_MAX = 2;
//
//	struct RSRVS
//	{
//		int						     iRefNum;				    //�f�[�^�̎Q�Ɛ�
//		wchar_t					     path[256];					//�t�@�C���p�X
//		ID3D11ShaderResourceView*    SRView;					//�ۑ��Ώ�
//		
//		RSRVS() :iRefNum(0), SRView(NULL) { path[0]= '\0'; }
//
//
//		void Release(bool bForece = false){      
//			if (iRefNum == 0)return;
//			if (--iRefNum <= 0) bForece = true;//�Q�Ɛ����c���Ă���Ȃ������Ȃ�     
//			//�������
//			if (bForece)
//			{
//				if (SRView)SRView->Release();
//				SRView = NULL;
//				iRefNum = 0;					//�Q�Ɛ���0
//				path[0] = '\0';					//�t�@�C���p�X������
//			}
//		}
//
//	};
//	
//
//	struct ResourceVertxShaders
//	{
//		int						     iRefNum;				    //�f�[�^�̎Q�Ɛ�
//		wchar_t					     path[256];					//�t�@�C���p�X
//		ID3D11VertexShader*	         VShader;						//�ۑ��Ώ�
//		ID3D11InputLayout*           Layout;
//
//		ResourceVertxShaders() :iRefNum(0), VShader(NULL),Layout(NULL) { path[0] = '\0'; }
//
//
//		void Release(bool bForece = false) {
//			if (iRefNum == 0)return;
//			if (--iRefNum <= 0) bForece = true;//�Q�Ɛ����c���Ă���Ȃ������Ȃ�     
//											   //�������
//			if (bForece)
//			{
//				if (VShader)VShader->Release();
//				if (Layout)Layout->Release();
//				Layout = NULL;
//				VShader = NULL;
//				iRefNum = 0;					//�Q�Ɛ���0
//				path[0] = '\0';					//�t�@�C���p�X������
//			}
//		}
//
//
//	};
//
//	struct ResourecePixelShader
//	{
//		int						     iRefNum;				    //�f�[�^�̎Q�Ɛ�
//		wchar_t					     path[256];					//�t�@�C���p�X
//		ID3D11PixelShader*           Pixel;						//�ۑ��Ώ�
//
//		ResourecePixelShader() :iRefNum(0), Pixel(NULL) { path[0] = '\0'; }
//
//
//		void Release(bool bForece = false) {
//			if (iRefNum ==0 )return;
//			if (--iRefNum <= 0) bForece = true;//�Q�Ɛ����c���Ă���Ȃ������Ȃ�     
//											   //�������
//			if (bForece)
//			{
//				if (Pixel)Pixel->Release();
//				Pixel = NULL;
//				iRefNum = 0;					//�Q�Ɛ���0
//				path[0] = '\0';					//�t�@�C���p�X������
//			}
//		}
//
//
//	};
//	
//	//�R���X�g���N�^/�f�X�g���N�^�͊O���Ŏg�p�֎~(new/delete�ł��Ȃ�)
//	ResourceManager() {};
//	~ResourceManager() {};
//
//	static RSRVS                 SRViews[RESOURCE_MAX];
//	static ResourceVertxShaders  VertexShaders[RESOURCE_MAX];
//	static ResourecePixelShader   Pixels[RESOURCE_MAX];
//	
//public:
//	static void Release();
//	
//	static  bool ResourceManager::LoadShaderResourceView(ID3D11Device*Device, const wchar_t*fileName, ID3D11ShaderResourceView**SRVviwe,D3D11_TEXTURE2D_DESC*TexDesc);
//	static  bool ResourceManager::LoadPixelShader       (ID3D11Device*Device,  const char*csofileName, ID3D11PixelShader**Pixel);
//	static  bool ResourceManager::LoadVertexShader		(ID3D11Device*Device, const char*csofileName,D3D11_INPUT_ELEMENT_DESC*InputElementDesc,int numElements,
//		ID3D11VertexShader**Vertex,ID3D11InputLayout**InputLayout);
//
//
//	static void ReleaseShaderResourceView(ID3D11ShaderResourceView * SRV);
//	static void ReleaseVertexShader(ID3D11VertexShader * VertexShader, ID3D11InputLayout * InputLayout);
//	static void ReleasePixelShader(ID3D11PixelShader*Pixel);
//
//};
////-----------------ResourceManager�͂����܂�----------


class ResourceManager
{
private:
	static const int	RESOURCE_MAX = 64;

	struct RSResourceViews {
		int		iRefNum;		//�Q�Ɛ�
		wchar_t	path[256];		//�t�@�C���p�X
		ID3D11ShaderResourceView* SRView;	//�ۑ��Ώ�

		RSResourceViews() :iRefNum(0), SRView(NULL) { path[0] = '\0'; }
		void Release(bool bForce = false)
		{
			if (iRefNum == 0) return;
			if (--iRefNum <= 0) bForce = true;		//�Q�Ɛ����c���Ă���Ȃ������Ȃ�
			if (bForce) {
				if (SRView) SRView->Release();
				SRView = NULL;
				iRefNum = 0;		//�@�Q�Ɛ���0
				path[0] = '\0';		//�t�@�C���p�X������
			}
		}
	};


	struct ResourceVertexShaders {
		int		iRefNum;		//�Q�Ɛ�
		wchar_t	path[256];		//�t�@�C���p�X
		ID3D11VertexShader*     VertexShader;	//�ۑ��Ώ�
		ID3D11InputLayout*      Layout;
		ResourceVertexShaders() :iRefNum(0), VertexShader(NULL), Layout(NULL) { path[0] = '\0'; }
		void Release(bool bForce = false)
		{
			if (iRefNum == 0) return;
			if (--iRefNum <= 0) bForce = true;		//�Q�Ɛ����c���Ă���Ȃ������Ȃ�
			if (bForce) {
				if (Layout) Layout->Release();
				if (VertexShader) VertexShader->Release();
				Layout = NULL;
				VertexShader = NULL;
				iRefNum = 0;		//�@�Q�Ɛ���0
				path[0] = '\0';		//�t�@�C���p�X������
			}
		}
	};


	struct ResourcePixelShaders {
		int		iRefNum;		//�Q�Ɛ�
		wchar_t	path[256];		//�t�@�C���p�X
		ID3D11PixelShader* PixelShader;	//�ۑ��Ώ�

		ResourcePixelShaders() :iRefNum(0), PixelShader(NULL) { path[0] = '\0'; }
		void Release(bool bForce = false)
		{
			if (iRefNum == 0) return;
			if (--iRefNum <= 0) bForce = true;		//�Q�Ɛ����c���Ă���Ȃ������Ȃ�
			if (bForce) {
				if (PixelShader) PixelShader->Release();
				PixelShader = NULL;
				iRefNum = 0;		//�@�Q�Ɛ���0
				path[0] = '\0';		//�t�@�C���p�X������
			}
		}
	};


	//�R���X�g���N�^/�f�X�g���N�^�͊O���Ŏg�p�֎~(new/delete�ł��Ȃ�)
	ResourceManager() {};
	~ResourceManager() {};

	static RSResourceViews SRViews[RESOURCE_MAX];
	static ResourceVertexShaders VShaders[RESOURCE_MAX];
	static ResourcePixelShaders PShaders[RESOURCE_MAX];

public:
	static void Release();
	static void ReleaseShaderResourceView(ID3D11ShaderResourceView* SRView);
	static void ReleaseVertexShaders(ID3D11VertexShader* VertexShader, ID3D11InputLayout* InputLayout);
	static void ReleasePixelShaders(ID3D11PixelShader* PixelShader);

	static bool LoadShaderResourceView(
		ID3D11Device *Device,
		const wchar_t* filename,
		ID3D11ShaderResourceView** SRView,
		D3D11_TEXTURE2D_DESC*  TexDesc
		);

	static bool LoadVertexShader(
		ID3D11Device *Device,
		const char* csoFileName,
		D3D11_INPUT_ELEMENT_DESC* InputElementDesc,
		int numElements,
		ID3D11VertexShader** VertexShader,
		ID3D11InputLayout** InputLayout
		);

	static bool LoadPixelShaders(
		ID3D11Device *Device,
		const char* csoFileName,
		ID3D11PixelShader** PixelShader
		);
};



