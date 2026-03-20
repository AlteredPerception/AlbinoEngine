#pragma once
#include <DirectXMath.h>

namespace AlbinoEngine
{
	struct alignas(16) GPU_DirectionalLight
	{
		DirectX::XMFLOAT3 direction = { 0.0f, -1.0f, 0.0f };
		float padding0 = 0.0f;

		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
	};

	struct alignas(16) GPU_PointLight
	{
		DirectX::XMFLOAT3 position = { 0.0f, 2.0f, 0.0f };
		float range = 10.0f;

		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
	};

	struct alignas(16) CB_FrameLighting
	{
		GPU_DirectionalLight directionalLights[2];
		GPU_PointLight pointLights[4];

		DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f,0.0f };
		float ambientStrength = 2.0f;

		DirectX::XMFLOAT3 specularColor = { 1.0f,1.0f,1.0f };
		float shininess = 16.0f;

		DirectX::XMMATRIX lightViewProjection = DirectX::XMMatrixIdentity();

		float shadowBias = 0.0015f;
		UINT numDirectionalLights = 1;
		UINT numPointLights = 1;
		float padding1 = 0.0f;
	};
}