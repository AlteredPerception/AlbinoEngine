#pragma once



#include <memory>
#include "EffectContext.h"

namespace AlbinoEngine
{
    class Engine;
    class IScene
    {
    public:
        virtual ~IScene() = default;
        virtual bool initialize(Engine& engine) = 0;
        virtual void update(Engine& engine, float dt) = 0;
        virtual void render(Engine& engine) = 0;

        virtual EffectContext buildEffectContext(Engine& engine) = 0;
    protected:
        std::unique_ptr<AlbinoEngine::Camera> m_camera;

    };
}
