#ifndef _RENDER_TARGETS_H_
#define _RENDER_TARGETS_H_

#include "../sunset_graphics_pch.h"

#include "Direct3D_12_def.h"

#include "RenderTargetView.h"
#include "DepthStencilView.h"

#include <memory>
#include <vector>

struct ID3D12GraphicsCommandList;

namespace DX12
{
	class RenderTargetView;
	class DepthStencilView;

	class RenderTargets
	{
	public:
		RenderTargets();
		~RenderTargets();

		static void OMSet(
			ID3D12GraphicsCommandList* pCommandList,
			RenderTargetView* pRTV,
			DepthStencilView* pDSV = nullptr
		);

	private:
		
	};
}

#endif // !_RENDER_TARGETS_H_
