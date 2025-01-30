#include "SunsetGraphics.h"

#include "Direct3D12/Direct3D_12.h"
#include "DXGI/DirectX_GI.h"

#include <stdio.h>

//------------------------------------------------------------------------------------------------
//�֐���`
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// ���e�@�@Graphics�n�̏������p�֐�
// �����@�@void�i�Ȃ��j
// �߂�l�@BOOL TRUE(1):����I���@FALSE(0):���s�A�ُ�I��
// 
// ���l�@�@�֐��FDllMain()���ŌĂ΂Ȃ��B
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
// ���e�@�@Graphics�n�̏������p�֐�
// �����@�@void�i�Ȃ��j
// �߂�l�@BOOL TRUE(1):����I���@FALSE(0):���s�A�ُ�I��
//------------------------------------------------------------------------------------------------
BOOL Sunset_Graphics_CleanUp()
{
	printf("SGraphics_CleanUp Start\n");

	BOOL retCleanUp = TRUE;

	BOOL retD3D12CleanUp = DX12::Direct3D_12_CleanUp();
	if (retD3D12CleanUp == FALSE) {
		//TODO::Dsp ErrorCode(���g��)
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