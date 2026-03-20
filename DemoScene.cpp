#include "DemoScene.h"

#include <Windows.h>

#include "..//AlbinoEngine/Engine.h"
#include "..//AlbinoEngine/Camera.h"
#include "..//AlbinoEngine/CubeMesh.h"
#include "..//AlbinoEngine/PlaneMesh.h"
#include "..//AlbinoEngine/VertexShader.h"
#include "..//AlbinoEngine/PixelShader.h"
#include "..//AlbinoEngine/Pass.h"
#include "..//AlbinoEngine/Technique.h"
#include "..//AlbinoEngine/TextureManager.h"

using Microsoft::WRL::ComPtr;
using namespace AlbinoEngine;

static bool keyDown(int vk)
{
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

void DemoScene::createDebugLightEffect(AlbinoEngine::Engine& engine)
{
    m_debugVS = std::make_shared<AlbinoEngine::VertexShader>(engine.device());
    m_debugPS = std::make_shared<AlbinoEngine::PixelShader>(engine.device());

    m_debugVS->loadVertexShader(L"media//shaders//UnlitColor3D.shader", "vs_main", "vs_5_0");
    m_debugPS->loadPixelShader(L"media//shaders//UnlitColor3D.shader", "ps_main", "ps_5_0");

    auto eff = engine.getEffectsManager().createEffect("DebugLight").get();
    auto& tech = eff->createTechnique("default");
    eff->setActiveTechnique("default");

    auto pass = std::make_unique<AlbinoEngine::Pass>(m_debugVS, m_debugPS);
    pass->name = "DebugLightPass";
    pass->buildInputLayout(engine.device());
    pass->setRasterizerState(m_rsSolid.Get());
    pass->setDepthStencialState(m_dsReadyOnly.Get());

    tech.addPass(std::move(pass));
}

void DemoScene::createStates(AlbinoEngine::Engine& engine)
{
    ID3D11Device* device = engine.device();

    // Rasterizer: solid
    {
        D3D11_RASTERIZER_DESC d{};
        d.FillMode = D3D11_FILL_SOLID;
        d.CullMode = D3D11_CULL_NONE;
        d.DepthClipEnable = TRUE;
        device->CreateRasterizerState(&d, m_rsSolid.ReleaseAndGetAddressOf());
    }

    // Rasterizer: overlay (keep solid here; wireframe + blending looks odd unless you're drawing tinted lines)
    {
        D3D11_RASTERIZER_DESC d{};
        d.FillMode = D3D11_FILL_SOLID;
        d.CullMode = D3D11_CULL_NONE;
        d.DepthClipEnable = TRUE;
        device->CreateRasterizerState(&d, m_rsOverlay.ReleaseAndGetAddressOf());
    }

    // Depth: solid pass writes depth
    {
        D3D11_DEPTH_STENCIL_DESC ds{};
        ds.DepthEnable = TRUE;
        ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        ds.DepthFunc = D3D11_COMPARISON_LESS;

        // Stencil 
        ds.StencilEnable = FALSE;
        ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        // Stencil FrontFace.
        ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil BackFace.
        ds.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        device->CreateDepthStencilState(&ds, m_dsSolid.ReleaseAndGetAddressOf());
    }

    // Depth: readOnly
    {
        D3D11_DEPTH_STENCIL_DESC dsReadOnly;
        dsReadOnly.DepthEnable = FALSE;
        dsReadOnly.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsReadOnly.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        // Stencil 
        dsReadOnly.StencilEnable = FALSE;
        dsReadOnly.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        dsReadOnly.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        // Stencil FrontFace.
        dsReadOnly.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsReadOnly.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dsReadOnly.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsReadOnly.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil BackFace.
        dsReadOnly.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsReadOnly.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dsReadOnly.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsReadOnly.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        device->CreateDepthStencilState(&dsReadOnly, m_dsReadyOnly.ReleaseAndGetAddressOf());
    }
    // Depth: overlay pass doesn't depth test/write (classic overlay)
    {
        D3D11_DEPTH_STENCIL_DESC ds{};
        ds.DepthEnable = FALSE;
        ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        // Stencil 
        ds.StencilEnable = FALSE;
        ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        // Stencil FrontFace.
        ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil BackFace.
        ds.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        device->CreateDepthStencilState(&ds, m_dsOverlay.ReleaseAndGetAddressOf());
    }

    // Blend: out = src * BlendFactor + dst * (1-BlendFactor)
    {
        D3D11_BLEND_DESC bd{};
        bd.AlphaToCoverageEnable = FALSE;
        bd.IndependentBlendEnable = FALSE;

        auto& rt = bd.RenderTarget[0];
        rt.BlendEnable = TRUE;
        rt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
        rt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
        rt.BlendOp = D3D11_BLEND_OP_ADD;

        // Alpha channel handling (not critical for this demo)
        rt.SrcBlendAlpha = D3D11_BLEND_ONE;
        rt.DestBlendAlpha = D3D11_BLEND_ZERO;
        rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;

        rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        device->CreateBlendState(&bd, m_bsOverlay.ReleaseAndGetAddressOf());
    }
}

void DemoScene::createEffect(AlbinoEngine::Engine& engine)
{
    // Shaders
    m_vs = std::make_shared<AlbinoEngine::VertexShader>(engine.device());
    m_ps = std::make_shared<AlbinoEngine::PixelShader>(engine.device());

    // NOTE: make sure your working directory is set so media/ resolves.
    m_vs->loadVertexShader(L"media//shaders//PositionTexture3D.shader", "vs_main", "vs_5_0");
    m_ps->loadPixelShader(L"media//shaders//PositionTexture3D.shader", "ps_main", "ps_5_0");
    m_vs->createConstantBuffers();

    auto eff = engine.getEffectsManager().createEffect("BasicEffect").get();
    auto& tech = eff->createTechnique("Default");
    auto& tech2 = eff->createTechnique("tech2");
    eff->setActiveTechnique("Default");
    eff->setActiveTechnique("tech2");
    // Fetch textures from the engine cache by *stem name* (metal_crate.jpg -> "metal_crate")
    auto crateA = engine.getTextureManager().getTexture(L"metal_crate");
    auto crateB = engine.getTextureManager().getTexture(L"metal_crate_two");
    auto tiles = engine.getTextureManager().getTexture(L"tiles");

    // tech2 
    // pass
    {
        auto techPass = std::make_unique<AlbinoEngine::Pass>(m_vs, m_ps);
        techPass->name = "tiles";
        techPass->buildInputLayout(engine.device());
        techPass->setRasterizerState(m_rsSolid.Get());
        techPass->setDepthStencialState(m_dsSolid.Get());
        this->setBlendFactor(1.0f, 0.0f, 0.0f, 1.0f);
        //techPass->setBlendState(m_bsOverlay.Get(), m_overlayFactor, 0xFFFFFFFF);
        if (tiles)
        {
            techPass->setTexture(0, tiles->getShaderResourceView());
            techPass->setSampler(0, tiles->getSamplerState());
        }
        tech2.addPass(std::move(techPass));
    }

    // Pass 1: opaque base
    {
        auto p1 = std::make_unique<AlbinoEngine::Pass>(m_vs, m_ps);
        p1->name = "Base";
        p1->buildInputLayout(engine.device());
        p1->setRasterizerState(m_rsSolid.Get());
        p1->setDepthStencialState(m_dsSolid.Get());

        if (crateA)
        {
            p1->setTexture(0, crateA->getShaderResourceView());
            p1->setSampler(0, crateA->getSamplerState());
        }
        tech.addPass(std::move(p1));
    }

    // Pass 2: overlay with blend factor
    {
        auto p2 = std::make_unique<AlbinoEngine::Pass>(m_vs, m_ps);
        p2->name = "Overlay";
        p2->buildInputLayout(engine.device());
        p2->setRasterizerState(m_rsOverlay.Get());
        p2->setDepthStencialState(m_dsOverlay.Get());
        //const float m_overlayFactor[4] = { 1.0f, 0.0f, 1.0f, 0.25f };
        this->setBlendFactor(1.0f, 0.0f, 1.0f, 0.25f);
        p2->setBlendState(m_bsOverlay.Get(), m_overlayFactor, 0xFFFFFFFF);

        if (crateB)
        {
            p2->setTexture(0, crateB->getShaderResourceView());
            p2->setSampler(0, crateB->getSamplerState());
        }
        tech.addPass(std::move(p2));
    }
}

void DemoScene::setBlendFactor(float r, float g, float b, float a)
{
    m_overlayFactor[0] = r;
    m_overlayFactor[1] = g;
    m_overlayFactor[2] = b;
    m_overlayFactor[3] = a;
}
bool DemoScene::initialize(AlbinoEngine::Engine& engine)
{
    // Camera
    m_camera = std::make_unique<AlbinoEngine::Camera>("Demo Camera");
    m_camera->setCameraEye(0.0f, 0.0f, -5.0f);
    m_camera->setCameraLookAt(0.0f, 0.0f, 0.0f);
    m_camera->setCameraUpVector(0.0f, 1.0f, 0.0f);
    m_camera->setCameraFov(75.0f);
    m_camera->setCameraAspectRation((float)engine.window().getWidth() / (float)engine.window().getHeight());
    m_camera->setCameraZNear(0.1f);
    m_camera->setCameraZFar(1000.0f);
    m_camera->setCameraPosition(0.0f, 0.0f, -3.0f);

    createStates(engine);
    createEffect(engine);
    createDebugLightEffect(engine);


    // One cube mesh
    m_cube = std::make_shared<AlbinoEngine::CubeMesh>(engine.device());
    m_cube->initliaze();
    m_cube->setPosition(0.0f, 0.0f, 6.0f);
    m_cube->setScale(1.0f, 1.0f, 1.0f);
    m_cube->setRotation(0.0f, 0.0f, 0.0f);
    std::wstring cubeName = L"cubeMesh";
    std::wstring cubeType = L"cubeMesh";
    AlbinoEngine::Mesh* cube = engine.getMeshManager().createMesh(cubeName, cubeType);
    AlbinoEngine::Mesh* cube2 = engine.getMeshManager().createMesh(L"cube 2", L"cubeMesh");
    AlbinoEngine::Mesh* plane = engine.getMeshManager().createMesh(L"planeMesh", L"planeMesh");
    plane->setCastShadows(false);
    AlbinoEngine::CubeMesh* cubeMesh = reinterpret_cast<AlbinoEngine::CubeMesh*>(cube);
    AlbinoEngine::CubeMesh* cubeMesh2 = reinterpret_cast<AlbinoEngine::CubeMesh*>(cube2);
    AlbinoEngine::PlaneMesh* planeMesh = reinterpret_cast<AlbinoEngine::PlaneMesh*>(plane);

    // Debug cube.
    AlbinoEngine::Mesh* lightMarker = engine.getMeshManager().createMesh(L"pointLightMarker", L"cubeMesh");
    lightMarker->setCastShadows(false);
    lightMarker->setReceiveShadows(false);
    AlbinoEngine::CubeMesh* lightMarkerMesh = reinterpret_cast<AlbinoEngine::CubeMesh*>(lightMarker);

    engine.getMeshManager().setMeshEffect(L"pointLightMarker", "DebugLight", "default");

    lightMarkerMesh->setScale(0.2f, 0.2f, 0.2f);
    lightMarkerMesh->setRotation(0, 0, 0);

    const AlbinoEngine::PointLight& pointLight = engine.getLightManager().getPointLight(0);
    lightMarkerMesh->setPosition(pointLight.position.x, pointLight.position.y, pointLight.position.z);

    engine.getMeshManager().setMeshEffect(L"cubeMesh", "BasicEffect", "Default");
    engine.getMeshManager().setMeshEffect(L"cube 2", "BasicEffect", "tech2");
    engine.getMeshManager().setMeshEffect(L"planeMesh", "BasicEffect", "tech2");

    planeMesh->setProperties(10.0f, 10.0f, 5.0f, 10.0f, 10.0f);
    planeMesh->initialize();
    
    planeMesh->setPosition(0, -1, 0);
    planeMesh->setScale(1, 1, 1);
    planeMesh->setRotation(0, 0, 0);
    
    cubeMesh2->serVisible(true);
    cubeMesh2->setCastShadows(true);
    cubeMesh2->setPosition(5.0f, 0.0f, 5.0f);
    cubeMesh2->setScale(1.0f, 1.0f, 1.0f);
    cubeMesh2->setRotation(0, 0, 0);
    
    cubeMesh->setPosition(0.0f, 0.0f, 0.0f);
    cubeMesh->setScale(1.0f, 1.0f, 1.0f);
    cubeMesh->setRotation(0.0f, 0.0f, 0.0f);

    return true;
}

void DemoScene::update(AlbinoEngine::Engine& engine, float dt)
{


    AlbinoEngine::Mesh* cube = engine.getMeshManager().getMeshByName(L"cubeMesh");
    AlbinoEngine::CubeMesh* cubeMesh = reinterpret_cast<AlbinoEngine::CubeMesh*>(cube);
    // Mouse look (hold LMB)
    HWND hwnd = engine.window().getWindowHandle();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);

    bool isDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    const float mouseSensitivity = 0.0035f;

    if (isDown)
    {
        if (!m_wasDown)
        {
            m_lastPt = pt;
            m_wasDown = true;
        }
        else
        {
            int dx = pt.x - m_lastPt.x;
            int dy = pt.y - m_lastPt.y;
            m_lastPt = pt;

            float yawDelta = dx * mouseSensitivity;
            float pitchDelta = dy * mouseSensitivity;
            m_camera->rotateYawPitch(yawDelta, pitchDelta);
        }
    }
    else
    {
        m_wasDown = false;
    }

    if (keyDown(VK_ESCAPE))
        PostQuitMessage(0);

    const float speed = 10.0f;
    float xRot = cubeMesh->getMeshRotation().x;
    float yRot = cubeMesh->getMeshRotation().y;
    float zRot = cubeMesh->getMeshRotation().z;
    xRot += speed * dt;
    yRot += speed * dt;
    zRot *= speed * dt;

    AlbinoEngine::PointLight movingLight = engine.getLightManager().getPointLight(0);
    
    float t = engine.getTotalTime();

    movingLight.position.x = cosf(t) * 3.0f;
    movingLight.position.y = 1.5f + sinf(t * 0.5f) * 0.5f;
    movingLight.position.z = sinf(t) * 3.0f;

    engine.getLightManager().setPointLight(0, movingLight);
    AlbinoEngine::Mesh* marker = engine.getMeshManager().getMeshByName(L"pointLightMarker");
    AlbinoEngine::CubeMesh* markerMesh = reinterpret_cast<AlbinoEngine::CubeMesh*>(marker);

    if (markerMesh)
    {
        const AlbinoEngine::PointLight& point = engine.getLightManager().getPointLight(0);
        marker->setPosition(point.position.x, point.position.y, point.position.z);
    }
    // Movement
    if (keyDown(VK_UP) || keyDown('W'))    m_camera->moveForward(speed * dt);
    if (keyDown(VK_DOWN) || keyDown('S'))  m_camera->moveBackwards(speed * dt);
    if (keyDown(VK_LEFT) || keyDown('A'))  m_camera->moveLeft(speed * dt);
    if (keyDown(VK_RIGHT) || keyDown('D')) m_camera->moveRight(speed * dt);
    if (keyDown(VK_SPACE)) m_camera->moveUp(speed * dt);
    if (keyDown('R')) m_camera->setCameraPosition(0, 0, -3);

    if (keyDown('X')) {
        Mesh* mesh = engine.getMeshManager().getMeshByName(L"cube 2");
        mesh->serVisible(false);
    }

    if (keyDown('C'))
    {
        Mesh* mesh = engine.getMeshManager().getMeshByName(L"cube 2");
        if (!mesh->isVisible())
            mesh->serVisible(true);
    }
    cubeMesh->setRotation(xRot, yRot, zRot);
}

void DemoScene::render(AlbinoEngine::Engine& engine)
{
    AlbinoEngine::Mesh* marker = engine.getMeshManager().getMeshByName(L"pointLightMarker");
    //marker->setCastShadows(false);
    if (marker && m_camera)
    {
        AlbinoEngine::EffectContext fx{};
        fx.device = engine.device();
        fx.context = engine.context();
        fx.camera = m_camera.get();
        fx.useDebugColor = true;

        const AlbinoEngine::PointLight& point = engine.getLightManager().getPointLight(0);
        fx.debugColor = { point.color.x, point.color.y, point.color.z, 1.0f };

        auto* eff = engine.getEffectsManager().getEffect("DebugLight");
        if (eff)
            eff->render(fx, *marker);
    }
    //if (!m_cube || !m_camera) return;

    //AlbinoEngine::EffectContext fx;
    //fx.device = engine.device();
    //fx.context = engine.context();
    //fx.camera = m_camera.get();

    //AlbinoEngine::Mesh* mesh = engine.getMeshManager().getMeshByName(L"cubeMesh");

    //m_effect.render(fx, *m_cube);
    //engine.getMeshManager().renderAll(engine.getEffectsManager(), fx);
}

void DemoScene::destroy()
{
    if (this->m_rsSolid) m_rsSolid.Reset();
    if (this->m_dsSolid) m_dsSolid.Reset();
    if (this->m_rsOverlay) m_rsOverlay.Reset();
    if (this->m_dsOverlay) m_dsOverlay.Reset();
    if (this->m_bsOverlay) m_bsOverlay.Reset();
    if (this->m_vs) m_vs.reset();
    if (this->m_ps) m_ps.reset();
    if (this->m_cube) m_cube.reset();
}

AlbinoEngine::EffectContext DemoScene::buildEffectContext(AlbinoEngine::Engine& engine)
{
    AlbinoEngine::EffectContext fx{};
    fx.device = engine.device();
    fx.context = engine.context();
    fx.camera = m_camera.get();
    return fx;
}
