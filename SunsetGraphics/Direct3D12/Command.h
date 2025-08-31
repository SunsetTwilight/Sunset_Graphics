#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

class FrameBuffer;

struct FrameContext 
{
	ComPtr<ID3D12CommandAllocator> cmdAlloc;
	UINT m_fenceValue = 0;
};

class SUNSET_GRAPHICS_CLASS Command
{
public:
	Command() {}
	virtual ~Command() {}

	virtual void Begin(FrameBuffer* pFrameBuffer) = 0;
		
	virtual void Close() = 0;

	virtual void Wait(FrameBuffer* pFrameBuffer) = 0;

	virtual void WaitForLastSubmittedFrame(FrameBuffer* pFrameBuffer) = 0;
	virtual FrameContext* WaitForNextFrameResources(FrameBuffer* pFrameBuffer) = 0;

	virtual ID3D12CommandQueue* GetCommandQueue() = 0;
	virtual ID3D12GraphicsCommandList* GetCommandList() = 0;
};

typedef BOOL(*PfnCreateCommand)(Command** ppCommand);
SUNSET_GRAPHICS_API BOOL CreateCommand(Command** ppCommand);

#endif // !_COMMAND_H_
