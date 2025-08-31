#ifndef _DIRECT3D_12
#define _DIRECT3D_12

#include "../sunset_graphics_pch.h"

#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>

#include <string.h>

//#include "ShaderVariable.h"
//#include "Shader.h"


namespace DX12
{
//------------------------------------------------------------------------------------------------
//関数プロトタイプ宣言	
//------------------------------------------------------------------------------------------------

	BOOL Direct3D_12_LoadLibraryAndModule();	//D3D12.dllと関連モジュールの読み込み

	BOOL Direct3D_12_Init();		//DirectX12系の初期化
	BOOL Direct3D_12_CleanUp();		//DirectX12系の解放

	BOOL EnableDebugLayer(); //デバックレイヤーを有効化

	//デバイスの作成
	BOOL CreateDevice(
		D3D_FEATURE_LEVEL minimumLevel = D3D_FEATURE_LEVEL_11_0
	);

	BOOL CreateCommandQueue(
		D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT
	);

	typedef ID3D12Device* (*PfnGetDevice)(void);
	SUNSET_GRAPHICS_API ID3D12Device* GetDevice();

	SUNSET_GRAPHICS_API ID3D12CommandQueue* GetCommandQueue();
}

#endif // !_DIRECT3D_12
