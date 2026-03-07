#pragma once

#include <d3d11.h>

namespace AlbinoEngine
{
	class RasterizerStates
	{
	public:
		static ID3D11RasterizerState* solidState(ID3D11Device* device, UINT cullMode = 0)
		{
			D3D11_CULL_MODE mode = getCullMode(cullMode);
			ID3D11RasterizerState* solid = nullptr;
			D3D11_RASTERIZER_DESC solidDesc = {};
			solidDesc.FillMode = D3D11_FILL_SOLID;
			solidDesc.CullMode = mode;
			device->CreateRasterizerState(&solidDesc, &solid);
			return solid;
		}

		static ID3D11RasterizerState* wireframeState(
			ID3D11Device* device, 
			INT depthBias = -3,
			FLOAT SlopScaleDepthBias = -3.0f,
			FLOAT depthBiasClamp = 0.0f,
			UINT cullMode = 0)
		{
			D3D11_CULL_MODE mode = getCullMode(cullMode);
			ID3D11RasterizerState* wireframe = nullptr;
			D3D11_RASTERIZER_DESC wireframeDesc = {};
			wireframeDesc.FillMode = D3D11_FILL_SOLID;
			wireframeDesc.CullMode = mode;
			wireframeDesc.DepthClipEnable = TRUE;
			wireframeDesc.DepthBias = depthBias;
			wireframeDesc.SlopeScaledDepthBias = SlopScaleDepthBias;
			wireframeDesc.DepthBiasClamp = depthBiasClamp;
			device->CreateRasterizerState(&wireframeDesc, &wireframe);
			return wireframe;
		}
	protected:
		static D3D11_CULL_MODE getCullMode(UINT cullMode)
		{
			D3D11_CULL_MODE mode = {};
			switch (mode) {
			case 0:
				mode = D3D11_CULL_NONE;
				break;
			case 1:
				mode = D3D11_CULL_BACK;
				break;
			case 2:
				mode = D3D11_CULL_FRONT;
				break;
			}
			return mode;
		}
	};


}