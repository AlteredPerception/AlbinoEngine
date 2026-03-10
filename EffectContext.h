#pragma once

#include <d3d11.h>

#include "Light.h"

namespace AlbinoEngine
{
	class Camera;

	struct EffectContext
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		Camera* camera = nullptr;

		const DirectionalLight* directionalLight = nullptr;
	};
}