#include "PointShadowMap.h"

namespace AlbinoEngine
{
	PointShadowMap::PointShadowMap(ID3D11Device* device, UINT size)
		: m_device(device), m_size(size)
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(size);
		m_viewport.Height = static_cast<float>(size);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
	}

	PointShadowMap::~PointShadowMap()
	{
		m_srv.Reset();

		for (auto& rtv : m_rtvFaces)
			rtv.Reset();

		m_srv.Reset();
		m_shadowTexture.Reset();

		m_dsv.Reset();
		m_depthTexture.Reset();

		m_sampler.Reset();
	}

	bool PointShadowMap::initialize()
	{
		if (!m_device)
			return false;

		D3D11_TEXTURE2D_DESC colorDesc{};
		colorDesc.Width = m_size;
		colorDesc.Height = m_size;
		colorDesc.MipLevels = 1;
		colorDesc.ArraySize = 6;
		colorDesc.Format = DXGI_FORMAT_R32_FLOAT;
		colorDesc.SampleDesc.Count = 1;
		colorDesc.SampleDesc.Quality = 0;
		colorDesc.Usage = D3D11_USAGE_DEFAULT;
		colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		colorDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		HRESULT hr = m_device->CreateTexture2D(&colorDesc, nullptr, m_shadowTexture.ReleaseAndGetAddressOf());
		if (FAILED(hr))
			return false;

		for (UINT i = 0; i < 6; ++i)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.FirstArraySlice = i;
			rtvDesc.Texture2DArray.ArraySize = 1;

			hr = m_device->CreateRenderTargetView(
				m_shadowTexture.Get(),
				&rtvDesc,
				m_rtvFaces[i].ReleaseAndGetAddressOf());

			if (FAILED(hr))
				return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.Texture3D.MipLevels = 1;

		hr = m_device->CreateShaderResourceView(
			m_shadowTexture.Get(),
			&srvDesc,
			m_srv.ReleaseAndGetAddressOf());

		if (FAILED(hr))
			return false;

		// Shared depth buffer for rasterization.
		D3D11_TEXTURE2D_DESC depthDesc{};
		depthDesc.Width = m_size;
		depthDesc.Height = m_size;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = m_device->CreateTexture2D(&depthDesc, nullptr, m_depthTexture.ReleaseAndGetAddressOf());

		if (FAILED(hr))
			return false;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		
		hr = m_device->CreateDepthStencilView(m_depthTexture.Get(), &dsvDesc, this->m_dsv.ReleaseAndGetAddressOf());
		if (FAILED(hr))
			return false;

		D3D11_SAMPLER_DESC sampDesc{};
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.MinLOD = 0.0f;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = m_device->CreateSamplerState(&sampDesc, m_sampler.ReleaseAndGetAddressOf());

		if (FAILED(hr))
			return false;

		return true;
	}

	void PointShadowMap::beginFace(ID3D11DeviceContext* context, UINT faceIndex)
	{
		if (!context || faceIndex >= 6)
			return;

		context->OMGetRenderTargets(1, &m_prevRTV, &m_prevDSV);

		m_prevViewportCount = 1;
		context->RSGetViewports(&m_prevViewportCount, &m_prevViewport);

		context->RSSetViewports(1, &m_viewport);

		ID3D11RenderTargetView* rtv = m_rtvFaces[faceIndex].Get();
		context->OMSetRenderTargets(1, &rtv, m_dsv.Get());

		const float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		context->ClearRenderTargetView(rtv, clearColor);
		context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void PointShadowMap::endFace(ID3D11DeviceContext* context)
	{
		if (!context)
			return;

		context->OMSetRenderTargets(1, &m_prevRTV, m_prevDSV);
		context->RSSetViewports(m_prevViewportCount, &m_prevViewport);

		if (m_prevRTV)
		{
			m_prevRTV->Release();
			m_prevRTV = nullptr;
		}

		if (m_prevDSV)
		{
			m_prevDSV->Release();
			m_prevDSV = nullptr;
		}

	}
}