#pragma once

#include "Light.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <array>


namespace AlbinoEngine
{
	class Camera;
	class ShadowMap;
	class PointShadowMap;
	struct EffectContext
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		Camera* camera = nullptr;

		static constexpr UINT MaxDirectionalLights = 2;
		static constexpr UINT MaxPointLights = 4;
		std::array<const DirectionalLight*, MaxDirectionalLights> directionalLights = {nullptr, nullptr};
		UINT numDirectionalLights = 0;

		std::array<const PointLight*, MaxPointLights> pointLights = { nullptr, nullptr, nullptr, nullptr };
		UINT numPointLights = 0;
		bool useViewProjOverride = false;
		DirectX::XMMATRIX viewProjOverride = DirectX::XMMatrixIdentity();

		// Shadow data for normal scene rendering
		ShadowMap* shadowMap = nullptr;
		PointShadowMap* pointShadowMap = nullptr;
		DirectX::XMMATRIX lightViewProjection = DirectX::XMMatrixIdentity();

		DirectX::XMFLOAT3 pointShadowLightPosition = { 0.0f,0.0f,0.0f };
		float pointShadowLightRange = 1.0f;
		bool usePointShadowData = false;

		DirectX::XMFLOAT4 debugColor = { 1,1,1,1 };
		bool useDebugColor = false;
	};
}