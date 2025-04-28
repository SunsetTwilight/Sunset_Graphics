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
		Shader();
		~Shader();

		static BOOL ShaderCompile(
			ID3DBlob** ppCode, 
			LPCWSTR pFileName, 
			LPCSTR pEntryPoint, 
			LPCSTR pTarget
		);

		ID3DBlob* GetBlob();

		ShaderType GetType();
		
		BOOL CreateVertexShaderEx(LPCWSTR pFileName);
		BOOL CreatePixelShaderEx(LPCWSTR pFileName);
		BOOL CreateGeometryShaderEx(LPCWSTR pFileName);
		BOOL CreateHullShaderEx(LPCWSTR pFileName);
		BOOL CreateDomainShaderEx(LPCWSTR pFileName);

	private:
		
	protected:
		ComPtr<ID3DBlob> code;
		
	private:
		ShaderType type;
	};
}

#endif // !_DIRECTX12_SHADER

