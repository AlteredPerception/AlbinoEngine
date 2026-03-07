#pragma once
#include <memory>
#include <vector>
#include <string>
#include "EffectContext.h"
#include "Pass.h"
namespace AlbinoEngine
{
	class Mesh;

	class Technique
	{
	public:
		explicit Technique(std::string name = "Technique") : m_name(std::move(name)) {}


		Technique(const Technique&) = delete;
		Technique& operator=(const Technique&) = delete;
		Technique(Technique&&) noexcept = default;
		Technique& operator=(Technique&&) noexcept = default;

		Pass& addPass(std::unique_ptr<Pass> pass);

		void render(EffectContext& fx, Mesh& mesh);

		const std::string& name() const { return m_name; }

		unsigned int getNumPasses() const { return (UINT)m_passes.size(); }
	private:
		std::string m_name;
		std::vector<std::unique_ptr<Pass>> m_passes;
	};
}