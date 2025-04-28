#include "VertexBuffer.h"

namespace DX12
{
	extern ComPtr<ID3D12Device>	g_d3d12_device;

	VertexBuffer::VertexBuffer()
	{
	}

	VertexBuffer::~VertexBuffer() 
	{
	}

	void VertexBuffer::CreateVertexBuffer(
		UINT _typeSize, 
		void* _pVertice, 
		UINT _vertexCount
	)
	{
		//�v���p�e�B�ݒ�p�\���̂̐ݒ�
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		//���\�[�X�ݒ�p�\���̂̐ݒ�
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

		//���\�[�X�̐���
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

		//���_�o�b�t�@�ݒ�p�\���̂̐ݒ�
		view.BufferLocation = resource->GetGPUVirtualAddress();
		view.SizeInBytes = typeSize * vertexCount;
		view.StrideInBytes = typeSize;
	}

	void VertexBuffer::Mapping(
		void* _pVertice, 
		UINT _vertexCount
	)
	{
		//���M�͈�
		D3D12_RANGE range = { 0,0 };

		//�}�b�s���O
		UINT8* data;
		HRESULT hr = resource->Map(0, &range, reinterpret_cast<void**>(&data));

		//���_�f�[�^�̃R�s�[
		memcpy(data, _pVertice, typeSize * _vertexCount);

		//�A���}�b�v
		resource->Unmap(0, nullptr);
	}

	void VertexBuffer::Draw(ID3D12GraphicsCommandList* pCmdList)
	{
		//���_�o�b�t�@�r���[�̃Z�b�g
		pCmdList->IASetVertexBuffers(0, 1, &view);

		//�g�|���W�[�^�C�v�̃Z�b�g
		pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�`��
		pCmdList->DrawInstanced(vertexCount, 1, 0, 0);
	}
}
