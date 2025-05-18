#ifndef _DIRECTX12_SHADER
#define _DIRECTX12_SHADER

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

#include "ShaderVariable.h"

#include <d3dcommon.h>
#include <d3d12.h>
#include <d3d12shader.h>

#include <vector>
#include <functional>

enum D3D12_SHADER_VISIBILITY;
struct D3D12_ROOT_PARAMETER;
struct D3D12_DESCRIPTOR_RANGE;
struct ID3D12Resource;
struct ID3D12DescriptorHeap;

namespace DX12
{
	enum class ShaderType
	{
		None = 0,
		Vertex,
		Pixcel,
		Geometry,
		Hull,
		Domain,
	};

	class Shader
	{
	public:
		SUNSET_GRAPHICS_CLASS Shader();
		SUNSET_GRAPHICS_CLASS ~Shader();

		SUNSET_GRAPHICS_CLASS static BOOL ShaderCompile(
			ID3DBlob** ppCode, 
			LPCWSTR pFileName, 
			LPCSTR pEntryPoint, 
			LPCSTR pTarget
		);

		SUNSET_GRAPHICS_CLASS ID3DBlob* GetBlob();

		SUNSET_GRAPHICS_CLASS ShaderType GetType();
		
		SUNSET_GRAPHICS_CLASS BOOL CreateVertexShaderEx(LPCWSTR pFileName);
		SUNSET_GRAPHICS_CLASS BOOL CreatePixelShaderEx(LPCWSTR pFileName);
		SUNSET_GRAPHICS_CLASS BOOL CreateGeometryShaderEx(LPCWSTR pFileName);
		SUNSET_GRAPHICS_CLASS BOOL CreateHullShaderEx(LPCWSTR pFileName);
		SUNSET_GRAPHICS_CLASS BOOL CreateDomainShaderEx(LPCWSTR pFileName);

	private:
		
	protected:
		ComPtr<ID3DBlob> code;
		
	private:
		ShaderType type;
	};
}

#endif // !_DIRECTX12_SHADER

