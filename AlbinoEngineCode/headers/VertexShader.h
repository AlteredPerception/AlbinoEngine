#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <sstream>
#include <vector>
#include "ConstantBuffer.h"
#include "Vertex.h"

namespace AlbinoEngine
{
	using VertexShaderPtr = Microsoft::WRL::ComPtr<ID3D11VertexShader>;
	using VertexShaderReflectionPtr = Microsoft::WRL::ComPtr<ID3D11ShaderReflection>;
	using VertexShaderConstantBuffer = std::shared_ptr<ConstantBuffer>;
	using VertexShaderConstants = std::vector<VertexShaderConstantBuffer>;
	class VertexShader
	{
	public:
		VertexShader(ID3D11Device* device);

		~VertexShader();

		bool loadVertexShader(
			std::wstring vertFile,
			std::string vertEntryPoint,
			std::string vertProfile,
			bool useDebug = true);

		ID3D11VertexShader* getVertexShader() const { return m_pVertexShader.Get(); }

		ID3DBlob* getVertexShaderBlob() const { return m_pVertexBlob; }

		bool createInputLayout();

		std::vector<D3D11_INPUT_ELEMENT_DESC> generateLayOut();

		std::string getShaderInfo() const;

		std::string getConstantBufferData() const;

		VertexShaderConstants createConstantBuffers();

		ConstantBuffer* getBuffer(int index) { return m_pVertexShaderConstants[index].get(); }
		int getCBufferBindPoint(const std::string& name) const;

		void dumpBindResources() const;

		ID3D11InputLayout* getInputLayout() const { return m_pVertexShaderLayout.Get(); }
	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pVertexShaderLayout;
		VertexShaderConstants m_pVertexShaderConstants;
		DXGI_FORMAT GetFormat(const D3D11_SIGNATURE_PARAMETER_DESC& param);
		D3D11_SHADER_DESC m_pShaderDesc = {};
		ID3D11Device* m_pVertexShaderDevice;
		VertexShaderPtr m_pVertexShader;
		VertexShaderReflectionPtr m_pVertexShaderReflection;
		ID3D11ShaderReflectionConstantBuffer* m_pConstantBuffers;
		ID3DBlob* m_pVertexBlob;
		ID3DBlob* m_pErrorBlob;
	
	};
}