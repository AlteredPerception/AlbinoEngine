#pragma once
#include <memory>
#include <vector>
#include <string>

namespace AlbinoEngine
{
	class Pass;
	class Mesh;
	struct EffectContext;

	class Technique
	{
	public:
		explicit Technique(std::string name = "Technique") : m_name(std::move(name)) {}

		Pass& addPass(std::unique_ptr<Pass> pass);

		void render(EffectContext& fx, Mesh& mesh);

		const std::string& name() const { return m_name; }

		unsigned int getNumPasses() const { return (UINT)m_passes.size(); }
	private:
		std::string m_name;
		std::vector<std::unique_ptr<Pass>> m_passes;
	};
}