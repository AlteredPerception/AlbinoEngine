#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <sstream>

namespace AlbinoEngine
{
	using PixelShaderPtr = Microsoft::WRL::ComPtr<ID3D11PixelShader>;

	class PixelShader
	{
	public:
		PixelShader(ID3D11Device* device);

		~PixelShader();

		bool loadPixelShader(
			std::wstring pixelFile, 
			std::string pixelEntryPoint, 
			std::string pixelProfile,
			bool useDebug = true);

		ID3D11PixelShader* getPixelShader() const { return m_pPixelShader.Get(); }

		std::string getPixelShaderInfo() const;
	protected:
		ID3D11Device* m_pPixelDevice;
		PixelShaderPtr m_pPixelShader;
		ID3DBlob* m_pPixelBlob;
		ID3DBlob* m_pErrorBlob;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_pPixelShaderReflection;
	};
}