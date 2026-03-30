#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace AlbinoEngine
{
	class ShadowMap
	{
	public:
		ShadowMap(ID3D11Device* device, UINT width, UINT height);
		~ShadowMap();

		bool initialize();

		void begin(ID3D11DeviceContext* context);
		void end(ID3D11DeviceContext* context);

		ID3D11DepthStencilView* getDSV() const { return m_dsv.Get(); }
		ID3D11ShaderResourceView* getSRV() const { return m_srv.Get(); }
		ID3D11SamplerState* getComparisonSampler() const { return this->m_comparisonSampler.Get(); }
		UINT getWidth() const { return m_width; }
		UINT getHeight() const { return m_height; }
	protected:
		ID3D11Device* m_device = nullptr;
		UINT m_width = 0;
		UINT m_height = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_comparisonSampler;

		D3D11_VIEWPORT m_viewport{};

		ID3D11RenderTargetView* m_preRTV = nullptr;
		ID3D11DepthStencilView* m_prevDSV = nullptr;
		D3D11_VIEWPORT m_prevViewport{};
		UINT m_prevViewportCount = 1;
	};
}