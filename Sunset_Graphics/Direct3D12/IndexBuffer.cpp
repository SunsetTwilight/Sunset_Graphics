#include "IndexBuffer.h"

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>

#define TYPE_SIZE_INDEX_BUFFER (4)

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	class Impl_IndexBuffer :
		public IndexBuffer
	{
	public:
		Impl_IndexBuffer(void* _pIndices, UINT _indexCount);
		~Impl_IndexBuffer();

		void Mapping(
			void* _pIndices,
			UINT _indexCount
		) override;

		void Set(ID3D12GraphicsCommandList* pCmdList) override;

		UINT GetIndexCount() override;

	private:
		ComPtr<ID3D12Resource> resource;
		D3D12_INDEX_BUFFER_VIEW view;

		UINT indexCount;
	};

	Impl_IndexBuffer::Impl_IndexBuffer(void* _pIndices, UINT _indexCount)
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
		desc.Width = TYPE_SIZE_INDEX_BUFFER * _indexCount;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		HRESULT hr = g_d3d12_device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&resource)
		);

		indexCount = _indexCount;

		Mapping(_pIndices, _indexCount);

		//頂点バッファ設定用構造体の設定
		view.BufferLocation = resource->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = TYPE_SIZE_INDEX_BUFFER * _indexCount;
	}

	Impl_IndexBuffer::~Impl_IndexBuffer()
	{
	}

	void Impl_IndexBuffer::Mapping(
		void* _pIndices,
		UINT _indexCount
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
				_pIndices,
				TYPE_SIZE_INDEX_BUFFER * _indexCount
			);

			//アンマップ
			resource->Unmap(0, nullptr);
		}
	}

	void Impl_IndexBuffer::Set(ID3D12GraphicsCommandList* pCmdList)
	{
		pCmdList->IASetIndexBuffer(&view);
	}

	UINT Impl_IndexBuffer::GetIndexCount()
	{
		return indexCount;
	}
}

BOOL CreateIndexBuffer(
	IndexBuffer** _ppIndexBuffer,
	void* _pIndices,
	UINT _indexCount
)
{
	(*_ppIndexBuffer) = new DX12::Impl_IndexBuffer(_pIndices, _indexCount);

	return TRUE;
}
