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
//グローバル変数定義
//------------------------------------------------------------------------------------------------
	HMODULE m_DXGI_DLL;

	typedef HRESULT (*DXGI_FUNC)(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);

	DXGI_FUNC CreateDXGIFactory;
	DXGI_FUNC DXGIGetDebugInterface;

	ComPtr<DXGI_Factory>				g_factory{};

	ComPtr<DXGI_Adapter>				m_use_adapter{};
	std::vector<ComPtr<DXGI_Adapter>>	m_adapters{};

//------------------------------------------------------------------------------------------------
//関数定義
//------------------------------------------------------------------------------------------------

	BOOL DirectX_GI_LoadLibraryAndModule()	//DXGI.dllと関連モジュールの読み込み
	{
		//DXGI.dllの読み込み
		m_DXGI_DLL = LoadLibrary(L"DXGI.dll");
		if (m_DXGI_DLL == NULL) {
			//DLLの読み込み：失敗
			MessageBox(NULL, L"ErrorCode_0002 : Load library DXGI.dll", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0002                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0002);

			return FALSE;
		}

		//関数を読み込む
		CreateDXGIFactory = (DXGI_FUNC)GetProcAddress(m_DXGI_DLL, "CreateDXGIFactory2");
		if (CreateDXGIFactory == NULL) {
			//関数の読み込み：失敗
			MessageBox(NULL, L"ErrorCode_0003 : Get Process CreateDXGIFactory2()", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0003                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0003);

			return FALSE;
		}

		return TRUE;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　DirectX GraphicInterface系の初期化用関数
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
	//------------------------------------------------------------------------------------------------
	BOOL DirectX_GI_Init()
	{
		BOOL retInit = TRUE; //正常（初期値）

		//ファクトリーの生成
		if (!CreateFactory()) {
			retInit = FALSE;
		}

		if (retInit) {
			//ファクトリーの生成が成功なら
			{
				//アダプター（ビデオカード）列挙
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
	// 内容　　DirectX GraphicInterface系の解放用関数
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
	//------------------------------------------------------------------------------------------------
	BOOL DirectX_GI_CleanUp()
	{
		BOOL retCleanUp = TRUE;	//正常（初期値）

		if (m_use_adapter.Reset() > 0) {
			//Error(参照カウントが１以上なので、どこかで使用されたまま)
			//m_Adaptersから参照した場合があるのでこの場合は正しい場合がある
			retCleanUp = FALSE;
		}

		while (1) {
			if (m_adapters.empty()) break; //空ならループを抜ける
			const auto& it = m_adapters.end() - 1;
			if (it->Reset() != 0) { //最終行リセット
				//Error(参照カウントが１以上なので、どこかで使用されたまま)
				retCleanUp = FALSE;
			}
			m_adapters.pop_back(); //最終要素削除
		}

		if (g_factory.Reset() != 0) {
			//Error(参照カウントが１以上なので、どこかで使用されたまま)
			retCleanUp = FALSE;
		}

		return retCleanUp;
	}


	//------------------------------------------------------------------------------------------------
	// 内容　　DXGI Factoryの生成
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
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
	// 内容　　現在使用中のアダプター（ビデオカード）の名前を取得する
	// 引数　　WCHAR pName[128]（戻り値として使用）
	// 戻り値　void（なし）
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API void GetUseGPUName(WCHAR pName[128])
	{
		DXGI_ADAPTER_DESC1 desc1;
		m_use_adapter->GetDesc1(&desc1);

		memcpy(pName, desc1.Description, 128);
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　現在使用中のアダプター（ビデオカード）の名前を取得する
	// 引数　　WCHAR pName[128]（戻り値として使用）
	// 戻り値　void（なし）
	//------------------------------------------------------------------------------------------------
	void SetUseAdapter(IDXGIAdapter1* adapter)
	{
		m_use_adapter = adapter;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　現在使用中のアダプター（ビデオカード）の名前を取得する
	// 引数　　WCHAR pName[128]（戻り値として使用）
	// 戻り値　void（なし）
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API BOOL SetWindowAssociation(HWND hWnd, UINT flags)
	{
		HRESULT hr = g_factory->MakeWindowAssociation(hWnd, flags);
		_ASSERT_EXPR(FAILED(hr), hr_trace(hr));
		return TRUE;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　引数２の名前と合ったアダプター（ビデオカード）を取得する
	// 引数１　IDXGIAdapter** pAdapter：戻り値として使用
	// 引数２　const wchar_t* findName：
	// 戻り値　void（なし）
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
	// 内容　　Hardwareのアダプター（ビデオカード）を取得する
	// 引数１　IDXGIAdapter1** pAdapter       ：戻り値として使用
	// 引数２　D3D_FEATURE_LEVEL minimumsLevel：保証する最低レベルの機能レベル
	// 戻り値　void（なし）
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

		while (adapterFound == FALSE) //未発見なら継続
		{	
			if (g_factory->EnumAdapters(index, (IDXGIAdapter**)pAdapter) == DXGI_ERROR_NOT_FOUND) {
				break; //未発見（ループ終了）
			}
			++index; //次の要素を指す

			//取得したアダプターの情報を取得
			DXGI_ADAPTER_DESC1 desc;
			(*pAdapter)->GetDesc1(&desc);

			UINT useFlagSoftware = desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
			if (!(useFlagSoftware)) { //DXGI_ADAPTER_FLAG_SOFTWARE を使用していないのなら

				//指定のアダプターでデバイスを生成できるか試す
				HRESULT hr = DX12::D3D12CreateDevice(*pAdapter, minimumsLevel, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hr)) //成功
				{
					
					adapterFound = TRUE; //発見（ループ終了）
				}
			}
		}

		return adapterFound; //発見の有無を返す
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　生成したFactoryの取得
	// 引数　　void（なし）
	// 戻り値　Use_DXGI_Factory*
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API DXGI_Factory* GetFactory()
	{
		return g_factory.Get();
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　使用中のAdapterを取得
	// 引数　　void（なし）
	// 戻り値　Use_DXGI_Adapter*
	//------------------------------------------------------------------------------------------------
	SUNSET_GRAPHICS_API DXGI_Adapter* GetUseAdapter()
	{
		return m_use_adapter.Get();
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　列挙したAdapterの1つを取得
	// 引数　　UINT index
	// 戻り値　Use_DXGI_Adapter*
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