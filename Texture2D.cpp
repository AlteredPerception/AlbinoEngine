#include "Texture2D.h"

namespace AlbinoEngine
{
	Texture2D::Texture2D(ID3D11Device* device)
		: Texture(device)
	{
		this->m_TextureSampler = std::make_unique<TextureSampler>(device);
	}

	Texture2D::~Texture2D()
	{
		if (this->m_texture2D)
		{
			this->m_texture2D.Reset();
		}
	}

	bool Texture2D::loadTexture(std::wstring textureName)
	{
		HRESULT hr;

		Microsoft::WRL::ComPtr<IWICImagingFactory> factory;

		hr = CoCreateInstance(CLSID_WICImagingFactory,
			nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));

		if (FAILED(hr)) return false;

		Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
		hr = factory->CreateDecoderFromFilename(textureName.c_str(),
			nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

		if (FAILED(hr)) return false;

		Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
		hr = decoder->GetFrame(0, &frame);

		if (FAILED(hr)) return false;

		Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
		hr = factory->CreateFormatConverter(&converter);
		if (FAILED(hr)) return false;

		hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPRGBA,
			WICBitmapDitherTypeNone,
			NULL, 0.0,
			WICBitmapPaletteTypeCustom);

		if (FAILED(hr)) return false;

		converter->GetSize(&m_width, &m_height);
		std::vector<uint8_t> pixels(m_width * m_height * 4);
		hr = converter->CopyPixels(nullptr,
			m_width * 4,
			static_cast<UINT>(pixels.size()),
			pixels.data());

		if (FAILED(hr)) return false;

		D3D11_TEXTURE2D_DESC descTexture2D = {};
		descTexture2D.Width = m_width;
		descTexture2D.Height = m_height;
		descTexture2D.MipLevels = m_MipMaps;
		descTexture2D.ArraySize = 1;
		descTexture2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descTexture2D.SampleDesc.Count = 1;
		descTexture2D.SampleDesc.Quality = 0;
		descTexture2D.Usage = D3D11_USAGE_IMMUTABLE;
		descTexture2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = pixels.data();
		initData.SysMemPitch = m_width * 4;

		hr = m_textureDevice->CreateTexture2D(&descTexture2D, &initData, this->m_texture2D.ReleaseAndGetAddressOf());
		if (FAILED(hr)) return false;
		hr = m_textureDevice->CreateShaderResourceView(this->m_texture2D.Get(), nullptr, this->m_ShaderResource.GetAddressOf());
		if (FAILED(hr)) return false;
		this->m_TextureSampler->createSampler();
		return true;
	}
}