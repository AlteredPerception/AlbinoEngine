#pragma once

#include <d3d11.h>
#include "RasterizerStates.h"
#include "DepthStencilStates.h"
#include "BlendStates.h"

namespace AlbinoEngine
{
	class CommonRenderStates
	{
	public:
		static RasterizerStates getRasterStates()
		{
			RasterizerStates states;
			return states;
		}

		static DepthStencilStates getDepthStencilStates()
		{
			DepthStencilStates states;
			return states;
		}

		static BlendStates getBlendStates()
		{
			BlendStates states;
			return states;
		}
	};
}