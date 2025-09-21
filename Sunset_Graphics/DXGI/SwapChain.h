#include "../sunset_graphics_pch.h"
#include "DirectX_GI_def.h"

#include <memory>

struct ID3D12CommandQueue;

namespace DXGI
{
	class SwapChain
	{
	public:
		SUNSET_GRAPHICS_CLASS SwapChain();
		SUNSET_GRAPHICS_CLASS ~SwapChain();

		SUNSET_GRAPHICS_CLASS DXGI_SwapChain* GetSwapChain();

		SUNSET_GRAPHICS_CLASS void WaitOn();
		SUNSET_GRAPHICS_CLASS void Swap();
		SUNSET_GRAPHICS_CLASS BOOL PresentTest();
		
		SUNSET_GRAPHICS_CLASS BOOL Resize(UINT width, UINT height);

		SUNSET_GRAPHICS_CLASS UINT GetCurrentBackBufferIndex();
		SUNSET_GRAPHICS_CLASS HANDLE GetHandleSwapChainWaitableObject();

		SUNSET_GRAPHICS_CLASS static BOOL CreateSwapChain(
			_Inout_ std::shared_ptr<SwapChain>& pSwapChain,
			_In_ ID3D12CommandQueue* pCommandQueue,
			_In_ HWND hWnd
		);

		SUNSET_GRAPHICS_CLASS BOOL GetStatusOccluded();

	private:
		HWND m_hWnd;
		HANDLE m_hSwapChainWaitableObject;
		BOOL m_tearingSupport;
		UINT m_frameRate;
		BOOL m_windowed;

		BOOL m_occluded				= TRUE;
		BOOL m_modeChanged			= FALSE;
		BOOL m_modeChangeInProgress	= FALSE;

		ComPtr<DXGI_SwapChain> m_pSwapChain;
	};

	
}
