#include "TextureManager.h"

namespace AlbinoEngine
{
	static bool hasSupportedExtension(const fs::path& p)
	{
		auto ext = p.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		// Add/remove based on what your loader supports.
		return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".dds" || ext == ".tga" || ext == ".bmp";
	}

	TextureManager::TextureManager(ID3D11Device* textDevice)
	{
		this->m_textureDevice = textDevice;
	
	}

	TextureManager::~TextureManager()
	{
	}

	UnorderedTextureMap TextureManager::cacheTextures()
	{
		UnorderedTextureMap out;

		const fs::path root = fs::path("media") / "textures";
		std::error_code ec;

		if (!fs::exists(root, ec) || !fs::is_directory(root, ec))
		{
			OutputDebugStringA("TextureManager::cacheTextures(): media/textures does not exist");
			return out;
		}

		for (const auto& entry : fs::recursive_directory_iterator(root, ec))
		{
			if (ec) break;
			if (!entry.is_regular_file()) continue;

			const fs::path& p = entry.path();
			if (!hasSupportedExtension(p)) continue;

			// Key = filename without extension
			std::wstring key = p.stem().wstring();

			// Avoid silent collisions
			if (out.find(key) != out.end())
			{
				// Keep logging simple here to avoid wide/narrow string mixing.
				std::string msg = "TextureManager::cacheTextures(): duplicate key from file: " + p.string() + "\n";
				OutputDebugStringA(msg.c_str());
				continue;
			}

			auto text = std::make_shared<Texture2D>(this->m_textureDevice);
			if (!text->loadTexture(p.wstring()))
			{
				std::string msg = "Failed to load texture: " + p.string() + "\n";
				OutputDebugStringA(msg.c_str());
				continue;
			}

			out.emplace(std::move(key), std::move(text));
		}

		this->m_textures = out;

		return out;
	}

	std::shared_ptr<Texture2D> TextureManager::getTexture(const std::wstring& name)
	{
		auto it = m_textures.find(name);
		return (it != m_textures.end()) ? it->second : nullptr;
	}
}