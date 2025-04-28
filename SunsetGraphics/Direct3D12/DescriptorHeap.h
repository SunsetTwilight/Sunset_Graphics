#ifndef _DESCRIPTOR_HEAP_H_
#define _DESCRIPTOR_HEAP_H_

#include "../sunset_graphics_pch.h"

#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "d3dx12.h"

#include <memory>
#include <vector>

enum D3D12_DESCRIPTOR_HEAP_TYPE;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_DESCRIPTOR_HANDLE;
struct ID3D12DescriptorHeap;

namespace DX12
{
	class DescriptorHeap
	{
	public:
		SUNSET_GRAPHICS_CLASS DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);
		SUNSET_GRAPHICS_CLASS ~DescriptorHeap();

		SUNSET_GRAPHICS_CLASS BOOL CreateDescriptorHeap(
			UINT numDescriptor = 1,
			D3D12_DESCRIPTOR_HEAP_FLAGS flagDescriptor = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
		);

		SUNSET_GRAPHICS_CLASS CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT bbIndex = 0);
		SUNSET_GRAPHICS_CLASS CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT bbIndex = 0);

	protected:
		ComPtr<ID3D12DescriptorHeap> m_heap;

	private:
		D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
		UINT m_numDescriptor;
	};
}

#endif // !_DESCRIPTOR_HEAP_H_
