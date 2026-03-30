#include "TextureSampler.h"

namespace AlbinoEngine
{
	TextureSampler::TextureSampler(ID3D11Device* device)
	{
		this->m_SamplerDevice = device;
		this->setFiler(D3D11_FILTER_ANISOTROPIC);
		this->setTextureAddressModeU(D3D11_TEXTURE_ADDRESS_WRAP);
		this->setTextureAddressModeV(D3D11_TEXTURE_ADDRESS_WRAP);
		this->setTextureAddressModeW(D3D11_TEXTURE_ADDRESS_WRAP);
	}

	TextureSampler::~TextureSampler()
	{
		if (m_Sampler)
		{
			m_Sampler.Reset();
		}
	}

	void TextureSampler::setFiler(D3D11_FILTER filter)
	{
		this->m_samplerFilter = filter;
	}

	void TextureSampler::setTextureAddressModeU(D3D11_TEXTURE_ADDRESS_MODE addressModeU)
	{
		m_addressMode[0] = addressModeU;
	}

	void TextureSampler::setTextureAddressModeV(D3D11_TEXTURE_ADDRESS_MODE addressModeV)
	{
		m_addressMode[1] = addressModeV;
	}

	void TextureSampler::setTextureAddressModeW(D3D11_TEXTURE_ADDRESS_MODE addressModeW)
	{
		m_addressMode[2] = addressModeW;
	}

	bool TextureSampler::createSampler()
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = this->m_samplerFilter;
		sampDesc.AddressU = this->m_addressMode[0];
		sampDesc.AddressV = this->m_addressMode[1];
		sampDesc.AddressW = this->m_addressMode[2];
	
		HRESULT hr = this->m_SamplerDevice->CreateSamplerState(&sampDesc, this->m_Sampler.GetAddressOf());
		if (FAILED(hr)) {
			OutputDebugString(L"Could not create sampler state\n");
			return false;
		}
		return true;
	}
}