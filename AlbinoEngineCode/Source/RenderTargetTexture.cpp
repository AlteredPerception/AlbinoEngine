#include "RenderTargetTexture.h"

namespace AlbinoEngine
{
	RenderTargetTexture::RenderTargetTexture(ID3D11Device* device)
	{
		this->m_renderDevice = device;
		this->m_textureSampler = std::make_shared<TextureSampler>(device);
	}

	RenderTargetTexture::~RenderTargetTexture()
	{
		m_renderShaderResource.Reset();
		m_renderTargetView.Reset();
		m_renderTargetTexture.Reset();
		m_textureSampler.reset();
		m_depthBuffer.reset();
	}

	void RenderTargetTexture::setDepthBuffer(std::shared_ptr<DepthBuffer> depthBuffer)
	{
		this->m_depthBuffer = depthBuffer;
	}

	bool RenderTargetTexture::createRenderTarget(UINT width, UINT height)
	{
		D3D11_TEXTURE2D_DESC renderTargetDesc = {};
		renderTargetDesc.Width = width;
		renderTargetDesc.Height = height;
		renderTargetDesc.MipLevels = 1;
		renderTargetDesc.ArraySize = 1;
		renderTargetDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		renderTargetDesc.SampleDesc.Count = 1;
		renderTargetDesc.SampleDesc.Quality = 0;
		renderTargetDesc.Usage = D3D11_USAGE_DEFAULT;
		renderTargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		HRESULT hr = this->m_renderDevice->CreateTexture2D(&renderTargetDesc, nullptr, this->m_renderTargetTexture.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugStringA("RenderTargetTexture::createRenderTarget(UINT width, UINT height): error creating render target.\n"); return false; }
		hr = this->m_renderDevice->CreateRenderTargetView(this->m_renderTargetTexture.Get(), nullptr, this->m_renderTargetView.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugStringA("RenderTargetTexture::createRenderTarget(UINT width, UINT height): Could not create Render target view\n"); return false; }
		hr = this->m_renderDevice->CreateShaderResourceView(this->m_renderTargetTexture.Get(), NULL, this->m_renderShaderResource.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugStringA("RenderTargetTexture::createRenderTarget(UINT width, UINT height): Could not create shader resource.\n"); return false; }
		this->m_textureSampler->createSampler();
		return (hr == S_OK);
	}
}