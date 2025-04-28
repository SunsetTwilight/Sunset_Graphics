#ifndef _GRAPHICS_PIPELINE_STATE_H_
#define _GRAPHICS_PIPELINE_STATE_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

struct ID3D12PipelineState;

namespace DX12
{
	class GraphicsPipelineState
	{
	public:
		GraphicsPipelineState();
		~GraphicsPipelineState();

	private:
		ComPtr<ID3D12PipelineState> pPipelineState;
	};
}

#endif // !_GRAPHICS_PIPELINE_STATE_H_
