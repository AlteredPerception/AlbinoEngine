#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "Shader.h"
#include <string>
#include <unordered_map>
#include <filesystem>

namespace AlbinoEngine
{
	using ShaderPtr = std::shared_ptr<Shader>;
	using UnorderedShaderMap = std::unordered_map<std::wstring, ShaderPtr>;

	class ShaderManager
	{
	public:
		ShaderManager(ID3D11Device* shaderDevice);

		~ShaderManager();

		UnorderedShaderMap cacheShaders();

		const UnorderedShaderMap& getShaderMap() const { return this->m_UnorderedShaders; }
	protected:
		UnorderedShaderMap m_UnorderedShaders;
	};
}