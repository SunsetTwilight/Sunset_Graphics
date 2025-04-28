#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <DirectXMath.h>
#include <d3d12.h>

struct SpriteBuffer 
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
};

D3D12_INPUT_ELEMENT_DESC inputElementsSpriteBuffer[]{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

SpriteBuffer spritebuffer[] = {
	{ { +0.5f, +0.0f, 0.0f }, { 1.0f, 1.0f } },
	{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
	{ { -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f } },
};

#endif // !_VERTEX_H_

