#ifndef _DX12_VERTEX_BUFFER_H_
#define _DX12_VERTEX_BUFFER_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

struct ID3D12GraphicsCommandList;
struct ID3D12Resource;

class IndexBuffer;

class SUNSET_GRAPHICS_CLASS VertexBuffer
{
public:
	VertexBuffer() {}
	virtual ~VertexBuffer() {}

	virtual void Mapping(
		void* _pVertice, 
		UINT _vertexCount
	) = 0;

	virtual void Draw(ID3D12GraphicsCommandList* _pCmdList) = 0;
	virtual void DrawIndexed(
		ID3D12GraphicsCommandList* pCmdList,
		IndexBuffer* pIndexBuffer
	) = 0;
};

typedef BOOL(*PfnCreateVertexBuffer)(
	VertexBuffer** _vertexBuffer,
	UINT _typeSize,
	void* _pVertice,
	UINT _vertexCount
);

SUNSET_GRAPHICS_API BOOL CreateVertexBuffer(
	VertexBuffer** _ppVertexBuffer,
	UINT _typeSize,
	void* _pVertice,
	UINT _vertexCount
);

#endif // !_DX12_VERTEX_BUFFER_H_
