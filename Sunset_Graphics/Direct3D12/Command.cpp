#include "Command.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include "../FrameBuffer.h"

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	class Impl_Command :
		public Command
	{
	public:
		Impl_Command();
		~Impl_Command();

		BOOL Create(D3D12_COMMAND_LIST_TYPE _type);

		void Begin() override;
		void Begin(FrameBuffer* pFrameBuffer) override;
		void Close() override;

		void Wait(FrameBuffer* pFrameBuffer) override;

		void WaitForLastSubmittedFrame() override;
		FrameContext* WaitForNextFrameResources(FrameBuffer* pFrameBuffer) override;

		ID3D12CommandQueue* GetCommandQueue() override;
		ID3D12GraphicsCommandList* GetCommandList() override;

		ComPtr<ID3D12CommandAllocator> cmdAlloc;

		ComPtr<ID3D12CommandQueue> cmdQueue;
		ComPtr<ID3D12GraphicsCommandList> cmdList;

		ComPtr<ID3D12Fence> fence;
		UINT m_fenceValue = 0;

		FrameContext m_frameContext[2];
		UINT m_frameIndex;

		FrameContext* currentFrameCtx;

	};

	Impl_Command::Impl_Command()
	{
	}

	Impl_Command::~Impl_Command()
	{
	}

	BOOL Impl_Command::Create(D3D12_COMMAND_LIST_TYPE _type)
	{
		HRESULT hr = S_OK;;

		{
			D3D12_COMMAND_QUEUE_DESC desc;

			desc.Type = _type;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 1;
			
			hr = g_d3d12_device->CreateCommandQueue(
				&desc,
				IID_PPV_ARGS(cmdQueue.GetAddressOf())
			);
		}

		{
			hr = g_d3d12_device->CreateCommandAllocator(
				_type,
				IID_PPV_ARGS(cmdAlloc.GetAddressOf())
			);

			for (UINT i = 0; i < 2; i++)
				hr = g_d3d12_device->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT, 
					IID_PPV_ARGS(m_frameContext[i].cmdAlloc.GetAddressOf())
				);
		}

		{
			hr = g_d3d12_device->CreateCommandList(
				0,
				_type,
				m_frameContext[0].cmdAlloc.Get(),
				nullptr,
				IID_PPV_ARGS(cmdList.GetAddressOf())
			);

			/* コマンドリストを一旦クローズしておく */
			cmdList->Close();
		}

		hr = g_d3d12_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		
		return TRUE;
	}

	void Impl_Command::Begin()
	{
		ID3D12CommandAllocator* alloc = currentFrameCtx->cmdAlloc.Get();

		HRESULT hr = alloc->Reset();
		hr = cmdList->Reset(alloc, nullptr);
	}

	void Impl_Command::Begin(FrameBuffer* pFrameBuffer)
	{
		currentFrameCtx = WaitForNextFrameResources(pFrameBuffer);
		ID3D12CommandAllocator* alloc = currentFrameCtx->cmdAlloc.Get();

		HRESULT hr = alloc->Reset();
		hr = cmdList->Reset(alloc, nullptr);
	}

	void Impl_Command::Close()
	{
		cmdList->Close();
	}

	UINT64 g_fenceLastSignaledValue = 0;

	void Impl_Command::Wait(FrameBuffer* pFrameBuffer)
	{
		//描画終了待ち
		g_fenceLastSignaledValue++;
		currentFrameCtx = &m_frameContext[m_frameIndex % 2];
		cmdQueue->Signal(fence.Get(), currentFrameCtx->m_fenceValue);
		currentFrameCtx->m_fenceValue = g_fenceLastSignaledValue;

		// 次のフレームの描画準備がまだであれば待機する.
		//if (fence->GetCompletedValue() < m_fenceValue)
		//{
		//	auto event = CreateEvent(nullptr, false, false, nullptr);
		//	// 完了時にイベントを設定.
		//	auto hr = fence->SetEventOnCompletion(m_fenceValue, event);
		//	if (FAILED(hr))
		//	{
		//		return;
		//	}
		//	// 待機処理.
		//	if (WAIT_OBJECT_0 != WaitForSingleObject(event, INFINITE))
		//	{
		//		return;
		//	}
		//	CloseHandle(event);
		//}

		return;
	}

	void Impl_Command::WaitForLastSubmittedFrame()
	{
		FrameContext* frameCtx = &m_frameContext[m_frameIndex % 2];

		UINT64 fenceValue = frameCtx->m_fenceValue;
		if (fenceValue == 0) 
			return; // No fence was signaled

		frameCtx->m_fenceValue = 0;
		if (fence->GetCompletedValue() >= fenceValue)
			return;

		auto event = CreateEvent(nullptr, false, false, nullptr);

		fence->SetEventOnCompletion(m_fenceValue, event);
		// 待機処理.
		if (WAIT_OBJECT_0 != WaitForSingleObject(event, INFINITE))
		{
			return;
		}
		CloseHandle(event);
	}

	FrameContext* Impl_Command::WaitForNextFrameResources(FrameBuffer* pFrameBuffer)
	{
		UINT nextFrameIndex = m_frameIndex + 1;
		m_frameIndex = nextFrameIndex;

		HANDLE waitableObjects[] = { pFrameBuffer->GetSwapChainHandle(), nullptr};
		DWORD numWaitableObjects = 1;

		HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		FrameContext* frameCtx = &m_frameContext[nextFrameIndex % 2];
		UINT64 fenceValue = frameCtx->m_fenceValue;
		if (fenceValue != 0) // means no fence was signaled
		{
			frameCtx->m_fenceValue = 0;

			fence->SetEventOnCompletion(fenceValue, event);
			waitableObjects[1] = event;
			numWaitableObjects = 2;
		}

		WaitForSingleObject(waitableObjects[0], INFINITE);

		CloseHandle(event);

		return frameCtx;
	}

	ID3D12CommandQueue* Impl_Command::GetCommandQueue()
	{
		return cmdQueue.Get();
	}

	ID3D12GraphicsCommandList* Impl_Command::GetCommandList()
	{
		return cmdList.Get();
	}
}

BOOL CreateCommand(Command** ppCommand)
{
	DX12::Impl_Command* pImplCommand = new DX12::Impl_Command;

	if (pImplCommand->Create(D3D12_COMMAND_LIST_TYPE_DIRECT)) {

		(*ppCommand) = pImplCommand;
		return TRUE;
	}
	else {

		delete pImplCommand;
		ppCommand = nullptr;
		return FALSE;
	}

	return FALSE;
}

BOOL CreateCommand_TypeCopy(Command** ppCommand)
{
	DX12::Impl_Command* pImplCommand = new DX12::Impl_Command;

	if (pImplCommand->Create(D3D12_COMMAND_LIST_TYPE_COPY)) {

		(*ppCommand) = pImplCommand;
		return TRUE;
	}
	else {

		delete pImplCommand;
		ppCommand = nullptr;
		return FALSE;
	}

	return FALSE;
}

