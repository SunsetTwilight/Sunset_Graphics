#include "ShaderResourceView.h"
#include "../DirectXTex/DirectXTex/DirectXTex.h"

#include "Command.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;
	extern Command* m_pCopyCommand;

	class Impl_ShaderResourceView :
		public ShaderResourceView
	{
	public:
		Impl_ShaderResourceView();
		~Impl_ShaderResourceView();

		Impl_ShaderResourceView(UINT num);

		ID3D12DescriptorHeap* GetDescriptorHeap() override;
		ID3D12DescriptorHeap** GetAddressDescriptorHeap() override;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT bbIndex = 0) override;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT bbIndex = 0) override;

		UINT GetDescriptorHandleIncrementSize() override;

		void CreateCommitedResourceFromTexMetadata(DirectX::TexMetadata metadata);
		void Create();

		void SetRowPitch(size_t row_pitch);

		DescriptorHeap* m_pHeap;

		ComPtr<ID3D12Resource> m_srv;

		size_t m_RowPitch;
	};

	Impl_ShaderResourceView::Impl_ShaderResourceView()
	{
		if (CreateDescriptorHeap(
			(DescriptorHeap**)(&m_pHeap),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		) != S_OK) {
			return;
		}
	}

	Impl_ShaderResourceView::~Impl_ShaderResourceView()
	{
		if (m_pHeap) {
			delete(m_pHeap);
			m_pHeap = nullptr;
		}
	}

	Impl_ShaderResourceView::Impl_ShaderResourceView(UINT num)
	{
		if (CreateDescriptorHeap(
			(DescriptorHeap**)(&m_pHeap),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			num,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		) != S_OK) {
			return;
		}
	}

	ID3D12DescriptorHeap* Impl_ShaderResourceView::GetDescriptorHeap()
	{
		return m_pHeap->GetDescriptorHeap();
	}

	ID3D12DescriptorHeap** Impl_ShaderResourceView::GetAddressDescriptorHeap()
	{
		return m_pHeap->GetAddressDescriptorHeap();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Impl_ShaderResourceView::GetCPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetCPUDescriptorHandle(bbIndex);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE Impl_ShaderResourceView::GetGPUDescriptorHandle(UINT bbIndex)
	{
		return m_pHeap->GetGPUDescriptorHandle(bbIndex);
	}

	UINT Impl_ShaderResourceView::GetDescriptorHandleIncrementSize()
	{
		return m_pHeap->GetDescriptorHandleIncrementSize();
	}

	void Impl_ShaderResourceView::CreateCommitedResourceFromTexMetadata(DirectX::TexMetadata metadata)
	{
		D3D12_HEAP_PROPERTIES prop;
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width;
		desc.Height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
		desc.Width = static_cast<UINT64>(metadata.width);
		desc.Height = static_cast<UINT>(metadata.height);
		desc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
		desc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
		desc.Format = metadata.format;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		HRESULT hr = DX12::g_d3d12_device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_srv.GetAddressOf())
		);
	}

	void Impl_ShaderResourceView::Create()
	{
		D3D12_RESOURCE_DESC desc = m_srv->GetDesc();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = static_cast<UINT>(desc.MipLevels);

		DX12::g_d3d12_device->CreateShaderResourceView(m_srv.Get(), &srvDesc, m_pHeap->GetCPUDescriptorHandle());
	}

	void Impl_ShaderResourceView::SetRowPitch(size_t row_pitch)
	{
		m_RowPitch = row_pitch;
	}
}

BOOL CreateShaderResourceView(
	ShaderResourceView** ppShaderResourceView, 
	UINT num
)
{
	DX12::Impl_ShaderResourceView* pImplShaderResourceView = new DX12::Impl_ShaderResourceView(num);

	(*ppShaderResourceView) = pImplShaderResourceView;
	return TRUE;
}

BOOL CreateShaderResourceView(
	ShaderResourceView** ppShaderResourceView,
	DirectX::TexMetadata metadata,
	size_t row_pitch
)
{
	DX12::Impl_ShaderResourceView* pImplShaderResourceView = new DX12::Impl_ShaderResourceView(1);
	pImplShaderResourceView->CreateCommitedResourceFromTexMetadata(metadata);
	pImplShaderResourceView->SetRowPitch(row_pitch);

	(*ppShaderResourceView) = pImplShaderResourceView;
	return TRUE;
}

BOOL LoadShaderResourceView(
	ShaderResourceView** ppShaderResourceView, 
	const wchar_t* szFile, 
	Command* pCommand)
{
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;

	HRESULT hr = DirectX::LoadFromWICFile(
		szFile,
		DirectX::WIC_FLAGS_NONE,
		&metadata,
		image
	);

	const DirectX::Image* pImage = image.GetImage(0, 0, 0);

	// ヒープのプロパティ
	D3D12_HEAP_PROPERTIES prop;
	{
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;						// cpuは書き込み、gpuは読み取り
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// プロパティ不明
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// プール不明
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;
	}

	// リソースの設定
	D3D12_RESOURCE_DESC desc;
	{
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = pImage->slicePitch;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;  // なし
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;   // データを連続して配置する
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;  // なし
	}

	ComPtr<ID3D12Resource> m_pTextureUpload;

	// データアップロード用のリソースを作成
	hr = DX12::g_d3d12_device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pTextureUpload)
	);

	UINT8* pDataBegin;

	hr = m_pTextureUpload->Map(0, nullptr, reinterpret_cast<void**>(&pDataBegin));
	memcpy(pDataBegin, pImage->pixels, pImage->slicePitch);
	m_pTextureUpload->Unmap(0, nullptr);

	/* コピー先リソース作成 */
	CreateShaderResourceView(ppShaderResourceView, metadata, pImage->rowPitch);


	pCommand->Begin();
	ID3D12GraphicsCommandList* pCmdList = pCommand->GetCommandList();
	
	ComPtr<ID3D12Resource> resource = ((DX12::Impl_ShaderResourceView*)(*ppShaderResourceView))->m_srv;
	desc = resource->GetDesc();

	D3D12_TEXTURE_COPY_LOCATION dest;
	dest.pResource = resource.Get();
	dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dest.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION src;
	src.pResource = m_pTextureUpload.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Format = desc.Format;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(desc.Width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(desc.Height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(desc.DepthOrArraySize);
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(pImage->rowPitch);

	// テクスチャのコピー
	pCmdList->CopyTextureRegion(
		&dest,
		0,      // 左上のx座標
		0,      // 左上のy座標
		0,      // 左上のz座標
		&src,
		nullptr // 3Dボックスの設定
	);

	// リソースの状態を遷移させる
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource.Get();

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pCmdList->ResourceBarrier(1, &barrier);

	hr = pCmdList->Close();

	ID3D12CommandList* ppCmdLists[] = { pCmdList };
	pCommand->GetCommandQueue()->ExecuteCommandLists(1, ppCmdLists);

	pCommand->Wait(nullptr);
	
	((DX12::Impl_ShaderResourceView*)(*ppShaderResourceView))->Create();
	
	// コピーへ移行
	//m_State = STATE_COPY;

	return TRUE;
}
