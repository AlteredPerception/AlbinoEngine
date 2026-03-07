#include "DepthBuffer.h"

namespace AlbinoEngine
{
	DepthBuffer::DepthBuffer(ID3D11Device* depthDevice)
	{
		m_pDevice = depthDevice;
	}

	DepthBuffer::~DepthBuffer()
	{
		if (m_pDepthStencil)
		{
			m_pDepthStencil.Reset();
		}
	}

	bool DepthBuffer::createDepthBuffer(UINT width, UINT height)
	{
		D3D11_TEXTURE2D_DESC depthText2D = {};
		depthText2D.Width = width;
		depthText2D.Height = height;
		depthText2D.ArraySize = 1;
		depthText2D.MipLevels = 1;
		depthText2D.Format = DXGI_FORMAT_D32_FLOAT;
		depthText2D.SampleDesc.Count = 1;
		depthText2D.SampleDesc.Quality = 0;
		depthText2D.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthText2D.CPUAccessFlags = 0;
		depthText2D.MiscFlags = 0;

		HRESULT hr = m_pDevice->CreateTexture2D(&depthText2D, nullptr, m_pDepthStencilTexture.ReleaseAndGetAddressOf());
		if (FAILED(hr)) 
		{ 
			OutputDebugString(L"Could not create depth stencil texture.\n"); 
			return false; 
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilView = {};
		depthStencilView.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilView.Texture2D.MipSlice = 0;
		

		hr = m_pDevice->CreateDepthStencilView(
			m_pDepthStencilTexture.Get(), 
			&depthStencilView, 
			m_pDepthStencil.ReleaseAndGetAddressOf());

		if (FAILED(hr)) 
		{ 
			OutputDebugString(L"Could not create Depth Stencil View"); 
			return false;
		}

		return true;
	}

	bool DepthBuffer::enableDepth()
	{
		D3D11_RASTERIZER_DESC depth = {};
		depth.FillMode = D3D11_FILL_SOLID;
		depth.CullMode = D3D11_CULL_NONE;
		depth.DepthClipEnable = TRUE;

		HRESULT hr = m_pDevice->CreateRasterizerState(&depth, m_pDepthRaster.ReleaseAndGetAddressOf());

		D3D11_DEPTH_STENCIL_DESC depthEnable = {};
		depthEnable.DepthEnable = TRUE;
		depthEnable.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthEnable.DepthFunc = D3D11_COMPARISON_LESS;
		depthEnable.StencilEnable = FALSE;
		
		hr = m_pDevice->CreateDepthStencilState(&depthEnable, this->m_pDepthStencilState.ReleaseAndGetAddressOf());
		return SUCCEEDED(hr);
	}

	bool DepthBuffer::disableDepth()
	{
		D3D11_RASTERIZER_DESC noDepth = {};
		noDepth.FillMode = D3D11_FILL_SOLID;
		noDepth.CullMode = D3D11_CULL_NONE;
		noDepth.DepthClipEnable = false;
		HRESULT hr = m_pDevice->CreateRasterizerState(&noDepth, m_pDepthRaster.ReleaseAndGetAddressOf());
		D3D11_DEPTH_STENCIL_DESC noDepthState = {};
		noDepthState.DepthEnable = FALSE;
		noDepthState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		noDepthState.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = m_pDevice->CreateDepthStencilState(&noDepthState, m_pDepthStencilState.ReleaseAndGetAddressOf());

		return SUCCEEDED(hr);
	}
}