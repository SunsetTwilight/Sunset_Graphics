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

		void WaitStart() override;

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

		UINT64 m_fenceLastSignaledValue = 0;

		FrameContext m_frameContext[2];
		UINT m_frameIndex;

		FrameContext* currentFrameCtx;

		bool m_isWaiting = false;

		HANDLE m_fance_event;
	};

	Impl_Command::Impl_Command()
	{
		m_fance_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fance_event == nullptr)
			return;
	}

	Impl_Command::~Impl_Command()
	{
		CloseHandle(m_fance_event);
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
					_type,
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
		ID3D12CommandAllocator* alloc = m_frameContext[0].cmdAlloc.Get();

		HRESULT hr = alloc->Reset();
		hr = cmdList->Reset(alloc, nullptr);
	}

	void Impl_Command::Begin(FrameBuffer* pFrameBuffer)
	{
		currentFrameCtx = WaitForNextFrameResources(pFrameBuffer);
		ID3D12CommandAllocator* alloc = currentFrameCtx->cmdAlloc.Get();

		HRESULT hr = alloc->Reset();
		if (hr != S_OK) {

			MessageBox(NULL, L"", L"", NULL);
		}

		hr = cmdList->Reset(alloc, nullptr);
	}

	void Impl_Command::Close()
	{
		cmdList->Close();
	}

	void Impl_Command::WaitStart()
	{
		m_isWaiting = true;
	}

	void Impl_Command::Wait(FrameBuffer* pFrameBuffer)
	{
		//描画終了待ち
		UINT64 fenceValue = m_fenceLastSignaledValue + 1;
		currentFrameCtx = &m_frameContext[m_frameIndex % 2];
		cmdQueue->Signal(fence.Get(), fenceValue);
		m_fenceLastSignaledValue = fenceValue;
		currentFrameCtx->m_fenceValue = fenceValue;

		m_isWaiting = false;

		return;
	}

	void Impl_Command::WaitForLastSubmittedFrame()
	{
		if (m_isWaiting) {

			FrameContext* frameCtx = &m_frameContext[m_frameIndex % 2];
		
			UINT64 fenceValue = frameCtx->m_fenceValue;
			if (fenceValue == 0)
				return; // No fence was signaled

			frameCtx->m_fenceValue = 0;
			if (fence->GetCompletedValue() >= fenceValue)
				return;

			fence->SetEventOnCompletion(fenceValue, m_fance_event);
			WaitForSingleObject(m_fance_event, INFINITE);
			
			m_isWaiting = false;
		}
	}

	FrameContext* Impl_Command::WaitForNextFrameResources(FrameBuffer* pFrameBuffer)
	{
		UINT nextFrameIndex = m_frameIndex + 1;
		m_frameIndex = nextFrameIndex;

		HANDLE waitableObjects[] = { pFrameBuffer->GetSwapChainHandle(), nullptr };
		DWORD numWaitableObjects = 1;

		FrameContext* frameCtx = &m_frameContext[nextFrameIndex % 2];
		UINT64 fenceValue = frameCtx->m_fenceValue;
		if (fenceValue != 0) // means no fence was signaled
		{
			frameCtx->m_fenceValue = 0;
			HRESULT hr = fence->SetEventOnCompletion(fenceValue, m_fance_event);
			waitableObjects[1] = m_fance_event;
			numWaitableObjects = 2;
		}

		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, 1000);
		
		/*std::wstring message = L"fenceValue:" + std::to_wstring(fenceValue) + L'\n';
		message += L"nextFrameIndex:" + std::to_wstring(nextFrameIndex);
		MessageBox(NULL, message.c_str(), L"Variable Display", MB_OK);*/

		m_isWaiting = false;

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

