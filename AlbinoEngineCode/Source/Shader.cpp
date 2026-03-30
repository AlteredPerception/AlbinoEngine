#include "Shader.h"

namespace AlbinoEngine
{
	Shader::Shader(ID3D11Device* shaderDevice)
	{
		this->m_pShaderDevice = shaderDevice;
	}

	Shader::~Shader()
	{
		m_pPixelShader.reset();
		m_pVertexShader.reset();
	}

	VertexShader* Shader::loadVertexShader(
		std::wstring vertFileName,
		std::string vertEntryPoint,
		std::string vertProfile,
		bool useDebug)
	{
		this->m_pVertexShader = std::make_shared<VertexShader>(m_pShaderDevice);
		this->m_pVertexShader->loadVertexShader(vertFileName, vertEntryPoint, vertProfile, useDebug);
		return this->m_pVertexShader.get();
	}

	PixelShader* Shader::loadPixelShader(
		std::wstring pixelFileName,
		std::string pixelEntryPoint,
		std::string pixelProfile,
		bool useDebug)
	{
		this->m_pPixelShader = std::make_shared<PixelShader>(m_pShaderDevice);
		this->m_pPixelShader->loadPixelShader(pixelFileName, pixelEntryPoint, pixelProfile, useDebug);
		return m_pPixelShader.get();
	}


}