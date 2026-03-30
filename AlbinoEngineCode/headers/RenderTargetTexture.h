#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Texture.h"
#include "TextureSampler.h"
#include "DepthBuffer.h"
#include <memory>

namespace AlbinoEngine
{
	class RenderTargetTexture
	{
	public:
		RenderTargetTexture(ID3D11Device* device);

		~RenderTargetTexture();

		bool createRenderTarget(UINT width, UINT height);

		ID3D11RenderTargetView* getRenderTargetView() const { return m_renderTargetView.Get(); }

		ID3D11Texture2D* getTexture2D() const { return this->m_renderTargetTexture.Get(); }

		ID3D11ShaderResourceView* getShaderResourceView() const { return this->m_renderShaderResource.Get(); }

		void setDepthBuffer(std::shared_ptr<DepthBuffer> depthBuffer);
	
		DepthBuffer* getDepthBuffer() const { return this->m_depthBuffer.get(); }

		ID3D11DepthStencilView* getDepthStencilView() const { return m_depthBuffer->getDepthStencilView(); }

		TextureSampler* getSampler() const { return this->m_textureSampler.get(); }

	protected:
		ID3D11Device* m_renderDevice;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTargetTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_renderShaderResource;
		std::shared_ptr<TextureSampler> m_textureSampler;
		std::shared_ptr<DepthBuffer> m_depthBuffer;
	};
}