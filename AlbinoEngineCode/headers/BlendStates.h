#pragma once

#include <d3d11.h>

namespace AlbinoEngine
{
	class BlendStates
	{
	public:
		static ID3D11BlendState* opaque(ID3D11Device* device)
		{
			D3D11_BLEND_DESC opaqueDesc = {};
			ID3D11BlendState* opaque = nullptr;
			opaqueDesc.AlphaToCoverageEnable = TRUE;
			opaqueDesc.IndependentBlendEnable = TRUE;

			auto& renderTarget = opaqueDesc.RenderTarget[0];

			renderTarget.BlendEnable = FALSE;
			renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
			renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			renderTarget.DestBlend = D3D11_BLEND_BLEND_FACTOR;
			renderTarget.DestBlendAlpha = D3D11_BLEND_BLEND_FACTOR;
			renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			renderTarget.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
			renderTarget.SrcBlendAlpha = D3D11_BLEND_BLEND_FACTOR;

			device->CreateBlendState(&opaqueDesc, &opaque);
			return opaque;
		}

		static ID3D11BlendState* alphaBlend(ID3D11Device* device)
		{
			D3D11_BLEND_DESC alphaBlendDesc = {};
			alphaBlendDesc.AlphaToCoverageEnable = FALSE;
			alphaBlendDesc.IndependentBlendEnable = FALSE;

			auto& renderTarget = alphaBlendDesc.RenderTarget[0];
			renderTarget.BlendEnable = TRUE;

			// Color: out.rgb = src.rgb * src.a + dst.rgb * (1 - src.a)
			renderTarget.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
			renderTarget.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			renderTarget.BlendOp = D3D11_BLEND_OP_ADD;

			// Alpha: out.a = src.a * 1 + dst.a * 0 (common choice)
			renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
			renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
			renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;

			renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			ID3D11BlendState* alphaBlendState = nullptr;
			HRESULT hr = device->CreateBlendState(&alphaBlendDesc, &alphaBlendState);
			return SUCCEEDED(hr) ? alphaBlendState : nullptr;
		}
	};
}