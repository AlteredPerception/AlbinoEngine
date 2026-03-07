#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <memory>
#include "CBufferVariable.h"

namespace AlbinoEngine
{
	using ConstantBufferPtr = Microsoft::WRL::ComPtr<ID3D11Buffer>;
	
	using ConstantBufferVariableList = std::vector<CBufferVariable>;

	class ConstantBuffer
	{
	public:
		ConstantBuffer(std::string conBufferName, ID3D11Device* device);

		~ConstantBuffer();

		bool createConstantBuffer(UINT ByteWidth, const void* initialData = nullptr);

		void addVariable(CBufferVariable variable);

		ConstantBufferVariableList getConstantBufferVariableList();

		void setBindPoint(uint32_t bindPoint);

		void setBindCount(uint32_t bindCount);

		std::string getConstantBufferName() const { return m_pConstBufferName; }

		uint32_t getBindPoint() const { return m_pBindPoint; }

		uint32_t getBindCount() const { return m_pBindCount; }

		ID3D11Buffer* getConstantBuffer() const { return m_pConstantBuffer.Get(); }

	protected:
		uint32_t m_pBindPoint;
		uint32_t m_pBindCount;
		ConstantBufferVariableList m_pConBufferVariableList;
		std::string m_pConstBufferName;
		ID3D11Device* m_pConstantBufferDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
		ConstantBufferPtr m_pConstantBuffer;
	};
}