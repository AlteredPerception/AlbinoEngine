#pragma once

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

	// Convenience helpers. These keep Vertex a simple POD-like type while
	// making geometry generation concise.

	static Vertex positionUV(
		float xPos, float yPos, float zPos,
		float u, float v)
	{
		Vertex out{};
		out.position = { xPos, yPos, zPos };
		out.uv = { u, v };
		out.normal = { 0.0f, 1.0f, 0.0f };
		out.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		out.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		return out;
	}

	static Vertex positionNormalUV(
		float xPos, float yPos, float zPos,
		float nX, float nY, float nZ,
		float u, float v)
	{
		Vertex out{};
		out.position = { xPos, yPos, zPos };
		out.normal = { nX, nY, nZ };
		out.uv = { u, v };
		out.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		out.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		return out;
	}

	static Vertex positionColor(
		float xPos, float yPos, float zPos,
		float r, float g, float b, float a = 1.0f)
	{
		Vertex out{};
		out.position = { xPos, yPos, zPos };
		out.normal = { 0.0f, 1.0f, 0.0f };
		out.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		out.uv = { 0.0f, 0.0f };
		out.color = { r, g, b, a };
		return out;
	}

	// Handy for debugging lighting + UVs at the same time (e.g., output normal/uv in PS).
	static Vertex positionNormalUVColor(
		float xPos, float yPos, float zPos,
		float nX, float nY, float nZ,
		float u, float v,
		float r, float g, float b, float a = 1.0f)
	{
		Vertex out{};
		out.position = { xPos, yPos, zPos };
		out.normal = { nX, nY, nZ };
		out.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
		out.uv = { u, v };
		out.color = { r, g, b, a };
		return out;
	}

	struct VertexElementInfo
	{
		DXGI_FORMAT format;	// Element format;
		UINT offset;		// Element offset;
	};

	inline bool GetVertexElementInfo(const char* semantic, UINT semanticIndex, VertexElementInfo& out)
	{
		if (_stricmp(semantic, "POSITION") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32B32A32_FLOAT, (UINT)offsetof(Vertex, position) };
			return true;
		}
		if (_stricmp(semantic, "NORMAL") == 0 && semanticIndex == 0)
		{
			out = {DXGI_FORMAT_R32G32B32_FLOAT, (UINT)offsetof(Vertex, normal)};
			return true;
		}
		if (_stricmp(semantic, "TANGENT") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32B32A32_FLOAT, (UINT)offsetof(Vertex, tangent) };
			return true;
		}
		if (_stricmp(semantic, "TEXCOORD") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32_FLOAT, (UINT)offsetof(Vertex, uv) };
			return true;
		}
		if (_stricmp(semantic, "COLOR") == 0 && semanticIndex == 0)
		{
			out = { DXGI_FORMAT_R32G32B32A32_FLOAT, (UINT)offsetof(Vertex,color) };
			return true;
		}
		return false;
	}
}
