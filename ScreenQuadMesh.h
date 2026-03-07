#pragma once

#include "Mesh.h"

namespace AlbinoEngine
{
	class ScreenQuadMesh : public Mesh
	{
	public:
		ScreenQuadMesh(ID3D11Device* device);

		bool initliaze();

	};
}