#include "ShadowMap.h"

namespace AlbinoEngine
{
	ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height)
		: m_device(device), m_width(width), m_height(height)
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(width);
		m_viewport.Height = static_cast<float>(height);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
	}

	bool ShadowMap::initialize()
	{
		if (!m_device)
			return false;

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = m_width;
		texDesc.Height = m_height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr = m_device->CreateTexture2D(&texDesc, nullptr, m_texture.ReleaseAndGetAddressOf());

		if (FAILED(hr))
			return false;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = m_device->CreateDepthStencilView(m_texture.Get(), &dsvDesc, m_dsv.ReleaseAndGetAddressOf());
		if (FAILED(hr))
			return false;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = m_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srv.ReleaseAndGetAddressOf());
		if (FAILED(hr))
			return false;

		return true;
	}

	void ShadowMap::begin(ID3D11DeviceContext* context)
	{
		if (!context)
			return;

		context->OMGetRenderTargets(1, &m_preRTV, &m_prevDSV);

		m_prevViewportCount = 1;
		context->RSGetViewports(&m_prevViewportCount, &m_prevViewport);

		context->RSSetViewports(1, &m_viewport);
		context->OMSetRenderTargets(0, nullptr, m_dsv.Get());
		context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void ShadowMap::end(ID3D11DeviceContext* context)
	{
		if (!context)
			return;

		context->OMSetRenderTargets(1, &m_preRTV, m_prevDSV);
		context->RSSetViewports(m_prevViewportCount, &m_prevViewport);

		if (m_preRTV)
		{
			m_preRTV->Release();
			m_preRTV = nullptr;
		}

		if (m_prevDSV)
		{
			m_prevDSV->Release();
			m_prevDSV = nullptr;
		}
	}
}