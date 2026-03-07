#include "EffectManager.h"

namespace AlbinoEngine
{
	EffectManager::EffectManager()
	{

	}
	EffectManager::~EffectManager()
	{

	}

	EffectPtr EffectManager::createEffect(const std::string effectName)
	{
		auto eff = std::make_shared<Effect>(effectName);
		m_EffectUnorderedMap[effectName] = eff;

		return eff;

	}

	Effect* EffectManager::getEffect(const std::string& effectName)
	{
		auto it = m_EffectUnorderedMap.find(effectName);
		return (it == m_EffectUnorderedMap.end()) ? nullptr : it->second.get();
	}

	void EffectManager::render(EffectContext& context, Mesh& mesh)
	{
		for (auto& effect : m_EffectUnorderedMap)
		{
			effect.second.get()->render(context, mesh);
		}
	}
}