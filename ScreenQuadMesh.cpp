#include "ScreenQuadMesh.h"

namespace AlbinoEngine
{
	ScreenQuadMesh::ScreenQuadMesh(ID3D11Device* device)
		: Mesh(device)
	{

	}

	bool ScreenQuadMesh::initliaze()
	{
		this->m_pMeshData = GeometryGenerator::generateFullscreenQuad();

		UINT vertexBufferByteWidth = static_cast<UINT>(m_pMeshData.vertices.size() * sizeof(m_pMeshData.vertices[0]));
		UINT indexBufferByteWidth = static_cast<UINT>(m_pMeshData.indices.size() * sizeof(uint32_t));

		D3D11_BUFFER_DESC vertDesc = {};
		vertDesc.Usage = D3D11_USAGE_DEFAULT;
		vertDesc.ByteWidth = vertexBufferByteWidth;
		vertDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertDesc.CPUAccessFlags = 0;
		vertDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertData = {};
		vertData.pSysMem = m_pMeshData.vertices.data();
		HRESULT hr = m_pMeshDevice->CreateBuffer(&vertDesc, &vertData, m_pMeshVertBuffer.ReleaseAndGetAddressOf());
		
		D3D11_BUFFER_DESC indexDesc = {};
		indexDesc.Usage = D3D11_USAGE_DEFAULT;
		indexDesc.ByteWidth = indexBufferByteWidth;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = m_pMeshData.indices.data();
		hr = m_pMeshDevice->CreateBuffer(&indexDesc, &indexData, m_pMeshIndexBuffer.ReleaseAndGetAddressOf());
		return true;
	}
}