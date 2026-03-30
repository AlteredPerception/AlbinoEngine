#pragma once


#include <cstdint>
#include <vector>

#include "Vertex.h"

namespace AlbinoEngine
{
	struct SubMesh
	{
		uint32_t startIndex = 0;
		uint32_t indexCount = 0;
		SubMesh() = default;
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
		// Fullscreen quad in clip space.
		static MeshData generateFullscreenQuad()
		{
			MeshData md{};

			md.vertices =
			{
				positionUV(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f),
				positionUV(-1.0f,  1.0f, 0.0f, 0.0f, 0.0f),
				positionUV(1.0f,  1.0f, 0.0f, 1.0f, 0.0f),
				positionUV(1.0f, -1.0f, 0.0f, 1.0f, 1.0f)
			};
			md.indices = {
				0, 1, 2,
				0, 2, 3
			};

			md.stride = sizeof(Vertex);
			md.offset = 0;

			return md;
		}

		// Plane on XZ (Y up), centered at origin
		static MeshData generatePlane(float width, float depth, float scale, float tileU, float tileV)
		{
			MeshData md{};

			const float halfW = (width * scale) * 0.5f;
			const float halfD = (depth * scale) * 0.5f;

			md.vertices =
			{
				positionNormalUV(-halfW, 0.0f, -halfD, 0.0f, 1.0f, 0.0f, 0.0f,  tileV),
				positionNormalUV(-halfW, 0.0f,  halfD, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f),
				positionNormalUV(halfW, 0.0f,  halfD, 0.0f, 1.0f, 0.0f, tileU, 0.0f),
				positionNormalUV(halfW, 0.0f, -halfD, 0.0f, 1.0f, 0.0f, tileU, tileV)
			};

			md.indices =
			{
				0, 1, 2,
				0, 2, 3
			};

			md.stride = sizeof(Vertex);
			md.offset = 0;
			return md;
		}

		// Debug cube / bounding box cube (8 verts, 36 indices)
		static MeshData generateDebugCube(float scale = 1.0f)
		{
			MeshData md{};

			const float h = 0.5f * scale;

			md.vertices =
			{
				positionColor(-h, -h,  h, 1,1,1,1), // 0
				positionColor(-h,  h,  h, 1,1,1,1), // 1
				positionColor(h,  h,  h, 1,1,1,1), // 2
				positionColor(h, -h,  h, 1,1,1,1), // 3
				positionColor(-h, -h, -h, 1,1,1,1), // 4
				positionColor(-h,  h, -h, 1,1,1,1), // 5
				positionColor(h,  h, -h, 1,1,1,1), // 6
				positionColor(h, -h, -h, 1,1,1,1)  // 7
			};

			md.indices =
			{
				0,1,2, 0,2,3, // front
				4,6,5, 4,7,6, // back
				4,5,1, 4,1,0, // left
				3,2,6, 3,6,7, // right
				1,5,6, 1,6,2, // top
				4,0,3, 4,3,7  // bottom
			};

			md.stride = sizeof(Vertex);
			md.offset = 0;
			return md;
		}

		// Textured/lit cube with 24 unique verts (4 per face)
		static MeshData generateUnitCubeTexture(float scale = 1.0f)
		{
			MeshData md{};

			const float h = 0.5f * scale;

			// +Z
			md.vertices.push_back(positionNormalUV(-h, -h, h, 0, 0, 1, 0, 1));
			md.vertices.push_back(positionNormalUV(-h, h, h, 0, 0, 1, 0, 0));
			md.vertices.push_back(positionNormalUV(h, h, h, 0, 0, 1, 1, 0));
			md.vertices.push_back(positionNormalUV(h, -h, h, 0, 0, 1, 1, 1));

			// -Z
			md.vertices.push_back(positionNormalUV(h, -h, -h, 0, 0, -1, 0, 1));
			md.vertices.push_back(positionNormalUV(h, h, -h, 0, 0, -1, 0, 0));
			md.vertices.push_back(positionNormalUV(-h, h, -h, 0, 0, -1, 1, 0));
			md.vertices.push_back(positionNormalUV(-h, -h, -h, 0, 0, -1, 1, 1));

			// +X
			md.vertices.push_back(positionNormalUV(h, -h, h, 1, 0, 0, 0, 1));
			md.vertices.push_back(positionNormalUV(h, h, h, 1, 0, 0, 0, 0));
			md.vertices.push_back(positionNormalUV(h, h, -h, 1, 0, 0, 1, 0));
			md.vertices.push_back(positionNormalUV(h, -h, -h, 1, 0, 0, 1, 1));

			// -X
			md.vertices.push_back(positionNormalUV(-h, -h, -h, -1, 0, 0, 0, 1));
			md.vertices.push_back(positionNormalUV(-h, h, -h, -1, 0, 0, 0, 0));
			md.vertices.push_back(positionNormalUV(-h, h, h, -1, 0, 0, 1, 0));
			md.vertices.push_back(positionNormalUV(-h, -h, h, -1, 0, 0, 1, 1));

			// +Y
			md.vertices.push_back(positionNormalUV(-h, h, h, 0, 1, 0, 0, 1));
			md.vertices.push_back(positionNormalUV(-h, h, -h, 0, 1, 0, 0, 0));
			md.vertices.push_back(positionNormalUV(h, h, -h, 0, 1, 0, 1, 0));
			md.vertices.push_back(positionNormalUV(h, h, h, 0, 1, 0, 1, 1));

			// -Y
			md.vertices.push_back(positionNormalUV(-h, -h, -h, 0, -1, 0, 0, 1));
			md.vertices.push_back(positionNormalUV(-h, -h, h, 0, -1, 0, 0, 0));
			md.vertices.push_back(positionNormalUV(h, -h, h, 0, -1, 0, 1, 0));
			md.vertices.push_back(positionNormalUV(h, -h, -h, 0, -1, 0, 1, 1));

			for (uint32_t face = 0; face < 6; ++face)
			{
				uint32_t base = face * 4;
				md.indices.push_back(base + 0);
				md.indices.push_back(base + 1);
				md.indices.push_back(base + 2);
				md.indices.push_back(base + 0);
				md.indices.push_back(base + 2);
				md.indices.push_back(base + 3);
			}

			md.stride = sizeof(Vertex);
			md.offset = 0;
			return md;
		}

		static MeshData generateTriangle()
		{
			MeshData md{};

			md.vertices =
			{
				positionColor(0.0f,  0.5f, 0.0f, 1,0,0,1),
				positionColor(0.5f, -0.5f, 0.0f, 0,1,0,1),
				positionColor(-0.5f, -0.5f, 0.0f, 0,0,1,1)
			};

			md.indices = { 0, 1, 2 };

			md.stride = sizeof(Vertex);
			md.offset = 0;
			return md;
		}

		static MeshData generateTwoTrianglesSubmeshes()
		{
			MeshData md{};

			md.vertices =
			{
				positionColor(-0.8f,  0.4f, 0.0f, 1,0,0,1),
				positionColor(-0.2f, -0.4f, 0.0f, 0,1,0,1),
				positionColor(-1.4f, -0.4f, 0.0f, 0,0,1,1),

				positionColor(0.8f,  0.4f, 0.0f, 1,1,0,1),
				positionColor(1.4f, -0.4f, 0.0f, 0,1,1,1),
				positionColor(0.2f, -0.4f, 0.0f, 1,0,1,1)
			};

			md.indices =
			{
				0, 1, 2,
				3, 4, 5
			};

			md.submeshes.emplace_back(0, 3);
			md.submeshes.emplace_back(3, 3);

			md.stride = sizeof(Vertex);
			md.offset = 0;
			return md;
		}
	};
}
