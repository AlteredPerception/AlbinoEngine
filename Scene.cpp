#include "Scene.h"
#include "Engine.h"
#include "Pass.h"
namespace AlbinoEngine
{
	EffectContext IScene::buildEffectContext(Engine& engine)
	{
		EffectContext fx = {};
		fx.device = engine.device();
		fx.context = engine.context();
		fx.camera = this->m_camera.get();

		return fx;
	}
}