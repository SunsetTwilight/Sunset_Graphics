#ifndef _DEPTH_STENCIL_VIEW_H_
#define _DEPTH_STENCIL_VIEW_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

#include <memory>

struct ID3D12DescriptorHeap;
struct ID3D12Resource;

namespace DX12
{
	class DepthStencilView :
		public DescriptorHeap
	{
	public:
		DepthStencilView();
		~DepthStencilView();

		void CleanUp();

		static BOOL CreateForSwapChain(
			std::shared_ptr<DepthStencilView>& pDSV,
			DXGI::DXGI_SwapChain* pSwapChain
		);

	protected:
		ComPtr<ID3D12Resource> m_dsv;

		friend class RenderTargets;
	};
}

#endif // !_DEPTH_STENCIL_VIEW_H_
