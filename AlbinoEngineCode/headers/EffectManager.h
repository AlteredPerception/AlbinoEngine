#pragma once

#include "Effect.h"
#include <unordered_map>


namespace AlbinoEngine
{
	using EffectPtr = std::shared_ptr<Effect>;
	using EffectUnorderedMap = std::unordered_map<std::string, EffectPtr>;

	class EffectManager
	{
	public:
		EffectManager();

		~EffectManager();

		EffectPtr createEffect(const std::string effectName);

		Effect* getEffect(const std::string& effectName);

		void render(EffectContext& context, Mesh& mesh);

	protected:
		EffectUnorderedMap m_EffectUnorderedMap;
	};
}