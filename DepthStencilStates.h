#pragma once

#include <d3d11.h>

namespace AlbinoEngine
{
	class DepthStencilStates
	{
	public:
		static ID3D11DepthStencilState* dsStencilSolid(ID3D11Device* device, UINT cullMode = 0)
		{
			D3D11_CULL_MODE mode = getCullMode(cullMode);
			ID3D11DepthStencilState* dsSolid = nullptr;
			D3D11_DEPTH_STENCIL_DESC dsSolidDesc = {};
			dsSolidDesc.DepthEnable = TRUE;
			dsSolidDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsSolidDesc.DepthFunc = D3D11_COMPARISON_LESS;
			device->CreateDepthStencilState(&dsSolidDesc, &dsSolid);
			return dsSolid;
		}

		static ID3D11DepthStencilState* dsStencilWireframe(ID3D11Device* device, UINT cullMode = 0)
		{
			D3D11_CULL_MODE mode = getCullMode(0);
			ID3D11DepthStencilState* dsWireframe = nullptr;
			D3D11_DEPTH_STENCIL_DESC dsWireFrameDesc = {};
			dsWireFrameDesc.DepthEnable = TRUE;
			dsWireFrameDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsWireFrameDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			device->CreateDepthStencilState(&dsWireFrameDesc, &dsWireframe);
			return dsWireframe;
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