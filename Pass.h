#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "EffectContext.h"
#include "VertexShader.h"
#include "PixelShader.h"
//#include "Effect.h"

//#include "ConstantBuffer.h"

namespace AlbinoEngine
{
	class Mesh;
	//class Camera;


	struct alignas(16) CB_WorldViewProj
	{
		DirectX::XMMATRIX worldViewProjction;
	};
	static_assert(sizeof(CB_WorldViewProj) == 64);

	class Pass
	{
	public:

		Pass(
			std::shared_ptr<VertexShader> vs,
			std::shared_ptr<PixelShader> ps);


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
	protected:
		std::shared_ptr<VertexShader> m_vs;
		std::shared_ptr<PixelShader> m_ps;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		
		// States (raw pointers ok if owned elsewhere; use ComPtr if you want ownership)
		ID3D11RasterizerState* m_rs = nullptr;
		ID3D11DepthStencilState* m_ds = nullptr;
		ID3D11BlendState* m_bs = nullptr;

		float m_blendFactor[4] = { 0,0,0,0 };
		UINT m_sampleMask = 0xFFFFFFFF;

		bool m_usePerObjectCB = true;
		// Per-object constant buffer (WVP)
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPerObject;

		// Textures/samplers
		std::unordered_map<UINT, ID3D11ShaderResourceView*> m_psSrvs;
		std::unordered_map<UINT, ID3D11SamplerState*> m_psSamplers;

	};
}