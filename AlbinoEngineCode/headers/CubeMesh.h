#pragma once

#include "Mesh.h"

namespace AlbinoEngine
{
	class CubeMesh : public Mesh
	{
	public:
		CubeMesh(ID3D11Device* device);

		~CubeMesh();

		bool initliaze();

	};
}