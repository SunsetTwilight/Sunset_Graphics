#ifndef _SUNSET_GRAPHICS
#define _SUNSET_GRAPHICS

#include "sunset_graphics_pch.h"

#include "Direct3D12/Command.h"
#include "Direct3D12/ShaderResourceView.h"

#include "DXGI/SwapChain.h"

#include "FrameBuffer.h"

SUNSET_GRAPHICS_API BOOL Sunset_Graphics_Init();	//グラフィック系の初期化
SUNSET_GRAPHICS_API BOOL Sunset_Graphics_CleanUp();	//グラフィック系の解放

#endif // !_SUNSET_GRAPHICS
