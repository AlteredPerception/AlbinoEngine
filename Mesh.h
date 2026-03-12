#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>
#include "GeometryGenerator.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "Pass.h"

namespace AlbinoEngine
{
	struct alignas(16) Offset
	{
		float xOffSet, yOffSet, pad;
	};
	static_assert(sizeof(Offset) % 16 == 0);

	struct alignas(16) WorldViewProjection
	{
		DirectX::XMMATRIX worldViewProjection;
	};
	static_assert(sizeof(WorldViewProjection) % 16 == 0);


	using MeshVertexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;
	using MeshIndexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;
	using MeshInputLayout = Microsoft::WRL::ComPtr<ID3D11InputLayout>;
	
	class Mesh
	{
	public:
		Mesh(ID3D11Device* meshDevice);

		~Mesh();

		virtual bool initliaze();

		void setVertexShader(VertexShader* vertShader);

		void setPixelShader(PixelShader* pixelShader);

		void render();

		MeshData getMeshData() const { return m_pMeshData; }

		void setPosition(float xPos, float yPos, float zPos);

		void setScale(float xScale, float yScale, float zScale);

		void setRotation(float xRot, float yRot, float zRot);


		void setCamera(Camera* cam) {
			m_pCamera = cam;
		}

		void drawGeometryOnly();

		ID3D11DeviceContext* getMeshContext() const { return this->m_pImmediateContext; }

		DirectX::XMFLOAT3 getMeshPosition() const { return m_pPosition; }

		DirectX::XMFLOAT3 getMeshScale() const { return m_pScale; }

		DirectX::XMFLOAT3 getMeshRotation() const { return m_pRotation; }

		WorldViewProjection getWorldViewProjection() const { return worldViewProjection; }

		std::shared_ptr<ConstantBuffer> getConstantBuffer() const { return this->m_pWorldViewProjection; }

		void setCastShadows(bool cast) { m_castShadows = cast; }

		bool getCastShadows() const { return this->m_castShadows; }

	protected:

		bool m_castShadows = true;
		std::vector<std::unique_ptr<Pass>> passes;
		DirectX::XMFLOAT3 m_pPosition;
		DirectX::XMFLOAT3 m_pScale;
		DirectX::XMFLOAT3 m_pRotation;
		Camera* m_pCamera;
		Offset offsetConstBuffer;
		WorldViewProjection worldViewProjection;
		std::shared_ptr<ConstantBuffer> m_pOffsetBuffer;
		std::shared_ptr<ConstantBuffer> m_pWorldViewProjection;
		VertexShader* m_pVertShader;
		PixelShader* m_pPixelShader;
		MeshData m_pMeshData;
		MeshVertexBuffer m_pMeshVertBuffer;
		MeshIndexBuffer m_pMeshIndexBuffer;
		MeshInputLayout m_pMeshInputLayout;
		ID3D11DeviceContext* m_pImmediateContext;
		ID3D11Device* m_pMeshDevice;
	};
}