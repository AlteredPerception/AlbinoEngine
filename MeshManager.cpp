#include "MeshManager.h"

#include "CubeMesh.h"
#include "PlaneMesh.h"
#include "ScreenQuadMesh.h"

#include <iostream>

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
		std::shared_ptr<Mesh> mesh;

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

		if (!mesh)
			return nullptr;

		m_Meshes[meshName].mesh = mesh;

		return mesh.get();
	}

	Mesh* MeshManager::createCube(const std::wstring& name)
	{
		return createMesh(name, L"cubeMesh");
	}

	Mesh* MeshManager::createPlane(const std::wstring& name)
	{
		return createMesh(name, L"planeMesh");
	}

	Mesh* MeshManager::createScreenQuad(const std::wstring& name)
	{
		return createMesh(name, L"screenQuadMesh");
	}

	Mesh* MeshManager::getMeshByName(const std::wstring& name)
	{
		auto it = m_Meshes.find(name);
		if (it == m_Meshes.end())
			return nullptr;

		return it->second.mesh.get();
	}

	void MeshManager::setMeshEffect(const std::wstring& meshName,
		const std::string& effectName, const std::string& techniqueName)
	{
		auto it = m_Meshes.find(meshName);
		if (it == m_Meshes.end()) return;

		it->second.effectName = effectName; //std::move(effectName);
		it->second.techniqueName = techniqueName; //std::move(techniqueName);
	}

	void MeshManager::renderAll(EffectManager& effects, EffectContext& fx)
	{
		// IMPORTANT: fx.camera must be valid here. Camera provided by the scene.
		for (auto& entry : m_Meshes)
		{
			if (!entry.second.mesh) 
				continue;

			if (entry.second.effectName.empty())
				continue;

			Effect* effect = effects.getEffect(entry.second.effectName);
			if (!effect) 
				continue;

			if (!entry.second.techniqueName.empty())
				effect->setActiveTechnique(entry.second.techniqueName);

			effect->render(fx, *entry.second.mesh);
		}
	}

	void MeshManager::renderAllExceptEffect(EffectManager& effects, EffectContext& fx, const std::string& excludedEffect)
	{
		for (auto& entry : m_Meshes)
		{
			if (!entry.second.mesh) 
				continue;

			if (entry.second.effectName == excludedEffect) 
				continue;

			Effect* effect = effects.getEffect(entry.second.effectName);
			if (!effect) 
				continue;

			if (!entry.second.techniqueName.empty())
				effect->setActiveTechnique(entry.second.techniqueName);

			effect->render(fx, *entry.second.mesh);
		}
	}

	void MeshManager::renderOnlyEffect(EffectManager& effects, EffectContext& fx, const std::string& effectName)
	{
		for (auto& entry : m_Meshes)
		{
			if (!entry.second.mesh) continue;
			if (entry.second.effectName != effectName) continue;


			//OutputDebugStringA("Rendering screen quad mesh!\n");
			Effect* effect = effects.getEffect(entry.second.effectName);
			if (!effect)
				continue;
			

			if (!entry.second.techniqueName.empty())
				effect->setActiveTechnique(entry.second.techniqueName);

			effect->render(fx, *entry.second.mesh);
		}
	}
}