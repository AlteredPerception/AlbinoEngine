#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <array>

namespace AlbinoEngine
{
	class PointShadowMap
	{
	public:
		PointShadowMap(ID3D11Device* device, UINT size);
		~PointShadowMap();

		bool initialize();

		void beginFace(ID3D11DeviceContext* context, UINT faceIndex);
		void endFace(ID3D11DeviceContext* context);

		ID3D11ShaderResourceView* getSRV() const { return m_srv.Get(); }
		ID3D11SamplerState* getSampler() const { return m_sampler.Get(); }
		UINT getSize() const { return m_size; }
	private:
		ID3D11Device* m_device = nullptr;
		UINT m_size = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowTexture;
		std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 6> m_rtvFaces;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
		
		D3D11_VIEWPORT m_viewport{};

		ID3D11RenderTargetView* m_prevRTV = nullptr;
		ID3D11DepthStencilView* m_prevDSV = nullptr;
		D3D11_VIEWPORT m_prevViewport{};
		UINT m_prevViewportCount = 1;
	};
}