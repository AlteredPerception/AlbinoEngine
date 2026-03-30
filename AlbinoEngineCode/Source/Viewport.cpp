#include "Viewport.h"

namespace AlbinoEngine
{
	Viewport::Viewport(float topX, float topY, float width, float height, float minZ, float maxZ)
	{
		this->m_topX = topX;
		this->m_topY = topY;
		this->m_width = width;
		this->m_height = height;
		this->m_minZ = minZ;
		this->m_maxZ = maxZ;
		this->m_color[0] = 1.0f;
		this->m_color[1] = 1.0f;
		this->m_color[2] = 1.0f;
		this->m_color[3] = 1.0f;
	}

	Viewport::~Viewport()
	{

	}

	void Viewport::setViewport(float topX, float topY, float width, float height, float minZ, float maxZ)
	{
		this->m_topX = topX;
		this->m_topY = topY;
		this->m_width = width;
		this->m_height = height;
		this->m_minZ = minZ;
		this->m_maxZ = maxZ;
	}

	void Viewport::setViewportColor(float r, float g, float b, float a)
	{
		this->m_color[0] = r;
		this->m_color[1] = g;
		this->m_color[2] = b;
		this->m_color[3] = a;
	}
}