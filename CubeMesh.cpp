#include "CubeMesh.h"

namespace AlbinoEngine
{
	CubeMesh::CubeMesh(ID3D11Device* device)
		: Mesh(device)
	{

	}

	CubeMesh::~CubeMesh()
	{

	}

	bool CubeMesh::initliaze()
	{
		this->m_pMeshData = GeometryGenerator::generateUnitCubeTexture();
		UINT vertexSize = static_cast<UINT>(m_pMeshData.vertices.size() * sizeof(m_pMeshData.vertices[0]));
		UINT indexSize = static_cast<UINT>(m_pMeshData.indices.size() * sizeof(uint32_t));

		// Create vertex buffer;
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = vertexSize;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		// Initial data.
		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = m_pMeshData.vertices.data();
		HRESULT hr = m_pMeshDevice->CreateBuffer(&vertexBufferDesc, &vertexData, this->m_pMeshVertBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugString(L"Could not create vertex buffer"); return false; }

		// Create index buffer;
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = indexSize;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = m_pMeshData.indices.data();
		hr = m_pMeshDevice->CreateBuffer(&indexBufferDesc, &indexData, m_pMeshIndexBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugString(L"Could not create index buffer."); return false; }

		if (!m_pWorldViewProjection->createConstantBuffer(sizeof(WorldViewProjection), &this->worldViewProjection))
		{
			OutputDebugString(L"Could not create Constant Buffer");
			return false;
		}

		if (m_pVertShader)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> elements = m_pVertShader->generateLayOut();
			m_pMeshDevice->CreateInputLayout(
				elements.data(), 
				(UINT)elements.size(), 
				m_pVertShader->getVertexShaderBlob()->GetBufferPointer(), 
				m_pVertShader->getVertexShaderBlob()->GetBufferSize(), 
				this->m_pMeshInputLayout.ReleaseAndGetAddressOf());
		}
		return true;
	}
}