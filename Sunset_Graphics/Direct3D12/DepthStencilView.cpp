#include "DepthStencilView.h"

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	class Impl_DepthStencilView :
		public DepthStencilView
	{
	public:
		Impl_DepthStencilView();
		~Impl_DepthStencilView();

		Impl_DepthStencilView(UINT num);

		ID3D12DescriptorHeap* GetDescriptorHeap() override;
		ID3D12DescriptorHeap** GetAddressDescriptorHeap() override;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT bbIndex = 0) override;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT bbIndex = 0) override;

		UINT GetDescriptorHandleIncrementSize() override;

		void CleanUp() override;

		DescriptorHeap* m_pHeap;
		ComPtr<ID3D12Resource> m_dsv;
	};

	Impl_DepthStencilView::Impl_DepthStencilView()
	{
	}

	Impl_DepthStencilView::~Impl_DepthStencilView()
	{
		CleanUp();
	}

	Impl_DepthStencilView::Impl_DepthStencilView(UINT num)
	{
	}

	ID3D12DescriptorHeap* Impl_DepthStencilView::GetDescriptorHeap()
	{
		return m_pHeap->GetDescriptorHeap();
	}

	ID3D12DescriptorHeap** Impl_DepthStencilView::GetAddressDescriptorHeap()
	{
		return m_pHeap->GetAddressDescriptorHeap();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Impl_DepthStencilView::GetCPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetCPUDescriptorHandle(bbIndex);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE Impl_DepthStencilView::GetGPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetGPUDescriptorHandle(bbIndex);
	}

	UINT Impl_DepthStencilView::GetDescriptorHandleIncrementSize()
	{
		return m_pHeap->GetDescriptorHandleIncrementSize();
	}

	void Impl_DepthStencilView::CleanUp()
	{
		if (m_pHeap){
			delete(m_pHeap);
			m_pHeap = nullptr;
		}

		m_dsv.Reset();
	}
}

BOOL CreateDepthStencilView(
	DepthStencilView** ppDepthStencilView, 
	DXGI::DXGI_SwapChain* pSwapChain
)
{
	ID3D12Device* pDevice = DX12::g_d3d12_device.Get();

	DX12::Impl_DepthStencilView* pImpl = new DX12::Impl_DepthStencilView();

	if (CreateDescriptorHeap(
		(DescriptorHeap**)(&pImpl->m_pHeap),
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		1,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE) == FALSE) {

		/* Failed Create DescriptorHeap */
		return FALSE;
	}

	ComPtr<ID3D12Resource> dsv;
	{
		DXGI_SWAP_CHAIN_DESC1 scdesc;
		pSwapChain->GetDesc1(&scdesc);

		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			scdesc.Width, scdesc.Height
		);

		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = desc.Format;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		HRESULT hr = pDevice->CreateCommittedResource(
			&properties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(dsv.GetAddressOf())
		);
	}
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc;

		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_DSV_FLAG_NONE;
		desc.Texture2D.MipSlice = 0;

		pDevice->CreateDepthStencilView(
			dsv.Get(),
			&desc,
			pImpl->GetCPUDescriptorHandle()
		);
	}

	pImpl->m_dsv = dsv;

	(*ppDepthStencilView) = pImpl;

	return TRUE;
}

/*BOOL CreateDepthStencilView(
	DepthStencilView** ppDepthStencilView,
	DXGI::DXGI_SwapChain* pSwapChain
)
{
	ID3D12Device* pDevice = g_d3d12_device.Get();

	Impl_DepthStencilView* pImpl = new Impl_DepthStencilView();

	ComPtr<ID3D12Resource> dsv;
	{
		DXGI_SWAP_CHAIN_DESC1 scdesc;
		pSwapChain->GetDesc1(&scdesc);

		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			scdesc.Width, scdesc.Height
		);

		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = desc.Format;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		HRESULT hr = pDevice->CreateCommittedResource(
			&properties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(dsv.GetAddressOf())
		);
	}
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc;

		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_DSV_FLAG_NONE;
		desc.Texture2D.MipSlice = 0;

		pDevice->CreateDepthStencilView(
			dsv.Get(),
			&desc,
			pImpl->GetCPUDescriptorHandle()
		);
	}

	return TRUE;
}*/
