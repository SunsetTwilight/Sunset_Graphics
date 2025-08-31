#include "ShaderResourceView.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	class Impl_ShaderResourceView :
		public ShaderResourceView
	{
	public:
		Impl_ShaderResourceView();
		~Impl_ShaderResourceView();

		Impl_ShaderResourceView(UINT num);

		ID3D12DescriptorHeap* GetDescriptorHeap() override;
		ID3D12DescriptorHeap** GetAddressDescriptorHeap() override;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT bbIndex = 0) override;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT bbIndex = 0) override;

		UINT GetDescriptorHandleIncrementSize() override;

	private:
		DescriptorHeap* m_pHeap;
	};

	Impl_ShaderResourceView::Impl_ShaderResourceView()
	{
		if (CreateDescriptorHeap(
			(DescriptorHeap**)(&m_pHeap),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		) != S_OK) {
			return;
		}
	}

	Impl_ShaderResourceView::~Impl_ShaderResourceView()
	{
		if (m_pHeap) {
			delete(m_pHeap);
			m_pHeap = nullptr;
		}
	}

	Impl_ShaderResourceView::Impl_ShaderResourceView(UINT num)
	{
		if (CreateDescriptorHeap(
			(DescriptorHeap**)(&m_pHeap),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			num,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		) != S_OK) {
			return;
		}
	}

	ID3D12DescriptorHeap* Impl_ShaderResourceView::GetDescriptorHeap()
	{
		return m_pHeap->GetDescriptorHeap();
	}

	ID3D12DescriptorHeap** Impl_ShaderResourceView::GetAddressDescriptorHeap()
	{
		return m_pHeap->GetAddressDescriptorHeap();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Impl_ShaderResourceView::GetCPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetCPUDescriptorHandle(bbIndex);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE Impl_ShaderResourceView::GetGPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetGPUDescriptorHandle(bbIndex);
	}

	UINT Impl_ShaderResourceView::GetDescriptorHandleIncrementSize()
	{
		return m_pHeap->GetDescriptorHandleIncrementSize();
	}
}

BOOL CreateShaderResourceView(
	ShaderResourceView** ppShaderResourceView, 
	UINT num
)
{
	DX12::Impl_ShaderResourceView* pImplShaderResourceView = new DX12::Impl_ShaderResourceView(num);

	(*ppShaderResourceView) = pImplShaderResourceView;
	return TRUE;
}