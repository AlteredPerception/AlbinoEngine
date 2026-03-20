#pragma once
#include <array>
#include "Light.h"

namespace AlbinoEngine
{
	class LightManager
	{
	public:
		static constexpr size_t MaxDirectionalLights = 2;
		static constexpr size_t MaxPointLights = 4;

		LightManager() = default;
		~LightManager() = default;

		void setDirectionalLight(size_t index, const DirectionalLight& light)
		{
			if (index >= MaxDirectionalLights) return;
			m_directionalLights[index] = light;
			if (index + 1 > m_numDirectionalLights)
				m_numDirectionalLights = index + 1;
		}

		const DirectionalLight& getDirectionalLight(size_t index = 0) const
		{ 
			return m_directionalLights[index];
		}

		const std::array<DirectionalLight, MaxDirectionalLights>& getDirectionalLights() const
		{
			return m_directionalLights;
		}

		unsigned int getNumDirectionalLights() const
		{
			return static_cast<unsigned int>(m_numDirectionalLights);
		}

		void setPointLight(size_t index, const PointLight& light)
		{
			if (index >= MaxPointLights) return;
			m_pointLights[index] = light;
			if (index + 1 > m_numPointLights)
				m_numPointLights = index + 1;
		}

		const PointLight& getPointLight(size_t index = 0) const
		{
			return m_pointLights[index];
		}

		unsigned int getNumPointLights() const
		{
			return static_cast<unsigned int>(m_numPointLights);
		}
	private:
		std::array<DirectionalLight, MaxDirectionalLights> m_directionalLights{};
		std::array<PointLight, MaxPointLights> m_pointLights{};

		size_t m_numDirectionalLights = 1;
		size_t m_numPointLights = 0;
	};
}
