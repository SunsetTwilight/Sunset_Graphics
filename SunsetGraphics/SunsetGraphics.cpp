#include "SunsetGraphics.h"

#include "Direct3D12/Direct3D_12.h"
#include "DXGI/DirectX_GI.h"

#include <stdio.h>

//------------------------------------------------------------------------------------------------
//関数定義
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// 内容　　Graphics系の初期化用関数
// 引数　　void（なし）
// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
// 
// 備考　　関数：DllMain()内で呼ばない。
//------------------------------------------------------------------------------------------------
BOOL Sunset_Graphics_Init()
{
	printf("SGraphics_Init Start\n");

	BOOL resultLoadLibrary = TRUE;

	resultLoadLibrary &= DXGI::DirectX_GI_LoadLibraryAndModule();
	resultLoadLibrary &= DX12::Direct3D_12_LoadLibraryAndModule();

	if (resultLoadLibrary == TRUE) return FALSE;

	DXGI::DirectX_GI_Init();
	DX12::Direct3D_12_Init();

	printf("SGraphics_Init End\n");
	return TRUE;
}

//------------------------------------------------------------------------------------------------
// 内容　　Graphics系の初期化用関数
// 引数　　void（なし）
// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
//------------------------------------------------------------------------------------------------
BOOL Sunset_Graphics_CleanUp()
{
	printf("SGraphics_CleanUp Start\n");

	BOOL retCleanUp = TRUE;

	BOOL retD3D12CleanUp = DX12::Direct3D_12_CleanUp();
	if (retD3D12CleanUp == FALSE) {
		//TODO::Dsp ErrorCode(中身は)
		retCleanUp = FALSE;
	}

	BOOL retDXGICleanUp = DXGI::DirectX_GI_CleanUp();
	if (retDXGICleanUp == FALSE) {
		//TODO::Dsp ErrorCode
		retCleanUp = FALSE;
	}

	printf("SGraphics_CleanUp End\n");

	return retCleanUp;
}