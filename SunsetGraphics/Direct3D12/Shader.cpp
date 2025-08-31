#include "Shader.h"

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>

#include <d3d12shader.h>
#include <d3dcompiler.h>

#pragma comment(lib, "D3DCompiler.lib")

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include <atlstr.h>

namespace DX12
{
	extern ComPtr<ID3D12Device> g_d3d12_device;

	class Impl_Shader :
		public Shader
	{
	public:
		Impl_Shader();
		~Impl_Shader();

		static BOOL ShaderCompile(
			ID3DBlob** ppCode,
			LPCWSTR pFileName,
			LPCSTR pEntryPoint,
			LPCSTR pTarget
		);

		ShaderType GetType() override;

		LPVOID GetBufferPointer() override;
		SIZE_T GetBufferSize() override;

		BOOL CreateVertexShaderEx(LPCWSTR pFileName);
		BOOL CreatePixelShaderEx(LPCWSTR pFileName);
		BOOL CreateGeometryShaderEx(LPCWSTR pFileName);
		BOOL CreateHullShaderEx(LPCWSTR pFileName);
		BOOL CreateDomainShaderEx(LPCWSTR pFileName);

	protected:
		ComPtr<ID3DBlob> code;

	private:
		ShaderType type;
	};

	Impl_Shader::Impl_Shader() : type(ShaderType::None) {}
	Impl_Shader::~Impl_Shader() {}

	BOOL Impl_Shader::ShaderCompile(
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

	ShaderType Impl_Shader::GetType()
	{
		return type;
	}

	LPVOID Impl_Shader::GetBufferPointer()
	{
		return code->GetBufferPointer();
	}

	SIZE_T Impl_Shader::GetBufferSize()
	{
		return code->GetBufferSize();
	}

	BOOL Impl_Shader::CreateVertexShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "VSMain", "vs_5_1"))
			return FALSE;

		type = ShaderType::Vertex;
		
		return TRUE;
	}

	BOOL Impl_Shader::CreatePixelShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "PSMain", "ps_5_1"))
			return FALSE;

		type = ShaderType::Pixcel;
		
		return TRUE;
	}

	BOOL Impl_Shader::CreateGeometryShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "GSMain", "gs_5_1"))
			return FALSE;

		type = ShaderType::Geometry;
		
		return TRUE;
	}

	BOOL Impl_Shader::CreateHullShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "HSMain", "hs_5_1"))
			return FALSE;

		type = ShaderType::Hull;
		
		return TRUE;
	}

	BOOL Impl_Shader::CreateDomainShaderEx(LPCWSTR pFileName)
	{
		if (!ShaderCompile(code.GetAddressOf(), pFileName, "DSMain", "ds_5_1"))
			return FALSE;

		type = ShaderType::Domain;
		
		return TRUE;
	}
}

BOOL CreateVertexShader(
	Shader** pShader, 
	LPCWSTR pFileName
)
{
	DX12::Impl_Shader* pImplShader = new DX12::Impl_Shader;
	if (pImplShader->CreateVertexShaderEx(pFileName)) {

		(*pShader) = static_cast<Shader*>(pImplShader);
		return TRUE;
	}
	else {
		(*pShader) = NULL;
		return FALSE;
	}

	return FALSE;
}

BOOL CreatePixelShader(
	Shader** pShader, 
	LPCWSTR pFileName
)
{
	DX12::Impl_Shader* pImplShader = new DX12::Impl_Shader;
	if (pImplShader->CreatePixelShaderEx(pFileName)) {

		(*pShader) = static_cast<Shader*>(pImplShader);
		return TRUE;
	}
	else {
		(*pShader) = NULL;
		return FALSE;
	}

	return FALSE;
}

BOOL CreateGeometryShader(
	Shader** pShader, 
	LPCWSTR pFileName
)
{
	DX12::Impl_Shader* pImplShader = new DX12::Impl_Shader;
	if (pImplShader->CreateGeometryShaderEx(pFileName)) {

		(*pShader) = static_cast<Shader*>(pImplShader);
		return TRUE;
	}
	else {
		(*pShader) = NULL;
		return FALSE;
	}

	return FALSE;
}

BOOL CreateHullShader(
	Shader** pShader, 
	LPCWSTR pFileName
)
{
	DX12::Impl_Shader* pImplShader = new DX12::Impl_Shader;
	if (pImplShader->CreateHullShaderEx(pFileName)) {

		(*pShader) = static_cast<Shader*>(pImplShader);
		return TRUE;
	}
	else {
		(*pShader) = NULL;
		return FALSE;
	}

	return FALSE;
}

BOOL CreateDomainShader(
	Shader** pShader, 
	LPCWSTR pFileName
)
{
	DX12::Impl_Shader* pImplShader = new DX12::Impl_Shader;
	if (pImplShader->CreateDomainShaderEx(pFileName)) {

		(*pShader) = static_cast<Shader*>(pImplShader);
		return TRUE;
	}
	else {
		(*pShader) = NULL;
		return FALSE;
	}

	return FALSE;
}

BOOL CreateComputeShader(
	Shader** pShader, 
	LPCWSTR pFileName
)
{
	/*Impl_Shader* pImplShader = new Impl_Shader;
	if (pImplShader->CreateComputeShaderEx(pFileName)) {

		pShader = static_cast<Shader*>(pImplShader);
		return TRUE;
	}
	else {
		pShader = NULL;
		return FALSE;
	}*/

	return FALSE;
}
