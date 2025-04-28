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

	DepthStencilView::DepthStencilView() : 
		DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
	{

	}

	DepthStencilView::~DepthStencilView()
	{

	}

	void DepthStencilView::CleanUp()
	{
		m_heap.Reset();
		m_dsv.Reset();
	}

	BOOL DepthStencilView::CreateForSwapChain(
		std::shared_ptr<DepthStencilView>& pDSV,
		DXGI::DXGI_SwapChain* pSwapChain
	)
	{
		ID3D12Device* pDevice = g_d3d12_device.Get();

		pDSV = std::make_shared<DepthStencilView>();

		pDSV->CreateDescriptorHeap();

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
				pDSV->GetCPUDescriptorHandle()
			);
		}

		pDSV->m_dsv = dsv;

		return TRUE;
	}
}