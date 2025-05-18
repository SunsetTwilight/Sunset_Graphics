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

namespace DX12
{
	class RenderTargetView;
	class DepthStencilView;
}

namespace DXGI
{
	class SwapChain;
}

class FrameBuffer
{
public:
	SUNSET_GRAPHICS_CLASS FrameBuffer(
		HWND _hWnd,
		ID3D12CommandQueue* pCmdQueue
	);

	SUNSET_GRAPHICS_CLASS ~FrameBuffer();

	SUNSET_GRAPHICS_CLASS BOOL PresentTest();

	SUNSET_GRAPHICS_CLASS BOOL Begin(ID3D12GraphicsCommandList* pCmdList);
	SUNSET_GRAPHICS_CLASS void Wait(ID3D12CommandQueue* pCmdQueue);
	SUNSET_GRAPHICS_CLASS BOOL End(
		ID3D12CommandQueue* pCmdQueue,
		ID3D12GraphicsCommandList* pCmdList
	);

	SUNSET_GRAPHICS_CLASS void Clear(ID3D12GraphicsCommandList* pCmdList);

	SUNSET_GRAPHICS_CLASS BOOL ResizeView();

	SUNSET_GRAPHICS_CLASS HANDLE GetSwapChainHandle();
	SUNSET_GRAPHICS_CLASS void SetFenceValue(UINT64 value);

	SUNSET_GRAPHICS_CLASS BOOL GetViewRect(D3D12_RECT* rc);

private:
	HWND hWnd;
	std::shared_ptr<DXGI::SwapChain> pSwapChain;
	std::shared_ptr<DX12::RenderTargetView> pFrameView;
	std::shared_ptr<DX12::DepthStencilView> pDepthFrameView;

	UINT m_CurrentBackBufferIndex = 0;

	ComPtr<ID3D12Fence> m_pFence = nullptr; // フェンス
	HANDLE m_fenceEvent = nullptr; // フェンスで使うイベント
	UINT64 m_fenceValue[2]; // フェンスの値（ダブルバッファリング用に2個）

	bool isResizing;
};

#endif // !_FRAME_VIEW_H_
