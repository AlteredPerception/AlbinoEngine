#pragma once

#include "Window.h"
#include "D3D11Renderer.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "RenderTargetManager.h"

namespace AlbinoEngine
{
	class IScene;

	class Engine
	{
	public:
		Engine();

		~Engine();

		bool initialize(std::wstring appName = L"Albino Engine", UINT width = 800, UINT height = 600);

		void messagePump();

		void update(float delta);

		// Scene (lives in the game project; Engine just hosts it)
		void setScene(std::unique_ptr<IScene> scene);

		int run();

		// Convenience accessors for game code / scenes
		Window& window() const { return *m_MainWindow; }
		D3D11Renderer& renderer() const { return *m_MainRenderer; }
		ID3D11Device* device() const { return m_MainRenderer ? m_MainRenderer->getDevice() : nullptr; }
		ID3D11DeviceContext* context() const { return m_MainRenderer ? m_MainRenderer->getContext() : nullptr; }

		TextureManager& getTextureManager() const { return *m_TextureManager.get(); }

		EffectManager& getEffectsManager() const { return *m_effectManager; }

		MeshManager& getMeshManager() const { return *m_MeshManager.get(); }

		RenderTargetManager& getRenderTargetManager() const { return *m_renderTargetManager.get(); }

		void createScreenQuad();

	protected:
		bool m_Running;
		std::unique_ptr<Window> m_MainWindow;
		std::unique_ptr<D3D11Renderer> m_MainRenderer;
		std::unique_ptr<TextureManager> m_TextureManager;
		std::unique_ptr<MeshManager> m_MeshManager;
		std::unique_ptr<IScene> m_Scene;
		std::unique_ptr<EffectManager> m_effectManager;
		std::unique_ptr<RenderTargetManager> m_renderTargetManager;
		std::unique_ptr<Viewport> m_renderViewport;
		
		// Screen Quad rasterizer state.
		ComPtr<ID3D11RasterizerState> m_screenRaster;
		// Screen quad variables.
		std::shared_ptr<VertexShader> m_screenVS;
		std::shared_ptr<PixelShader> m_screenPS;

	};
}