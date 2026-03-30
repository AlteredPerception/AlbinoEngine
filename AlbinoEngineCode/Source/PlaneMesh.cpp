#include "PlaneMesh.h"

namespace AlbinoEngine
{
	PlaneMesh::PlaneMesh(ID3D11Device* planeDevice)
		: Mesh(planeDevice)
	{

	}

	PlaneMesh::~PlaneMesh()
	{

	}

	void PlaneMesh::setProperties(float width, float depth, float scale, float tileU, float tileV)
	{
		this->m_pMeshData = GeometryGenerator::generatePlane(width, depth, scale, tileU, tileV);
	}

	bool PlaneMesh::initialize()
	{
		UINT vertSize = static_cast<UINT>(m_pMeshData.vertices.size() * sizeof(m_pMeshData.vertices[0]));
		UINT indexSize = static_cast<UINT>(m_pMeshData.indices.size() * sizeof(uint32_t));

		D3D11_BUFFER_DESC vertBufferDesc = {};
		vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertBufferDesc.ByteWidth = vertSize;
		vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertBufferDesc.CPUAccessFlags = 0;
		vertBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertSubData = {};
		vertSubData.pSysMem = m_pMeshData.vertices.data();

		HRESULT hr = m_pMeshDevice->CreateBuffer(&vertBufferDesc, &vertSubData, this->m_pMeshVertBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugStringA("PlaneMesh::initialize(): could not create vertex buffer"); return false; }

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = indexSize;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = m_pMeshData.indices.data();

		hr = m_pMeshDevice->CreateBuffer(&indexBufferDesc, &indexData, this->m_pMeshIndexBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugStringA("PlaneMesh::initialize(): could not create index buffer"); return false; }

		return true;
	}
}