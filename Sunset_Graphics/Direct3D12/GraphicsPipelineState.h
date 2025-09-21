#ifndef _GRAPHICS_PIPELINE_STATE_H_
#define _GRAPHICS_PIPELINE_STATE_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

#include <d3d12.h>
#include <d3dcommon.h>

struct ID3D12PipelineState;
struct D3D12_GRAPHICS_PIPELINE_STATE_DESC;

class Shader;
class RootSignature;

class GraphicsPipelineState
{
public:
	GraphicsPipelineState() {}
	virtual ~GraphicsPipelineState() {}

	virtual void SetRootSignature(RootSignature* pRootSignature) = 0;

	virtual void SetVertexShader(Shader* pShader) = 0;
	virtual void SetPixelShader(Shader* pShader) = 0;

	virtual void SetInputElementDesc(
		D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
		UINT numElements
	) = 0;

	virtual void Active(ID3D12GraphicsCommandList* pCmdList) = 0;
};

typedef BOOL(*PfnCreateGraphicsPipeline)(
	GraphicsPipelineState** ppGraphicsPipelineState,
	RootSignature* pRootSignature,
	D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
	UINT countofInputElement,
	Shader* pVS,
	Shader* pPS
);
SUNSET_GRAPHICS_API BOOL CreateGraphicsPipeline(
	GraphicsPipelineState** ppGraphicsPipelineState,
	RootSignature* pRootSignature,
	D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
	UINT countofInputElement,
	Shader* pVS,
	Shader* pPS
);

#endif // !_GRAPHICS_PIPELINE_STATE_H_
