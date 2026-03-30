//#include "..//AlbinoEngine/App.h"
//#include <iostream>
//#include <filesystem>
//#include <sstream>
//
//#pragma comment(lib, "..//bin/x64/Debug/AlbinoEngine_d.lib")
//
//using namespace AlbinoEngine;
//
//namespace fs = std::filesystem;
//
//int main()
//{
//	
//	App app;
//
//	Engine& engine = app.createEngine();
//
//	engine.initialize();
//	TextureManager& manager = engine.getTextureManager();
//	UnorderedTextureMap textureMap = manager.getUnorderedTextureMap();
//
//	MeshManager& meshManager = engine.getMeshManager();
//
//	CubeMesh* cubeMesh = reinterpret_cast<CubeMesh*>(meshManager.createMesh(L"cube", L"cubeMesh"));
//
//
//	std::wstring metal_crate_name = L"metal_crate";
//	Texture2D* metal_crate = reinterpret_cast<Texture2D*>(&textureMap[metal_crate_name]);
//
//	
//	
//	return engine.run();
//}

#include <Windows.h>
#include <memory>

#include "..//AlbinoEngine/App.h"
#include "..//AlbinoEngine/Engine.h"

#include "DemoScene.h"

#pragma comment(lib, "..//bin/x64/Debug/AlbinoEngine_d.lib")

using namespace AlbinoEngine;

int main() {

    App app;

    Engine& engine = app.createEngine();
    if (!engine.initialize(L"Albino Engine", 800, 600))
        return -1;

    // Scene lives in the *game* project.
    engine.setScene(std::make_unique<DemoScene>());

    return engine.run();
}
