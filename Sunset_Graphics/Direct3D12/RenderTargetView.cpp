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

	class Impl_RenderTargetView :
		public RenderTargetView
	{
	public:
		Impl_RenderTargetView();
		~Impl_RenderTargetView();

		ID3D12DescriptorHeap* GetDescriptorHeap() override;
		ID3D12DescriptorHeap** GetAddressDescriptorHeap() override;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT bbIndex = 0) override;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT bbIndex = 0) override;

		UINT GetDescriptorHandleIncrementSize() override;

		void CleanUp() override;
		ID3D12Resource* GetResource(UINT index) override;

		DescriptorHeap* m_pHeap;
		std::vector<ComPtr<ID3D12Resource>> m_rtvs;
		UINT bufferCount;
	};

	Impl_RenderTargetView::Impl_RenderTargetView()
	{	
	}

	Impl_RenderTargetView::~Impl_RenderTargetView()
	{
		CleanUp();
	}

	ID3D12DescriptorHeap* Impl_RenderTargetView::GetDescriptorHeap()
	{
		return m_pHeap->GetDescriptorHeap();
	}

	ID3D12DescriptorHeap** Impl_RenderTargetView::GetAddressDescriptorHeap()
	{
		return m_pHeap->GetAddressDescriptorHeap();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Impl_RenderTargetView::GetCPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetCPUDescriptorHandle(bbIndex);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE Impl_RenderTargetView::GetGPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetGPUDescriptorHandle(bbIndex);
	}

	UINT Impl_RenderTargetView::GetDescriptorHandleIncrementSize()
	{
		return m_pHeap->GetDescriptorHandleIncrementSize();
	}

	void Impl_RenderTargetView::CleanUp()
	{
		if (m_pHeap) {
			delete(m_pHeap);
			m_pHeap = nullptr;
		}
		m_rtvs.clear();
	}

	ID3D12Resource* Impl_RenderTargetView::GetResource(UINT index)
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
}

BOOL CreateRenderTargetView(
	RenderTargetView** ppRenderTargetView, 
	DXGI::DXGI_SwapChain* pSwapChain
)
{
	HRESULT hr = S_OK;
	ID3D12Device* pDevice = DX12::g_d3d12_device.Get();

	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		hr = pSwapChain->GetDesc1(&sd);
		if (hr != S_OK) {
			/* Ž¸”s */
			return FALSE;
		}
	}
		
	DX12::Impl_RenderTargetView* pImpl = new DX12::Impl_RenderTargetView();

	if (CreateDescriptorHeap(
		(DescriptorHeap**)(&pImpl->m_pHeap),
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		sd.BufferCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE) == FALSE) {

		/* Failed Create DescriptorHeap */
		return FALSE;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = pImpl->GetCPUDescriptorHandle();
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

	pImpl->m_rtvs = rtvs;
	pImpl->bufferCount = sd.BufferCount;

	(*ppRenderTargetView) = pImpl;

	return TRUE;
}
