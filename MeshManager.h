#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include <d3d11.h>

#include "Mesh.h"
#include "EffectManager.h"
#include "EffectContext.h"


namespace AlbinoEngine
{
	
	struct MeshEntry
	{
		std::shared_ptr<Mesh> mesh;
		std::string effectName;
		std::string techniqueName;
	};
	class MeshManager
	{
	public:
		MeshManager(ID3D11Device* device, ID3D11DeviceContext* context);
		~MeshManager() = default;

		// Generic creation.
		Mesh* createMesh(const std::wstring& meshName, const std::wstring& meshType);
		// Typed helpers.
		Mesh* createCube(const std::wstring& name);
		Mesh* createPlane(const std::wstring& name);
		Mesh* createScreenQuad(const std::wstring& name);

		Mesh* getMeshByName(const std::wstring& name);

		void setMeshEffect(const std::wstring& meshName,
			const std::string& effectName, 
			const std::string& techniqueName);

		// Render paths
		void renderAll(EffectManager& effect, EffectContext& fx);
		void renderAllExceptEffect(EffectManager& effects, EffectContext& fx, const std::string& excludedEffect);
		void renderOnlyEffect(EffectManager& effects, EffectContext& fx, const std::string& effectName);

	protected:
		ID3D11Device* m_meshDevice = nullptr;
		ID3D11DeviceContext* m_meshContext = nullptr;
		
		std::unordered_map<std::wstring, MeshEntry> m_Meshes;
		
	};
}