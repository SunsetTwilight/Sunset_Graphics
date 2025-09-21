#include "Direct3D_12.h"

#include "Command.h"
#include "../DXGI/DirectX_GI.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <stdexcept>
#include <vector>

//#pragma comment(lib, "d3dcompiler.lib")

//#include "Shader.h"

namespace DXGI
{
	extern void SetUseAdapter(IDXGIAdapter1* adapter);
}

namespace DX12
{
//------------------------------------------------------------------------------------------------
//グローバル変数宣言
//------------------------------------------------------------------------------------------------

	HMODULE	m_D3D12_DLL;

	PFN_D3D12_SERIALIZE_ROOT_SIGNATURE						D3D12SerializeRootSignature = nullptr;
	PFN_D3D12_CREATE_ROOT_SIGNATURE_DESERIALIZER			D3D12CreateRootSignatureDeserializer;
	PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE			D3D12SerializeVersionedRootSignature;
	PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER	D3D12CreateVersionedRootSignatureDeserializer;
	
	PFN_D3D12_CREATE_DEVICE									D3D12CreateDevice = nullptr;
	PFN_D3D12_GET_DEBUG_INTERFACE							D3D12GetDebugInterface = nullptr;

	PFN_D3D12_GET_INTERFACE									D3D12GetInterface = nullptr;
	
	ComPtr<ID3D12Debug>			m_d3d12_debug{};
	ComPtr<ID3D12Device>		g_d3d12_device{};	//Direct3D12のデバイスの実体
	ComPtr<ID3D12CommandQueue>	cmd_queue{};

	Command* m_pCopyCommand = nullptr;		/* リソースコピー用コマンド */

//------------------------------------------------------------------------------------------------
//関数定義
//------------------------------------------------------------------------------------------------

	BOOL Direct3D_12_LoadLibraryAndModule()	//D3D12.dllと関連モジュールの読み込み
	{
		//D3D12.dllの読み込み
		m_D3D12_DLL = LoadLibrary(L"D3D12.dll");
		if (m_D3D12_DLL == NULL) {
			//DLLの読み込み：失敗
			MessageBox(NULL, L"ErrorCode_0002 : Load library D3D12.dll", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0002                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0002);

			return FALSE;
		}

		//関数を読み込む
		D3D12CreateDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(m_D3D12_DLL, "D3D12CreateDevice");
		if (D3D12CreateDevice == NULL) {
			//関数の読み込み：失敗
			MessageBox(NULL, L"ErrorCode_0003 : Get Process D3D12CreateDevice()", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0003                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0003);

			return FALSE;
		}

		//関数を読み込む
		D3D12GetDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(m_D3D12_DLL, "D3D12GetDebugInterface");
		if (D3D12GetDebugInterface == NULL) {
			//関数の読み込み：失敗
			MessageBox(NULL, L"ErrorCode_0003 : Get Process D3D12GetDebugInterface()", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0003                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0003);

			return FALSE;
		}

		D3D12SerializeRootSignature = (PFN_D3D12_SERIALIZE_ROOT_SIGNATURE)GetProcAddress(m_D3D12_DLL, "D3D12SerializeRootSignature");
		if (D3D12SerializeRootSignature == NULL) {
			//関数の読み込み：失敗
			MessageBox(NULL, L"ErrorCode_0003 : Get Process D3D12SerializeRootSignature()", L"Failed                                                                                                                         ", MB_OK | MB_ICONERROR);
			MessageBox(NULL, L"Exit Application", L"ErrorCode : 0003                                                                                                               ", MB_OK | MB_ICONERROR);
			exit(0003);

			return FALSE;
		}

		return TRUE;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　Direct3D 12系の初期化用関数
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
	//------------------------------------------------------------------------------------------------
	BOOL Direct3D_12_Init()
	{
		BOOL retInit = TRUE;

		if (SGraphics::isDebug) {
			if(!(EnableDebugLayer())) {
				retInit = FALSE;


			}
		}
		if (!(CreateDevice(D3D_FEATURE_LEVEL_11_0))) {
			retInit = FALSE;
		}

		CreateCommand_TypeCopy(&m_pCopyCommand);

		return retInit;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　Direct3D 12系の終了時解放関数
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
	//------------------------------------------------------------------------------------------------
	BOOL Direct3D_12_CleanUp()
	{
		BOOL retCleanUp = TRUE;	//正常（初期値）

		if (m_d3d12_debug.Reset() != 0) {
			//Error(参照カウントが１以上なので、どこかで使用されたまま)
			retCleanUp = FALSE;	//異常
		}
		if (g_d3d12_device.Reset() != 0) {
			//Error(参照カウントが１以上なので、どこかで使用されたまま)
			retCleanUp = FALSE;	//異常
		}

		return retCleanUp;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　Direct3D 12系デバッグレイヤーの有効化
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
	//------------------------------------------------------------------------------------------------
	BOOL EnableDebugLayer()
	{
		BOOL retEnable = TRUE;

#if defined(_DEBUG)
		HRESULT hr = S_OK;

		if (D3D12GetDebugInterface != nullptr) {

			ComPtr<ID3D12Debug> debugController;
			hr = D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if (FAILED(hr)) {
				retEnable = FALSE;
			}

			if (retEnable) {
				debugController->EnableDebugLayer();

				ComPtr<ID3D12Debug3> debug3;
				if (SUCCEEDED(debugController.As(&debug3))) {
					debug3->SetEnableGPUBasedValidation(TRUE);	//GPU ベースの検証
				}
			}
		}
		else {
			retEnable = FALSE;
		}
#endif	
		return retEnable;
	}

	//------------------------------------------------------------------------------------------------
	// 内容　　Direct3D 12系デバイスの生成
	// 引数　　void（なし）
	// 戻り値　BOOL TRUE(1):正常終了　FALSE(0):失敗、異常終了
	//------------------------------------------------------------------------------------------------
	BOOL CreateDevice(
		D3D_FEATURE_LEVEL minimumLevel
	)
	{
		HRESULT hr = S_OK;

		ComPtr<IDXGIAdapter1> adapter1{};

		BOOL isGetAdapterForName = FALSE;

#if 0 //GPUNameが設定されているかどうか（されている場合はsearch）
		{
			//名前ベースでGPUを探す
			ComPtr<IDXGIAdapter> adapter{};
			if (DXGI::GetAdapterForName(adapter.GetAddressOf(), L"NVIDIA")) {
				adapter.As(&adapter1);
				isGetAdapterForName = TRUE;
			}
		}
#endif	

		if (!isGetAdapterForName)
		{
			//名前ベースでGPUを見つけれなかった。

			//機能レベルをベースにHardwareAdapterを見つける
			ComPtr<IDXGIAdapter1> adapter{};
			if (DXGI::GetAdapterForHardware(&adapter, minimumLevel)) {
				adapter.As(&adapter1);
			}
			else throw std::runtime_error("Failed CreateDevice() Adapter No Find");
		}
		/*else 
		{

		}*/

		//デバイスの生成
		hr = D3D12CreateDevice(
			adapter1.Get(),
			minimumLevel,
			IID_PPV_ARGS(g_d3d12_device.GetAddressOf())
		);
		if ( FAILED(hr) ) { 
			//生成失敗
			throw std::runtime_error("Failed CreateDevice!");
		}

		DXGI::SetUseAdapter(adapter1.Get());
		return TRUE;
	}

	SUNSET_GRAPHICS_API ID3D12Device* GetDevice()
	{
		return g_d3d12_device.Get();
	}

	SUNSET_GRAPHICS_API ID3D12CommandQueue* GetCommandQueue()
	{
		return cmd_queue.Get();
	}

	BOOL CreateCommandQueue(
		D3D12_COMMAND_LIST_TYPE type
	)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 1;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cmdQueueDesc.Type = type;

		HRESULT hr = g_d3d12_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmd_queue.GetAddressOf()));
		_ASSERT_EXPR(FAILED(hr), hr_trace(hr));
		
		return TRUE;
	}

//	void CreateBuffer(
//		ID3D12Resource** ppResource,
//		UINT bufferSize
//	)
//	{
//		D3D12_HEAP_PROPERTIES heap_prop{};
//		heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
//		heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//		heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//
//		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
//
//		HRESULT hr = m_d3d12_device->CreateCommittedResource(
//			&heap_prop,
//			D3D12_HEAP_FLAG_NONE,
//			&desc,
//			D3D12_RESOURCE_STATE_GENERIC_READ,
//			nullptr,
//			IID_PPV_ARGS(ppResource)
//		);
//		_ASSERT_EXPR(FAILED(hr), hr_trace(hr));
//	}
//
//	void CreateVertexBuffer(
//		VertexBuffer* vertexBuffer,
//		Vertex* vertices
//	)
//	{
//		CreateBuffer(
//			vertexBuffer->resource.GetAddressOf(),
//			sizeof(vertices)
//		);
//
//		Vertex* Map = nullptr;
//		HRESULT hr = vertexBuffer->resource->Map(0, nullptr, (void**)Map);
//		_ASSERT_EXPR(FAILED(hr), hr_trace(hr));
//
//		std::memcpy(Map, vertices, sizeof(vertices));
//		vertexBuffer->resource->Unmap(0, nullptr);
//
//		vertexBuffer->vbView.BufferLocation = vertexBuffer->resource->GetGPUVirtualAddress();
//		vertexBuffer->vbView.SizeInBytes = sizeof(vertices);
//		vertexBuffer->vbView.StrideInBytes = sizeof(vertices[0]);
//	}
//
//	void CreateIndexBuffer(
//		IndexBuffer* indexBuffer,
//		Index* indices
//	)
//	{
//		CreateBuffer(
//			indexBuffer->resource.GetAddressOf(),
//			sizeof(indices)
//		);
//
//		Index* Map = nullptr;
//		HRESULT hr = indexBuffer->resource->Map(0, nullptr, (void**)Map);
//		_ASSERT_EXPR(FAILED(hr), hr_trace(hr));
//
//		std::memcpy(Map, indices, sizeof(indices));
//		indexBuffer->resource->Unmap(0, nullptr);
//
//		indexBuffer->ibView.BufferLocation = indexBuffer->resource->GetGPUVirtualAddress();
//		indexBuffer->ibView.Format = DXGI_FORMAT_R32_UINT;
//		indexBuffer->ibView.SizeInBytes = sizeof(indices);
//	}
//
//	
//
//
//
//
//	
//
//	InputLayout::InputLayout()
//	{
//
//	}
//
//	InputLayout::~InputLayout()
//	{
//
//	}
//
//	void InputLayout::CreateElement(ShaderReflectionD3D12* pShaderReflection)
//	{
//		vbElement.clear();
//
//		D3D12_SHADER_DESC shader_desc = pShaderReflection->GetShaderDesc();
//	
//		ComPtr<ID3D12ShaderReflection> ref = pShaderReflection->GetShaderReflection();
//		for (u_int i = 0; i < shader_desc.InputParameters; ++i)
//		{
//			D3D12_SIGNATURE_PARAMETER_DESC sigdesc;
//			ref->GetInputParameterDesc(i, &sigdesc);
//
//			DXGI_FORMAT format;
//
//			switch (sigdesc.Mask)
//			{
//			case 0b1111:
//				switch (sigdesc.ComponentType)
//				{
//				case D3D_REGISTER_COMPONENT_FLOAT32:
//					format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//					break;
//				case D3D_REGISTER_COMPONENT_UINT32:
//					format = DXGI_FORMAT_R32G32B32A32_UINT;
//					break;
//				case D3D_REGISTER_COMPONENT_SINT32:
//					format = DXGI_FORMAT_R32G32B32A32_SINT;
//					break;
//				default:
//					break;
//				}
//				break;
//			case 0b0111:
//				switch (sigdesc.ComponentType)
//				{
//				case D3D_REGISTER_COMPONENT_FLOAT32:
//					format = DXGI_FORMAT_R32G32B32_FLOAT;
//					break;
//				case D3D_REGISTER_COMPONENT_UINT32:
//					format = DXGI_FORMAT_R32G32B32_UINT;
//					break;
//				case D3D_REGISTER_COMPONENT_SINT32:
//					format = DXGI_FORMAT_R32G32B32_SINT;
//					break;
//				default:
//					break;
//				}
//				break;
//			case 0b0011:
//				switch (sigdesc.ComponentType)
//				{
//				case D3D_REGISTER_COMPONENT_FLOAT32:
//					format = DXGI_FORMAT_R32G32_FLOAT;
//					break;
//				case D3D_REGISTER_COMPONENT_UINT32:
//					format = DXGI_FORMAT_R32G32_UINT;
//					break;
//				case D3D_REGISTER_COMPONENT_SINT32:
//					format = DXGI_FORMAT_R32G32_SINT;
//					break;
//				default:
//					break;
//				}
//				break;
//			case 0b0001:
//				switch (sigdesc.ComponentType)
//				{
//				case D3D_REGISTER_COMPONENT_FLOAT32:
//					format = DXGI_FORMAT_R32_FLOAT;
//					break;
//				case D3D_REGISTER_COMPONENT_UINT32:
//					format = DXGI_FORMAT_R32_UINT;
//					break;
//				case D3D_REGISTER_COMPONENT_SINT32:
//					format = DXGI_FORMAT_R32_SINT;
//					break;
//				default:
//					break;
//				}
//				break;
//			default:
//				break;
//			}
//
//			D3D12_INPUT_ELEMENT_DESC eledesc = {
//					sigdesc.SemanticName,
//					sigdesc.SemanticIndex,
//					format,
//					0,
//					D3D12_APPEND_ALIGNED_ELEMENT,
//					D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
//					0
//			};
//			vbElement.push_back(eledesc);
//		}
//	}
//
//	D3D12_INPUT_LAYOUT_DESC InputLayout::GetDesc()
//	{
//		D3D12_INPUT_LAYOUT_DESC desc;
//		desc.pInputElementDescs = vbElement.data();
//		desc.NumElements = (UINT)vbElement.size();
//
//		return desc;
//	}
//
//	BOOL VertexShader::CreateVertexShader(LPCWSTR pFileName)
//	{
//		CreateVertexShaderEx(pFileName);
//
//		code.Reflect(shader_reflection.GetAddressShaderReflection());
//		inputLayout.CreateElement(&shader_reflection);
//
//		return TRUE;
//	}
//
//	BOOL RootSignature::Create()
//	{
//		ComPtr<ID3DBlob> blob;
//		ComPtr<ID3DBlob> pErrorMsgs;
//
//		HRESULT hr = D3D12SerializeRootSignature(
//			&desc,
//			D3D_ROOT_SIGNATURE_VERSION_1_0,
//			&blob,
//			&pErrorMsgs
//		);
//
//		if (FAILED(hr))
//		{
//			if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
//			{
//				std::string errstr;
//
//				errstr.resize(pErrorMsgs->GetBufferSize());
//				std::copy_n((char*)pErrorMsgs->GetBufferPointer(),
//					pErrorMsgs->GetBufferSize(),
//					errstr.begin()
//				);
//
//				OutputDebugStringA(errstr.c_str());
//			}
//
//			return FALSE;
//		}
//
//		hr = m_d3d12_device->CreateRootSignature(
//			0,
//			blob->GetBufferPointer(),
//			blob->GetBufferSize(),
//			IID_PPV_ARGS(root_signature.GetAddressOf())
//		);
//
//
//
//		return TRUE;
//	}
//
//	GraphicsPipeline::GraphicsPipeline()
//	{
//		desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//
//		desc.NodeMask = 0;
//
//		desc.CachedPSO.pCachedBlob = NULL;
//		desc.CachedPSO.CachedBlobSizeInBytes = 0;
//
//#if _DEBUG
//		desc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
//#else
//		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
//#endif
//	}
//
//	GraphicsPipeline::~GraphicsPipeline()
//	{
//	}
//
//	void GraphicsPipeline::SetRootSignature(RootSignature* pRootSignature)
//	{
//		root_signature = pRootSignature->root_signature;
//	}
//
//	void GraphicsPipeline::SetShader(ShaderD3D12* pShader)
//	{
//		switch (pShader->type)
//		{
//		case ShaderType::Vertex:
//			//vsBlob = pShader->code.GetCode();
//			desc.VS = CD3DX12_SHADER_BYTECODE(
//				pShader->code.GetBufferPointer(),
//				pShader->code.GetBufferSize()
//			);
//			break;
//		case ShaderType::Pixcel:
//			//psBlob = pShader->code.GetCode();
//			desc.PS = CD3DX12_SHADER_BYTECODE(
//				pShader->code.GetBufferPointer(),
//				pShader->code.GetBufferSize()
//			);
//			break;
//		case ShaderType::Geometry:
//			//gsBlob = pShader->code.GetCode();
//			desc.GS = CD3DX12_SHADER_BYTECODE(
//				pShader->code.GetBufferPointer(),
//				pShader->code.GetBufferSize()
//			);
//			break;
//		case ShaderType::Hull:
//			//hsBlob = pShader->code.GetCode();
//			desc.HS = CD3DX12_SHADER_BYTECODE(
//				pShader->code.GetBufferPointer(),
//				pShader->code.GetBufferSize()
//			);
//			break;
//		case ShaderType::Domain:
//			//dsBlob = pShader->code.GetCode();
//			desc.DS = CD3DX12_SHADER_BYTECODE(
//				pShader->code.GetBufferPointer(),
//				pShader->code.GetBufferSize()
//			);
//			break;
//		default:
//			break;
//		}
//	}
//	
//	void GraphicsPipeline::SetBlend(D3D12_BLEND_DESC blend)
//	{
//		desc.BlendState = blend;
//	}
//
//	void GraphicsPipeline::SetBlend(D3D12_BLEND_DESC blend, UINT sampleMask)
//	{
//		desc.BlendState = blend;
//		desc.SampleMask = sampleMask;
//	}
//
//	void GraphicsPipeline::SetRasterizer(D3D12_RASTERIZER_DESC rasterizer)
//	{
//		desc.RasterizerState = rasterizer;
//	}
//
//	void GraphicsPipeline::SetDepthStencil(D3D12_DEPTH_STENCIL_DESC depthStencil)
//	{
//		desc.DepthStencilState = depthStencil;
//	}
//
//	void GraphicsPipeline::SetInputLayout(D3D12_INPUT_LAYOUT_DESC inputLayout)
//	{
//		desc.InputLayout = inputLayout;
//	}
//
//	void GraphicsPipeline::SetPromitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
//	{
//		desc.PrimitiveTopologyType = type;
//	}
//
//	void GraphicsPipeline::SetRTVFormat(DXGI_FORMAT format, UINT index)
//	{
//		if (8 <= index) return;
//
//		desc.RTVFormats[index] = format;
//		if (desc.NumRenderTargets <= index) desc.NumRenderTargets = index;
//	}
//
//	void GraphicsPipeline::SetDSVFormat(DXGI_FORMAT format)
//	{
//		desc.DSVFormat = format;
//	}
}
