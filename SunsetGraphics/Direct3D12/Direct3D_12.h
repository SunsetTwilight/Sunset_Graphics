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
//�֐��v���g�^�C�v�錾	
//------------------------------------------------------------------------------------------------

	BOOL Direct3D_12_LoadLibraryAndModule();	//D3D12.dll�Ɗ֘A���W���[���̓ǂݍ���

	BOOL Direct3D_12_Init();		//DirectX12�n�̏�����
	BOOL Direct3D_12_CleanUp();		//DirectX12�n�̉��

	BOOL EnableDebugLayer(); //�f�o�b�N���C���[��L����

	//�f�o�C�X�̍쐬
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
