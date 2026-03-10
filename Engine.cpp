#include "Engine.h"

#include <Windows.h>
#include <cstdlib>

#include "Pass.h"
#include "Technique.h"
#include "ScreenQuadMesh.h"

namespace AlbinoEngine
{
	Engine::Engine() = default;
	

	Engine::~Engine()
	{
		shutDown();
	}

	bool Engine::initialize(const wchar_t* title, UINT width, UINT height)
	{
		m_MainWindow = std::make_unique<Window>();
		if (!m_MainWindow->createWindow(title, width, height))
			return false;

		m_MainWindow->show();

		m_MainRenderer = std::make_unique<D3D11Renderer>();
		if (!m_MainRenderer->createD3D11Renderer(m_MainWindow->getWindowHandle(), width, height))
			return false;

		m_renderViewport = std::make_unique<Viewport>(
			0.0f, 0.0f,
			static_cast<float>(width), 
			static_cast<float>(height),
			0.0f,1.0f);

		m_renderViewport->setViewportColor(0.0f, 0.0f, 0.0f, 1.0f);

		m_renderTargetManager = std::make_unique<RenderTargetManager>(m_MainRenderer->getDevice());
		m_renderTargetManager->createRenderTarget(width, height);

		m_TextureManager = std::make_unique<TextureManager>(m_MainRenderer->getDevice());
		m_TextureManager->cacheTextures();

		m_MeshManager = std::make_unique<MeshManager>(
			this->m_MainRenderer->getDevice(),
			this->m_MainRenderer->getContext());

		m_effectManager = std::make_unique<EffectManager>();

		createScreenQuad();

		m_Running = true;
		return true;
	}

	void Engine::shutDown()
	{
		if (m_MainRenderer)
		{
			m_MainRenderer->destroy();
		}

		m_Scene.reset();
		m_effectManager.reset();
		m_MeshManager.reset();
		m_TextureManager.reset();
		m_renderTargetManager.reset();
		m_renderViewport.reset();
		m_MainRenderer.reset();
		m_MainWindow.reset();

		m_Running = false;
	}
	

	void Engine::setScene(std::unique_ptr<IScene> scene)
	{
		m_Scene = std::move(scene);
		if (m_Scene)
			m_Scene->initialize(*this);
	}

	void Engine::messagePump()
	{
		MSG msg = {};
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				m_Running = false;

		}
	}

	
	int Engine::run()
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);

		LARGE_INTEGER prev;
		QueryPerformanceCounter(&prev);

		while (m_Running)
		{
			this->messagePump();

			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			float dt = static_cast<float>(now.QuadPart - prev.QuadPart) / float(freq.QuadPart);
			prev = now;

			if (m_Scene)
				m_Scene->update(*this, dt);

			// Keep viewport with current window size.
			m_renderViewport->setViewport(0.0f, 0.0f, 
				static_cast<float>(m_MainWindow->getWidth()), 
				static_cast<float>(m_MainWindow->getHeight()), 
				0.0f, 1.0f);
			
			// ====================================================//
			// Stage 1: Render world-space meshes to offscreen RT
			// ===================================================//
			this->m_renderTargetManager->beginRender(0, 
				m_MainRenderer->getContext(), 
				*this->m_renderViewport, 
				this->m_MainRenderer->getDepthBuffer()->getDepthStencilView());

			if (m_Scene)
			{
				EffectContext sceneFX = m_Scene->buildEffectContext(*this);
				m_MeshManager->renderAllExceptEffect(*m_effectManager, sceneFX, "ScreenQuad");
			}
				//m_Scene->render(*this);
			
			this->m_renderTargetManager->endRender(m_MainRenderer->getContext());
			
			// ======================================================= //
			// Stage 2: Bind backbuffer and draw fullscreen quad only
			// ======================================================= //
			m_MainRenderer->beginFrame(0, 0, 0, 1.0);

			EffectContext quadFX = {};
			quadFX.device = this->device();
			quadFX.context = this->context();
			quadFX.camera = nullptr;
			m_MeshManager->renderOnlyEffect(*m_effectManager, quadFX, "ScreenQuad");

			// Unbind SRV used by fullscreen pass to avoid next-frame RTV/SRV
			ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
			m_MainRenderer->getContext()->PSSetShaderResources(0, 1, nullSRV);

			if (m_Scene)
				m_Scene->render(*this);

			this->m_MainRenderer->present();
		}

		return EXIT_SUCCESS;
	}

	void Engine::createScreenQuad()
	{
		// Load screen-space shaders.
		m_screenVS = std::make_shared<VertexShader>(m_MainRenderer->getDevice());
		m_screenPS = std::make_shared<PixelShader>(m_MainRenderer->getDevice());

		m_screenVS->loadVertexShader(L"media//shaders//PositionTexture2DQuad.shader", "vs_mainQuad", "vs_5_0");
		m_screenPS->loadPixelShader(L"media//shaders//PositionTexture2DQuad.shader", "ps_mainQuad", "ps_5_0");
		if (!m_screenVS->getVertexShader()) 
			return;

		if (!m_screenPS->getPixelShader()) 
			return;

		// create the screen quad effect.
		auto screenEff = this->getEffectsManager().createEffect("ScreenQuad").get();
		auto& screenTech = screenEff->createTechnique("screen_quad");
		screenEff->setActiveTechnique("screen_quad");

		// Rasterizer state.
		{
			D3D11_RASTERIZER_DESC screenDesc = {};
			screenDesc.FillMode = D3D11_FILL_SOLID;
			screenDesc.CullMode = D3D11_CULL_NONE;
			screenDesc.DepthClipEnable = TRUE;
			this->device()->CreateRasterizerState(&screenDesc, m_screenRaster.ReleaseAndGetAddressOf());
		}
		
		// Depth-disabled state for fullscreen pass
		{
			D3D11_DEPTH_STENCIL_DESC ds = {};
			ds.DepthEnable = FALSE;
			ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			ds.DepthFunc = D3D11_COMPARISON_ALWAYS;
			ds.StencilEnable = FALSE;
			this->device()->CreateDepthStencilState(&ds, this->m_screenDepthDIsabled.ReleaseAndGetAddressOf());
		}

		// Create fullscreen Pass
		{
			auto fullscreenPass = std::make_unique<Pass>(m_screenVS, m_screenPS);
			fullscreenPass->name = "Fullscreen";

			if (!fullscreenPass->buildInputLayout(this->device()))
				return;

			fullscreenPass->setRasterizerState(m_screenRaster.Get());
			fullscreenPass->setDepthStencialState(this->m_screenDepthDIsabled.Get());
			fullscreenPass->setUsePerObjectCB(false);

			auto* rt = this->m_renderTargetManager->get(0);
			if (!rt)
				return;

			if (!rt->getShaderResourceView())
				return;

			if (!rt->getSampler())
				return; 

			if (!rt->getSampler()->getSamplerState())
				return;

			fullscreenPass->setTexture(0, rt->getShaderResourceView());
			fullscreenPass->setSampler(0, rt->getSampler()->getSamplerState());
			screenTech.addPass(std::move(fullscreenPass));
		}

		Mesh* screenQuad = this->getMeshManager().createMesh(L"mainQuad", L"screenQuadMesh");
		if (!screenQuad)
			return;

		this->getMeshManager().setMeshEffect(L"mainQuad", "ScreenQuad", "screen_quad");

	}
}