#ifndef _DIRECTX_GI
#define _DIRECTX_GI

#include "../sunset_graphics_pch.h"
#include "DirectX_GI_def.h"

#include <d3dcommon.h>



struct ID3D12CommandQueue;

namespace DXGI
{
//------------------------------------------------------------------------------------------------
//関数プロトタイプ宣言	
//------------------------------------------------------------------------------------------------

	BOOL DirectX_GI_LoadLibraryAndModule();	//DXGI.dllと関連モジュールの読み込み

	BOOL DirectX_GI_Init();		//DirectX GraphicInterface系の初期化
	BOOL DirectX_GI_CleanUp();	//DirectX GraphicInterface系の解放
	
	//Factoryの生成
	BOOL CreateFactory();				

	SUNSET_GRAPHICS_API void GetUseGPUName(WCHAR pName[128]);
	SUNSET_GRAPHICS_API BOOL GetAdapterForName(
		IDXGIAdapter1** pAdapter,
		const wchar_t* searchName
	);
	SUNSET_GRAPHICS_API BOOL GetAdapterForHardware(
		IDXGIAdapter1** pAdapter,
		D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_1
	);

	SUNSET_GRAPHICS_API BOOL SetWindowAssociation(HWND hWnd, UINT flags);
	SUNSET_GRAPHICS_API DXGI_Factory* GetFactory();
	SUNSET_GRAPHICS_API DXGI_Adapter* GetUseAdapter();
	SUNSET_GRAPHICS_API DXGI_Adapter* GetAdapter(UINT index);
}

#endif // !_DIRECTX_GI
