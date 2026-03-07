#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "DepthBuffer.h"
#include <memory>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
using Microsoft::WRL::ComPtr;

#define ReleaseInterface(x) {if(x){x->Release(); x=0;}}
#define ReleaseClass(x) {if(x){delete x; x = 0;}}

namespace AlbinoEngine
{
	using D3D11Device = ComPtr<ID3D11Device>;
	using D3D11DeviceContext = ComPtr<ID3D11DeviceContext>;
	using DXGISwapChain = ComPtr<IDXGISwapChain>;
	using D3D11RenderTargetView = ComPtr<ID3D11RenderTargetView>;
	using D3D11Debug = ComPtr<ID3D11Debug>;
	using DXGIDebug = ComPtr<IDXGIDebug>;
	using DepthBufferPtr = std::shared_ptr<DepthBuffer>;
	class D3D11Renderer
	{
	public:
		D3D11Renderer();

		~D3D11Renderer();

		bool createD3D11Renderer(HWND hWnd, UINT width, UINT height);

		bool destroy();

		void beginFrame(float r, float b, float g, float a);

		void present();

		ID3D11Device* getDevice() const { return m_pDevice.Get(); }

		ID3D11DeviceContext* getContext() const { return m_pDevContext.Get(); }

		DepthBuffer* getDepthBuffer() const { return m_pDepthBuffer.get(); }

	protected:
		D3D11_VIEWPORT mainViewport;
		void createRenderTarget();
		D3D11Device m_pDevice;
		D3D11DeviceContext m_pDevContext;
		D3D11RenderTargetView m_pRenderTargetView;
		D3D11Debug m_pD3DDebugInfo;
		DXGISwapChain m_pDXGISwapChain;
		DXGIDebug m_pDXGIDebug;
		DepthBufferPtr m_pDepthBuffer;
	};
}