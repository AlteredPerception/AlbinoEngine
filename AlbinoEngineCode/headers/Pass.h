#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "EffectContext.h"
#include "FrameConstants.h"
#include "VertexShader.h"
#include "PixelShader.h"

namespace AlbinoEngine
{
	class Mesh;

	struct alignas(16) CB_PointShadowData
	{
		DirectX::XMFLOAT3 lightPosition = { 0.0f, 0.0f,0.0f };
		float lightRange = 8.0f;
	};

	struct alignas(16) CB_WorldViewProj
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
		DirectX::XMMATRIX worldViewProjction;

		//float receiveShadows;
		float padding[3];
	};
	//static_assert(sizeof(CB_WorldViewProj) == 64);

	struct alignas(16) CB_DebugColor
	{
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
	};


	class Pass
	{
	public:
		Pass(std::shared_ptr<VertexShader> vs, std::shared_ptr<PixelShader> ps);

		~Pass();

		bool buildInputLayout(ID3D11Device* device); // uses VS reflection

		
		// Optional per-pass states
		void setRasterizerState(ID3D11RasterizerState* s) { m_rs = s; }
		void setDepthStencialState(ID3D11DepthStencilState* ds) { m_ds = ds; }
		void setBlendState(ID3D11BlendState* bs, const float blendFactor[4] = nullptr, UINT sampleMask = 0xFFFFFFFF);

		// Bind an SRV + sampler to PS slot N (basic texture support)
		void setTexture(UINT slot, ID3D11ShaderResourceView* srv);
		void setSampler(UINT slot, ID3D11SamplerState* samp);
		
		// New: for screen quad / postprocess passes.
		void setUsePerObjectCB(bool use) { m_usePerObjectCB = use; }

		// Apply pipeline state (shaders, IA layout, RS/DS/Blend, textures/samplers.
		void apply(ID3D11DeviceContext* ctx);

		// Render mesh for this pass (updates WVP CB and draws)
		void render(EffectContext& fx, Mesh& mesh); 

		// For debug
		const char* name = "Pass";
	private:
		bool ensurePerObjectCB(ID3D11Device* device);
		void updateAndBindPerObjectCB(EffectContext& fx, Mesh& mes);
		
		bool ensureFrameLightingCB(ID3D11Device* device);
		void updateAndBindFrameLightingCB(EffectContext& fx);

		bool ensurePointShadowCB(ID3D11Device* device);
		void updateAndBindPointShadowCB(EffectContext& fx);
	protected:
		std::shared_ptr<VertexShader> m_vs;
		std::shared_ptr<PixelShader> m_ps;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbFrameLighting;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPointShadowData;

		// States (raw pointers ok if owned elsewhere; use ComPtr if you want ownership)
		ID3D11RasterizerState* m_rs = nullptr;
		ID3D11DepthStencilState* m_ds = nullptr;
		ID3D11BlendState* m_bs = nullptr;

		float m_blendFactor[4] = { 0,0,0,0 };
		UINT m_sampleMask = 0xFFFFFFFF;

		bool m_usePerObjectCB = true;

		// Textures/samplers
		std::unordered_map<UINT, ID3D11ShaderResourceView*> m_psSrvs;
		std::unordered_map<UINT, ID3D11SamplerState*> m_psSamplers;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbDebugColor;
		bool ensureDebugColorCB(ID3D11Device* device);
		void updateAndBindDebugColorCB(EffectContext& fx, const DirectX::XMFLOAT4& color);

	};
}