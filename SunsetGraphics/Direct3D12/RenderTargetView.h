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

class SUNSET_GRAPHICS_CLASS RenderTargetView :
	public DescriptorHeap
{
public:
	RenderTargetView() {}
	virtual ~RenderTargetView() {}

	virtual void CleanUp() = 0;
	virtual ID3D12Resource* GetResource(UINT index) = 0;

	friend class RenderTargets;
};

SUNSET_GRAPHICS_CLASS BOOL CreateRenderTargetView(
	RenderTargetView** ppRenderTargetView,
	DXGI::DXGI_SwapChain* pSwapChain
);

#endif // !_RENDER_TARGET_H_
