#include "Effect.h"
#include "Technique.h"

namespace AlbinoEngine
{
	Technique& Effect::createTechnique(const std::string& techName)
	{
		auto it = m_techniques.find(techName);
		if (it == m_techniques.end())
		{
			auto [insIt, _] = m_techniques.emplace(techName, Technique(techName));
			return insIt->second;
		}
		return it->second;
	}

	Technique* Effect::getTechnique(const std::string& techName)
	{
		auto it = m_techniques.find(techName);
		return (it == m_techniques.end()) ? nullptr : &it->second;
	}

	void Effect::setActiveTechnique(const std::string& techName)
	{
		m_activeTechnique = techName;
	}

	void Effect::render(EffectContext& fx, Mesh& mesh)
	{
		Technique* t = getTechnique(m_activeTechnique);
		if (!t)return;
		t->render(fx, mesh);
	}
}