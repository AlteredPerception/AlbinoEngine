#pragma once
#include "Mesh.h"

namespace AlbinoEngine
{
	class TriangleMesh : public Mesh
	{
	public:
		TriangleMesh(ID3D11Device* device);

		~TriangleMesh();

		bool initliaze();
	};
}