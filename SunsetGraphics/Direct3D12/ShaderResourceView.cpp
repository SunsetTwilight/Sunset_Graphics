#include "ShaderResourceView.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	ShaderResourceView::ShaderResourceView() :
		DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	{
		if (CreateDescriptorHeap(
			1,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		) != S_OK) {
			return;
		}
	}
	ShaderResourceView::~ShaderResourceView()
	{

	}

	ShaderResourceView::ShaderResourceView(UINT num) :
		DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	{
		if (CreateDescriptorHeap(
			num,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		) != S_OK) {
			return;
		}


	}



}