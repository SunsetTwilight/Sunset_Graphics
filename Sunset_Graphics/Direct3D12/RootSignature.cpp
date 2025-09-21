#include "RootSignature.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>

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

	class Impl_RootSignature :
		public RootSignature
	{
	public:
		Impl_RootSignature();
		~Impl_RootSignature();

		BOOL FetchRootSignatureFromShader(Shader* pShader);

		void Active(ID3D12GraphicsCommandList* pCmdList) override;

		void* GetRootSignature() override;

	private:
		ComPtr<ID3D12RootSignature> pRootSignature;
		ComPtr<ID3DBlob> pRootSignatureBlob;

		friend class GraphicsPipelineState;
	};

	Impl_RootSignature::Impl_RootSignature()
	{
		/*D3D12_ROOT_SIGNATURE_DESC desc;
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
		);*/
	}

	Impl_RootSignature::~Impl_RootSignature()
	{

	}

	BOOL Impl_RootSignature::FetchRootSignatureFromShader(Shader* pShader)
	{
		HRESULT hr = D3DGetBlobPart(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
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
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			IID_PPV_ARGS(pRootSignature.GetAddressOf())
		);
		
		if (FAILED(hr)) {
			/* Ž¸”s */

			return FALSE;
		}

		return TRUE;
	}

	void Impl_RootSignature::Active(ID3D12GraphicsCommandList* pCmdList)
	{
		pCmdList->SetGraphicsRootSignature(pRootSignature.Get());
	}

	void* Impl_RootSignature::GetRootSignature()
	{
		return pRootSignature.Get();
	}
}

BOOL CreateRootSignatureByFetchFromShader(
	RootSignature** ppRootSignature,
	Shader* pShader
)
{
	DX12::Impl_RootSignature* pImplRootSignature = new DX12::Impl_RootSignature;

	if (pImplRootSignature->FetchRootSignatureFromShader(pShader)) {

		(*ppRootSignature) = pImplRootSignature;
		return TRUE;
	}
	else {

		(*ppRootSignature) = nullptr;
		return FALSE;
	}

	return FALSE;
}
