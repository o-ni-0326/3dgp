#include"framework.h"
#include"WICTextureLoader.h"
#include "sprite.h"
#include "ResourceManager.h"
ResourceManager::RSResourceViews ResourceManager::SRViews[ResourceManager::RESOURCE_MAX];
ResourceManager::ResourceVertexShaders ResourceManager::VShaders[ResourceManager::RESOURCE_MAX];
ResourceManager::ResourcePixelShaders ResourceManager::PShaders[ResourceManager::RESOURCE_MAX];


bool ReadBinaryFile(const char* filename, BYTE**data, unsigned int &size);


bool ResourceManager::LoadShaderResourceView(
	ID3D11Device *Device,
	const wchar_t* filename,
	ID3D11ShaderResourceView** SRView,
	D3D11_TEXTURE2D_DESC*  TexDesc
	)
{
	int no = -1;							//新データ保存先番号
	RSResourceViews* find = NULL;			//見つかったデータor新データ
	ID3D11Resource* Resource = NULL;

	//データ検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		RSResourceViews* p = &SRViews[n];

		//データが無いなら無視
		//但し最初に見つけた空き領域ならセット用に確保
		if (p->iRefNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		//ファイルパスが違うなら無視
		if (wcscmp(p->path, filename) != 0) continue;

		//同名ファイルが存在した
		find = p;								//発見
		p->SRView->GetResource(&Resource);		//Resourceの準備
		break;
	}

	//データが見つからなかった＝新規読み込み
	if (!find)
	{
		RSResourceViews* p = &SRViews[no];
		HRESULT hr = DirectX::CreateWICTextureFromFile(Device, filename, &Resource, &(p->SRView));
		if (FAILED(hr))return false;
		find = p;			//発見
		wcscpy_s(p->path, 256, filename);	//名前コピー
	}


	//最終処理(参照渡しでデータを返す) 見つかったもの見つかってないもの共通の処理
	ID3D11Texture2D *tex2D;
	Resource->QueryInterface(&tex2D);		//テクスチャデータの準備

	tex2D->GetDesc(TexDesc);
	*SRView = find->SRView;					//データコピー

	find->iRefNum++;						//数を追加

	tex2D->Release();
	Resource->Release();					//不要データの解放

	return true;
}



bool ResourceManager::LoadVertexShader(
	ID3D11Device *Device,
	const char* csoFileName,
	D3D11_INPUT_ELEMENT_DESC* InputElementDesc,
	int numElements,
	ID3D11VertexShader** VertexShader,
	ID3D11InputLayout** InputLayout
	)
{
	int no = -1;							//新データ保存先番号
	ResourceVertexShaders* find = NULL;			//見つかったデータor新データ

	*VertexShader = NULL;
	*InputLayout = NULL;

	wchar_t filename[256];
	size_t stringSize = 0;
	mbstowcs_s(&stringSize, filename, csoFileName, strlen(csoFileName));

	//データ検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceVertexShaders* p = &VShaders[n];

		//データが無いなら無視
		//但し最初に見つけた空き領域ならセット用に確保
		if (p->iRefNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		//ファイルパスが違うなら無視
		if (wcscmp(p->path, filename) != 0) continue;

		//同名ファイルが存在した
		find = p;								//発見
		break;
	}

	//データが見つからなかった＝新規読み込み
	if (!find)
	{
		ResourceVertexShaders* p = &VShaders[no];
		BYTE* data;
		unsigned int size;

		//コンパイル済み頂点シェーダーオブジェクトの読み込み
		if (!ReadBinaryFile(csoFileName, &data, size))return false;

		//頂点シェーダーオブジェクトの生成
		HRESULT hr = Device->CreateVertexShader(data, size, NULL, &p->VertexShader);
		if (FAILED(hr)) {
			delete[] data;
			return false;
		}

		//入力レイアウトの作成
		hr = Device->CreateInputLayout(InputElementDesc, numElements, data, size, &p->Layout);
		delete[] data;
		if (FAILED(hr)) return false;

		//新規データの保存
		find = p;			//発見
		wcscpy_s(find->path, 256, filename);	//名前コピー
	}


	//最終処理(参照渡しでデータを返す) 見つかったもの見つかってないもの共通の処理
	*VertexShader = find->VertexShader;
	*InputLayout = find->Layout;

	find->iRefNum++;						//数を追加

	return true;

}



bool ResourceManager::LoadPixelShaders(
	ID3D11Device *Device,
	const char* csoFileName,
	ID3D11PixelShader** PixelShader
	)
{
	int no = -1;							//新データ保存先番号
	ResourcePixelShaders* find = NULL;			//見つかったデータor新データ
	*PixelShader = NULL;

	wchar_t filename[256];
	size_t stringSize = 0;
	mbstowcs_s(&stringSize, filename, csoFileName, strlen(csoFileName));

	//データ検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &PShaders[n];

		//データが無いなら無視
		//但し最初に見つけた空き領域ならセット用に確保
		if (p->iRefNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		//ファイルパスが違うなら無視
		if (wcscmp(p->path, filename) != 0) continue;

		//同名ファイルが存在した
		find = p;								//発見
		break;
	}

	//データが見つからなかった＝新規読み込み
	if (!find)
	{
		ResourcePixelShaders* p = &PShaders[no];
		BYTE* data;
		unsigned int size;

		//コンパイル済み頂点シェーダーオブジェクトの読み込み
		if (!ReadBinaryFile(csoFileName, &data, size))return false;

		//ピクセルシェーダーオブジェクトの生成
		HRESULT hr = Device->CreatePixelShader(data, size, NULL, &p->PixelShader);
		delete[] data;
		if (FAILED(hr))return false;

		find = p;			//発見
		wcscpy_s(find->path, 256, filename);	//名前コピー
	}


	//最終処理(参照渡しでデータを返す) 見つかったもの見つかってないもの共通の処理
	*PixelShader = find->PixelShader;
	find->iRefNum++;

	return true;
}

void ResourceManager::ReleaseShaderResourceView(ID3D11ShaderResourceView* SRView)
{
	if (!SRView) return;						//探索データが無いなら終了

	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		RSResourceViews* p = &SRViews[n];

		if (p->iRefNum == 0) continue;			//参照数が0(データが無い)なら無視

		if (SRView != p->SRView) continue;		//データが違うなら無視

		p->Release();							//データが存在した
		break;
	}
}

void ResourceManager::ReleaseVertexShaders(ID3D11VertexShader* VertexShader, ID3D11InputLayout* InputLayout)
{
	if (!VertexShader) return;
	if (!InputLayout) return;			//探索データが無いなら終了

	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceVertexShaders* p = &VShaders[n];

		if (p->iRefNum == 0) continue;			//参照数が0(データが無い)なら無視

		if (VertexShader != p->VertexShader) continue;
		if (InputLayout != p->Layout) continue;		//データが違うなら無視

		p->Release();	//データが存在した
		break;
	}
}

void ResourceManager::ReleasePixelShaders(ID3D11PixelShader* PixelShader)
{
	if (!PixelShader) return;						//探索データが無いなら終了

	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &PShaders[n];

		if (p->iRefNum == 0) continue;			//参照数が0(データが無い)なら無視

		if (PixelShader != p->PixelShader) continue;		//データが違うなら無視

		p->Release();							//データが存在した
		break;
	}
}

void ResourceManager::Release()
{
	for (int i = 0; i < RESOURCE_MAX; i++)
	{
		SRViews[i].Release(true);
		VShaders[i].Release(true);
		PShaders[i].Release(true);
	}
}

//bool ReadBinaryFile(const char*filename, BYTE**data, unsigned int &size);
//
//ResourceManager::RSRVS				      ResourceManager::SRViews[ResourceManager::RESOURCE_MAX];
//ResourceManager::ResourceVertxShaders     ResourceManager::VertexShaders[ResourceManager::RESOURCE_MAX];
//ResourceManager::ResourecePixelShader     ResourceManager::Pixels [ResourceManager::RESOURCE_MAX];
//
//void ResourceManager::Release()
//{
//	for (int i = 0; i < RESOURCE_MAX; i++)
//	{
//		SRViews[i].Release(true);
//		VertexShaders[i].Release(true);
//		Pixels[i].Release(true);
//	}
//}
//
//bool ResourceManager::LoadShaderResourceView(ID3D11Device*Device, const wchar_t*fileName, ID3D11ShaderResourceView**SRView, D3D11_TEXTURE2D_DESC*TexDesc)
//{
//	
//	int no = -1;			//新データ保存先番号
//	RSRVS*find = NULL;		//見つかったデータor新データ
//	ID3D11Resource*Resource = NULL;
//	//データ検索
//	for (int n = 0; n < RESOURCE_MAX; n++) {
//		RSRVS*p = &SRViews[n];
//
//
//		//データが無いなら無視
//		//最初に見つけた?開き領域なら、セット用に確保
//		if (p->iRefNum == 0) {
//			if (no == -1)no = n;
//			continue;
//		}
//		//ファイルパスが違うなら
//		if (wcscmp(p->path, fileName) != 0) continue;
//
//		//同盟ファイルが存在した
//		find = p;		//発見
//		p->SRView->GetResource(&Resource);   //Resourceの準備
//		break;
//	}
//	//データが見つからなかったら=新規読み込み
//	if (!find)
//	{
//
//	RSRVS* p = &SRViews[no];
//	HRESULT	hr = DirectX::CreateWICTextureFromFile(Device, fileName, &Resource, &(p->SRView));  
//	if (FAILED(hr))return false;
//		find = p;					          //発見
//		wcscpy_s(p->path, 256, fileName);    //名前コピー
//	}
//
//	//最終処理(参照渡しでデータを返す)
//	ID3D11Texture2D*texture2d;
//	Resource->QueryInterface(&texture2d);  //テクスチャデータの準備
//
//	texture2d->GetDesc(TexDesc);		   //データコピー
//	*SRView= find->SRView;
//	
//
//	find->iRefNum++;					   //数を追加
//
//	texture2d->Release();				   //不要データ解放
//	Resource->Release();			  
//
//	return true;
//
//
//}
//bool ResourceManager::LoadVertexShader(ID3D11Device*Device, const char*csofileName, D3D11_INPUT_ELEMENT_DESC*InputElementDesc, int numElements, ID3D11VertexShader**VertexShader, ID3D11InputLayout**InputLayout)
//{
//
//	*VertexShader = NULL;
//	*InputLayout = NULL;
//
//	ResourceVertxShaders* find = NULL;		  //新データ保存先番号
//	int                   no = -1;			  //見つかったデータor新データ
//
//	wchar_t   filename[256];
//	size_t    stringSize = 0;
//	mbstowcs_s(&stringSize, filename, csofileName ,strlen(csofileName));
//
//	//データ検索
//	for (int n = 0; n < RESOURCE_MAX; n++) {
//		ResourceVertxShaders*p = &VertexShaders[n];
//
//
//		if (p->iRefNum == 0)
//		{
//			if (no == -1)no = n;
//			continue;
//		}
//		//ファイルパスが違うなら虫
//		if (wcscmp(p->path, filename) != 0)continue;
//
//		//同名ファイルが存在した
//		find = p;
//		break;
//	}
//
//	//データが見つからなかったら=新規読み込み
//	if (!find)
//	{
//		ResourceVertxShaders*p = &VertexShaders[no];
//
//		BYTE*data;
//		unsigned int size;
//		//コンパイル済みシェーダーオブジェクトの読み込み
//		if (!ReadBinaryFile(csofileName, &data, size))return false;
//		//頂点シェーダーオブジェクトの生成
//		HRESULT hr = Device->CreateVertexShader(data, size, nullptr, &p->VShader);
//		if (FAILED(hr)) {
//			delete[] data;
//			return false;
//		}
//		//入力レイアウト作成
//		hr = Device->CreateInputLayout(InputElementDesc, numElements, data, size, &p->Layout);
//		delete[]data;
//		if (FAILED(hr))return false;
//		//新規データ
//		find = p;
//		wcscpy_s(find->path, 256, filename);    //名前コピー
//	}
//
//	//最終処理(参照渡しでデータを返す)
//	*VertexShader = find->VShader;
//	*InputLayout = find->Layout;
//	find->iRefNum++;
//
//	return true;
//}
//bool ResourceManager::LoadPixelShader(ID3D11Device*Device,  const char *csofileName,ID3D11PixelShader**Pixel)
//{
//	int no = -1;							//新データ保存先番号
//	ResourecePixelShader*find = NULL;		//見つかったデータor新データ
//
//	wchar_t filename[256];
//	size_t stringSize = 0;
//	mbstowcs_s(&stringSize, filename, csofileName,strlen(csofileName));
//	//データ検索
//	for (int n = 0; n < RESOURCE_MAX; n++) {
//		ResourecePixelShader*p = &Pixels[n];
//
//
//		//データが無いなら無視
//		//最初に見つけた?開き領域なら、セット用に確保
//		if (p->iRefNum == 0) {
//			if (no == -1)no = n;
//			continue;
//		}
//		//ファイルパスが違うなら
//		if (wcscmp(p->path, filename) != 0) continue;
//
//		//同盟ファイルが存在した
//		find = p;		//発見
//		break;
//	}
//	//データが見つからなかったら=新規読み込み
//	if (!find)
//	{
//		ResourecePixelShader*p = &Pixels[no];
//
//		BYTE*data;
//		unsigned int size;
//
//		if (!ReadBinaryFile(csofileName, &data, size))return false;
//		HRESULT hr = Device->CreatePixelShader(data, size, nullptr, &p->Pixel);
//		delete[] data;
//		if (FAILED(hr)) {
//			return false;
//		}
//
//		//新規データ
//		find = p;
//		wcscpy_s(p->path, 256, filename);    //名前コピー
//	}
//
//	//最終処理(参照渡しでデータを返す)
//	*Pixel = find->Pixel;
//
//	find->iRefNum++;					  
//
//	return true;
//
//
//}
//
//
//
//void ResourceManager::ReleaseShaderResourceView(ID3D11ShaderResourceView*SRV) {
//
//	if (!SRV) return;			//検索データが無いなら終了
//
//	for (int n = 0; n < RESOURCE_MAX; n++) {
//
//		RSRVS*p = &SRViews[n];
//
//		//データが無いなら無視
//		if (p->iRefNum == 0) continue;
//
//		//データが違うなら
//		if (SRV != p->SRView) continue;
//
//		//データが存在した
//		p->Release();   //データ解放
//		break;
//
//	}
//}
// 
//void ResourceManager::ReleaseVertexShader(ID3D11VertexShader * VertexShader, ID3D11InputLayout * InputLayout)
//{
//	if (!VertexShaders)return;
//	if (!InputLayout)return;
//
//	for (int n = 0; n < RESOURCE_MAX; n++) {
//
//		ResourceVertxShaders*p = &VertexShaders[n];
//
//		//データが無いなら無視
//		if (p->iRefNum == 0) continue;
//
//		//データが違うなら
//		if (VertexShader!= p->VShader) continue;
//		if (InputLayout != p->Layout)continue;
//
//		//データが存在した
//		p->Release();   //データ解放
//		break;
//
//	}
//}
//
//void ResourceManager::ReleasePixelShader(ID3D11PixelShader*Pixel) {
//
//	if (!Pixel) return;   //検索データが無いなら終了
//
//	for (int n = 0; n < RESOURCE_MAX; n++) {
//
//		ResourecePixelShader*p = &Pixels[n];
//
//		//データが無いなら無視
//		if (p->iRefNum == 0)continue;
//
//		//データが違うなら
//		if (Pixel != p->Pixel) continue;
//
//		//データが存在した
//		p->Release(); //データ解放
//		break;
//	}
//
//}
//
