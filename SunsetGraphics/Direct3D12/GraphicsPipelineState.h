#ifndef _GRAPHICS_PIPELINE_STATE_H_
#define _GRAPHICS_PIPELINE_STATE_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

#include <d3d12.h>
#include <d3dcommon.h>

struct ID3D12PipelineState;
struct D3D12_GRAPHICS_PIPELINE_STATE_DESC;

namespace DX12
{
	class RootSignature;

	class GraphicsPipelineState
	{
	public:
		SUNSET_GRAPHICS_CLASS GraphicsPipelineState();
		SUNSET_GRAPHICS_CLASS ~GraphicsPipelineState();

		SUNSET_GRAPHICS_CLASS void CreateGraphicsPipeline();

		SUNSET_GRAPHICS_CLASS void SetRootSignature(RootSignature* pRootSignature);

		SUNSET_GRAPHICS_CLASS void SetVertexShader(ID3DBlob* pBlob);
		SUNSET_GRAPHICS_CLASS void SetPixelShader(ID3DBlob* pBlob);

		SUNSET_GRAPHICS_CLASS void SetInputElementDesc(
			D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
			UINT numElements
		);

		SUNSET_GRAPHICS_CLASS void Active(ID3D12GraphicsCommandList* pCmdList);

	private:
		ComPtr<ID3D12PipelineState> pPipelineState;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;

	};
}

#endif // !_GRAPHICS_PIPELINE_STATE_H_
