#pragma once
#include <DirectXMath.h>

namespace AlbinoEngine
{
	struct alignas(16) CB_FrameLighting
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0, -1.0, 0.0 };
		float padding0 = 0.0f;

		DirectX::XMFLOAT3 lightColor = { 1.0f, 0.5f, 0.2f };
		float lightIntensity = 0.5f;

		DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f,0.0f };
		float ambientStrength = 0.1f;

		DirectX::XMFLOAT3 specularColor = { 1.0f,1.0f,1.0f };
		float shininess = 16.0f;

		DirectX::XMMATRIX lightViewProjection = DirectX::XMMatrixIdentity();

		float shadowBias = 0.01f;
		DirectX::XMFLOAT3 padding1 = { 0.0f,0.0f,0.0f };
	};
}