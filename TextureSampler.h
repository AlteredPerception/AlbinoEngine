#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace AlbinoEngine
{
	class TextureSampler
	{
	public:
		TextureSampler(ID3D11Device* device);

		~TextureSampler();


		void setFiler(D3D11_FILTER filter);

		void setTextureAddressModeU(D3D11_TEXTURE_ADDRESS_MODE addressU);

		void setTextureAddressModeV(D3D11_TEXTURE_ADDRESS_MODE addressV);

		void setTextureAddressModeW(D3D11_TEXTURE_ADDRESS_MODE addressW);

		bool createSampler();

		ID3D11SamplerState* getSamplerState() const { return this->m_Sampler.Get(); }

	protected:
		D3D11_FILTER m_samplerFilter;
		D3D11_TEXTURE_ADDRESS_MODE m_addressMode[3] = {};

		ID3D11Device* m_SamplerDevice;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_Sampler;
	};
}