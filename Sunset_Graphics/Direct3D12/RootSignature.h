#ifndef _ROOT_SIGNATURE_H_
#define _ROOT_SIGNATURE_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

struct ID3D12GraphicsCommandList;
struct ID3D12RootSignature;

class Shader;

class SUNSET_GRAPHICS_CLASS RootSignature
{
public:
	RootSignature() {}
	virtual ~RootSignature() {}

	virtual void Active(ID3D12GraphicsCommandList* pCmdList) = 0;

	virtual void* GetRootSignature() = 0;
};

typedef BOOL(*PfnCreateRootSignatureByFetchFromShader)(
	RootSignature** pRootSignature,
	Shader* pShader
	);
SUNSET_GRAPHICS_API BOOL CreateRootSignatureByFetchFromShader(
	RootSignature** pRootSignature,
	Shader* pShader
);

#endif // !_ROOT_SIGNATURE_H_
