#ifndef _DIRECTX_GI
#define _DIRECTX_GI

#include "../sunset_graphics_pch.h"

#include <dxgi1_6.h>
#include <d3dcommon.h>

namespace DXGI
{
	typedef IDXGIFactory4 DXGI_Factory;
	typedef IDXGIAdapter1 DXGI_Adapter;

//------------------------------------------------------------------------------------------------
//�֐��v���g�^�C�v�錾	
//------------------------------------------------------------------------------------------------

	BOOL DirectX_GI_LoadLibraryAndModule();	//DXGI.dll�Ɗ֘A���W���[���̓ǂݍ���

	BOOL DirectX_GI_Init();		//DirectX GraphicInterface�n�̏�����
	BOOL DirectX_GI_CleanUp();	//DirectX GraphicInterface�n�̉��
	
	BOOL CreateFactory();	//Factory�̐���

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
