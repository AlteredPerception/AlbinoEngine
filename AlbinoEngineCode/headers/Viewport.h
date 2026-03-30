#pragma once

namespace AlbinoEngine
{
	class Viewport
	{
	public:

		Viewport() = default;

		Viewport(float topX, float topY, float width, float height, float minZ, float maxZ);

		~Viewport();

		void setViewport(float topX, float topY, float width, float height, float minZ, float maxZ);

		void setViewportColor(float r, float g, float b, float a);

		float getTopX() const { return m_topX; }

		float getTopY() const { return m_topY; }

		float getWidth() const { return m_width; }

		float getHeight() const { return m_height; }

		float getMinZ() const { return m_minZ; }

		float getMaxZ() const { return m_maxZ; }

		float getRedChannel() const { return m_color[0]; }

		float getGreenChannel() const { return m_color[1]; }

		float getBlueChannel() const { return m_color[2]; }

		float getAlphaChannel() const { return m_color[3]; }
	protected:
		float m_color[4];
		float m_topX;
		float m_topY;
		float m_width;
		float m_height;
		float m_minZ;
		float m_maxZ;
	};
}