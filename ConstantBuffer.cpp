#include "ConstantBuffer.h"

namespace AlbinoEngine
{
	ConstantBuffer::ConstantBuffer(std::string conBufferName, ID3D11Device* device)
	{
		this->m_pConstBufferName = conBufferName;
		this->m_pConstantBufferDevice = device;
		this->m_pConstantBufferDevice->GetImmediateContext(this->m_pImmediateContext.ReleaseAndGetAddressOf());
	}

	ConstantBuffer::~ConstantBuffer()
	{
		m_pConstantBuffer.Reset();
		m_pImmediateContext.Reset();
	}

	bool ConstantBuffer::createConstantBuffer(UINT ByteWidth, const void* initialData)
	{
		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constBufferDesc.ByteWidth = (ByteWidth + 15) & ~15;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = 0;
		constBufferDesc.MiscFlags = 0;

		const D3D11_SUBRESOURCE_DATA* initPtr = nullptr;
		D3D11_SUBRESOURCE_DATA subData = {};

		if (initialData) 
		{ 
			subData.pSysMem = initialData;
			initPtr = &subData; 
		}
		HRESULT hr = m_pConstantBufferDevice->CreateBuffer(&constBufferDesc, initPtr, this->m_pConstantBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			OutputDebugString(L"Could not create constant buffer.\n");
			return false;
		}

		return true;
	}

	void ConstantBuffer::setBindPoint(uint32_t bindPoint)
	{
		this->m_pBindPoint = bindPoint;
	}

	void ConstantBuffer::setBindCount(uint32_t bindCount)
	{
		this->m_pBindCount = bindCount;
	}
	void ConstantBuffer::addVariable(CBufferVariable variable)
	{
		m_pConBufferVariableList.push_back(variable);
	}

	ConstantBufferVariableList ConstantBuffer::getConstantBufferVariableList()
	{
		return m_pConBufferVariableList;
	}
}
