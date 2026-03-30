#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace AlbinoEngine
{
	class RenderStates
	{
	public:
		
		static void RSSolid();

		static void RSWireFrame();

		static void RSCullingFront();

		static void RSCullingBack();


	};
}