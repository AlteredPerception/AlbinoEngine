#include "Mesh.h"

namespace AlbinoEngine
{
	Mesh::Mesh(ID3D11Device* meshDevice)
	{
		this->m_pVertShader = nullptr;
		this->m_pPixelShader = nullptr;
		this->m_pCamera = nullptr;
		this->m_pMeshDevice = meshDevice;
		this->m_pMeshDevice->GetImmediateContext(&this->m_pImmediateContext);
		//this->m_pOffsetBuffer = std::make_shared<ConstantBuffer>("OffsetBuffer",meshDevice);
		this->m_pWorldViewProjection = std::make_shared<ConstantBuffer>("WorldViewProjection", meshDevice);

		m_pPosition = { 0.0f, 0.0f, 0.0f };
		m_pScale = { 1.0f, 1.0f, 1.0f };
		m_pRotation = { 0.0f, 0.0f,0.0f };

	}

	Mesh::~Mesh()
	{
		this->m_pMeshInputLayout.Reset();
		this->m_pMeshVertBuffer.Reset();
		this->m_pMeshIndexBuffer.Reset();
		if (m_pImmediateContext) { m_pImmediateContext->Release(); m_pImmediateContext = 0; }
	}


	void Mesh::setVertexShader(VertexShader* vertShader)
	{
		this->m_pVertShader = vertShader;
	}

	void Mesh::setPixelShader(PixelShader* pixelShader)
	{
		this->m_pPixelShader = pixelShader;
	}

	bool Mesh::initliaze()
	{
		return true;
	}

	void Mesh::setPosition(float xPos, float yPos, float zPos)
	{
		m_pPosition = { xPos,yPos,zPos };
	}

	void Mesh::setScale(float xScale, float yScale, float zScale)
	{
		m_pScale = { xScale,yScale,zScale };
	}

	void Mesh::setRotation(float xRot, float yRot, float zRot)
	{
		m_pRotation = { xRot,yRot,zRot };
	}

	void Mesh::render()
	{
		if (!m_pCamera) return;

		if (!m_pVertShader) {
			OutputDebugStringA("Mesh::render(): m_pVertShader is nullptr.\n");
			return;
		}
		ID3D11Buffer* worldViewProjBuffer = m_pWorldViewProjection->getConstantBuffer();
	
//		DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(
//			m_pPosition.x, 
//			m_pPosition.y, 
//			m_pPosition.z);
//		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(
//			m_pScale.x, 
//			m_pScale.y, 
//			m_pScale.z);
//		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(
//			DirectX::XMConvertToRadians(m_pRotation.x), 
//			DirectX::XMConvertToRadians(m_pRotation.y),
//			DirectX::XMConvertToRadians(m_pRotation.z));
//		DirectX::XMMATRIX world = scale * rotation * translate;
//		worldViewProjection.worldViewProjection = DirectX::XMMatrixTranspose(
//			world * m_pCamera->getViewProjMatrix()
//		);
//		m_pImmediateContext->UpdateSubresource(worldViewProjBuffer, 0, 0, &this->worldViewProjection, 0, 0);
		
		
		if (passes.empty())
		{
			m_pImmediateContext->VSSetShader(this->m_pVertShader->getVertexShader(), 0, 0);
			m_pImmediateContext->PSSetShader(this->m_pPixelShader->getPixelShader(), 0, 0);
			m_pImmediateContext->IASetInputLayout(this->m_pVertShader->getInputLayout());
			drawGeometryOnly();
			return;
		}
		
	}

	void Mesh::drawGeometryOnly()
	{
		UINT stride = m_pMeshData.stride;
		UINT offset = m_pMeshData.offset;
		m_pImmediateContext->IASetVertexBuffers(0, 1, m_pMeshVertBuffer.GetAddressOf(), &stride, &offset);
		m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (m_pMeshIndexBuffer)
		{
			m_pImmediateContext->IASetIndexBuffer(m_pMeshIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}
		if (m_pMeshData.submeshes.empty())
		{
			if (m_pMeshIndexBuffer)
			{
				m_pImmediateContext->DrawIndexed((UINT)m_pMeshData.indices.size(), 0, 0);
			}
			else
			{
				m_pImmediateContext->Draw((UINT)m_pMeshData.vertices.size(), 0);
			}
			return;
		}

		// Draw each submesh as a subset of the index buffer;
		for (const auto& sm : m_pMeshData.submeshes)
		{
			if (sm.indexCount == 0) continue;
			if (sm.startIndex + sm.indexCount > m_pMeshData.indices.size())
				continue;
			m_pImmediateContext->DrawIndexed(sm.indexCount, sm.startIndex, 0);
		}
	}
}