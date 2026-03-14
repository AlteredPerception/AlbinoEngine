#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl/client.h>

#include "..//AlbinoEngine/Scene.h"
#include "..//AlbinoEngine/Effect.h"
#include "..//AlbinoEngine/EffectContext.h"

namespace AlbinoEngine
{
    class Camera;
    class CubeMesh;
    class VertexShader;
    class PixelShader;
    class Engine;
}

class DemoScene final : public AlbinoEngine::IScene
{
public:
    bool initialize(AlbinoEngine::Engine& engine) override;
    void update(AlbinoEngine::Engine& engine, float dt) override;
    void render(AlbinoEngine::Engine& engine) override;
    void destroy() override;
    AlbinoEngine::EffectContext buildEffectContext(AlbinoEngine::Engine& engine) override;

private:
    void createStates(AlbinoEngine::Engine& engine);
    void createEffect(AlbinoEngine::Engine& engine);
    void setBlendFactor(float r, float g, float b, float a);
private:
    std::unique_ptr<AlbinoEngine::Camera> m_camera;

    std::shared_ptr<AlbinoEngine::VertexShader> m_vs;
    std::shared_ptr<AlbinoEngine::PixelShader>  m_ps;

    std::shared_ptr<AlbinoEngine::CubeMesh> m_cube;

    AlbinoEngine::Effect m_effect{ "BasicEffect" };

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsSolid;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsOverlay;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dsSolid;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dsOverlay;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_bsOverlay;

    float m_overlayFactor[4] = { 0.35f, 0.35f, 0.35f, 0.35f };

    bool  m_wasDown = false;
    POINT m_lastPt{ 0,0 };
};