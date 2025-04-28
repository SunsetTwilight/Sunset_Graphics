#ifndef _ROOT_SIGNATURE_H_
#define _ROOT_SIGNATURE_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

#include <d3dcommon.h>

struct ID3D12RootSignature;

namespace DX12
{
	class Shader;

	class RootSignature
	{
	public:
		RootSignature();
		~RootSignature();

		BOOL FetchRootSignatureFromShader(Shader* pShader);

	private:
		ComPtr<ID3D12RootSignature> pRootSignature;
		ComPtr<ID3DBlob> pRootSignatureBlob;
	};
}

#endif // !_ROOT_SIGNATURE_H_
