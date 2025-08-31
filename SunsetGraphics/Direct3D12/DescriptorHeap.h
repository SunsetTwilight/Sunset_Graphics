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

class SUNSET_GRAPHICS_CLASS DescriptorHeap
{
public:
	DescriptorHeap() {}
	virtual ~DescriptorHeap() {}

	virtual ID3D12DescriptorHeap* GetDescriptorHeap() = 0;
	virtual ID3D12DescriptorHeap** GetAddressDescriptorHeap() = 0;

	virtual CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT bbIndex = 0) = 0;
	virtual CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT bbIndex = 0) = 0;

	virtual UINT GetDescriptorHandleIncrementSize() = 0;
};

typedef BOOL(*PfnCreateDescriptor)(
	DescriptorHeap** ppDescriptorHeap,
	D3D12_DESCRIPTOR_HEAP_TYPE typeDescriptor,
	UINT numDescriptor,
	D3D12_DESCRIPTOR_HEAP_FLAGS flagDescriptor
);
SUNSET_GRAPHICS_CLASS BOOL CreateDescriptorHeap(
	DescriptorHeap** ppDescriptorHeap,
	D3D12_DESCRIPTOR_HEAP_TYPE typeDescriptor, 
	UINT numDescriptor = 1,
	D3D12_DESCRIPTOR_HEAP_FLAGS flagDescriptor = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
);

#endif // !_DESCRIPTOR_HEAP_H_
