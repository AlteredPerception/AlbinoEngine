#include "Engine.h"
#include "Scene.h"
#include "EffectManager.h"

namespace AlbinoEngine
{
	Engine::Engine()
	{
		this->m_MainWindow = std::make_unique<Window>();
		this->m_MainRenderer = std::make_unique<D3D11Renderer>();
		this->m_renderViewport = std::make_unique<Viewport>();
		this->m_Running = true;
	}

	Engine::~Engine()
	{
		if (m_MainWindow) m_MainWindow.reset();
		if (m_MainRenderer) m_MainRenderer.reset();
		if (m_TextureManager) m_TextureManager.reset();
		if (m_effectManager) m_effectManager.reset();
		if (m_renderTargetManager) m_renderTargetManager.reset();
		if (m_screenRaster) m_screenRaster.Reset();
	}

	bool Engine::initialize(std::wstring appName, UINT width, UINT height)
	{
		m_MainWindow->createWindow(appName, width, height);
		m_MainRenderer->createD3D11Renderer(m_MainWindow->getWindowHandle(), m_MainWindow->getWidth(), m_MainWindow->getHeight());
		m_MainWindow->show();

		m_TextureManager = std::make_unique<TextureManager>(this->m_MainRenderer->getDevice());
		m_TextureManager->cacheTextures();
		if (m_TextureManager->getUnorderedTextureMap().empty())
			OutputDebugStringA("Textures not cached (map empty)\n");
		else
			OutputDebugStringA("Textures cached\n");

		m_effectManager = std::make_unique<EffectManager>();
		m_MeshManager = std::make_unique<MeshManager>(this->m_MainRenderer->getDevice(), this->m_MainRenderer->getContext());
		m_renderTargetManager = std::make_unique<RenderTargetManager>(m_MainRenderer->getDevice());

		m_renderTargetManager->createRenderTarget(this->m_MainWindow->getWidth(), this->m_MainWindow->getHeight());
		
		this->createScreenQuad();
		return true;
	}

	void Engine::update(float delta)
	{
		if (m_Scene)
			m_Scene->update(*this, delta);
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
			if (msg.message == WM_QUIT)
			{
				m_Running = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Engine::createScreenQuad()
	{
		// Load the shaders;
		m_screenVS = std::make_shared<VertexShader>(m_MainRenderer->getDevice());
		m_screenPS = std::make_shared<PixelShader>(m_MainRenderer->getDevice());
		m_screenVS->loadVertexShader(L"media//shaders//PositionTexture2DQuad.shader", "vs_mainQuad", "vs_5_0");
		m_screenPS->loadPixelShader(L"media//shaders//PositionTexture2DQuad.shader", "ps_mainQuad", "ps_5_0");
		if (!m_screenVS) return;
		if (!m_screenPS) return;

		// create the screen quad effect.
		auto screenEff = this->getEffectsManager().createEffect("ScreenQuad").get();
		auto& screenTech = screenEff->createTechnique("screen quad");
		screenEff->setActiveTechnique(screenTech.name());

		D3D11_RASTERIZER_DESC screenDesc = {};
		screenDesc.FillMode = D3D11_FILL_SOLID;
		screenDesc.CullMode = D3D11_CULL_NONE;
		this->device()->CreateRasterizerState(&screenDesc, m_screenRaster.ReleaseAndGetAddressOf());
		// Create Pass
		{
			auto fullscreenPass = std::make_unique<Pass>(m_screenVS, m_screenPS);
			fullscreenPass->name = "Fullscreen";
			fullscreenPass->buildInputLayout(this->device());
			fullscreenPass->setRasterizerState(m_screenRaster.Get());
			fullscreenPass->setDepthStencialState(m_MainRenderer->getDepthBuffer()->getDepthStencilState());
			fullscreenPass->setUsePerObjectCB(false);
			fullscreenPass->setTexture(0, this->m_renderTargetManager->get(0)->getShaderResourceView());
			fullscreenPass->setSampler(0, this->m_renderTargetManager->get(0)->getSampler()->getSamplerState());
			screenTech.addPass(std::move(fullscreenPass));
		}
		
		Mesh* screenQuad = this->getMeshManager().createMesh(L"mainQuad", L"screenQuadMesh");

		this->getMeshManager().setMeshEffect(L"mainQuad", "ScreenQuad", "screen quad");

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
			float dt = float(now.QuadPart - prev.QuadPart) / float(freq.QuadPart);
			prev = now;
			m_renderViewport->setViewport(0.0f, 0.0f, 
				static_cast<float>(m_MainWindow->getWidth()), 
				static_cast<float>(m_MainWindow->getHeight()), 
				0.0f, 1.0f);
			
			m_renderViewport->setViewportColor(0.0f, 1.0f, 1.0f, 1.0f);
			if(m_Scene) m_Scene->update(*this, dt);
			this->m_renderTargetManager->beginRender(0, m_MainRenderer->getContext(), *this->m_renderViewport, this->m_MainRenderer->getDepthBuffer()->getDepthStencilView());
			if (m_Scene)
			{
				EffectContext sceneFX = m_Scene->buildEffectContext(*this);
				m_MeshManager->renderAllExceptEffect(*m_effectManager, sceneFX, "ScreenQuad");
			}
				//m_Scene->render(*this);
			
			this->m_renderTargetManager->endRender(m_MainRenderer->getContext());
			
			this->m_MainRenderer->beginFrame(0, 0, 1, 1.0);
			EffectContext quadFX = {};
			quadFX.device = this->device();
			quadFX.context = this->context();
			quadFX.camera = nullptr;
			m_MeshManager->renderOnlyEffect(*m_effectManager, quadFX, "ScreenQuad");

			// 3 Unbind SRV used by fullscreen pass to avoid next-frame RTV/SRV
			ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
			m_MainRenderer->getContext()->PSGetShaderResources(0, 1, nullSRV);
			this->m_MainRenderer->present();
		}

		return EXIT_SUCCESS;
	}
}