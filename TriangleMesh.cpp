#include "TriangleMesh.h"

namespace AlbinoEngine
{
	TriangleMesh::TriangleMesh(ID3D11Device* device)
		: Mesh(device)
	{

	}

	TriangleMesh::~TriangleMesh()
	{

	}

	bool TriangleMesh::initliaze()
	{
		this->m_pMeshData = GeometryGenerator::generateTwoTriangleSubmeshes();
		UINT vertexSize = static_cast<UINT>(m_pMeshData.vertices.size() * sizeof(m_pMeshData.vertices[0]));
		UINT indexSize = static_cast<UINT>(m_pMeshData.indices.size() * sizeof(uint32_t));

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = vertexSize;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = m_pMeshData.vertices.data();

		HRESULT hr = m_pMeshDevice->CreateBuffer(&vertexBufferDesc, &subData, this->m_pMeshVertBuffer.ReleaseAndGetAddressOf());

		if (FAILED(hr)) {
			OutputDebugString(L"Could not create VertexBuffer");
			return false;
		}

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = indexSize;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = m_pMeshData.indices.data();
		hr = m_pMeshDevice->CreateBuffer(&indexBufferDesc, &indexData, m_pMeshIndexBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr)) { OutputDebugString(L"Could not create index buffer."); return false; }

		UINT ByteWidth = sizeof(offsetConstBuffer);

		//if (!m_pOffsetBuffer->createConstantBuffer(ByteWidth, (void**)&offsetConstBuffer))
		//{
		//	OutputDebugString(L"Could not create offset buffer.\n");
		//	return false;
		//}
		
		if (!m_pWorldViewProjection->createConstantBuffer(sizeof(WorldViewProjection), (void**)&this->worldViewProjection))
		{
			OutputDebugString(L"Could not create Constant buffer.");
			return false;
		}
		if (m_pVertShader)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> elements = m_pVertShader->generateLayOut();
			hr = m_pMeshDevice->CreateInputLayout(elements.data(), (UINT)elements.size(),
				m_pVertShader->getVertexShaderBlob()->GetBufferPointer(),
				m_pVertShader->getVertexShaderBlob()->GetBufferSize(),
				m_pMeshInputLayout.ReleaseAndGetAddressOf());

			if (FAILED(hr)) {
				OutputDebugString(L"Could not create input layout");
				return false;
			}
		}
		return true;
	}

}