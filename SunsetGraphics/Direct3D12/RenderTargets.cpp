#include "RenderTargets.h"

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

	RenderTargets::RenderTargets()
	{
	}

	RenderTargets::~RenderTargets()
	{
	}

	void RenderTargets::OMSet(
		ID3D12GraphicsCommandList* pCommandList,
		RenderTargetView* pRTV,
		DepthStencilView* pDSV
	)
	{
		//D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
		//D3D12_CPU_DESCRIPTOR_HANDLE dsvH;

		//rtvH = pRTV->m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//rtvH.ptr += pRTV->bbIndex * g_d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		//if (pDSV != nullptr) {
		//	dsvH = pDSV->m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		//}

		//pCommandList->OMSetRenderTargets(
		//	1,
		//	&rtvH,	/* レンダーターゲット */
		//	true,
		//	((pDSV != nullptr) ? &dsvH : nullptr)	/* 深度バッファー */
		//);
	}
}