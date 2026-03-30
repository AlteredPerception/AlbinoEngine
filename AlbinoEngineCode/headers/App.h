#pragma once

#include "Engine.h"

namespace AlbinoEngine
{
	class App
	{
	public:
		App();

		~App();

		Engine& createEngine();
	protected:
		std::unique_ptr<Engine> m_Engine;
	};
}