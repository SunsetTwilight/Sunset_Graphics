#ifndef _DIRECTX_GI_DEF_H_
#define _DIRECTX_GI_DEF_H_

struct IDXGIFactory4;
struct IDXGIAdapter1;

struct IDXGISwapChain1;
struct IDXGISwapChain2;
struct IDXGISwapChain3;

namespace DXGI
{
	typedef IDXGIFactory4	DXGI_Factory;
	typedef IDXGIAdapter1	DXGI_Adapter;
	typedef IDXGISwapChain3	DXGI_SwapChain;
}

#endif // !_DIRECTX_GI_DEF_H_

