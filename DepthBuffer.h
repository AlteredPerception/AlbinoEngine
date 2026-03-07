#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace AlbinoEngine
{
	using DepthStencilPtr = Microsoft::WRL::ComPtr<ID3D11DepthStencilView>;
	using DepthStencilState = Microsoft::WRL::ComPtr<ID3D11DepthStencilState>;
	using DepthStencilTexturePtr = Microsoft::WRL::ComPtr<ID3D11Texture2D>;
	using RasterizerDepthStatePtr = Microsoft::WRL::ComPtr<ID3D11RasterizerState>;

	class DepthBuffer
	{
	public:
		DepthBuffer(ID3D11Device* depthDevice);

		~DepthBuffer();

		bool createDepthBuffer(UINT width, UINT height);

		ID3D11DepthStencilView* getDepthStencilView() const { return m_pDepthStencil.Get(); }

		ID3D11DepthStencilState* getDepthStencilState() const { return this->m_pDepthStencilState.Get(); }

		bool enableDepth();

		bool disableDepth();

		ID3D11RasterizerState* getRasterState() const { return this->m_pDepthRaster.Get(); }
	protected:
		ID3D11Device* m_pDevice;
		DepthStencilPtr m_pDepthStencil;
		DepthStencilTexturePtr m_pDepthStencilTexture;
		DepthStencilState m_pDepthStencilState;
		RasterizerDepthStatePtr m_pDepthRaster;
	};
}