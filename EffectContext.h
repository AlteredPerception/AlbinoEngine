#pragma once

#include <d3d11.h>

namespace AlbinoEngine
{
	class Camera;

	struct EffectContext
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		Camera* camera = nullptr;
	};
}