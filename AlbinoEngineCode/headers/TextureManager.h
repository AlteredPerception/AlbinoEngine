#pragma once

#include "Texture.h"
#include "Texture2D.h"
#include <unordered_map>
#include <filesystem>
#include <string>
#include <memory>
#include <algorithm>

namespace fs = std::filesystem;

#define TEXTURE_PATH "media//textures"

namespace AlbinoEngine
{
	using UnorderedTextureMap = std::unordered_map<std::wstring, std::shared_ptr<Texture2D>>;
	class TextureManager
	{
	public:
		TextureManager(ID3D11Device* textDevice);

		~TextureManager();

		UnorderedTextureMap cacheTextures();

		const UnorderedTextureMap& getUnorderedTextureMap() const { return m_textures; }

		std::shared_ptr<Texture2D> getTexture(const std::wstring& name);
	protected:
		ID3D11Device* m_textureDevice;
		UnorderedTextureMap m_textures;
	};
}