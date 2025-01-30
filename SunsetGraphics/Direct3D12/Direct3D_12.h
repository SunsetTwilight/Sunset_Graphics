#ifndef _DIRECT3D_12
#define _DIRECT3D_12

#include "../sunset_graphics_pch.h"

#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>

#include <string.h>

//#include "ShaderVariable.h"
//#include "Shader.h"


namespace DX12
{
//------------------------------------------------------------------------------------------------
//関数プロトタイプ宣言	
//------------------------------------------------------------------------------------------------

	BOOL Direct3D_12_LoadLibraryAndModule();	//D3D12.dllと関連モジュールの読み込み

	BOOL Direct3D_12_Init();		//DirectX12系の初期化
	BOOL Direct3D_12_CleanUp();		//DirectX12系の解放

	BOOL EnableDebugLayer(); //デバックレイヤーを有効化

	//デバイスの作成
	BOOL CreateDevice(
		D3D_FEATURE_LEVEL minimumLevel = D3D_FEATURE_LEVEL_11_0
	);

	BOOL CreateCommandQueue(
		D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT
	);

	SUNSET_GRAPHICS_API ID3D12Device* GetDevice();
	SUNSET_GRAPHICS_API ID3D12CommandQueue* GetCommandQueue();

	/*struct
	{
		float pos;
		float normal;
		float texcoord;
	} 
	typedef Vertex;
	typedef UINT Index;

	struct VertexBuffer
	{
		ComPtr<ID3D12Resource> resource;
		D3D12_VERTEX_BUFFER_VIEW vbView;
	};
	struct IndexBuffer
	{
		ComPtr<ID3D12Resource> resource;
		D3D12_INDEX_BUFFER_VIEW  ibView;
	};
	

	void CreateBuffer(
		ID3D12Resource** ppResource,
		UINT bufferSize
	);

	void CreateVertexBuffer(
		VertexBuffer* vertexBuffer,
		Vertex* vertices
	);
	void CreateIndexBuffer(
		IndexBuffer* indexBuffer,
		Index* indices
	);
	
	class InputLayout
	{
	public:
		InputLayout();
		~InputLayout();

		void CreateElement(ShaderReflectionD3D12* pShaderReflection);

		D3D12_INPUT_LAYOUT_DESC GetDesc();

	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> vbElement;
	};

	class VertexShader : public ShaderD3D12
	{
	public:
		BOOL CreateVertexShader(LPCWSTR pFileName);

	private:
		InputLayout inputLayout;
	};

	
	
	class RootSignature
	{
	public:
		BOOL Create();


	private:
		D3D12_ROOT_SIGNATURE_DESC desc;

		ComPtr<ID3D12RootSignature> root_signature;

		friend class GraphicsPipeline;
	};

	class ShaderD3D12;

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline();
		~GraphicsPipeline();

		void SetRootSignature(RootSignature* pRootSignature);

		void SetShader(ShaderD3D12* pShader);

		void SetBlend(D3D12_BLEND_DESC blend);
		void SetBlend(D3D12_BLEND_DESC blend, UINT sampleMask);
		void SetRasterizer(D3D12_RASTERIZER_DESC rasterizer);
		void SetDepthStencil(D3D12_DEPTH_STENCIL_DESC depthStencil);
		void SetInputLayout(D3D12_INPUT_LAYOUT_DESC desc);

		void SetPromitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

		void SetRTVFormat(DXGI_FORMAT format, UINT index);
		void SetDSVFormat(DXGI_FORMAT format);

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;

		ComPtr<ID3D12RootSignature> root_signature;

		ComPtr<ID3DBlob> vsBlob;
		ComPtr<ID3DBlob> psBlob;
		ComPtr<ID3DBlob> gsBlob;
		ComPtr<ID3DBlob> hsBlob;
		ComPtr<ID3DBlob> dsBlob;

		ComPtr<ID3D12PipelineState> pipeline_state;
	};*/

}

#endif // !_DIRECT3D_12
