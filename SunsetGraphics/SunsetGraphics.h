#ifndef _SUNSET_GRAPHICS
#define _SUNSET_GRAPHICS

#include "sunset_graphics_pch.h"

#include "Direct3D12/Command.h"
#include "Direct3D12/ShaderResourceView.h"

#include "DXGI/SwapChain.h"

#include "FrameBuffer.h"

SUNSET_GRAPHICS_API BOOL Sunset_Graphics_Init();	//�O���t�B�b�N�n�̏�����
SUNSET_GRAPHICS_API BOOL Sunset_Graphics_CleanUp();	//�O���t�B�b�N�n�̉��

#endif // !_SUNSET_GRAPHICS
