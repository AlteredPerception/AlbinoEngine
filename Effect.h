#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include "Technique.h"
#include "EffectContext.h"

namespace AlbinoEngine
{
	class Mesh;
	class Effect
	{
	public:
		explicit Effect(std::string name = "Effect") : m_name(std::move(name)) {}
		

		Effect(const Effect&) = delete;
		Effect& operator=(const Effect&) = delete;
		Effect(Effect&&) noexcept = default;
		Effect& operator=(Effect&&) noexcept = default;

		Technique& createTechnique(const std::string& techName);

		Technique* getTechnique(const std::string& techName);

		void setActiveTechnique(const std::string& techName);

		void render(EffectContext& fx, Mesh& mesh);

		const std::string& name() const { return m_name; }
	private:
		std::string m_name;
		std::unordered_map<std::string, Technique> m_techniques;
		std::string m_activeTechnique = "Default";
	};
}