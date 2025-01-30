//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

#ifndef _SUNSET_GRAPHICS_PCH_H_
#define _SUNSET_GRAPHICS_PCH_H_

//------------------------------------------------------------------------------------------------
//ヘッダーファイル　読み込み
//------------------------------------------------------------------------------------------------

#include <Windows.h>
#include <wrl.h>
#include <crtdbg.h>

using namespace Microsoft::WRL;

namespace SGraphics {
#if defined(_DEBUG)
    CONST BOOL isDebug = TRUE;
#else
    CONST BOOL isDebug = FALSE;
#endif
}

inline LPWSTR hr_trace(HRESULT hr)
{
	LPWSTR msg;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&msg), 0, NULL);
	return msg;
}

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw hr_trace(hr);
    }
}

#define THROW_IF_FAILED(hr) ThrowIfFailed(hr)

#ifdef SUNSET_GRAPHICS_EXPORTS
#define SUNSET_GRAPHICS_API extern "C" __declspec(dllexport)
#else
#define SUNSET_GRAPHICS_API extern "C" __declspec(dllimport)
#endif

#endif // !_SUNSET_GRAPHICS_PCH_H_

