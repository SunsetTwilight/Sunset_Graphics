#ifndef _VERTEX_BUFFER_H_
#define _VERTEX_BUFFER_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

struct ID3D12GraphicsCommandList;
struct ID3D12Resource;

namespace DX12
{
	class VertexBuffer
	{
	public:
		SUNSET_GRAPHICS_CLASS VertexBuffer();
		SUNSET_GRAPHICS_CLASS ~VertexBuffer();

		SUNSET_GRAPHICS_CLASS void CreateVertexBuffer(
			UINT _typeSize, 
			void* _pVertice,
			UINT _vertexCount
		);

		SUNSET_GRAPHICS_CLASS void Mapping(
			void* _pVertice, 
			UINT _vertexCount
		);

		SUNSET_GRAPHICS_CLASS void Draw(ID3D12GraphicsCommandList* _pCmdList);

	private:
		ComPtr<ID3D12Resource> resource;

		UINT typeSize;
		UINT vertexCount;

		D3D12_VERTEX_BUFFER_VIEW view;

	};
}

#endif // !_VERTEX_BUFFER_H_
