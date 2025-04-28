#include "DescriptorHeap.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
		: m_heapType(type), m_numDescriptor(1)
	{

	}

	DescriptorHeap::~DescriptorHeap()
	{

	}

	BOOL DescriptorHeap::CreateDescriptorHeap(
		UINT numDescriptor,
		D3D12_DESCRIPTOR_HEAP_FLAGS flagDescriptor
	)
	{
		m_numDescriptor = numDescriptor;

		D3D12_DESCRIPTOR_HEAP_DESC desc{
			m_heapType,
			numDescriptor,
			flagDescriptor,
			NULL /* マルチアダプターで使用 */
		};

		ThrowIfFailed(g_d3d12_device->CreateDescriptorHeap(
			&desc,
			IID_PPV_ARGS(m_heap.GetAddressOf()))
		);

		return TRUE;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(UINT bbIndex)
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_heap->GetCPUDescriptorHandleForHeapStart(),
			bbIndex,
			g_d3d12_device->GetDescriptorHandleIncrementSize(m_heapType)
		);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(UINT bbIndex)
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(
			m_heap->GetGPUDescriptorHandleForHeapStart(),
			bbIndex,
			g_d3d12_device->GetDescriptorHandleIncrementSize(m_heapType)
		);
	}
}