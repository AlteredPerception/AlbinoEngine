#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>

#include "Window.h"
#include "D3D11Renderer.h"
#include "Viewport.h"
#include "RenderTargetManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "EffectManager.h"
#include "InputManager.h"
#include "Scene.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "LightManager.h"
#include "ShadowMap.h"
#include "PointShadowMap.h"


namespace AlbinoEngine
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		bool initialize(const wchar_t* title, UINT width, UINT height);

		int run();
		void shutDown();

		// Scene (lives in the game project; Engine just hosts it)
		void setScene(std::unique_ptr<IScene> scene);


		// Convenience accessors for game code / scenes
		Window& window() { return *m_MainWindow; }
		const Window& window() const { return *m_MainWindow; }
		InputManager& input() { return *m_input; }
		const InputManager& input() const { return *m_input; }

		D3D11Renderer& renderer() const { return *m_MainRenderer; }

		ID3D11Device* device() const { return m_MainRenderer ? m_MainRenderer->getDevice() : nullptr; }
		ID3D11DeviceContext* context() const { return m_MainRenderer ? m_MainRenderer->getContext() : nullptr; }

		TextureManager& getTextureManager() const { return *m_TextureManager.get(); }
		MeshManager& getMeshManager() const { return *m_MeshManager.get(); }
		EffectManager& getEffectsManager() const { return *m_effectManager; }
		LightManager& getLightManager() const { return *m_lightManager; }

		// Shorter accessors.
		TextureManager& texture() { return *m_TextureManager; }
		MeshManager& meshes() { return *m_MeshManager; }
		EffectManager& effects() { return *m_effectManager; }
		LightManager& lights() { return *m_lightManager; }
		RenderTargetManager& getRenderTargetManager() const { return *m_renderTargetManager.get(); }

		void createScreenQuad();
		void createShadowPass();
		void createPointShadowPass();
		float getTotalTime() const { return m_totalTime; }

		
	private:
		void messagePump();

	protected:
		bool m_Running = false;

		float m_totalTime = 0.0f;
		std::unique_ptr<Window> m_MainWindow;
		std::unique_ptr<D3D11Renderer> m_MainRenderer;
		std::unique_ptr<Viewport> m_renderViewport;
		std::unique_ptr<RenderTargetManager> m_renderTargetManager;
		std::unique_ptr<TextureManager> m_TextureManager;
		std::unique_ptr<MeshManager> m_MeshManager;
		std::unique_ptr<EffectManager> m_effectManager;
		std::unique_ptr<LightManager> m_lightManager;
		std::unique_ptr<ShadowMap> m_shadowMap;
		std::unique_ptr<IScene> m_Scene;
		std::unique_ptr<InputManager> m_input;
		// Fullscreen quad rendering resources.
		std::shared_ptr<VertexShader> m_screenVS;
		std::shared_ptr<PixelShader> m_screenPS;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_screenRaster;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_shadowRaster;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_screenDepthDIsabled;

		// Shadow pass
		std::shared_ptr<VertexShader> m_shadowVS;
		std::shared_ptr<PixelShader> m_shadowPS;
		Effect* m_shadowEffect = nullptr;

		// PointShadowMap
		std::unique_ptr<PointShadowMap> m_pointShadowMap;
		std::shared_ptr<VertexShader> m_pointShadowVS;
		Effect* m_pointShadowEffect = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pointShadowRaster;

	};
}