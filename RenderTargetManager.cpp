#include "RenderTargetManager.h"

namespace AlbinoEngine
{
	RenderTargetManager::RenderTargetManager(ID3D11Device* device)
		: m_device(device)
	{
		m_renderTargets.reserve(8);
	}

	bool RenderTargetManager::createRenderTarget(UINT width, UINT height)
	{
		auto target = std::make_shared<RenderTargetTexture>(m_device);

		if (!target->createRenderTarget(width, height))
			return false;

		m_renderTargets.push_back(target);
		return true;
	}

	RenderTargetTexture* RenderTargetManager::get(size_t index) 
	{
		if (index >= m_renderTargets.size())
			return nullptr;

		return m_renderTargets[index].get();
	}

	//void RenderTargetManager::pushState(ID3D11DeviceContext* ctx)
	//{
	//	SavedState s;

		// Save OM.
	//	ID3D11RenderTargetView* prevRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	//	ID3D11DepthStencilView* prevDSV = nullptr;
	//	ctx->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, prevRTVs, &prevDSV);

	//	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	//	{
	//		s.rtvs[i] = prevRTVs[i];
	//		if (prevRTVs[i]) prevRTVs[i]->Release();
	//	}

	//	s.dsv = prevDSV;
	//	if (prevDSV) prevDSV->Release();

		// Save viewports
	//	s.numVP = 16;
	//	ctx->RSGetViewports(&s.numVP, s.vps.data());

	//	m_stateStack.push_back(s);
	//}

	//void RenderTargetManager::popState(ID3D11DeviceContext* ctx)
	//{
	//	if (m_stateStack.empty())
	//		return;

	//	SavedState s = std::move(m_stateStack.back());
	//	m_stateStack.pop_back();

		// Restore OM
	//	ID3D11RenderTargetView* rtvsRaw[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	//	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	//		rtvsRaw[i] = s.rtvs[i].Get();
		
	//	ctx->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvsRaw, s.dsv.Get());

		// Restore viewports.
	//	if (s.numVP > 0)
	//		ctx->RSSetViewports(s.numVP, s.vps.data());
	//}

	static void setViewportFrom(const Viewport& vp, ID3D11DeviceContext* ctx)
	{
		D3D11_VIEWPORT dxvp = {};
		dxvp.TopLeftX = vp.getTopX();
		dxvp.TopLeftY = vp.getTopY();
		dxvp.Width = vp.getWidth();
		dxvp.Height = vp.getHeight();
		dxvp.MinDepth = vp.getMinZ();
		dxvp.MaxDepth = vp.getMaxZ();
		ctx->RSSetViewports(1, &dxvp);
	}

	static void getClearColorFrom(const Viewport& vp, float outcolor[4])
	{
		outcolor[0] = vp.getRedChannel();
		outcolor[1] = vp.getGreenChannel();
		outcolor[2] = vp.getBlueChannel();
		outcolor[3] = vp.getAlphaChannel();
	}

	void RenderTargetManager::beginRender(UINT index, ID3D11DeviceContext* context, const Viewport& viewport, ID3D11DepthStencilView* dsv)
	{
		if (!context)
			return;

		if (index >= m_renderTargets.size())
			return;

		RenderTargetTexture* target = m_renderTargets[index].get();

		if (!target)
			return;

		// Save current render target.
		context->OMGetRenderTargets(1, &m_previousRTV, &m_previousDSV);

		// Save viewport
		m_previousViewportCount = 1;
		context->RSGetViewports(&m_previousViewportCount, &m_previousViewport);

		// Set viewport
		D3D11_VIEWPORT vp{};
		vp.TopLeftX = viewport.getTopX();
		vp.TopLeftY = viewport.getTopY();
		vp.Width = viewport.getWidth();
		vp.Height = viewport.getHeight();
		vp.MinDepth = viewport.getMinZ();
		vp.MaxDepth = viewport.getMaxZ();

		context->RSSetViewports(1, &vp);

		// Bind new render target
		ID3D11RenderTargetView* rtv = target->getRenderTargetView();
		context->OMSetRenderTargets(1, &rtv, dsv);
		// Clear
		float color[4] =
		{
			viewport.getRedChannel(),
			viewport.getBlueChannel(),
			viewport.getGreenChannel(),
			viewport.getAlphaChannel()
		};
		context->ClearRenderTargetView(rtv, color);
	}

	//bool RenderTargetManager::beginRenderMRT(size_t start, size_t count, ID3D11DeviceContext* ctx, const Viewport& vp, bool clear)
	//{
	//	if (!ctx) return false;
	//	if (count == 0) return false;
	//	if (start >= m_renderTargets.size()) return false;
	//	if (start + count > m_renderTargets.size()) return false;

	//	pushState(ctx);

	//	setViewportFrom(vp, ctx);
		
		// Collect RTVs
	//	std::vector<ID3D11RenderTargetView*> rtvs;
	//	rtvs.reserve(count);

	//	for (size_t i = 0; i < count; ++i)
	//	{
	//		auto* rtv = m_renderTargets[start + i]->getRenderTargetView();
	//		if (!rtv) { popState(ctx); return false; }
	//		rtvs.push_back(rtv);
	//	}

	//	ctx->OMSetRenderTargets((UINT)rtvs.size(), rtvs.data(), nullptr);

	//	if (clear)
	//	{
	//		float c[4];
	//		getClearColorFrom(vp, c);
	//		for (auto* rtv : rtvs)
	//			ctx->ClearRenderTargetView(rtv, c);
	//	}
	//	return true;
	//}

	void RenderTargetManager::endRender(ID3D11DeviceContext* context)
	{
		if (!context)
			return;

		// Restore render target
		context->OMSetRenderTargets(1, &m_previousRTV, m_previousDSV);

		// Restore viewport
		context->RSSetViewports(m_previousViewportCount, &m_previousViewport);

		if (m_previousRTV)
			m_previousRTV->Release();

		if (m_previousDSV)
			m_previousDSV->Release();
	}

	/*bool RenderTargetManager::clearTarget(size_t index, ID3D11DeviceContext* ctx, const float color[4])
	{
		if (!ctx) return false;
		if (index >= m_renderTargets.size()) return false;

		pushState(ctx);

		ID3D11RenderTargetView* rtv = m_renderTargets[index]->getRenderTargetView();
		if (!rtv) { popState(ctx); return false; }

		ctx->OMSetRenderTargets(1, &rtv, nullptr);
		ctx->ClearRenderTargetView(rtv, color);

		popState(ctx);
		return true;
	}*/
}