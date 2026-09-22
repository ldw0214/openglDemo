#pragma once
#include <vector>
#include <memory>

//条件编译 根据平台选择链接库
#ifdef  _WIN32
	#include <Windows.h>
#else
	#include <dlfcn.h>
#endif //  _WIN32

#include "Core/Logger.h"
#include "Core/String.h"
#include "Core/ResourceManager.h"
#include "Plugin/IPlugin.h"



namespace MiniEngine {
	class Renderer;
	class Shader;


	//插件管理器，负责动态加载.so/.dll
	class PluginManager {
	public:
		//动态库加载
		void loadPlugin(const String& path);
		//mesh传递
		void setPluginMesh(const ResourceManager* rm);
		//更新插件
		void updateAll(float dt);
		void updateAspectRatio(float aspectRatio);
		//绘制插件
		void renderAll(Renderer& renderer, Shader& shader);

		void scanAndLoadPlugins(const std::string& directory);

		void setGlfunCallBack(void* (*fun)(const char*)) { glfunCB = fun; };
		void setModelCallBack(glm::mat4(*modelCB)()) { this->modelCB = modelCB; };
		void keyEventUpdate(int key);


	private:
		std::vector<std::unique_ptr<IPlugin>> m_plugins;
		std::vector<SetMeshSingleton> m_setSingletonFuns;
		void* (*glfunCB)(const char*) = nullptr;
		glm::mat4(*modelCB)() = nullptr;

		float m_aspectRatio = 1280.0f / 720.0f;
	
	};

	// 插件创建函数签名，需要extern"C"导出
	using CreatePluginFunc = IPlugin * (*)();
	using SetGLfunCallBack = void(*)(void* (*)(const char*));
	using SetModelfunCallBack = void(*)(glm::mat4(*)());
}// namespace MiniEngine