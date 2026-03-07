#pragma once

#include <d3d11.h>
#include <wrl/client.h>


#include <vector>
#include <memory>
#include <array>
#include <cassert>

#include "RenderTargetTexture.h"
#include "Viewport.h"

namespace AlbinoEngine
{
	using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;
	
	class RenderTargetManager
	{
	public:
		explicit RenderTargetManager(ID3D11Device* device);
		~RenderTargetManager() = default;

		// Create + store a render target texture. Returns index, or SIZE_MAX on failure.
		size_t createRenderTarget(UINT width, UINT height);

		size_t getCount() const { return m_renderTargets.size(); }
		RenderTargetTexture* get(size_t index) const;

		// Bind ONE render target by index. Optionally clear.
		bool beginRender(size_t index, ID3D11DeviceContext* ctx, const Viewport& vp, ID3D11DepthStencilView* view = nullptr, bool clear = true);

		// Bind multiple render targets (MRT) starting at "start", count, "count". Optionally clear all bound RTs,
		bool beginRenderMRT(size_t start, size_t count, ID3D11DeviceContext* ctx, const Viewport& vp, bool clear = true);

		// Restore previous OM render targets + viewport(s).
		void endRender(ID3D11DeviceContext* ctx);

		// Convinience: clear a specific RT without binding it (still needs binding in D3D11 to clear properly).
		// So we provide a helper that binds, clears, then restores.
		bool clearTarget(size_t index, ID3D11DeviceContext* ctx, const float color[4]);

	private:
		struct SavedState
		{
			std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> rtvs;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;

			std::array<D3D11_VIEWPORT, 16> vps{};
			UINT numVP = 0;
		};

		void pushState(ID3D11DeviceContext* ctx);
		void popState(ID3D11DeviceContext* ctx);

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		std::vector<RenderTargetTexturePtr> m_renderTargets;

		// Allow nesting begin/end safely
		std::vector<SavedState> m_stateStack;
	};
	
}