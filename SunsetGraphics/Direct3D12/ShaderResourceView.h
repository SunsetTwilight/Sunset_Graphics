#ifndef _SHADER_RESOURCE_VIEW_H_
#define _SHADER_RESOURCE_VIEW_H_

#include "../sunset_graphics_pch.h"

#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

class SUNSET_GRAPHICS_CLASS ShaderResourceView : 
    public DescriptorHeap
{
public:
    ShaderResourceView() {}
    virtual ~ShaderResourceView() {}
};

typedef BOOL(*PfnCreateShaderResourceView)(
    ShaderResourceView** ppShaderResourceView,
    UINT num
);
SUNSET_GRAPHICS_API BOOL CreateShaderResourceView(
    ShaderResourceView** ppShaderResourceView, 
    UINT num
);

#endif // !_SHADER_RESOURCE_VIEW_H_
