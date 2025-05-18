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
}

FrameBuffer::FrameBuffer(
	HWND _hWnd,
	ID3D12CommandQueue* pCmdQueue
) : hWnd(_hWnd) 
{
	BOOL res = FALSE;

	res = DXGI::SwapChain::CreateSwapChain(
		pSwapChain,
		pCmdQueue,
		hWnd
	);	
	if(!res){
		/* エラー */

		return;
	}

	res = DX12::RenderTargetView::CreateForSwapChain(
		pFrameView,
		pSwapChain->GetSwapChain()
	);
	if (!res) {
		/* エラー */

		return;
	}

	res = DX12::DepthStencilView::CreateForSwapChain(
		pDepthFrameView,
		pSwapChain->GetSwapChain()
	);
	if (!res) {
		/* エラー */

		return;
	}
}

FrameBuffer::~FrameBuffer()
{
}

BOOL FrameBuffer::PresentTest()
{
	if (pSwapChain->GetStatusOccluded()) {
		return pSwapChain->PresentTest() == TRUE ? FALSE : TRUE;
	}
	else {
		return TRUE;
	}
}

BOOL FrameBuffer::Begin(ID3D12GraphicsCommandList* pCmdList)
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
}

void FrameBuffer::Wait(ID3D12CommandQueue* pCmdQueue)
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

BOOL FrameBuffer::End(
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

void FrameBuffer::Clear(ID3D12GraphicsCommandList* pCmdList)
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

BOOL FrameBuffer::ResizeView()
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	pFrameView->CleanUp();
	pDepthFrameView->CleanUp();
	
	if (pSwapChain->Resize(
			rc.right - rc.left,
			rc.bottom - rc.top
	) != TRUE){
		/* 成功していない */
		isResizing = true;
		return FALSE;
	}

	if (DX12::RenderTargetView::CreateForSwapChain(
		pFrameView,
		pSwapChain->GetSwapChain()
	) != TRUE) {
		/* 成功していない */
		return FALSE;
	}

	if(DX12::DepthStencilView::CreateForSwapChain(
		pDepthFrameView,
		pSwapChain->GetSwapChain()
	) != TRUE) {
		/* 成功していない */
		return FALSE;
	}

	return TRUE;
}

HANDLE FrameBuffer::GetSwapChainHandle()
{
	return pSwapChain->GetHandleSwapChainWaitableObject();
}

void FrameBuffer::SetFenceValue(UINT64 value)
{
	
}

BOOL FrameBuffer::GetViewRect(D3D12_RECT* rc)
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
