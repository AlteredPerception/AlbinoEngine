#include "Technique.h"
#include "Pass.h"
#include "Mesh.h"

namespace AlbinoEngine
{
	Pass& Technique::addPass(std::unique_ptr<Pass> pass)
	{
		m_passes.emplace_back(std::move(pass));
		return *m_passes.back();
	}

	void Technique::render(EffectContext& fx, Mesh& mesh)
	{
		for (auto& p : m_passes)
			p->render(fx, mesh);
	}
}