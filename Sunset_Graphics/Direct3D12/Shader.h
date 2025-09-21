#ifndef _DIRECTX12_SHADER
#define _DIRECTX12_SHADER

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

//#include "ShaderVariable.h"

enum class ShaderType
{
	None = 0,
	Vertex,
	Pixcel,
	Geometry,
	Hull,
	Domain,
};

class SUNSET_GRAPHICS_CLASS Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	virtual ShaderType GetType() = 0;
		
	virtual LPVOID GetBufferPointer() = 0;
	virtual SIZE_T GetBufferSize() = 0;
};

typedef BOOL (*PfnCreateVertexShader)(
	Shader** pShader,
	LPCWSTR pFileName
);
SUNSET_GRAPHICS_API BOOL CreateVertexShader(
	Shader** pShader,
	LPCWSTR pFileName
);

typedef BOOL (*PfnCreatePixelShader)(
	Shader** pShader,
	LPCWSTR pFileName
);
SUNSET_GRAPHICS_API BOOL CreatePixelShader(
	Shader** pShader,
	LPCWSTR pFileName
);

typedef BOOL (*PfnCreateGeometryShader)(
	Shader** pShader,
	LPCWSTR pFileName
);
SUNSET_GRAPHICS_API BOOL CreateGeometryShader(
	Shader** pShader,
	LPCWSTR pFileName
);

typedef BOOL (*PfnCreateHullShader)(
	Shader** pShader,
	LPCWSTR pFileName
);
SUNSET_GRAPHICS_API BOOL CreateHullShader(
	Shader** pShader,
	LPCWSTR pFileName
);

typedef BOOL (*PfnCreateDomainShader)(
	Shader** pShader,
	LPCWSTR pFileName
);
SUNSET_GRAPHICS_API BOOL CreateDomainShader(
	Shader** pShader,
	LPCWSTR pFileName
);

typedef BOOL (*PfnCreateComputeShader)(
	Shader** pShader,
	LPCWSTR pFileName
);
SUNSET_GRAPHICS_API BOOL CreateComputeShader(
	Shader** pShader,
	LPCWSTR pFileName
);

#endif // !_DIRECTX12_SHADER

