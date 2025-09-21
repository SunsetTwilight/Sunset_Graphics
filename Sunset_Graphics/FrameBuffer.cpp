#include "FrameBuffer.h"

#include "DXGI/SwapChain.h"
#include "Direct3D12/RenderTargets.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	class Impl_FrameBuffer :
		public FrameBuffer
	{
	public:
		Impl_FrameBuffer();
		virtual ~Impl_FrameBuffer() override;

		BOOL CreateFrameBuffer(
			HWND _hWnd,
			ID3D12CommandQueue* pCmdQueue
		);

		virtual BOOL PresentTest() override;

		virtual void Clear(ID3D12GraphicsCommandList* pCmdList) override;
		virtual BOOL Begin(ID3D12GraphicsCommandList* pCmdList) override;
		virtual void Wait(ID3D12CommandQueue* pCmdQueue) override;
		virtual BOOL End(
			ID3D12CommandQueue* pCmdQueue,
			ID3D12GraphicsCommandList* pCmdList
		) override;

		virtual BOOL ResizeView() override;

		virtual HANDLE GetSwapChainHandle() override;
		virtual void SetFenceValue(UINT64 value) override;

		virtual BOOL GetViewRect(D3D12_RECT* rc) override;

	private:
		std::shared_ptr<DXGI::SwapChain> pSwapChain;
		RenderTargetView* pFrameView;
		DepthStencilView* pDepthFrameView;

		HWND hWnd = NULL;

		UINT m_CurrentBackBufferIndex = 0;

		ComPtr<ID3D12Fence> m_pFence = nullptr; // フェンス
		HANDLE m_fenceEvent = nullptr; // フェンスで使うイベント
		UINT64 m_fenceValue[2]; // フェンスの値（ダブルバッファリング用に2個）

		bool isResizing;
	};

	Impl_FrameBuffer::Impl_FrameBuffer()
	{
	}

	Impl_FrameBuffer::~Impl_FrameBuffer()
	{
		delete(pFrameView);
		delete(pDepthFrameView);
	}

	BOOL Impl_FrameBuffer::CreateFrameBuffer(
		HWND _hWnd,
		ID3D12CommandQueue* pCmdQueue
	)
	{
		RenderTargetView* renderTargetView = nullptr;
		DepthStencilView* depthStencilView = nullptr;

		if (DXGI::SwapChain::CreateSwapChain(pSwapChain, pCmdQueue, _hWnd) == FALSE) { return FALSE; /* エラー */ }
		if (CreateRenderTargetView(&renderTargetView, pSwapChain->GetSwapChain()) == FALSE) { return FALSE; /* エラー */ }
		if (CreateDepthStencilView(&depthStencilView, pSwapChain->GetSwapChain()) == FALSE) { return FALSE; /* エラー */ }

		pFrameView = renderTargetView;
		pDepthFrameView = depthStencilView;

		hWnd = _hWnd;

		return TRUE;
	}

	BOOL Impl_FrameBuffer::PresentTest()
	{
		if (pSwapChain->GetStatusOccluded()) {
			return pSwapChain->PresentTest() == TRUE ? FALSE : TRUE;
		}
		else {
			return TRUE;
		}
	}

	BOOL Impl_FrameBuffer::Begin(ID3D12GraphicsCommandList* pCmdList)
	{
		if (isResizing == true) return FALSE;

		m_CurrentBackBufferIndex = pSwapChain->GetCurrentBackBufferIndex();

		D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH;

		// レンダーターゲットが使用可能になるまで待つ
		auto resBarrierForPR_RT = CD3DX12_RESOURCE_BARRIER::Transition(
			pFrameView->GetResource(m_CurrentBackBufferIndex),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		pCmdList->ResourceBarrier(1, &resBarrierForPR_RT);

		Clear(pCmdList);

		rtvH = pFrameView->GetCPUDescriptorHandle(m_CurrentBackBufferIndex);

		if (pDepthFrameView) {
			/* 深度バッファーあり */
			dsvH = pDepthFrameView->GetCPUDescriptorHandle();

			pCmdList->OMSetRenderTargets(
				1,
				&rtvH,	/* レンダーターゲット */
				true,
				&dsvH	/* 深度バッファー */
			);
		}
		else {
			/* 深度バッファーなし */
			pCmdList->OMSetRenderTargets(
				1,
				&rtvH,	/* レンダーターゲット */
				true,
				NULL	/* 深度バッファー */
			);
		}

		return TRUE;
	}

	void Impl_FrameBuffer::Wait(ID3D12CommandQueue* pCmdQueue)
	{
		//描画終了待ち
		const UINT64 fenceValue = m_fenceValue[m_CurrentBackBufferIndex];
		pCmdQueue->Signal(m_pFence.Get(), fenceValue);
		m_fenceValue[m_CurrentBackBufferIndex]++;

		// 次のフレームの描画準備がまだであれば待機する.
		//if (m_pFence->GetCompletedValue() < fenceValue)
		//{
		//	// 完了時にイベントを設定.
		//	auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent);
		//	if (FAILED(hr))
		//	{
		//		return;
		//	}

		//	// 待機処理.
		//	if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE))
		//	{
		//		return;
		//	}
		//}
	}

	BOOL Impl_FrameBuffer::End(
		ID3D12CommandQueue* pCmdQueue,
		ID3D12GraphicsCommandList* pCmdList
	)
	{
		// レンダーターゲットに書き込み終わるまで待つ
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pFrameView->GetResource(m_CurrentBackBufferIndex),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		pCmdList->ResourceBarrier(1, &barrier);

		// コマンドの記録を終了
		pCmdList->Close();

		// コマンドを実行
		ID3D12CommandList* ppCmdLists[] = { pCmdList };
		pCmdQueue->ExecuteCommandLists(1, ppCmdLists);

		// スワップチェーンを切り替え
		pSwapChain->Swap();

		// 描画完了を待つ
		//Wait(pCmdQueue);

		// バックバッファ番号更新
		return TRUE;
	}

	void Impl_FrameBuffer::Clear(ID3D12GraphicsCommandList* pCmdList)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
		rtvH = pFrameView->GetCPUDescriptorHandle(m_CurrentBackBufferIndex);

		const float clearColor[] = { 0.3f,0.3f,0.3f,1.0f }; // クリア⾊
		pCmdList->ClearRenderTargetView(
			rtvH,
			clearColor,
			0, nullptr
		);

		D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
		if (pDepthFrameView) {
			/* 深度バッファーあり */
			dsvH = pDepthFrameView->GetCPUDescriptorHandle();

			pCmdList->ClearDepthStencilView(
				dsvH, D3D12_CLEAR_FLAG_DEPTH,
				1.0f, 0,
				0, nullptr
			);
		}
	}

	BOOL Impl_FrameBuffer::ResizeView()
	{
		RECT rc;
		GetClientRect(hWnd, &rc);

		pFrameView->CleanUp();
		pDepthFrameView->CleanUp();

		if (pSwapChain->Resize(
			rc.right - rc.left,
			rc.bottom - rc.top
		) != TRUE) {
			/* 成功していない */
			isResizing = true;
			return FALSE;
		}

		RenderTargetView* renderTargetView = nullptr;
		DepthStencilView* depthStencilView = nullptr;

		if (CreateRenderTargetView(
			&renderTargetView,
			pSwapChain->GetSwapChain()
		) != TRUE) {
			/* 成功していない */
			return FALSE;
		}

		if (CreateDepthStencilView(
			&depthStencilView,
			pSwapChain->GetSwapChain()
		) != TRUE) {
			/* 成功していない */
			return FALSE;
		}

		pFrameView = renderTargetView;
		pDepthFrameView = depthStencilView;

		return TRUE;
	}

	HANDLE Impl_FrameBuffer::GetSwapChainHandle()
	{
		return pSwapChain->GetHandleSwapChainWaitableObject();
	}

	void Impl_FrameBuffer::SetFenceValue(UINT64 value)
	{

	}

	BOOL Impl_FrameBuffer::GetViewRect(D3D12_RECT* rc)
	{
		DXGI_SWAP_CHAIN_DESC1 desc;
		DXGI::DXGI_SwapChain* pSC = pSwapChain->GetSwapChain();

		if (pSC == nullptr) return FALSE;
		if (pSC->GetDesc1(&desc) != S_OK) return FALSE;

		rc->left = 0;
		rc->top = 0;
		rc->right = desc.Width;
		rc->bottom = desc.Height;

		return TRUE;
	}
}

BOOL CreateFrameBuffer(
	FrameBuffer** frameBuffer, 
	HWND _hWnd, 
	ID3D12CommandQueue* pCmdQueue
)
{
	DX12::Impl_FrameBuffer* pImpl_frameBuffer = new DX12::Impl_FrameBuffer;

	if (pImpl_frameBuffer->CreateFrameBuffer(_hWnd, pCmdQueue)) {

		(*frameBuffer) = pImpl_frameBuffer;
		return TRUE;
	}
	else {
		
		delete pImpl_frameBuffer;
		(*frameBuffer) = NULL;
		return FALSE;
	}

	return FALSE;
}
