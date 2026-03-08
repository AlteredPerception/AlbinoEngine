#include "Pass.h"
#include "Mesh.h"
#include "Camera.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace AlbinoEngine
{
	Pass::Pass(std::shared_ptr<VertexShader> vs,
		std::shared_ptr<PixelShader> ps)
		: m_vs(std::move(vs)), m_ps(std::move(ps))
	{}


	bool Pass::buildInputLayout(ID3D11Device* device)
	{
		if (!device || !m_vs) return false;

		auto elements = m_vs->generateLayOut();

		if (elements.empty()) return false;

		auto* blob = m_vs->getVertexShaderBlob();
		if (!blob) return false;

		HRESULT hr = device->CreateInputLayout(
			elements.data(),
			(UINT)elements.size(),
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			m_inputLayout.ReleaseAndGetAddressOf()
		);

		return SUCCEEDED(hr);
	}

	void Pass::setBlendState(ID3D11BlendState* bs, const float blendFactor[4], UINT sampleMask)
	{
		m_bs = bs;
		m_sampleMask = sampleMask;

		if (blendFactor)
		{
			this->m_blendFactor[0] = blendFactor[0];
			this->m_blendFactor[1] = blendFactor[1];
			this->m_blendFactor[2] = blendFactor[2];
			this->m_blendFactor[3] = blendFactor[3];
		}
		else
		{
			m_blendFactor[0] = m_blendFactor[1] = m_blendFactor[2] = m_blendFactor[3] = 0.0f;
		}
	}
	
	void Pass::setTexture(UINT slot, ID3D11ShaderResourceView* srv)
	{
		this->m_psSrvs[slot] = srv;
	}

	void Pass::setSampler(UINT slot, ID3D11SamplerState* samp)
	{
		m_psSamplers[slot] = samp;
	}

	bool Pass::ensurePerObjectCB(ID3D11Device* device)
	{
		if (m_cbPerObject) return true;
		if (!device) return false;

		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = sizeof(CB_WorldViewProj); // 64 (already multiple of 16)

		HRESULT hr = device->CreateBuffer(&bd, nullptr, m_cbPerObject.ReleaseAndGetAddressOf());
		return SUCCEEDED(hr);
	}

	void Pass::apply(ID3D11DeviceContext* ctx)
	{
		if (!ctx) return;

		if (!m_rs) OutputDebugStringA("Pass::apply(): m_rs is null\n");
		if (!m_inputLayout) OutputDebugStringA("Pass:apply(): m_inputLayout is null\n");
		if (m_ds == nullptr)
			OutputDebugStringA("WARNING: Pass has no depth state; inheriting previous OM depth state.\n");
		// States
		ctx->RSSetState(m_rs);
		ctx->OMSetDepthStencilState(m_ds, 0);
		ctx->OMSetBlendState(m_bs, m_blendFactor, m_sampleMask);

		// IA + shaders
		ctx->IASetInputLayout(m_inputLayout.Get());
		ctx->VSSetShader(m_vs ? m_vs->getVertexShader() : nullptr, nullptr, 0);
		ctx->PSSetShader(m_ps ? m_ps->getPixelShader() : nullptr, nullptr, 0);

		//char buf[128];
		//sprintf_s(buf, "[%s] slot0 SRV=%p\n", name ? name : "Pass", this->m_psSrvs.count(0) ? this->m_psSrvs.at(0) : nullptr);
		//OutputDebugStringA(buf);

		// PS textures/samplers
		for (auto& [slot, srv] : m_psSrvs)
			ctx->PSSetShaderResources(slot, 1, &srv);

		for (auto& [slot, samp] : m_psSamplers)
			ctx->PSSetSamplers(slot, 1, &samp);
	}

	void Pass::updateAndBindPerObjectCB(EffectContext& fx, Mesh& mesh)
	{
		if (!fx.context || !fx.camera) return;
		if (!ensurePerObjectCB(fx.device)) return;

		// Build world matrix from mesh TRS 
		XMFLOAT3 pos = mesh.getMeshPosition();
		XMFLOAT3 sca = mesh.getMeshScale();
		XMFLOAT3 rot = mesh.getMeshRotation();

		XMMATRIX Translate = XMMatrixTranslation(pos.x, pos.y, pos.z);
		XMMATRIX Scale = XMMatrixScaling(sca.x, sca.y, sca.z);
		XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(rot.x),
			XMConvertToRadians(rot.y),
			XMConvertToRadians(rot.z)
		);

		XMMATRIX world = Scale * Rotation * Translate;

		CB_WorldViewProj cb{};
		cb.worldViewProjction = XMMatrixTranspose(world * fx.camera->getViewProjMatrix());
		fx.context->UpdateSubresource(m_cbPerObject.Get(), 0, nullptr, &cb, 0, 0);

		// Bind to the slot the VS expects.
		// Your reflection dump indicates cbuffer name = "WorldViewProjection" at b0.
		UINT slot = 0;
		if (m_vs)
		{
			UINT s = m_vs->getCBufferBindPoint("WorldViewProjection");
			if (s != UINT(-1)) slot = s;
		}
		ID3D11Buffer* buffer = m_cbPerObject.Get();
		fx.context->VSSetConstantBuffers(slot, 1, &buffer);
	}

	void Pass::render(EffectContext& fx, Mesh& mesh)
	{
		if (!fx.device || !fx.context) return;

		// Apply pass here.
		apply(fx.context);

		if (m_usePerObjectCB)
		{
			if (!fx.camera) return;
			updateAndBindPerObjectCB(fx, mesh);
		}
		// Per-object data
		//updateAndBindPerObjectCB(fx, mesh);
		
		// Draw geometry (IMPORTANT: Mesh::drawGeometryOnly() must NOT override IA input layout)
		mesh.drawGeometryOnly();
	}
}