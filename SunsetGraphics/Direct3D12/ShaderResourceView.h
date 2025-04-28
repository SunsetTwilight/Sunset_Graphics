#ifndef _SHADER_RESOURCE_VIEW_H_
#define _SHADER_RESOURCE_VIEW_H_

#include "../sunset_graphics_pch.h"

#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

namespace DX12
{
    class ShaderResourceView :
        public DescriptorHeap
    {
    public:
        SUNSET_GRAPHICS_CLASS ShaderResourceView();
        SUNSET_GRAPHICS_CLASS virtual ~ShaderResourceView();

        SUNSET_GRAPHICS_CLASS ShaderResourceView(UINT num);

    };
}

#endif // !_SHADER_RESOURCE_VIEW_H_
