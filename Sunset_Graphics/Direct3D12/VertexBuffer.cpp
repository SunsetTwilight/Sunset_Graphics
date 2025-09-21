#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	class Impl_VertexBuffer :
		public VertexBuffer
	{
	public:
		Impl_VertexBuffer(
			UINT _typeSize,
			void* _pVertice,
			UINT _vertexCount
		);
		~Impl_VertexBuffer();

		void Mapping(
			void* _pVertice,
			UINT _vertexCount
		) override;

		void Draw(ID3D12GraphicsCommandList* _pCmdList) override;
		void DrawIndexed(
			ID3D12GraphicsCommandList* pCmdList,
			IndexBuffer* pIndexBuffer
		) override;

	private:
		ComPtr<ID3D12Resource> resource;

		UINT typeSize;
		UINT vertexCount;

		D3D12_VERTEX_BUFFER_VIEW view;

	};

	Impl_VertexBuffer::Impl_VertexBuffer(
		UINT _typeSize,
		void* _pVertice,
		UINT _vertexCount
	)
	{
		//プロパティ設定用構造体の設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		//リソース設定用構造体の設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = _typeSize * _vertexCount;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//リソースの生成
		HRESULT hr = g_d3d12_device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&resource)
		);

		typeSize = _typeSize;
		vertexCount = _vertexCount;

		Mapping(_pVertice, _vertexCount);

		//頂点バッファ設定用構造体の設定
		view.BufferLocation = resource->GetGPUVirtualAddress();
		view.SizeInBytes = typeSize * vertexCount;
		view.StrideInBytes = typeSize;
	}

	Impl_VertexBuffer::~Impl_VertexBuffer()
	{
	}

	void Impl_VertexBuffer::Mapping(
		void* _pVertice,
		UINT _vertexCount
	)
	{
		if (resource != nullptr) {


			//送信範囲
			D3D12_RANGE range = { 0,0 };

			//マッピング
			UINT8* data;
			HRESULT hr = resource->Map(0, &range, reinterpret_cast<void**>(&data));

			//頂点データのコピー
			memcpy(
				data,
				_pVertice,
				typeSize * _vertexCount
			);

			//アンマップ
			resource->Unmap(0, nullptr);
		}
	}

	void Impl_VertexBuffer::Draw(ID3D12GraphicsCommandList* pCmdList)
	{
		pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCmdList->IASetVertexBuffers(0, 1, &view);
		pCmdList->DrawInstanced(vertexCount, 1, 0, 0);
	}

	void Impl_VertexBuffer::DrawIndexed(
		ID3D12GraphicsCommandList* pCmdList, 
		IndexBuffer* pIndexBuffer
	)
	{
		pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCmdList->IASetVertexBuffers(0, 1, &view);
		pIndexBuffer->Set(pCmdList);
		pCmdList->DrawIndexedInstanced(pIndexBuffer->GetIndexCount(), 1, 0, 0, 0);
	}
}

BOOL CreateVertexBuffer(
	VertexBuffer** _ppVertexBuffer, 
	UINT _typeSize, 
	void* _pVertice,
	UINT _vertexCount
)
{
	(*_ppVertexBuffer) = new DX12::Impl_VertexBuffer(
		_typeSize,
		_pVertice,
		_vertexCount
	);

	return TRUE;
}
