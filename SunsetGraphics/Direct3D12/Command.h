#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

class FrameBuffer;

namespace DX12
{
	struct FrameContext 
	{
		ComPtr<ID3D12CommandAllocator> cmdAlloc;
		UINT m_fenceValue = 0;
	};

	class  Command
	{
	public:
		SUNSET_GRAPHICS_CLASS Command();
		SUNSET_GRAPHICS_CLASS ~Command();

		SUNSET_GRAPHICS_CLASS void Create();

		SUNSET_GRAPHICS_CLASS void Begin(FrameBuffer* pFrameBuffer);

		SUNSET_GRAPHICS_CLASS void Close();

		SUNSET_GRAPHICS_CLASS void Wait(FrameBuffer* pFrameBuffer);

		SUNSET_GRAPHICS_CLASS void WaitForLastSubmittedFrame(FrameBuffer* pFrameBuffer);
		SUNSET_GRAPHICS_CLASS FrameContext* WaitForNextFrameResources(FrameBuffer* pFrameBuffer);

		SUNSET_GRAPHICS_CLASS ID3D12CommandQueue* GetCommandQueue();
		SUNSET_GRAPHICS_CLASS ID3D12GraphicsCommandList* GetCommandList();

	private:

		ComPtr<ID3D12CommandAllocator> cmdAlloc;
		
		ComPtr<ID3D12CommandQueue> cmdQueue;
		ComPtr<ID3D12GraphicsCommandList> cmdList;
		
		ComPtr<ID3D12Fence> fence;
		UINT m_fenceValue = 0;

		FrameContext m_frameContext[2];
		UINT m_frameIndex;

		FrameContext* currentFrameCtx;
	};
}

#endif // !_COMMAND_H_
