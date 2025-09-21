#include "DirectX_GI.h"
#include "../sunset_graphics_pch.h"

#include <dxgi1_6.h>

#include <d3d11_4.h>
#include <d3d12.h>
#include <wrl.h>

#include <vector>
#include <string>
#include <stdexcept>

namespace DX12
{
	extern PFN_D3D12_CREATE_DEVICE D3D12CreateDevice;
}

namespace DXGI
{
//------------------------------------------------------------------------------------------------
//�O���[�o���ϐ���`
//------------------------------------------------------------------------------------------------
	HMODULE m_DXGI_DLL;

	typedef HRESULT (*DXGI_FUNC)(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);

	DXGI_FUNC CreateDXGIFactory;
	DXGI_FUNC DXGIGetDebugInterface;

	ComPtr<DXGI_Factory>				g_factory{};

	ComPtr<DXGI_Adapter>				m_use_adapter{};
	std::vector<ComPtr<DXGI_Adapter>>	m_adapters{};

//------------------------------------------------------------------------------------------------
//�֐���`
//------------------------------------------------------------------------------------------------

	BOOL DirectX_GI_LoadLibraryAndModule()	//DXGI.dll�Ɗ֘A���W���[���̓ǂݍ���
	{
		//DXGI.dll�̓ǂݍ���
		m_DXGI_DLL = LoadLibrary(L"DXGI.dll");
		if (m_DXGI_DLL == NULL) {
			//DLL�̓ǂݍ��݁F���s
			MessageBox(NULL, L"ErrorCode_0002 : Load library DXGI.dll", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0002                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0002);

			return FALSE;
		}

		//�֐���ǂݍ���
		CreateDXGIFactory = (DXGI_FUNC)GetProcAddress(m_DXGI_DLL, "CreateDXGIFactory2");
		if (CreateDXGIFactory == NULL) {
			//�֐��̓ǂݍ��݁F���s
			MessageBox(NULL, L"ErrorCode_0003 : Get Process CreateDXGIFactory2()", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0003                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0003);

			return FALSE;
		}

		return TRUE;
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@DirectX GraphicInterface�n�̏������p�֐�
	// �����@�@void�i�Ȃ��j
	// �߂�l�@BOOL TRUE(1):����I���@FALSE(0):���s�A�ُ�I��
	//------------------------------------------------------------------------------------------------
	BOOL DirectX_GI_Init()
	{
		BOOL retInit = TRUE; //����i�����l�j

		//�t�@�N�g���[�̐���
		if (!CreateFactory()) {
			retInit = FALSE;
		}

		if (retInit) {
			//�t�@�N�g���[�̐����������Ȃ�
			{
				//�A�_�v�^�[�i�r�f�I�J�[�h�j��
				IDXGIAdapter1* tmpAdapter;

				UINT count = 0;
				while (g_factory->EnumAdapters1(count, &tmpAdapter) != DXGI_ERROR_NOT_FOUND) {
					m_adapters.push_back(tmpAdapter);
					tmpAdapter->Release();
					count++;
				}
			}
		}

		return retInit;
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@DirectX GraphicInterface�n�̉���p�֐�
	// �����@�@void�i�Ȃ��j
	// �߂�l�@BOOL TRUE(1):����I���@FALSE(0):���s�A�ُ�I��
	//------------------------------------------------------------------------------------------------
	BOOL DirectX_GI_CleanUp()
	{
		BOOL retCleanUp = TRUE;	//����i�����l�j

		if (m_use_adapter.Reset() > 0) {
			//Error(�Q�ƃJ�E���g���P�ȏ�Ȃ̂ŁA�ǂ����Ŏg�p���ꂽ�܂�)
			//m_Adapters����Q�Ƃ����ꍇ������̂ł��̏ꍇ�͐������ꍇ������
			retCleanUp = FALSE;
		}

		while (1) {
			if (m_adapters.empty()) break; //��Ȃ烋�[�v�𔲂���
			const auto& it = m_adapters.end() - 1;
			if (it->Reset() != 0) { //�ŏI�s���Z�b�g
				//Error(�Q�ƃJ�E���g���P�ȏ�Ȃ̂ŁA�ǂ����Ŏg�p���ꂽ�܂�)
				retCleanUp = FALSE;
			}
			m_adapters.pop_back(); //�ŏI�v�f�폜
		}

		if (g_factory.Reset() != 0) {
			//Error(�Q�ƃJ�E���g���P�ȏ�Ȃ̂ŁA�ǂ����Ŏg�p���ꂽ�܂�)
			retCleanUp = FALSE;
		}

		return retCleanUp;
	}


	//------------------------------------------------------------------------------------------------
	// ���e�@�@DXGI Factory�̐���
	// �����@�@void�i�Ȃ��j
	// �߂�l�@BOOL TRUE(1):����I���@FALSE(0):���s�A�ُ�I��
	//------------------------------------------------------------------------------------------------
	BOOL CreateFactory()
	{
		BOOL retCreate = TRUE;

		UINT dbgFlag = (SGraphics::isDebug) ? (DXGI_CREATE_FACTORY_DEBUG) : 0;
		HRESULT hr = CreateDXGIFactory(dbgFlag, IID_PPV_ARGS(g_factory.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (FAILED(hr)) {
			retCreate = FALSE;
		}

		return retCreate;
	}

	

	//------------------------------------------------------------------------------------------------
	// ���e�@�@���ݎg�p���̃A�_�v�^�[�i�r�f�I�J�[�h�j�̖��O���擾����
	// �����@�@WCHAR pName[128]�i�߂�l�Ƃ��Ďg�p�j
	// �߂�l�@void�i�Ȃ��j
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API void GetUseGPUName(WCHAR pName[128])
	{
		DXGI_ADAPTER_DESC1 desc1;
		m_use_adapter->GetDesc1(&desc1);

		memcpy(pName, desc1.Description, 128);
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@���ݎg�p���̃A�_�v�^�[�i�r�f�I�J�[�h�j�̖��O���擾����
	// �����@�@WCHAR pName[128]�i�߂�l�Ƃ��Ďg�p�j
	// �߂�l�@void�i�Ȃ��j
	//------------------------------------------------------------------------------------------------
	void SetUseAdapter(IDXGIAdapter1* adapter)
	{
		m_use_adapter = adapter;
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@���ݎg�p���̃A�_�v�^�[�i�r�f�I�J�[�h�j�̖��O���擾����
	// �����@�@WCHAR pName[128]�i�߂�l�Ƃ��Ďg�p�j
	// �߂�l�@void�i�Ȃ��j
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API BOOL SetWindowAssociation(HWND hWnd, UINT flags)
	{
		HRESULT hr = g_factory->MakeWindowAssociation(hWnd, flags);
		_ASSERT_EXPR(FAILED(hr), hr_trace(hr));
		return TRUE;
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@�����Q�̖��O�ƍ������A�_�v�^�[�i�r�f�I�J�[�h�j���擾����
	// �����P�@IDXGIAdapter** pAdapter�F�߂�l�Ƃ��Ďg�p
	// �����Q�@const wchar_t* findName�F
	// �߂�l�@void�i�Ȃ��j
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API BOOL GetAdapterForName(IDXGIAdapter1** pAdapter, const wchar_t* findName)
	{
		if (findName == nullptr ||
			g_factory.operator Microsoft::WRL::Details::BoolType()) {
			return FALSE;
		}

		BOOL adapterFind = FALSE;
		
		int count = 0;
		while (g_factory->EnumAdapters(count, (IDXGIAdapter**)pAdapter) != DXGI_ERROR_NOT_FOUND)
		{	
			DXGI_ADAPTER_DESC adesc = {};
			(*pAdapter)->GetDesc(&adesc);

			std::wstring strDesc = adesc.Description;
			if (strDesc.find(findName) != std::string::npos) {
				adapterFind = TRUE;
				
				break;
			}
			count++;
		}
		return adapterFind;
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@Hardware�̃A�_�v�^�[�i�r�f�I�J�[�h�j���擾����
	// �����P�@IDXGIAdapter1** pAdapter       �F�߂�l�Ƃ��Ďg�p
	// �����Q�@D3D_FEATURE_LEVEL minimumsLevel�F�ۏ؂���Œ჌�x���̋@�\���x��
	// �߂�l�@void�i�Ȃ��j
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API BOOL GetAdapterForHardware(
		IDXGIAdapter1** pAdapter,
		D3D_FEATURE_LEVEL minimumsLevel
	)
	{
		if (!g_factory.operator Microsoft::WRL::Details::BoolType()) {
			return FALSE;
		}

		INT index = 0;
		BOOL adapterFound = FALSE;

		while (adapterFound == FALSE) //�������Ȃ�p��
		{	
			if (g_factory->EnumAdapters(index, (IDXGIAdapter**)pAdapter) == DXGI_ERROR_NOT_FOUND) {
				break; //�������i���[�v�I���j
			}
			++index; //���̗v�f���w��

			//�擾�����A�_�v�^�[�̏����擾
			DXGI_ADAPTER_DESC1 desc;
			(*pAdapter)->GetDesc1(&desc);

			UINT useFlagSoftware = desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
			if (!(useFlagSoftware)) { //DXGI_ADAPTER_FLAG_SOFTWARE ���g�p���Ă��Ȃ��̂Ȃ�

				//�w��̃A�_�v�^�[�Ńf�o�C�X�𐶐��ł��邩����
				HRESULT hr = DX12::D3D12CreateDevice(*pAdapter, minimumsLevel, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hr)) //����
				{
					
					adapterFound = TRUE; //�����i���[�v�I���j
				}
			}
		}

		return adapterFound; //�����̗L����Ԃ�
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@��������Factory�̎擾
	// �����@�@void�i�Ȃ��j
	// �߂�l�@Use_DXGI_Factory*
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API DXGI_Factory* GetFactory()
	{
		return g_factory.Get();
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@�g�p����Adapter���擾
	// �����@�@void�i�Ȃ��j
	// �߂�l�@Use_DXGI_Adapter*
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API DXGI_Adapter* GetUseAdapter()
	{
		return m_use_adapter.Get();
	}

	//------------------------------------------------------------------------------------------------
	// ���e�@�@�񋓂���Adapter��1���擾
	// �����@�@UINT index
	// �߂�l�@Use_DXGI_Adapter*
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API DXGI_Adapter* GetAdapter(UINT index)
	{
		if (m_adapters.size() <= index) {
			return nullptr;
		} else {
			return m_adapters[index].Get();
		}
	}
}