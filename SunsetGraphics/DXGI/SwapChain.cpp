#include "SwapChain.h"
#include "../sunset_graphics_pch.h"
#include "DirectX_GI.h"

#include <dxgi1_6.h>

#include <d3d11_4.h>
#include <d3d12.h>
#include <wrl.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#define GET_DXGI_STATUS(hr, dxgi_status) ((hr == dxgi_status) ? TRUE : FALSE)
 
namespace DXGI
{
	extern ComPtr<DXGI_Factory>	g_factory;

	std::vector<std::weak_ptr<SwapChain>> g_pSwapChainManaged;

	SwapChain::SwapChain() :
		m_hWnd(NULL),
		m_hSwapChainWaitableObject(NULL),
		m_tearingSupport(FALSE)
	{
	}

	SwapChain::~SwapChain()
	{
	}

	DXGI_SwapChain* SwapChain::GetSwapChain()
	{
		return m_pSwapChain.Get();
	}

	void SwapChain::WaitOn()
	{
		DWORD result = WaitForSingleObjectEx(
			m_hSwapChainWaitableObject,
			1000, // 1 second timeout (shouldn't ever occur) 
			true
		);
	}

	void SwapChain::Swap()
	{
		UINT presentFlags = (m_tearingSupport && m_windowed) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		UINT interval = (presentFlags & DXGI_PRESENT_ALLOW_TEARING) ? 0 : 1;
		// Present the frame.
		HRESULT hr =  m_pSwapChain->Present(interval, presentFlags);
		m_occluded				= GET_DXGI_STATUS(hr, DXGI_STATUS_OCCLUDED);
		m_modeChanged			= GET_DXGI_STATUS(hr, DXGI_STATUS_MODE_CHANGED);
		m_modeChangeInProgress	= GET_DXGI_STATUS(hr, DXGI_STATUS_MODE_CHANGE_IN_PROGRESS);
	}


	BOOL SwapChain::PresentTest()
	{
		HRESULT hr = m_pSwapChain->Present(0, DXGI_PRESENT_TEST);
		m_occluded = GET_DXGI_STATUS(hr, DXGI_STATUS_OCCLUDED);
		m_modeChanged = GET_DXGI_STATUS(hr, DXGI_STATUS_MODE_CHANGED);
		m_modeChangeInProgress = GET_DXGI_STATUS(hr, DXGI_STATUS_MODE_CHANGE_IN_PROGRESS);

		return m_occluded;
	}

	BOOL SwapChain::Resize(UINT width, UINT height)
	{
		HRESULT hr = S_OK;

		DXGI_SWAP_CHAIN_DESC1 sd;
		hr = m_pSwapChain->GetDesc1(&sd);
		if (hr != S_OK) {
			return FALSE;
		}

		hr = m_pSwapChain->ResizeBuffers(
			NULL,						/* 既存の数を保持 */
			static_cast<UINT>(width),
			static_cast<UINT>(height),
			DXGI_FORMAT_UNKNOWN,		/* 既存のフォーマットを保持 */
			sd.Flags
		);
		if (hr != S_OK) {
			return FALSE;
		}
	
		return TRUE;
	}

	UINT SwapChain::GetCurrentBackBufferIndex()
	{
		return m_pSwapChain->GetCurrentBackBufferIndex();
	}

	HANDLE SwapChain::GetHandleSwapChainWaitableObject()
	{
		return m_hSwapChainWaitableObject;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　DXGI SwapChainの生成
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
	//------------------------------------------------------------------------------------------------
	BOOL SwapChain::CreateSwapChain(
		_Inout_ std::shared_ptr<SwapChain>& pSwapChain,
		_In_ ID3D12CommandQueue* pCommandQueue,
		_In_ HWND hWnd
	)
	{
		HRESULT hr = S_OK;

		IDXGISwapChain1* swapChain1 = nullptr;

		BOOL m_tearingSupport;
		{
			BOOL allowTearing = FALSE;

			IDXGIFactory5* pFactory5;
			g_factory->QueryInterface(IID_PPV_ARGS(&pFactory5));

			hr = pFactory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing,
				sizeof(allowTearing)
			);
			m_tearingSupport = SUCCEEDED(hr) && allowTearing;

			pFactory5->Release();
		}

		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC1 sd;
		{
			RECT rc;
			if (!GetClientRect(hWnd, &rc)) {
				return FALSE;
			}

			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = APP_NUM_BACK_BUFFERS;
			sd.Width = (rc.right - rc.left);
			sd.Height = (rc.bottom - rc.top);
			sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			sd.Scaling = DXGI_SCALING_STRETCH;
			sd.Stereo = FALSE;

			sd.Flags |= m_tearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		}

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd{};
		{
			fd.RefreshRate.Denominator = 1;
			fd.RefreshRate.Numerator = 120;
			fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			fd.Windowed = TRUE;
		}

		ThrowIfFailed(g_factory->CreateSwapChainForHwnd(
			pCommandQueue,
			hWnd,
			&sd,
			&fd,
			nullptr,
			&swapChain1)
		);
		if (hr != S_OK) {
			return FALSE;
		}

		// This sample does not support fullscreen transitions.
		g_factory->MakeWindowAssociation(
			hWnd, 
			DXGI_MWA_NO_ALT_ENTER
		);

		//Allocate
		pSwapChain = std::make_shared<SwapChain>();
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&pSwapChain->m_pSwapChain)) != S_OK) {
			return FALSE;
		}
		swapChain1->Release();

		pSwapChain->m_hWnd = hWnd;
		pSwapChain->m_tearingSupport = m_tearingSupport;
		pSwapChain->m_pSwapChain->SetMaximumFrameLatency(APP_NUM_BACK_BUFFERS);
		pSwapChain->m_hSwapChainWaitableObject = pSwapChain->m_pSwapChain->GetFrameLatencyWaitableObject();
		pSwapChain->m_frameRate = (fd.RefreshRate.Numerator / fd.RefreshRate.Denominator);
		pSwapChain->m_windowed = fd.Windowed;

		g_pSwapChainManaged.push_back(pSwapChain);

		return TRUE;
	}

	SUNSET_GRAPHICS_CLASS BOOL SwapChain::GetStatusOccluded()
	{
		return m_occluded;
	}
}