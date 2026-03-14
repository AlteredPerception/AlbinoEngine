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
		this->shutDown();
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

		m_renderViewport->setViewportColor(0.0f, 0.5f, 0.0f, 1.0f);

		m_renderTargetManager = std::make_unique<RenderTargetManager>(m_MainRenderer->getDevice());
		m_renderTargetManager->createRenderTarget(width, height);

		m_TextureManager = std::make_unique<TextureManager>(m_MainRenderer->getDevice());
		m_TextureManager->cacheTextures();

		m_MeshManager = std::make_unique<MeshManager>(
			this->m_MainRenderer->getDevice(),
			this->m_MainRenderer->getContext());

		m_effectManager = std::make_unique<EffectManager>();
		m_lightManager = std::make_unique<LightManager>();

		// Default directional light
		DirectionalLight sun{};
		sun.direction = { 0.3f, -1.0f, 0.2f };

		DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&sun.direction);
		d = DirectX::XMVector3Normalize(d);
		DirectX::XMStoreFloat3(&sun.direction, d);
		sun.color = { 1.0f, 0.75f, 1.0f };
		sun.intensity = 1.5f;
		m_lightManager->setDirectionalLight(sun);

		m_shadowMap = std::make_unique<ShadowMap>(m_MainRenderer->getDevice(), 4096, 4096);
		if (!m_shadowMap->initialize())
			return false;
		createScreenQuad();
		createShadowPass();

		m_Running = true;
		return true;
	}

	void Engine::shutDown()
	{
		if (!m_MainRenderer && !m_MainWindow)
			return;

		if (m_Scene)
			m_Scene->destroy();

		m_Scene.reset();

		m_shadowMap.reset();
		m_shadowVS.reset();
		m_screenVS.reset();
		m_screenPS.reset();

		m_shadowRaster.Reset();
		m_screenRaster.Reset();
		m_screenDepthDIsabled.Reset();

		m_lightManager.reset();
		m_effectManager.reset();
		m_MeshManager.reset();
		m_TextureManager.reset();
		m_renderTargetManager.reset();
		m_renderViewport.reset();

		if (m_MainRenderer)
		{
			m_MainRenderer->destroy();
		}
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

	void Engine::createShadowPass()
	{
		m_shadowVS = std::make_shared<VertexShader>(m_MainRenderer->getDevice());
		m_shadowVS->loadVertexShader(
			L"media//shaders//ShadowDepth.shader",
			"vs_shadow",
			"vs_5_0");

		if (!m_shadowVS->getVertexShader())
			return;

		m_shadowEffect = m_effectManager->createEffect("ShadowDepth").get();
		auto& tech = m_shadowEffect->createTechnique("shadow_depth");
		m_shadowEffect->setActiveTechnique("shadow_depth");

		D3D11_RASTERIZER_DESC shadowRS{};
		shadowRS.FillMode = D3D11_FILL_SOLID;
		shadowRS.CullMode = D3D11_CULL_BACK;
		shadowRS.DepthClipEnable = TRUE;
		shadowRS.DepthBias = 1000;
		shadowRS.SlopeScaledDepthBias = 1.0f;
		shadowRS.DepthBiasClamp = 0.0f;
		
		this->device()->CreateRasterizerState(&shadowRS, m_shadowRaster.ReleaseAndGetAddressOf());
		auto shadowPass = std::make_unique<Pass>(m_shadowVS, std::shared_ptr<PixelShader>{});
		shadowPass->name = "ShadowDepth";
		
		if (!shadowPass->buildInputLayout(device()))
			return;

		shadowPass->setRasterizerState(m_shadowRaster.Get());
		tech.addPass(std::move(shadowPass));
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

			m_renderViewport->setViewport(
				0.0f, 0.0f,
				static_cast<float>(m_MainWindow->getWidth()),
				static_cast<float>(m_MainWindow->getHeight()),
				0.0f, 1.0f);

			DirectionalLight sun = m_lightManager->getDirectionalLight();

			// Build light matrices ONCE per frame, before both passes
			DirectX::XMMATRIX lightViewProj = DirectX::XMMatrixIdentity();

			if (m_Scene)
			{
				EffectContext tempFX = m_Scene->buildEffectContext(*this);

				if (tempFX.camera)
				{
					using namespace DirectX;

					const float lightDistance = 25.0f;
					const float orthoWidth = 35.0f;
					const float orthoHeight = 35.0f;
					const float orthoNear = 1.0f;
					const float orthoFar = 100.0f;

					const float shadowMapWidth = static_cast<float>(m_shadowMap->getWidth());
					const float shadowMapHeight = static_cast<float>(m_shadowMap->getHeight());

					XMFLOAT3 camPosFloat = tempFX.camera->getCameraPosition();
					XMVECTOR camPos = XMLoadFloat3(&camPosFloat);

					XMVECTOR camForward = XMVector3Normalize(tempFX.camera->getForwardVector());

					// Center the shadow volume in front of the camera
					XMVECTOR focusPoint =
						XMVectorAdd(
							camPos,
							XMVectorScale(camForward, 15.0f));

					XMVECTOR lightDir =
						XMVector3Normalize(
							XMLoadFloat3(&sun.direction));

					XMVECTOR up = XMVectorSet(0, 1, 0, 0);
					if (fabsf(XMVectorGetY(lightDir)) > 0.95f)
						up = XMVectorSet(0, 0, 1, 0);

					XMVECTOR lightPos =
						XMVectorSubtract(
							focusPoint,
							XMVectorScale(lightDir, lightDistance));

					XMMATRIX lightView = XMMatrixLookAtLH(lightPos, focusPoint, up);

					// -------- Texel snapping --------
					float worldUnitsPerTexelX = orthoWidth / shadowMapWidth;
					float worldUnitsPerTexelY = orthoHeight / shadowMapHeight;

					XMVECTOR focusLS = XMVector3TransformCoord(focusPoint, lightView);

					float snappedX =
						floorf(XMVectorGetX(focusLS) / worldUnitsPerTexelX) * worldUnitsPerTexelX;

					float snappedY =
						floorf(XMVectorGetY(focusLS) / worldUnitsPerTexelY) * worldUnitsPerTexelY;

					XMVECTOR snappedFocusLS = XMVectorSet(
						snappedX,
						snappedY,
						XMVectorGetZ(focusLS),
						1.0f);

					XMMATRIX invLightView = XMMatrixInverse(nullptr, lightView);
					XMVECTOR snappedFocusWS = XMVector3TransformCoord(snappedFocusLS, invLightView);

					lightPos =
						XMVectorSubtract(
							snappedFocusWS,
							XMVectorScale(lightDir, lightDistance));

					lightView = XMMatrixLookAtLH(lightPos, snappedFocusWS, up);
					// --------------------------------

					XMMATRIX lightProj =
						XMMatrixOrthographicLH(orthoWidth, orthoHeight, orthoNear, orthoFar);

					lightViewProj = lightView * lightProj;
				}
			}

			// ===================================================== //
			// Stage 0: Shadow map pass
			// ===================================================== //
			ID3D11ShaderResourceView* nullShadowSRV[1] = { nullptr };
			context()->PSSetShaderResources(1, 1, nullShadowSRV);

			if (m_shadowEffect)
			{
				EffectContext shadowFx{};
				shadowFx.device = device();
				shadowFx.context = context();
				shadowFx.camera = nullptr;
				shadowFx.directionalLight = nullptr;
				shadowFx.useViewProjOverride = true;
				shadowFx.viewProjOverride = lightViewProj;

				m_shadowMap->begin(context());
				m_MeshManager->renderAllWithOverrideEffect(*m_shadowEffect, shadowFx, "ScreenQuad");
				m_shadowMap->end(context());
			}

			// ===================================================== //
			// Stage 1: Render world-space meshes to offscreen RT
			// ===================================================== //
			m_renderTargetManager->beginRender(
				0,
				m_MainRenderer->getContext(),
				*m_renderViewport,
				m_MainRenderer->getDepthBuffer()->getDepthStencilView());

			if (m_Scene)
			{
				EffectContext sceneFX = m_Scene->buildEffectContext(*this);
				sceneFX.directionalLight = &m_lightManager->getDirectionalLight();
				sceneFX.shadowMap = m_shadowMap.get();
				sceneFX.lightViewProjection = lightViewProj;

				m_MeshManager->renderAllExceptEffect(*m_effectManager, sceneFX, "ScreenQuad");
			}

			m_renderTargetManager->endRender(m_MainRenderer->getContext());

			// ===================================================== //
			// Stage 2: Bind backbuffer and draw fullscreen quad only
			// ===================================================== //
			m_MainRenderer->beginFrame(0, 0, 0, 1.0f);

			EffectContext quadFX{};
			quadFX.device = device();
			quadFX.context = context();
			quadFX.camera = nullptr;
			quadFX.directionalLight = nullptr;
			quadFX.shadowMap = nullptr;

			m_MeshManager->renderOnlyEffect(*m_effectManager, quadFX, "ScreenQuad");

			ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
			m_MainRenderer->getContext()->PSSetShaderResources(0, 1, nullSRV);

			if (m_Scene)
				m_Scene->render(*this);

			m_MainRenderer->present();
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