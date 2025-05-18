#include "Shader.h"

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>

#include <d3d12shader.h>

#pragma comment(lib, "D3DCompiler.lib")

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include <d3dcompiler.h>

#include <atlstr.h>

namespace DX12
{
	extern ComPtr<ID3D12Device> g_d3d12_device;

	Shader::Shader() : type(ShaderType::None) {}
	Shader::~Shader() {}

	BOOL Shader::ShaderCompile(
		ID3DBlob** ppCode, 
		LPCWSTR pFileName, 
		LPCSTR pEntryPoint, 
		LPCSTR pTarget
	)
	{
		ComPtr<ID3DBlob> pErrorMsgs;

		HRESULT hr = D3DCompileFromFile(
			pFileName,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			pEntryPoint,
			pTarget,
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			ppCode,
			&pErrorMsgs
		);

		if (FAILED(hr)) {
			/* 失敗 */
			if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
				/* 原因がファイルが見つからなかった以外 */

				std::string errstr;
				errstr.resize(pErrorMsgs->GetBufferSize());

				std::copy_n(
					(char*)pErrorMsgs->GetBufferPointer(),
					pErrorMsgs->GetBufferSize(),
					errstr.begin()
				);
				OutputDebugStringA(errstr.c_str());
				MessageBoxA(NULL, errstr.c_str(), "Compile Error ShaderCode", NULL);
			}
			else {
				/* ファイルが見つからなかった */

			}

			return FALSE;
		}

		return TRUE;
	}

	ID3DBlob* Shader::GetBlob()
	{
		return code.Get();
	}

	ShaderType Shader::GetType()
	{
		return type;
	}

	BOOL Shader::CreateVertexShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "VSMain", "vs_5_1"))
			return FALSE;

		type = ShaderType::Vertex;
		
		return TRUE;
	}

	BOOL Shader::CreatePixelShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "PSMain", "ps_5_1"))
			return FALSE;

		type = ShaderType::Pixcel;
		
		return TRUE;
	}

	BOOL Shader::CreateGeometryShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "GSMain", "gs_5_1"))
			return FALSE;

		type = ShaderType::Geometry;
		
		return TRUE;
	}

	BOOL Shader::CreateHullShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "HSMain", "hs_5_1"))
			return FALSE;

		type = ShaderType::Hull;
		
		return TRUE;
	}

	BOOL Shader::CreateDomainShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "DSMain", "ds_5_1"))
			return FALSE;

		type = ShaderType::Domain;
		
		return TRUE;
	}


}