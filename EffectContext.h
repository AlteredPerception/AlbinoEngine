#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Light.h"

namespace AlbinoEngine
{
	class Camera;
	class ShadowMap;

	struct EffectContext
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		Camera* camera = nullptr;

		const DirectionalLight* directionalLight = nullptr;

		bool useViewProjOverride = false;
		DirectX::XMMATRIX viewProjOverride = DirectX::XMMatrixIdentity();

		// Shadow data for normal scene rendering
		ShadowMap* shadowMap = nullptr;
		DirectX::XMMATRIX lightViewProjection = DirectX::XMMatrixIdentity();
	};
}