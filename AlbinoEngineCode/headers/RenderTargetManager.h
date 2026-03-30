#pragma once

#include <memory>
#include <vector>

#include <d3d11.h>

#include "RenderTargetTexture.h"
#include "Viewport.h"


namespace AlbinoEngine
{
	
	class RenderTargetManager
	{
	public:
		RenderTargetManager(ID3D11Device* device);
		~RenderTargetManager() = default;

		// Create + store a render target texture. Returns index, or SIZE_MAX on failure.
		bool createRenderTarget(UINT width, UINT height);

		RenderTargetTexture* get(size_t index);

		// Bind ONE render target by index. Optionally clear.
		void beginRender(
			UINT index, 
			ID3D11DeviceContext* context, 
			const Viewport& viewport, 
			ID3D11DepthStencilView* dsv);

		// Bind multiple render targets (MRT) starting at "start", count, "count". Optionally clear all bound RTs,
		//bool beginRenderMRT(size_t start, size_t count, ID3D11DeviceContext* ctx, const Viewport& vp, bool clear = true);

		// Restore previous OM render targets + viewport(s).
		void endRender(ID3D11DeviceContext* context);

		// Convinience: clear a specific RT without binding it (still needs binding in D3D11 to clear properly).
		// So we provide a helper that binds, clears, then restores.
		//bool clearTarget(size_t index, ID3D11DeviceContext* ctx, const float color[4]);

	private:
		//struct SavedState
		//{
		//	std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> rtvs;
		//	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;

		//	std::array<D3D11_VIEWPORT, 16> vps{};
		//	UINT numVP = 0;
		//};

		//void pushState(ID3D11DeviceContext* ctx);
		//void popState(ID3D11DeviceContext* ctx);

	private:
		ID3D11Device* m_device = nullptr;
		std::vector<std::shared_ptr<RenderTargetTexture>> m_renderTargets;


		D3D11_VIEWPORT m_previousViewport{};
		UINT m_previousViewportCount = 1;

		ID3D11RenderTargetView* m_previousRTV = nullptr;
		ID3D11DepthStencilView* m_previousDSV = nullptr;
		// Allow nesting begin/end safely
		//std::vector<SavedState> m_stateStack;
	};
	
}