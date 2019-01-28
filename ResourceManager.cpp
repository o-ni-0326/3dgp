#include"framework.h"
#include"WICTextureLoader.h"
#include"DDSTextureLoader.h"
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
	int no = -1;							//�V�f�[�^�ۑ���ԍ�
	RSResourceViews* find = NULL;			//���������f�[�^or�V�f�[�^
	ID3D11Resource* Resource = NULL;

	//�f�[�^����
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		RSResourceViews* p = &SRViews[n];

		//�f�[�^�������Ȃ疳��
		//�A���ŏ��Ɍ������󂫗̈�Ȃ�Z�b�g�p�Ɋm��
		if (p->iRefNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		//�t�@�C���p�X���Ⴄ�Ȃ疳��
		if (wcscmp(p->path, filename) != 0) continue;

		//�����t�@�C�������݂���
		find = p;								//����
		p->SRView->GetResource(&Resource);		//Resource�̏���
		break;
	}

	//�f�[�^��������Ȃ��������V�K�ǂݍ���
	if (!find)
	{
		HRESULT hr = S_OK;
		RSResourceViews* p = &SRViews[no];
		if (_wcsicmp(&filename[wcslen(filename) - 3], L"TGA") == 0) {
			hr = DirectX::CreateWICTextureFromFile(Device, filename, &Resource, &(p->SRView));
		}
		else {
			hr = DirectX::CreateWICTextureFromFile(Device, filename, &Resource, &(p->SRView));
		}
		if (FAILED(hr)) {
			//_ASSERTE(false, "FAILED LOAD TEXTURE");
			return false;
		}
		find = p;			//����
		wcscpy_s(p->path, 256, filename);	//���O�R�s�[
	}


	//�ŏI����(�Q�Ɠn���Ńf�[�^��Ԃ�) �����������̌������ĂȂ����̋��ʂ̏���
	ID3D11Texture2D *tex2D;
	Resource->QueryInterface(&tex2D);		//�e�N�X�`���f�[�^�̏���

	tex2D->GetDesc(TexDesc);
	*SRView = find->SRView;					//�f�[�^�R�s�[

	find->iRefNum++;						//����ǉ�

	tex2D->Release();
	Resource->Release();					//�s�v�f�[�^�̉��

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
	int no = -1;							//�V�f�[�^�ۑ���ԍ�
	ResourceVertexShaders* find = NULL;			//���������f�[�^or�V�f�[�^

	*VertexShader = NULL;
	*InputLayout = NULL;

	wchar_t filename[256];
	size_t stringSize = 0;
	mbstowcs_s(&stringSize, filename, csoFileName, strlen(csoFileName));

	//�f�[�^����
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceVertexShaders* p = &VShaders[n];

		//�f�[�^�������Ȃ疳��
		//�A���ŏ��Ɍ������󂫗̈�Ȃ�Z�b�g�p�Ɋm��
		if (p->iRefNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		//�t�@�C���p�X���Ⴄ�Ȃ疳��
		if (wcscmp(p->path, filename) != 0) continue;

		//�����t�@�C�������݂���
		find = p;								//����
		break;
	}

	//�f�[�^��������Ȃ��������V�K�ǂݍ���
	if (!find)
	{
		ResourceVertexShaders* p = &VShaders[no];
		BYTE* data;
		unsigned int size;

		//�R���p�C���ςݒ��_�V�F�[�_�[�I�u�W�F�N�g�̓ǂݍ���
		if (!ReadBinaryFile(csoFileName, &data, size))return false;

		//���_�V�F�[�_�[�I�u�W�F�N�g�̐���
		HRESULT hr = Device->CreateVertexShader(data, size, NULL, &p->VertexShader);
		if (FAILED(hr)) {
			delete[] data;
			return false;
		}

		//���̓��C�A�E�g�̍쐬
		hr = Device->CreateInputLayout(InputElementDesc, numElements, data, size, &p->Layout);
		delete[] data;
		if (FAILED(hr)) return false;

		//�V�K�f�[�^�̕ۑ�
		find = p;			//����
		wcscpy_s(find->path, 256, filename);	//���O�R�s�[
	}


	//�ŏI����(�Q�Ɠn���Ńf�[�^��Ԃ�) �����������̌������ĂȂ����̋��ʂ̏���
	*VertexShader = find->VertexShader;
	*InputLayout = find->Layout;

	find->iRefNum++;						//����ǉ�

	return true;

}



bool ResourceManager::LoadPixelShaders(
	ID3D11Device *Device,
	const char* csoFileName,
	ID3D11PixelShader** PixelShader
	)
{
	int no = -1;							//�V�f�[�^�ۑ���ԍ�
	ResourcePixelShaders* find = NULL;			//���������f�[�^or�V�f�[�^
	*PixelShader = NULL;

	wchar_t filename[256];
	size_t stringSize = 0;
	mbstowcs_s(&stringSize, filename, csoFileName, strlen(csoFileName));

	//�f�[�^����
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &PShaders[n];

		//�f�[�^�������Ȃ疳��
		//�A���ŏ��Ɍ������󂫗̈�Ȃ�Z�b�g�p�Ɋm��
		if (p->iRefNum == 0)
		{
			if (no == -1) no = n;
			continue;
		}

		//�t�@�C���p�X���Ⴄ�Ȃ疳��
		if (wcscmp(p->path, filename) != 0) continue;

		//�����t�@�C�������݂���
		find = p;								//����
		break;
	}

	//�f�[�^��������Ȃ��������V�K�ǂݍ���
	if (!find)
	{
		ResourcePixelShaders* p = &PShaders[no];
		BYTE* data;
		unsigned int size;

		//�R���p�C���ςݒ��_�V�F�[�_�[�I�u�W�F�N�g�̓ǂݍ���
		if (!ReadBinaryFile(csoFileName, &data, size))return false;

		//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
		HRESULT hr = Device->CreatePixelShader(data, size, NULL, &p->PixelShader);
		delete[] data;
		if (FAILED(hr))return false;

		find = p;			//����
		wcscpy_s(find->path, 256, filename);	//���O�R�s�[
	}


	//�ŏI����(�Q�Ɠn���Ńf�[�^��Ԃ�) �����������̌������ĂȂ����̋��ʂ̏���
	*PixelShader = find->PixelShader;
	find->iRefNum++;

	return true;
}

void ResourceManager::ReleaseShaderResourceView(ID3D11ShaderResourceView* SRView)
{
	if (!SRView) return;						//�T���f�[�^�������Ȃ�I��

	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		RSResourceViews* p = &SRViews[n];

		if (p->iRefNum == 0) continue;			//�Q�Ɛ���0(�f�[�^������)�Ȃ疳��

		if (SRView != p->SRView) continue;		//�f�[�^���Ⴄ�Ȃ疳��

		p->Release();							//�f�[�^�����݂���
		break;
	}
}

void ResourceManager::ReleaseVertexShaders(ID3D11VertexShader* VertexShader, ID3D11InputLayout* InputLayout)
{
	if (!VertexShader) return;
	if (!InputLayout) return;			//�T���f�[�^�������Ȃ�I��

	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourceVertexShaders* p = &VShaders[n];

		if (p->iRefNum == 0) continue;			//�Q�Ɛ���0(�f�[�^������)�Ȃ疳��

		if (VertexShader != p->VertexShader) continue;
		if (InputLayout != p->Layout) continue;		//�f�[�^���Ⴄ�Ȃ疳��

		p->Release();	//�f�[�^�����݂���
		break;
	}
}

void ResourceManager::ReleasePixelShaders(ID3D11PixelShader* PixelShader)
{
	if (!PixelShader) return;						//�T���f�[�^�������Ȃ�I��

	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ResourcePixelShaders* p = &PShaders[n];

		if (p->iRefNum == 0) continue;			//�Q�Ɛ���0(�f�[�^������)�Ȃ疳��

		if (PixelShader != p->PixelShader) continue;		//�f�[�^���Ⴄ�Ȃ疳��

		p->Release();							//�f�[�^�����݂���
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

