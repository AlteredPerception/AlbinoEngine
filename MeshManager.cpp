#include "MeshManager.h"
#include "CubeMesh.h"
#include "PlaneMesh.h"
#include "screenQuadMesh.h"
#include "EffectManager.h"
#include "Effect.h"

namespace AlbinoEngine
{
	MeshManager::MeshManager(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		m_meshDevice = device;
		m_meshContext = context;
	
		//m_effectManager = {};
	}

	Mesh* MeshManager::createMesh(const std::wstring& meshName, const std::wstring& meshType)
	{
		MeshPtr mesh;

		if (meshType == L"cubeMesh")
		{
			mesh = std::make_shared<CubeMesh>(m_meshDevice);
			mesh->initliaze();
		}
		else if (meshType == L"planeMesh")
		{
			mesh = std::make_shared<PlaneMesh>(m_meshDevice);
		}
		else if (meshType == L"screenQuadMesh")
		{
			mesh = std::make_shared<ScreenQuadMesh>(m_meshDevice);
			mesh->initliaze();
		}

		if (!mesh) return nullptr;

		m_Meshes[meshName].mesh = mesh;
		return mesh.get();
	}

	Mesh* MeshManager::getMeshByName(const std::wstring& name)
	{
		auto it = m_Meshes.find(name);
		if (it == m_Meshes.end() || !it->second.mesh)
			return nullptr;
		return it->second.mesh.get();
	}

	void MeshManager::setMeshEffect(const std::wstring& meshName,
		const std::string& effectName, const std::string& techniqueName)
	{
		auto it = m_Meshes.find(meshName);
		if (it == m_Meshes.end()) return;

		it->second.effectName = std::move(effectName);
		it->second.techniqueName = std::move(techniqueName);
	}

	void MeshManager::renderAll(EffectManager& effects, EffectContext& fx)
	{
		// IMPORTANT: fx.camera must be valid here. Camera provided by the scene.
		for (auto& [name, entry] : m_Meshes)
		{
			if (!entry.mesh) return;

			Effect* e = effects.getEffect(entry.effectName);
			if (!e) continue;

			if (!entry.techniqueName.empty())
				e->setActiveTechnique(entry.techniqueName);

			e->render(fx, *entry.mesh);
		}
	}
	void MeshManager::renderAllExceptEffect(EffectManager& effects, EffectContext& fx, const std::string& excludedEffect)
	{
		for (auto& [name, entry] : m_Meshes)
		{
			if (!entry.mesh) continue;
			if (entry.effectName == excludedEffect) continue;

			Effect* e = effects.getEffect(entry.effectName);
			if (!e) continue;

			if (!entry.techniqueName.empty())
				e->setActiveTechnique(entry.techniqueName);

			e->render(fx, *entry.mesh);
		}
	}

	void MeshManager::renderOnlyEffect(EffectManager& effects, EffectContext& fx, const std::string& effectName)
	{
		for (auto& [name, entry] : m_Meshes)
		{
			if (!entry.mesh) continue;
			if (entry.effectName != effectName) continue;

			Effect* e = effects.getEffect(entry.effectName);
			if (!e) continue;

			if (!entry.techniqueName.empty())
				e->setActiveTechnique(entry.techniqueName);

			e->render(fx, *entry.mesh);
		}
	}
}