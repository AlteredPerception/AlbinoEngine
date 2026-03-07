#pragma once

#include <DirectXMath.h>

namespace AlbinoEngine
{
	struct VertexPNT
	{
	
		VertexPNT() {
			position = { 0.0f, 0.0f, 0.0f };
			normal = { 0.0f,0.0f,0.0f };
			uv = { 0.0f, 0.0f };
		}

		VertexPNT(
			float xPos, float yPos, float zPos,
			float xNom, float yNom, float zNom,
			float u, float v)
		{
			position = { xPos,yPos,zPos };
			normal = { xNom,yNom,zNom };
			uv = { u,v };
		}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};
}