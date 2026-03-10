#pragma once
#include "Light.h"

namespace AlbinoEngine
{
	class LightManager
	{
	public:
		LightManager() = default;
		~LightManager() = default;

		void setDirectionalLight(const DirectionalLight& light)
		{
			m_directionalLight = light;
		}

		const DirectionalLight& getDirectionalLight() const
		{ 
			return m_directionalLight;
		}
	private:
		DirectionalLight m_directionalLight;
	};
}
