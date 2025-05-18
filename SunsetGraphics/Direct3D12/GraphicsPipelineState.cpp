#include "GraphicsPipelineState.h"

#include "RootSignature.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	GraphicsPipelineState::GraphicsPipelineState()
	{
		desc.pRootSignature = nullptr;

		desc.VS = D3D12_SHADER_BYTECODE();
		desc.PS = D3D12_SHADER_BYTECODE();
		desc.DS = D3D12_SHADER_BYTECODE();
		desc.HS = D3D12_SHADER_BYTECODE();
		desc.GS = D3D12_SHADER_BYTECODE();

		desc.StreamOutput;

		desc.BlendState.AlphaToCoverageEnable = false;
		desc.BlendState.IndependentBlendEnable = false;
		desc.BlendState.RenderTarget[0].BlendEnable = false;
		desc.BlendState.RenderTarget[0].LogicOpEnable = false;
		desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		desc.RasterizerState.MultisampleEnable = false;
		desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		desc.RasterizerState.DepthClipEnable = true;
		desc.DepthStencilState;

		/* -------- 頂点データに依存 -------- */
		desc.InputLayout.pInputElementDescs;	
		desc.InputLayout.NumElements;

		desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		/* ---------------------------------- */

		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.NodeMask = 0;
		desc.CachedPSO;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	}

	GraphicsPipelineState::~GraphicsPipelineState()
	{

	}

	void GraphicsPipelineState::CreateGraphicsPipeline()
	{
		g_d3d12_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pPipelineState.GetAddressOf()));
	}

	void GraphicsPipelineState::SetRootSignature(RootSignature* pRootSignature)
	{
		auto ptr = pRootSignature->pRootSignature.Get();
		if (ptr != nullptr) {
			desc.pRootSignature = ptr;
		}
	}

	void GraphicsPipelineState::SetVertexShader(ID3DBlob* pBlob)
	{
		desc.VS.pShaderBytecode = pBlob->GetBufferPointer();
		desc.VS.BytecodeLength = pBlob->GetBufferSize();
	}

	void GraphicsPipelineState::SetPixelShader(ID3DBlob* pBlob)
	{
		desc.PS.pShaderBytecode = pBlob->GetBufferPointer();
		desc.PS.BytecodeLength = pBlob->GetBufferSize();
	}

	void GraphicsPipelineState::SetInputElementDesc(
		D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
		UINT numElements
	)
	{
		desc.InputLayout.pInputElementDescs = pInputElementDesc;
		desc.InputLayout.NumElements = numElements;
	}

	void GraphicsPipelineState::Active(ID3D12GraphicsCommandList* pCmdList)
	{
		pCmdList->SetPipelineState(pPipelineState.Get());
	}
}