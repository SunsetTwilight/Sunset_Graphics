#include "RenderTargetView.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	RenderTargetView::RenderTargetView() : 
		DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	{

	}

	RenderTargetView::~RenderTargetView()
	{
		CleanUp();
	}

	void RenderTargetView::CleanUp()
	{
		m_heap.Reset();
		m_rtvs.clear();
	}

	ID3D12Resource* RenderTargetView::GetResource(UINT index)
	{
		if (index < bufferCount) {
			try {
				return m_rtvs.at(index).Get();
			}
			catch (...) {
				/* —áŠO */
				return nullptr;
			}
		}

		else {
			return nullptr;
		}
	}

	BOOL RenderTargetView::CreateForSwapChain(
		std::shared_ptr<RenderTargetView>& pRenderTarget,
		DXGI::DXGI_SwapChain* pSwapChain
	)
	{
		HRESULT hr = S_OK;
		ID3D12Device* pDevice = g_d3d12_device.Get();

		pRenderTarget = std::make_shared<RenderTargetView>();
		
		DXGI_SWAP_CHAIN_DESC1 sd;
		{
			hr = pSwapChain->GetDesc1(&sd);
			if (hr != S_OK) {
				/* Ž¸”s */
				pRenderTarget.reset();
				return FALSE;
			}
		}

		if (!pRenderTarget->CreateDescriptorHeap(sd.BufferCount)) {
			/* Ž¸”s */
			pRenderTarget.reset();
			return FALSE;
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = pRenderTarget->GetCPUDescriptorHandle();
		UINT rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		std::vector<ComPtr<ID3D12Resource>> rtvs;
		rtvs.resize(sd.BufferCount);
		for (size_t i = 0; i < sd.BufferCount; i++) {

			ThrowIfFailed(pSwapChain->GetBuffer(
				(UINT)i, 
				IID_PPV_ARGS(&rtvs[i]))
			);

			pDevice->CreateRenderTargetView(
				rtvs[i].Get(),
				nullptr,
				handle
			);

			handle.Offset(rtvDescriptorSize);
		}

		pRenderTarget->m_rtvs = rtvs;
		pRenderTarget->bufferCount = sd.BufferCount;

		return TRUE;
	}
}