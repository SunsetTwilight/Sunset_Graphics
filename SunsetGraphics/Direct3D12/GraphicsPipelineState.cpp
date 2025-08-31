#include "GraphicsPipelineState.h"

#include "Shader.h"
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

	class Impl_RootSignature;

	class Impl_GraphicsPipelineState :
		public GraphicsPipelineState
	{
	public:
		Impl_GraphicsPipelineState();
		~Impl_GraphicsPipelineState();

		void CreateGraphicsPipeline();

		void SetRootSignature(RootSignature* pRootSignature);

		void SetVertexShader(Shader* pShader);
		void SetPixelShader(Shader* pShader);

		void SetInputElementDesc(
			D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
			UINT numElements
		);

		void Active(ID3D12GraphicsCommandList* pCmdList);

	private:
		ComPtr<ID3D12PipelineState> pPipelineState;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;

	};

	Impl_GraphicsPipelineState::Impl_GraphicsPipelineState()
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

	Impl_GraphicsPipelineState::~Impl_GraphicsPipelineState()
	{
	}

	void Impl_GraphicsPipelineState::CreateGraphicsPipeline()
	{
		g_d3d12_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pPipelineState.GetAddressOf()));
	}

	void Impl_GraphicsPipelineState::SetRootSignature(RootSignature* pRootSignature)
	{
		auto ptr = pRootSignature->GetRootSignature();
		if (ptr != nullptr) {
			desc.pRootSignature = (ID3D12RootSignature*)(ptr);
		}
	}

	void Impl_GraphicsPipelineState::SetVertexShader(Shader* pShader)
	{
		desc.VS.pShaderBytecode = pShader->GetBufferPointer();
		desc.VS.BytecodeLength = pShader->GetBufferSize();
	}

	void Impl_GraphicsPipelineState::SetPixelShader(Shader* pShader)
	{
		desc.PS.pShaderBytecode = pShader->GetBufferPointer();
		desc.PS.BytecodeLength = pShader->GetBufferSize();
	}

	void Impl_GraphicsPipelineState::SetInputElementDesc(
		D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
		UINT numElements
	)
	{
		desc.InputLayout.pInputElementDescs = pInputElementDesc;
		desc.InputLayout.NumElements = numElements;
	}

	void Impl_GraphicsPipelineState::Active(ID3D12GraphicsCommandList* pCmdList)
	{
		pCmdList->SetPipelineState(pPipelineState.Get());
	}
}

BOOL CreateGraphicsPipeline(
	GraphicsPipelineState** ppGraphicsPipelineState, 
	RootSignature* pRootSignature,
	D3D12_INPUT_ELEMENT_DESC* pInputElementDesc,
	UINT countofInputElement,
	Shader* pVS, 
	Shader* pPS
)
{
	DX12::Impl_GraphicsPipelineState* impl = new DX12::Impl_GraphicsPipelineState;

	impl->SetRootSignature(pRootSignature);
		
	impl->SetInputElementDesc(
		pInputElementDesc,
		countofInputElement
	);
		
	impl->SetVertexShader(pVS);
	impl->SetPixelShader(pPS);

	impl->CreateGraphicsPipeline();

	(*ppGraphicsPipelineState) = impl;

	return TRUE;
}
