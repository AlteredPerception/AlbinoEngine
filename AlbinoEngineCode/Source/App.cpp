#include "App.h"

namespace AlbinoEngine
{
	App::App()
	{
		this->m_Engine = std::make_unique<Engine>();
	}

	App::~App()
	{
		m_Engine.reset();
	}

	Engine& App::createEngine()
	{
		return *m_Engine.get();
	}

}