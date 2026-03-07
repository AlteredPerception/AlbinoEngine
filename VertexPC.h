#pragma once
#include <DirectXMath.h>

struct VertexPC
{
	VertexPC()
		: point(0.0f, 0.0f, 0.0f),
		color(1.0f, 1.0f, 1.0f, 1.0f) {}

	VertexPC(float x, float y, float z,
		float r, float g, float b, float a)
		: point(x, y, z),
		color(r, g, b, a)
	{

	}
	DirectX::XMFLOAT3 point;
	DirectX::XMFLOAT4 color;
};