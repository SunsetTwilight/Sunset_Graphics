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

	class Impl_DescriptorHeap :
		public DescriptorHeap
	{
	public:
		Impl_DescriptorHeap();
		Impl_DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);
		~Impl_DescriptorHeap();

		ID3D12DescriptorHeap* GetDescriptorHeap() override;
		ID3D12DescriptorHeap** GetAddressDescriptorHeap() override;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT bbIndex = 0) override;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT bbIndex = 0) override;

		UINT GetDescriptorHandleIncrementSize() override;

		BOOL CreateDescriptorHeap(
			UINT numDescriptor = 1,
			D3D12_DESCRIPTOR_HEAP_FLAGS flagDescriptor = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
		);

		ComPtr<ID3D12DescriptorHeap> m_heap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
		UINT m_numDescriptor;
	};

	Impl_DescriptorHeap::Impl_DescriptorHeap()
		: m_heapType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), m_numDescriptor(1)
	{
	}

	Impl_DescriptorHeap::Impl_DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
		: m_heapType(type), m_numDescriptor(1)
	{
	}

	Impl_DescriptorHeap::~Impl_DescriptorHeap()
	{
	}

	ID3D12DescriptorHeap* Impl_DescriptorHeap::GetDescriptorHeap()
	{
		return m_heap.Get();
	}

	ID3D12DescriptorHeap** Impl_DescriptorHeap::GetAddressDescriptorHeap()
	{
		return m_heap.GetAddressOf();
	}

	BOOL Impl_DescriptorHeap::CreateDescriptorHeap(
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

		HRESULT hr = g_d3d12_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_heap.GetAddressOf()));
		if (hr != S_OK) {

			return FALSE;
		}

		return TRUE;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Impl_DescriptorHeap::GetCPUDescriptorHandle(UINT bbIndex)
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_heap->GetCPUDescriptorHandleForHeapStart(),
			bbIndex,
			g_d3d12_device->GetDescriptorHandleIncrementSize(m_heapType)
		);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE Impl_DescriptorHeap::GetGPUDescriptorHandle(UINT bbIndex)
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(
			m_heap->GetGPUDescriptorHandleForHeapStart(),
			bbIndex,
			g_d3d12_device->GetDescriptorHandleIncrementSize(m_heapType)
		);
	}

	UINT Impl_DescriptorHeap::GetDescriptorHandleIncrementSize()
	{
		return g_d3d12_device->GetDescriptorHandleIncrementSize(m_heapType);
	}
}

BOOL CreateDescriptorHeap(
	DescriptorHeap** ppDescriptorHeap,
	D3D12_DESCRIPTOR_HEAP_TYPE typeDescriptor, 
	UINT numDescriptor, 
	D3D12_DESCRIPTOR_HEAP_FLAGS flagDescriptor
)
{
	DX12::Impl_DescriptorHeap* impl = new DX12::Impl_DescriptorHeap(typeDescriptor);

	if (impl->CreateDescriptorHeap(numDescriptor, flagDescriptor) == FALSE) {

		return FALSE;
	}

	(*ppDescriptorHeap) = impl;

	return TRUE;
}
