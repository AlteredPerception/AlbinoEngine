#pragma once
#include "VertexShader.h"
#include "PixelShader.h"
#include <string>

namespace AlbinoEngine
{
	using VertexShaderSharedPtr = std::shared_ptr<VertexShader>;
	using PixelShaderSharedPtr = std::shared_ptr<PixelShader>;

	class Shader
	{
	public:
		Shader(ID3D11Device* shaderDevice);

		~Shader();

		VertexShader* loadVertexShader(
			std::wstring vertFileName, 
			std::string vertEntryPoint, 
			std::string vertProfile,
			bool useDebug = true);

		PixelShader* loadPixelShader(
			std::wstring pixelFileName, 
			std::string pixelEntryPoint, 
			std::string pixelProfile, 
			bool useDebug = true);

	protected:
		ID3D11Device* m_pShaderDevice;
		VertexShaderSharedPtr m_pVertexShader;
		PixelShaderSharedPtr m_pPixelShader;
	};
}