#pragma once
#include <DirectXMath.h>

namespace AlbinoEngine
{
	struct DirectionalLight
	{
		DirectX::XMFLOAT3 direction = { 0.0f, -1.0f, 0.0f };
		float padding0 = 0.0f;

		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;

		bool enable = true;
		bool castShadows = true;
	};

	struct PointLight
	{
		DirectX::XMFLOAT3 position = { 0.0f, 2.0f, 0.0f };
		float range = 10.0f;

		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;

		bool enable = true;
		bool castShadows = true;
	};
}