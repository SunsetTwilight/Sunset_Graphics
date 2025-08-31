#ifndef _FRAME_VIEW_H_
#define _FRAME_VIEW_H_

#include "sunset_graphics_pch.h"
#include "Direct3D12/Direct3D_12_def.h"
#include "DXGI/DirectX_GI_def.h"

#include <d3d12.h>

#include <memory>

struct ID3D12CommandQueue;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

class RenderTargetView;
class DepthStencilView;

namespace DXGI
{
	class SwapChain;
}

class SUNSET_GRAPHICS_CLASS FrameBuffer
{
public:
	FrameBuffer() {}
	virtual ~FrameBuffer() {};

	virtual BOOL PresentTest() = 0;

	virtual void Clear(ID3D12GraphicsCommandList* pCmdList) = 0;
	virtual BOOL Begin(ID3D12GraphicsCommandList* pCmdList) = 0;
	virtual void Wait(ID3D12CommandQueue* pCmdQueue) = 0;
	virtual BOOL End(
		ID3D12CommandQueue* pCmdQueue,
		ID3D12GraphicsCommandList* pCmdList
	) = 0;

	virtual BOOL ResizeView() = 0;

	virtual HANDLE GetSwapChainHandle() = 0;
	virtual void SetFenceValue(UINT64 value) = 0;

	virtual BOOL GetViewRect(D3D12_RECT* rc) = 0;

};

typedef BOOL(*PfnCreateFrameBuffer)(
	FrameBuffer** frameBuffer,
	HWND _hWnd,
	ID3D12CommandQueue* pCmdQueue
);
SUNSET_GRAPHICS_API BOOL CreateFrameBuffer(
	FrameBuffer** frameBuffer,
	HWND _hWnd,
	ID3D12CommandQueue* pCmdQueue
);

#endif // !_FRAME_VIEW_H_
