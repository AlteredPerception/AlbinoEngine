#include "ShaderInclude.h"
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
		m_pPixelShader.Reset();
		m_pPixelShaderReflection.Reset();
		m_pPixelDevice = nullptr;

		if (m_pPixelBlob)
		{
			m_pPixelBlob->Release();
			m_pPixelBlob = 0;
		}

		if (m_pErrorBlob)
		{
			m_pErrorBlob->Release();
			m_pErrorBlob = 0;
		}
	}

	bool PixelShader::loadPixelShader(std::wstring pixelFile, std::string pixelEntryPoint, std::string pixelProfile, bool useDebug)
	{
		if (m_pPixelBlob)
		{
			m_pPixelBlob->Release();
			m_pPixelBlob = nullptr;
		}

		if (m_pErrorBlob)
		{
			m_pErrorBlob->Release();
			m_pErrorBlob = nullptr;
		}

		m_pPixelShader.Reset();
		m_pPixelShaderReflection.Reset();
		UINT flags = 0;
#if defined(_DEBUG)
		if (useDebug)
		{
			flags |= D3DCOMPILE_DEBUG;
		}
#endif
		ShaderInclude includeHandler(pixelFile);
		HRESULT hr = D3DCompileFromFile(
			pixelFile.c_str(), 
			0, 
			&includeHandler, 
			pixelEntryPoint.c_str(), 
			pixelProfile.c_str(), 
			flags, 
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
			OutputDebugStringA("Pixel Shader reflection acquired.\n");
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