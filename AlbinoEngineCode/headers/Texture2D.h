#pragma once

#include "Texture.h"

namespace AlbinoEngine
{
	class Texture2D : public Texture
	{
	public:
		Texture2D(ID3D11Device* device);

		~Texture2D();

		bool loadTexture(std::wstring textureName);

		ID3D11ShaderResourceView* getShaderResourceView() const { return m_ShaderResource.Get(); }

		ID3D11SamplerState* getSamplerState() const { return m_TextureSampler->getSamplerState(); }


		TextureSampler* getTextureSampler() const { return m_TextureSampler.get(); }
	protected:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture2D;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResource;
		std::unique_ptr<TextureSampler> m_TextureSampler;
	};
}