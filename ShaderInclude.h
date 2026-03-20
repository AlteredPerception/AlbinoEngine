#pragma once

#include <d3dcompiler.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

namespace AlbinoEngine
{
	class ShaderInclude final : public ID3DInclude
	{
	public:
		explicit ShaderInclude(const std::wstring& sourceFile)
		{
			std::filesystem::path p(sourceFile);
			m_baseDirectory = p.parent_path();
		}

		HRESULT __stdcall Open(
			D3D_INCLUDE_TYPE includeType,
			LPCSTR pFileName,
			LPCVOID pParentData,
			LPCVOID* ppData,
			UINT* pBytes) override
		{
			if (!pFileName || !ppData || !pBytes)
				return E_INVALIDARG;

			std::filesystem::path includePath;

			if (includeType == D3D_INCLUDE_LOCAL)
			{
				includePath = m_baseDirectory / pFileName;
			}
			else
			{
				includePath = pFileName;
			}

			std::ifstream file(includePath, std::ios::binary | std::ios::ate);
			if (!file.is_open())
				return E_FAIL;

			std::streamsize size = file.tellg();
			if (size <= 0)
				return E_FAIL;

			file.seekg(0, std::ios::beg);

			char* data = new char[static_cast<size_t>(size)];
			if (!file.read(data, size))
			{
				delete[] data;
				return E_FAIL;
			}

			*ppData = data;
			*pBytes = static_cast<UINT>(size);
			return S_OK;
		}

		HRESULT __stdcall Close(LPCVOID pData) override
		{
			delete[] reinterpret_cast<const char*>(pData);
			return S_OK;
		}
	protected:
		std::filesystem::path m_baseDirectory;
	};
}