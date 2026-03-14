#include "D3D11Renderer.h"
#include <windows.h>
#include <strsafe.h>

namespace
{
	using DXGIGetDebugInterfaceFn = HRESULT(WINAPI*)(REFIID riid, void** ppv);
	using DXGIGetDebugInterface1Fn = HRESULT(WINAPI*)(UINT flags, REFIID riid, void** ppv);

	HRESULT GetDXGIDebug(Microsoft::WRL::ComPtr<IDXGIDebug>& outDbg)
	{
		outDbg.Reset();

		HMODULE h = LoadLibraryW(L"dxgidebug.dll");
		if (!h) return HRESULT_FROM_WIN32(GetLastError());

		// Prefer Interface1 if present
		auto p1 = reinterpret_cast<DXGIGetDebugInterface1Fn>(
			GetProcAddress(h, "DXGIGetDebugInterface1"));

		if (p1)
		{
			IDXGIDebug* dbg = nullptr;
			HRESULT hr = p1(0, __uuidof(IDXGIDebug), (void**)&dbg);
			if (SUCCEEDED(hr)) outDbg.Attach(dbg);
			FreeLibrary(h);
			return hr;
		}

		// Fallback to older export
		auto p0 = reinterpret_cast<DXGIGetDebugInterfaceFn>(
			GetProcAddress(h, "DXGIGetDebugInterface"));

		if (!p0)
		{
			HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
			FreeLibrary(h);
			return hr;
		}

		IDXGIDebug* dbg = nullptr;
		HRESULT hr = p0(__uuidof(IDXGIDebug), (void**)&dbg);
		if (SUCCEEDED(hr)) outDbg.Attach(dbg);

		FreeLibrary(h);
		return hr;
	}
}


namespace AlbinoEngine
{
	D3D11Renderer::D3D11Renderer()
	{
		mainViewport = {};
	}

	D3D11Renderer::~D3D11Renderer()
	{
		
	}

	void D3D11Renderer::createRenderTarget()
	{
		ID3D11Texture2D* backBuffer = nullptr;
		m_pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		m_pDevice->CreateRenderTargetView(backBuffer, NULL, m_pRenderTargetView.ReleaseAndGetAddressOf());
		ReleaseInterface(backBuffer);
	}

	void D3D11Renderer::unbindEverything()
	{
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		m_pDevContext->OMSetRenderTargets(1, nullRTV, nullptr);

		ID3D11ShaderResourceView* nullSRV[16] = {};
		m_pDevContext->VSSetShaderResources(0, 16, nullSRV);
		m_pDevContext->PSSetShaderResources(0, 16, nullSRV);

		ID3D11SamplerState* nullSamplers[16] = {};
		m_pDevContext->PSSetSamplers(0, 16, nullSamplers);
		m_pDevContext->VSSetSamplers(0, 16, nullSamplers);

		ID3D11Buffer* nullCBs[14] = {};
		m_pDevContext->VSSetConstantBuffers(0, 14, nullCBs);
		m_pDevContext->PSSetConstantBuffers(0, 14, nullCBs);

		m_pDevContext->VSSetShader(nullptr, nullptr, 0);
		m_pDevContext->PSSetShader(nullptr, nullptr, 0);
		m_pDevContext->IASetInputLayout(nullptr);

	}

	bool D3D11Renderer::destroy()
	{
		if (m_pDevContext)
		{
			this->unbindEverything();

			m_pDevContext->ClearState();
			m_pDevContext->Flush();
		}

		m_pRenderTargetView.Reset();
		m_pDepthBuffer.reset();

		if (m_pDXGISwapChain)
		{
			m_pDXGISwapChain->SetFullscreenState(FALSE, 0);
			m_pDXGISwapChain.Reset();
		}

		if (m_pDevContext)
		{
			m_pDevContext.Reset();
		}

#if defined(_DEBUG)
		if (m_pD3DDebugInfo)
		{
			m_pD3DDebugInfo->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
			m_pD3DDebugInfo.Reset();
		}
		if (m_pDXGIDebug)
		{
			m_pDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
			m_pDXGIDebug.Reset();
		}
#endif
		if (m_pDevice)
		{
			m_pDevice.Reset();
			
		}
		return true;
	}
	bool D3D11Renderer::createD3D11Renderer(HWND hWnd, UINT width, UINT height)
	{
		UINT flags = 0;
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = TRUE;

#if defined(_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT hr = D3D11CreateDeviceAndSwapChain(0, 
			D3D_DRIVER_TYPE_HARDWARE, 
			NULL, 
			flags, 
			0, 
			0, 
			D3D11_SDK_VERSION, 
			&swapChainDesc, 
			m_pDXGISwapChain.ReleaseAndGetAddressOf(), 
			m_pDevice.ReleaseAndGetAddressOf(), 
			0, 
			m_pDevContext.ReleaseAndGetAddressOf());

		if (FAILED(hr)) {
			OutputDebugString(L"Device, Context, and swap chain not created.");
			return false;
		}

#if defined(_DEBUG)
		if (m_pDevice)
		{
			hr = m_pDevice->QueryInterface(IID_PPV_ARGS(this->m_pD3DDebugInfo.ReleaseAndGetAddressOf()));
			if (FAILED(hr)) {
				OutputDebugString(L"Could not get debug info.");
				return false;
			}
		}

		HRESULT hrDbg = GetDXGIDebug(m_pDXGIDebug);

		if (FAILED(hrDbg))
		{
			OutputDebugString(L"Could not get IDXGIDebug1 (Graphics Tools may be missing");
		}
#endif
		
		m_pDepthBuffer = std::make_shared<DepthBuffer>(m_pDevice.Get());
		m_pDepthBuffer->createDepthBuffer(width, height);
		m_pDepthBuffer->enableDepth();
		m_pDevContext->RSSetState(m_pDepthBuffer->getRasterState());
		this->createRenderTarget();
		mainViewport = {};
		mainViewport.TopLeftX = 0;
		mainViewport.TopLeftY = 0;
		mainViewport.Width = width;
		mainViewport.Height = height;
		mainViewport.MinDepth = 0.0f;
		mainViewport.MaxDepth = 1.0f;
		m_pDevContext->RSSetViewports(1, &mainViewport);
		return (hr == S_OK);
	}
	void D3D11Renderer::beginFrame(float r, float b, float g, float a)
	{
		float color[4] = { r,b,g,a };
		m_pDevContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), this->m_pDepthBuffer->getDepthStencilView());
		m_pDevContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
		m_pDevContext->ClearDepthStencilView(
			m_pDepthBuffer->getDepthStencilView(), 
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
			1.0f, 0);
	}

	void D3D11Renderer::present()
	{
		m_pDXGISwapChain->Present(0, 0);
	}

}
