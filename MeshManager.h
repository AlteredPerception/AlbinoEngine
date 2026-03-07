#pragma once

#include "Mesh.h"
#include <unordered_map>
#include <string>

namespace AlbinoEngine
{
	class EffectManager;
	struct EffectContext;

	using MeshPtr = std::shared_ptr<Mesh>;
	using UnorderedMeshMap = std::unordered_map<std::wstring, MeshPtr>;

	struct MeshEntry
	{
		MeshPtr mesh;
		std::string effectName;
		std::string techniqueName;
	};
	class MeshManager
	{
	public:
		MeshManager(ID3D11Device* device, ID3D11DeviceContext* context);

		Mesh* createMesh(const std::wstring& meshName, const std::wstring& meshType);

		Mesh* getMeshByName(const std::wstring& name);

		void setMeshEffect(const std::wstring& meshName,
			const std::string& effectName, 
			const std::string& techniqueName);

		void renderAll(EffectManager& effect, EffectContext& fx);
		void renderAllExceptEffect(EffectManager& effects, EffectContext& fx, const std::string& excludedEffect);
		void renderOnlyEffect(EffectManager& effects, EffectContext& fx, const std::string& effectName);
	protected:
		ID3D11Device* m_meshDevice = nullptr;
		ID3D11DeviceContext* m_meshContext = nullptr;
		
		std::unordered_map<std::wstring, MeshEntry> m_Meshes;
		//UnorderedMeshMap m_MeshMap;
		//Effect m_Effect;
		//EffectManager m_effectManager;
	};
}