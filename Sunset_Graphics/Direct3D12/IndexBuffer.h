#ifndef _DX12_INDEX_BUFFER_H_
#define _DX12_INDEX_BUFFER_H_

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"
#include "../DXGI/DirectX_GI_def.h"

#include "DescriptorHeap.h"

class SUNSET_GRAPHICS_CLASS IndexBuffer
{
public:
	IndexBuffer() {}
	virtual ~IndexBuffer() {}

	virtual void Mapping(
		void* _pIndices,
		UINT _indexCount
	) = 0;

	virtual void Set(ID3D12GraphicsCommandList* pCmdList) = 0;

	virtual UINT GetIndexCount() = 0;

};

typedef BOOL (*PfnCreateIndexBuffer) (
	IndexBuffer** pIndexBuffer,
	void* _pIndices,
	UINT _indexCount
);

SUNSET_GRAPHICS_API BOOL CreateIndexBuffer(
	IndexBuffer** _ppIndexBuffer,
	void* _pIndices,
	UINT _indexCount
);

#endif // !_DX12_INDEX_BUFFER_H_
