#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_

#include "../sunset_graphics_pch.h"

#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

#include <memory>
#include <vector>

struct ID3D12DescriptorHeap;
struct ID3D12Resource;

namespace DX12
{
	class RenderTargetView : 
		public DescriptorHeap
	{
	public:
		RenderTargetView();
		~RenderTargetView();

		void CleanUp();

		ID3D12Resource* GetResource(UINT index);

		static BOOL CreateForSwapChain(
			std::shared_ptr<RenderTargetView>& pRenderTarget,
			DXGI::DXGI_SwapChain* pSwapChain
		);

	protected:
		std::vector<ComPtr<ID3D12Resource>> m_rtvs;

	private:
		UINT bufferCount;

		friend class RenderTargets;
	};
}

#endif // !_RENDER_TARGET_H_
