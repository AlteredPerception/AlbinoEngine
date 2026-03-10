#include "VertexShader.h"
#include "Vertex.h"

#include <sstream>

namespace AlbinoEngine
{
	VertexShader::VertexShader(ID3D11Device* device)
	{
		this->m_pVertexShaderDevice = device;
		this->m_pVertexBlob = nullptr;
		this->m_pErrorBlob = nullptr;
		this->m_pConstantBuffers = nullptr;
	}

	VertexShader::~VertexShader()
	{
		if (m_pVertexShader)
		{
			m_pVertexShader.Reset();
		}
	}

	bool VertexShader::loadVertexShader(std::wstring vertFile, std::string vertEntryPoint, std::string vertProfile, bool useDebug)
	{
		UINT flags = 0;

#if defined(_DEBUG)
		if (useDebug)
		{
			flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		}
#endif
		HRESULT hr = D3DCompileFromFile(
			vertFile.c_str(),
			nullptr,
			nullptr,
			vertEntryPoint.c_str(),
			vertProfile.c_str(),
			flags,
			0,
			&m_pVertexBlob,
			&m_pErrorBlob);

		if (FAILED(hr)) {
			OutputDebugString(L"Something went wrong loading vertex shader.");
			if (m_pErrorBlob)
			{
				std::string message;
				message += (char*)m_pErrorBlob->GetBufferPointer();
				OutputDebugStringA(message.c_str());
				m_pErrorBlob->Release();
				m_pErrorBlob = 0;
			}
			//m_pVertexBlob->Release();
			//m_pVertexBlob = 0;
			return false;
		}

		hr = m_pVertexShaderDevice->CreateVertexShader(m_pVertexBlob->GetBufferPointer(), m_pVertexBlob->GetBufferSize(), nullptr, this->m_pVertexShader.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			OutputDebugStringA("Could not create vertexShader.\n");
			if (m_pVertexBlob) 
			{
				m_pVertexBlob->Release();
				m_pVertexBlob = 0;
			}
			m_pVertexShader.Reset();
			return false;
		}


		hr = D3DReflect(
			m_pVertexBlob->GetBufferPointer(),
			m_pVertexBlob->GetBufferSize(), 
			IID_ID3D11ShaderReflection, 
			(void**)m_pVertexShaderReflection.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			OutputDebugString(L"Could not get reflection.\n");
		}

		m_pVertexShaderReflection->GetDesc(&m_pShaderDesc);
		
		return true;
	}

	bool VertexShader::createInputLayout()
	{
		if (!m_pVertexShader)
			return false;

		std::vector<D3D11_INPUT_ELEMENT_DESC> elem = this->generateLayOut();
		HRESULT hr = m_pVertexShaderDevice->CreateInputLayout(
			elem.data(),
			static_cast<UINT>(elem.size()),
			this->m_pVertexBlob->GetBufferPointer(),
			this->m_pVertexBlob->GetBufferSize(),
			this->m_pVertexShaderLayout.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			OutputDebugString(L"Could not create input layout.\n");
			return false;
		}
		return true;
	}

	DXGI_FORMAT VertexShader::GetFormat(const D3D11_SIGNATURE_PARAMETER_DESC& param)
	{
		if (param.Mask == 1)
		{
			if (param.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32_UINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32_SINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32_FLOAT;
		}
		else if (param.Mask <= 3)
		{
			if (param.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32_UINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32_SINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (param.Mask <= 7)
		{
			if (param.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32_UINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32_SINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (param.Mask <= 15)
		{
			if (param.ComponentType == D3D_REGISTER_COMPONENT_UINT32) return DXGI_FORMAT_R32G32B32A32_UINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_SINT32) return DXGI_FORMAT_R32G32B32A32_SINT;
			if (param.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexShader::generateLayOut()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
		if (!m_pVertexShaderReflection) 
			return layout;

		for (UINT i = 0; i < m_pShaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc = {};
			m_pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

			VertexElementInfo info{};
			if (!GetVertexElementInfo(paramDesc.SemanticName, paramDesc.SemanticIndex, info))
			{
				// The shader wants a semantic your Vertex struct doesn't provide.
				// This is a HARD error for input layout creation.
				std::string msg = "VertexShader::generateLayOut(): missing vertex element for semantic ";
				msg += paramDesc.SemanticName;
				msg += std::to_string(paramDesc.SemanticIndex);
				msg += "\n";
				OutputDebugStringA(msg.c_str());
				continue;
			}

			D3D11_INPUT_ELEMENT_DESC elem = {};
			elem.SemanticName = paramDesc.SemanticName;
			elem.SemanticIndex = paramDesc.SemanticIndex;
			elem.Format = info.format;
			elem.InputSlot = 0;
			elem.AlignedByteOffset = info.offset;
			elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elem.InstanceDataStepRate = 0;

			layout.push_back(elem);
		}
		return layout;
	}
	std::string VertexShader::getShaderInfo() const
	{
		std::ostringstream shaderInfo;

		if (m_pVertexShaderReflection)
		{
			shaderInfo << "Vertex Shader version: " << std::to_string(this->m_pShaderDesc.Version) << std::endl;
			shaderInfo << "Creator: " << m_pShaderDesc.Creator << std::endl;
			shaderInfo << "Num Constant Buffers: " << m_pShaderDesc.ConstantBuffers << std::endl;
			shaderInfo << "Input parameters: " << m_pShaderDesc.InputParameters << std::endl;
		}
		else {
			shaderInfo << "Could not get shader description." << std::endl;
		}
		return shaderInfo.str();
	}

	std::string VertexShader::getConstantBufferData() const
	{
		std::ostringstream stringStream;

		if (this->m_pVertexShaderReflection)
		{
			for (UINT i = 0; i < m_pShaderDesc.ConstantBuffers; ++i)
			{
				ID3D11ShaderReflectionConstantBuffer* buffer = this->m_pVertexShaderReflection->GetConstantBufferByIndex(i);
				D3D11_SHADER_BUFFER_DESC shaderBuffer = {};
				buffer->GetDesc(&shaderBuffer);

				stringStream << "Constat Buffer name: " << shaderBuffer.Name << std::endl;
				stringStream << "Size: " << shaderBuffer.Size << std::endl;
				stringStream << "Type: " << shaderBuffer.Type << std::endl;
				stringStream << "Variables: " << std::endl;
				for (UINT j = 0; j < shaderBuffer.Variables; ++j)
				{
					ID3D11ShaderReflectionVariable* variables = buffer->GetVariableByIndex(j);
					D3D11_SHADER_VARIABLE_DESC variablesDesc = {};
					variables->GetDesc(&variablesDesc);
					stringStream << "  Variable name: " << variablesDesc.Name << std::endl;
					stringStream << "  Variable size: " << variablesDesc.Size << std::endl;
				}
			}
			for (UINT i = 0; i < this->m_pVertexShaderConstants.size(); ++i)
			{
				ConstantBuffer* buffers = m_pVertexShaderConstants[i].get();
				stringStream << "Num bind points: " << buffers->getBindPoint() << std::endl;
				stringStream << "Num bind count: " << buffers->getBindCount() << std::endl;
			}
		}
		return stringStream.str();
	}

	int VertexShader::getCBufferBindPoint(const std::string& name) const
	{
		if (!m_pVertexShaderReflection) 
			return -1;

		D3D11_SHADER_INPUT_BIND_DESC bind = {};
		HRESULT hr = m_pVertexShaderReflection->GetResourceBindingDescByName(name.c_str(),&bind);
		if (FAILED(hr) || bind.Type != D3D_SIT_CBUFFER)
			return -1;
		
		return static_cast<int>(bind.BindPoint);
	}

	void VertexShader::dumpBindResources() const
	{
		if (!m_pVertexShaderReflection) 
			return;

		D3D11_SHADER_DESC desc = {};
		m_pVertexShaderReflection->GetDesc(&desc);

		std::ostringstream oss;
		oss << "=== VS BoundResources: " << desc.BoundResources << " ===\n" << std::endl;
		for (UINT i = 0; i < desc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC b = {};
			if (SUCCEEDED(m_pVertexShaderReflection->GetResourceBindingDesc(i, &b)))
			{
				oss << i << ": Binded resource name=" << (b.Name ? b.Name : "(null)")
					<< " Type=" << b.Type
					<< " BindPoint=" << b.BindPoint
					<< " BindCount=" << b.BindCount << "\n";
			}
		}
		OutputDebugStringA(oss.str().c_str());
	}

	VertexShaderConstants VertexShader::createConstantBuffers()
	{
		VertexShaderConstantBuffer conBuffer;
		if (m_pVertexShaderReflection)
		{
			for (UINT i = 0; i < this->m_pShaderDesc.ConstantBuffers; ++i) {
				ID3D11ShaderReflectionConstantBuffer* con = this->m_pVertexShaderReflection->GetConstantBufferByIndex(i);
				D3D11_SHADER_BUFFER_DESC bufferDesc = {};
				con->GetDesc(&bufferDesc);
				conBuffer = std::make_shared<ConstantBuffer>(bufferDesc.Name, this->m_pVertexShaderDevice);
				D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
				HRESULT hr = m_pVertexShaderReflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);
				

				if (SUCCEEDED(hr) && bindDesc.Type == D3D_SIT_CBUFFER)
				{
					conBuffer->setBindPoint(bindDesc.BindPoint);
					conBuffer->setBindCount(bindDesc.BindCount);
					std::string message = "";
					message += "Num bind points: ";
					message += std::to_string(conBuffer->getBindPoint());
					message += "\nNum bind counts: ";
					message += std::to_string(conBuffer->getBindCount());
					message += "\n";
					OutputDebugStringA(message.c_str());
				}
				else
				{
					OutputDebugStringA("Could not get Resource Binding.\n");
				}
				conBuffer->createConstantBuffer(bufferDesc.Size);
				for (UINT j = 0; j < bufferDesc.Variables; ++j)
				{
					ID3D11ShaderReflectionVariable* variable = con->GetVariableByIndex(j);
					D3D11_SHADER_VARIABLE_DESC shaderVariable = {};
					variable->GetDesc(&shaderVariable);
					conBuffer->addVariable(CBufferVariable(
						shaderVariable.Name, shaderVariable.StartOffset, shaderVariable.Size, shaderVariable.uFlags)
					);
				}
				this->m_pVertexShaderConstants.push_back(conBuffer);
				conBuffer.reset();
			}
		}
		return m_pVertexShaderConstants;
	}
}