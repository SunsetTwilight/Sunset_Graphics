#ifndef _DEPTH_STENCIL_VIEW_H_
#define _DEPTH_STENCIL_VIEW_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

#include <memory>

struct ID3D12DescriptorHeap;
struct ID3D12Resource;

class SUNSET_GRAPHICS_CLASS DepthStencilView :
	public DescriptorHeap
{
public:
	DepthStencilView() {}
	virtual ~DepthStencilView() {}

	virtual void CleanUp() = 0;

protected:
	//ComPtr<ID3D12Resource> m_dsv;

	friend class RenderTargets;
};

SUNSET_GRAPHICS_CLASS BOOL CreateDepthStencilView(
	DepthStencilView** ppDepthStencilView,
	DXGI::DXGI_SwapChain* pSwapChain
);

/*SUNSET_GRAPHICS_CLASS BOOL CreateDepthStencilView(
	DepthStencilView** ppDepthStencilView,
	DXGI::DXGI_SwapChain* pSwapChain
);*/

#endif // !_DEPTH_STENCIL_VIEW_H_
