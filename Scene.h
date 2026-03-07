#pragma once

#include <memory>

namespace AlbinoEngine
{
    class Engine;
    struct EffectContext;
    // Implement this in the *game* project.
    // Engine owns one active scene and calls initialize/update/render.
    class IScene
    {
    public:
        virtual ~IScene() = default;
        virtual bool initialize(Engine& engine) = 0;
        virtual void update(Engine& engine, float dt) = 0;
        virtual void render(Engine& engine) = 0;

        // New: allows Engine / meshManager to get the correct context.
        virtual EffectContext buildEffectContext(Engine& engine) = 0;
    protected:
        std::unique_ptr<AlbinoEngine::Camera> m_camera;

    };
}
