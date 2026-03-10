#pragma once

#include <cstddef>
#include <cstring>
#include <dxgiformat.h>
#include <DirectXMath.h>

namespace AlbinoEngine
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position{}; // POSITION
		DirectX::XMFLOAT3 normal{};   // NORMAL
		DirectX::XMFLOAT4 tangent{};  // TANGENT (w = handedness)
		DirectX::XMFLOAT2 uv{};       // TEXCOORD0
		DirectX::XMFLOAT4 color{};    // COLOR


	};
	
	struct VertexElementInfo
	{
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;	// Element format;
		UINT offset = 0;					        // Element offset;
	};

	inline bool GetVertexElementInfo(const char* semantic, UINT semanticIndex, VertexElementInfo& out)
	{
		if (_stricmp(semantic, "POSITION") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32B32_FLOAT, static_cast<UINT>(offsetof(Vertex, position)) };
			return true;
		}
		if (_stricmp(semantic, "NORMAL") == 0 && semanticIndex == 0)
		{
			out = {DXGI_FORMAT_R32G32B32_FLOAT, static_cast<UINT>(offsetof(Vertex, normal)) };
			return true;
		}
		if (_stricmp(semantic, "TANGENT") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<UINT>(offsetof(Vertex, tangent)) };
			return true;
		}
		if (_stricmp(semantic, "TEXCOORD") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32_FLOAT, static_cast<UINT>(offsetof(Vertex, uv)) };
			return true;
		}
		if (_stricmp(semantic, "COLOR") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<UINT>(offsetof(Vertex,color)) };
			return true;
		}
		return false;
	}

	inline Vertex positionUV(float x, float y, float z, float u, float v)
	{
		Vertex vert{};
		vert.position = { x,y,z };
		vert.normal = { 0.0f, 1.0f, 0.0f };
		vert.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		vert.uv = { u,v };
		vert.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		return vert;
	}

	inline Vertex positionNormalUV(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		Vertex vert{};

		vert.position = { x,y,z };
		vert.normal = { nx, ny, nz };
		vert.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		vert.uv = { u,v };
		vert.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		return vert;
	}

	inline Vertex positionColor(
		float x, float y, float z,
		float r, float b, float g, float a)
	{
		Vertex vert{};
		vert.position = { x,y,z };
		vert.normal = { 0.0f, 1.0f, 0.0f };
		vert.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		vert.uv = { 0.0f,0.0f };
		vert.color = { r,g,b,a };
		return vert;
	}

	inline Vertex positionNormalUVColor(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v,
		float r, float g, float b, float a)
	{
		Vertex vert{};
		vert.position = { x,y,z };
		vert.normal = { nx,ny, nz };
		vert.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		vert.uv = { u,v };
		vert.color = { r,g,b,a };
		return vert;
	}
}
