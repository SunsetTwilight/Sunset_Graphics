#include "RootSignature.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include "Shader.h"

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;
	extern PFN_D3D12_SERIALIZE_ROOT_SIGNATURE D3D12SerializeRootSignature;

	RootSignature::RootSignature()
	{
		D3D12_ROOT_SIGNATURE_DESC desc;
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> pErrorBlob;
		
		D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,
			pRootSignatureBlob.GetAddressOf(),
			pErrorBlob.GetAddressOf()
		);

		g_d3d12_device->CreateRootSignature(
			0,
			pRootSignatureBlob->GetBufferPointer(),
			pRootSignatureBlob->GetBufferSize(),
			IID_PPV_ARGS(pRootSignature.GetAddressOf())
		);
	}

	RootSignature::~RootSignature()
	{

	}

	BOOL RootSignature::FetchRootSignatureFromShader(Shader* pShader)
	{
		ID3DBlob* shader = pShader->GetBlob();
		
		HRESULT hr = D3DGetBlobPart(
			shader->GetBufferPointer(),
			shader->GetBufferSize(),
			D3D_BLOB_ROOT_SIGNATURE,
			0,
			pRootSignatureBlob.GetAddressOf()
		);

		if (FAILED(hr)) {
			/* Ž¸”s */

			return FALSE;
		}
		
		/* ¬Œ÷ */
		hr = g_d3d12_device->CreateRootSignature(
			0,
			pRootSignatureBlob->GetBufferPointer(),
			pRootSignatureBlob->GetBufferSize(),
			IID_PPV_ARGS(pRootSignature.GetAddressOf())
		);
		
		if (FAILED(hr)) {
			/* Ž¸”s */

			return FALSE;
		}

		return TRUE;
	}
}