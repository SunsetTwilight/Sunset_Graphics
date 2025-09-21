#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <DirectXMath.h>
#include <d3d12.h>

struct SpriteBuffer 
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
};



SpriteBuffer spritebuffer[] = {
	{ { +0.5f, +0.0f, 0.0f }, { 1.0f, 1.0f } },
	{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
	{ { -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f } },
};

#endif // !_VERTEX_H_

