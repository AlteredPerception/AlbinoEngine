#pragma once

#include "Vertex.h"
#include <cstdint>
#include <vector>

namespace AlbinoEngine
{
	struct SubMesh
	{
		uint32_t startIndex = 0;
		uint32_t indexCount = 0;

		SubMesh(uint32_t startIndex, uint32_t indexCount)
			: startIndex(startIndex), indexCount(indexCount) {}
	};

	struct MeshData
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<SubMesh> submeshes;

		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
	};

	class GeometryGenerator
	{
	public:
		// Basic plane on XZ (Y up), centered at origin.
		static MeshData generatePlane(float width, float depth, float scale, 
									  float tileU, float tileV)
		{
			MeshData planeData;

			const float halfWidth = (width * scale) * 0.5f;
			const float halfDepth = (depth * scale) * 0.5f;

			// CCW winding when viewed from +Y.
			planeData.vertices = {
				positionNormalUV(-halfWidth, 0.0f, -halfDepth, 0.0f, 1.0f, 0.0f, 0.0f, tileV),
				positionNormalUV(-halfWidth, 0.0f,  halfDepth, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
				positionNormalUV( halfWidth, 0.0f,  halfDepth, 0.0f, 1.0f, 0.0f, tileU, 0.0f),
				positionNormalUV( halfWidth, 0.0f, -halfDepth, 0.0f, 1.0f, 0.0f, tileU, tileV),
			};

			planeData.indices = {
				0, 1, 2,
				0, 2, 3
			};

			planeData.stride = sizeof(Vertex);
			planeData.offset = 0;
			return planeData;
		}

		// Unit cube centered at origin with UVs per face.
		// 24 unique vertices (4 per face) so each face has correct normal + UVs.
		static MeshData generateUnitCubeTexture()
		{
			MeshData data;
			const float h = 0.5f;

			data.vertices = {
				// Front (+Z)
				positionNormalUV(-h, -h,  h, 0, 0, 1, 0, 1),
				positionNormalUV(-h,  h,  h, 0, 0, 1, 0, 0),
				positionNormalUV( h,  h,  h, 0, 0, 1, 1, 0),
				positionNormalUV( h, -h,  h, 0, 0, 1, 1, 1),

				// Back (-Z)
				positionNormalUV( h, -h, -h, 0, 0, -1, 0, 1),
				positionNormalUV( h,  h, -h, 0, 0, -1, 0, 0),
				positionNormalUV(-h,  h, -h, 0, 0, -1, 1, 0),
				positionNormalUV(-h, -h, -h, 0, 0, -1, 1, 1),

				// Left (-X)
				positionNormalUV(-h, -h, -h, -1, 0, 0, 0, 1),
				positionNormalUV(-h,  h, -h, -1, 0, 0, 0, 0),
				positionNormalUV(-h,  h,  h, -1, 0, 0, 1, 0),
				positionNormalUV(-h, -h,  h, -1, 0, 0, 1, 1),

				// Right (+X)
				positionNormalUV( h, -h,  h, 1, 0, 0, 0, 1),
				positionNormalUV( h,  h,  h, 1, 0, 0, 0, 0),
				positionNormalUV( h,  h, -h, 1, 0, 0, 1, 0),
				positionNormalUV( h, -h, -h, 1, 0, 0, 1, 1),

				// Top (+Y)
				positionNormalUV(-h,  h,  h, 0, 1, 0, 0, 1),
				positionNormalUV(-h,  h, -h, 0, 1, 0, 0, 0),
				positionNormalUV( h,  h, -h, 0, 1, 0, 1, 0),
				positionNormalUV( h,  h,  h, 0, 1, 0, 1, 1),

				// Bottom (-Y)
				positionNormalUV(-h, -h, -h, 0, -1, 0, 0, 1),
				positionNormalUV(-h, -h,  h, 0, -1, 0, 0, 0),
				positionNormalUV( h, -h,  h, 0, -1, 0, 1, 0),
				positionNormalUV( h, -h, -h, 0, -1, 0, 1, 1),
			};

			// 6 faces * 2 triangles * 3 indices = 36
			data.indices = {
				// Front
				0, 1, 2, 0, 2, 3,
				// Back
				4, 5, 6, 4, 6, 7,
				// Left
				8, 9, 10, 8, 10, 11,
				// Right
				12, 13, 14, 12, 14, 15,
				// Top
				16, 17, 18, 16, 18, 19,
				// Bottom
				20, 21, 22, 20, 22, 23
			};

			data.stride = sizeof(Vertex);
			data.offset = 0;
			return data;
		}

		// Unit cube centered at origin using vertex color (no UV requirement).
		static MeshData generateUnitCubeColor()
		{
			MeshData data;
			const float h = 0.5f;

			// Reuse 24-vertex cube so faces are independent (nice for flat shading/debug).
			auto V = [&](float x, float y, float z, float r, float g, float b) {
				return positionColor(x, y, z, r, g, b, 1.0f);
			};

			data.vertices = {
				// Front
				V(-h, -h,  h, 1, 0, 0), V(-h,  h,  h, 1, 0, 0), V( h,  h,  h, 1, 0, 0), V( h, -h,  h, 1, 0, 0),
				// Back
				V( h, -h, -h, 0, 1, 0), V( h,  h, -h, 0, 1, 0), V(-h,  h, -h, 0, 1, 0), V(-h, -h, -h, 0, 1, 0),
				// Left
				V(-h, -h, -h, 0, 0, 1), V(-h,  h, -h, 0, 0, 1), V(-h,  h,  h, 0, 0, 1), V(-h, -h,  h, 0, 0, 1),
				// Right
				V( h, -h,  h, 1, 1, 0), V( h,  h,  h, 1, 1, 0), V( h,  h, -h, 1, 1, 0), V( h, -h, -h, 1, 1, 0),
				// Top
				V(-h,  h,  h, 1, 0, 1), V(-h,  h, -h, 1, 0, 1), V( h,  h, -h, 1, 0, 1), V( h,  h,  h, 1, 0, 1),
				// Bottom
				V(-h, -h, -h, 0, 1, 1), V(-h, -h,  h, 0, 1, 1), V( h, -h,  h, 0, 1, 1), V( h, -h, -h, 0, 1, 1),
			};

			data.indices = {
				0, 1, 2, 0, 2, 3,
				4, 5, 6, 4, 6, 7,
				8, 9, 10, 8, 10, 11,
				12, 13, 14, 12, 14, 15,
				16, 17, 18, 16, 18, 19,
				20, 21, 22, 20, 22, 23
			};

			data.stride = sizeof(Vertex);
			data.offset = 0;
			return data;
		}

		static MeshData generateTriangle()
		{
			MeshData data;
			data.vertices = {
				positionColor(0.0f, 0.5f, 0.0f, 1, 0, 0, 1),
				positionColor(0.5f, -0.5f, 0.0f, 0, 1, 0, 1),
				positionColor(-0.5f, -0.5f, 0.0f, 0, 0, 1, 1),
			};
			data.indices = { 0, 1, 2 };
			data.stride = sizeof(Vertex);
			data.offset = 0;
			return data;
		}

		// Two triangles in one index buffer, each as its own submesh.
		static MeshData generateTwoTriangleSubmeshes()
		{
			MeshData data;
			data.vertices = {
				// left triangle
				positionColor(-0.75f, 0.5f, 0.0f, 1, 0, 0, 1),
				positionColor(-0.25f, -0.5f, 0.0f, 0, 1, 0, 1),
				positionColor(-1.25f, -0.5f, 0.0f, 0, 0, 1, 1),

				// right triangle
				positionColor(0.75f, 0.5f, 0.0f, 1, 1, 0, 1),
				positionColor(1.25f, -0.5f, 0.0f, 0, 1, 1, 1),
				positionColor(0.25f, -0.5f, 0.0f, 1, 0, 1, 1),
			};

			data.indices = {
				0, 1, 2,
				3, 4, 5
			};

			data.submeshes.emplace_back(0, 3);
			data.submeshes.emplace_back(3, 3);

			data.stride = sizeof(Vertex);
			data.offset = 0;
			return data;
		}

		static MeshData generateFullscreenQuad()
		{
			MeshData quad;

			quad.vertices = {
				positionUV(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f),
				positionUV( 1.0f, 1.0f, 0.0f, 1.0f, 0.0f),
				positionUV(-1.0f,-1.0f, 0.0f, 0.0f, 1.0f),
				positionUV( 1.0f,-1.0f, 0.0f, 1.0f, 1.0f)
			};

			quad.indices = {
				0, 1, 2,
				2, 1, 3
			};

			quad.stride = sizeof(quad.vertices[0]);
			quad.offset = 0;

			return quad;
		}
	};
}
