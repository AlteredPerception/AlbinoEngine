#include "PixelShader.h"

namespace AlbinoEngine
{
	PixelShader::PixelShader(ID3D11Device* device)
	{
		this->m_pPixelDevice = device;
		this->m_pPixelBlob = nullptr;
		this->m_pErrorBlob = nullptr;
	}

	PixelShader::~PixelShader()
	{
		//if (m_pPixelShader)
		//{
		//	m_pPixelShader.Reset();
		//}
	}

	bool PixelShader::loadPixelShader(std::wstring pixelFile, std::string pixelEntryPoint, std::string pixelProfile, bool useDebug)
	{
		HRESULT hr = D3DCompileFromFile(
			pixelFile.c_str(), 
			0, 
			0, 
			pixelEntryPoint.c_str(), 
			pixelProfile.c_str(), 
			D3DCOMPILE_DEBUG, 
			0, 
			&m_pPixelBlob, 
			&m_pErrorBlob);

		if (FAILED(hr))
		{
			OutputDebugString(L"Could not load Pixel Shader.\n");
			if (useDebug && pixelFile.size() > 0 && m_pErrorBlob)
			{
				std::string message;
				message += "Pixel Shader errors: ";
				message += (char*)m_pErrorBlob->GetBufferPointer();
				OutputDebugStringA(message.c_str());
				m_pErrorBlob->Release();
				m_pErrorBlob = 0;
			}
			return false;
		}
		
		hr = m_pPixelDevice->CreatePixelShader(m_pPixelBlob->GetBufferPointer(), m_pPixelBlob->GetBufferSize(), NULL, m_pPixelShader.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			OutputDebugString(L"Could not create Pixel Shader.");
				
			m_pPixelBlob->Release();
			m_pPixelBlob = 0;
			return false;
		}
		
		hr = D3DReflect(
			this->m_pPixelBlob->GetBufferPointer(), 
			this->m_pPixelBlob->GetBufferSize(), 
			IID_ID3D11ShaderReflection, 
			(void**)this->m_pPixelShaderReflection.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			OutputDebugStringA("Could not get pixel shader reflection.");
			return false;
		}
		else {
			OutputDebugStringA("Pixel Shader reflection acquired.");
		}
		
		return true;
	}

	std::string PixelShader::getPixelShaderInfo() const
	{
		std::ostringstream output;
		D3D11_SHADER_DESC pixelShaderDesc = {};
		m_pPixelShaderReflection->GetDesc(&pixelShaderDesc);
		output << "Creator: " << pixelShaderDesc.Creator << std::endl;
		output << "Version: " << pixelShaderDesc.Version << std::endl;
		return output.str();
	}
}