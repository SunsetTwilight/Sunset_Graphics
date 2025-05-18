#ifndef _ROOT_SIGNATURE_H_
#define _ROOT_SIGNATURE_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

#include <d3dcommon.h>

struct ID3D12RootSignature;
struct ID3D12GraphicsCommandList;

namespace DX12
{
	class Shader;

	class RootSignature
	{
	public:
		SUNSET_GRAPHICS_CLASS RootSignature();
		SUNSET_GRAPHICS_CLASS ~RootSignature();

		SUNSET_GRAPHICS_CLASS BOOL FetchRootSignatureFromShader(Shader* pShader);

		SUNSET_GRAPHICS_CLASS void Active(ID3D12GraphicsCommandList* pCmdList);

	private:
		ComPtr<ID3D12RootSignature> pRootSignature;
		ComPtr<ID3DBlob> pRootSignatureBlob;

		friend class GraphicsPipelineState;
	};
}

#endif // !_ROOT_SIGNATURE_H_
