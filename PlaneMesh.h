#pragma once

#include "Mesh.h"

namespace AlbinoEngine
{
	class PlaneMesh : public Mesh
	{
	public:
		PlaneMesh(ID3D11Device* planeDevice);

		~PlaneMesh();

		void setProperties(float width, float depth, float scale, 
			float tileU, float tileV);

		bool initialize();
	};
}