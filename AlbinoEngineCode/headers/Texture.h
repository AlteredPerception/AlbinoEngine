#pragma once

#include <d3d11.h>
#include <wincodec.h>
#include <vector>
#include <string>
#include <memory>
#include <wrl/client.h>
#include "TextureSampler.h"


#pragma comment(lib, "windowscodecs.lib")

namespace AlbinoEngine
{
	class Texture
	{
	public:
		Texture(ID3D11Device* device) {
			m_textureDevice = device; 
			this->m_width = 1;
			this->m_height = 1;
			this->m_MipMaps = 1;
		}

		virtual bool createTexture(UINT width, UINT height, void* data = nullptr) { return true; }

		virtual bool loadTexture(std::wstring textureName) { return true; }

		const std::string& getTextureName() const { return this->m_textureName; }
	protected:
		ID3D11Device* m_textureDevice;
		std::string m_textureName;
		UINT m_width;
		UINT m_height;
		UINT m_MipMaps;

	};
}