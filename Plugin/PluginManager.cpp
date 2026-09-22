#include "Plugin/PluginManager.h"
#include <Graphics/Renderer.h>
#include <Graphics/Shader.h>

namespace MiniEngine {
    void PluginManager::loadPlugin(const String& path)
    {
        // 打开动态库
        void* handle = nullptr;
#ifdef _WIN32
        handle = LoadLibraryA(path.c_str());  // 使用 ANSI 版本
        if (!handle) {
            DWORD err = GetLastError();
            char buf[256];
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, buf, sizeof(buf), nullptr);
            LOG_INFO(String("Failed to load plugin: ") + buf);
            return;
        }
#else
        handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            LOG_INFO(String("Failed to load plugin: ") + dlerror());
            return;
        }
#endif

        // 查找创建函数
#ifdef _WIN32
        auto create = reinterpret_cast<CreatePluginFunc>(GetProcAddress((HMODULE)handle, "createPlugin"));
        if (!create) {
            LOG_INFO("Invalid plugin (missing createPlugin)");
            FreeLibrary((HMODULE)handle);
            return;
        }
        auto setm = reinterpret_cast<SetMeshSingleton>(GetProcAddress((HMODULE)handle, "setMeshSingleton"));
        auto sets = reinterpret_cast<SetShader>(GetProcAddress((HMODULE)handle, "setShader"));
        auto setglfcb = reinterpret_cast<SetGLfunCallBack>(GetProcAddress((HMODULE)handle, "setGLFunCallback"));
        auto setModefcb = reinterpret_cast<SetModelfunCallBack>(GetProcAddress((HMODULE)handle, "setModelFunCallback"));
#else
        auto create = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
        if (!create) {
            LOG_INFO(String("Invalid plugin (missing createPlugin): ") + dlerror());
            dlclose(handle);
            return;
        }
        auto set = reinterpret_cast<SetMeshSingleton>(dlsym(handle, "setMeshSingleton"));
        auto sets = reinterpret_cast<SetShader>(dlsym(handle, "setShader"));
#endif
        // 传递gl函数注册回调
        if (setglfcb)
        {
            setglfcb(glfunCB);
        }
        
        // 传递shader，生命周期由插件保证。
        if (sets)
        {
            sets(new Shader());
        }
        // 传递坐标回调
        if (setModefcb)
        {
            setModefcb(modelCB);
        }

        // 创建插件实例
        auto plugin = std::unique_ptr<IPlugin>(create());
        LOG_INFO(String("Plugin loaded: ") + plugin->getName());
        
        // 保存需要传递Mesh网格的插件函数，作为预制图形api
        if (!setm) {
            LOG_INFO(plugin->getName()+String(" plugin without mesh"));
        }
        else
        {
            m_setSingletonFuns.push_back(setm);
        }
        
        //保存
        m_plugins.push_back(std::move(plugin));
    }
    void PluginManager::setPluginMesh(const ResourceManager* rm)
    {
        for (auto itf :m_setSingletonFuns)
        {
            itf(rm);
        }
    }
    void PluginManager::updateAll(float dt)
    {
        for (auto& p : m_plugins)p->onUpdate(dt);
    }
    void PluginManager::renderAll(Renderer& renderer, Shader& shader)
    {
        for (auto& p : m_plugins) {
            p->onRender(renderer, shader);
        }
    }

    void PluginManager::scanAndLoadPlugins(const std::string& directory)
    {
        if (!std::filesystem::exists(directory)) {
            LOG_INFO("Plugin directory not found: " + String(directory.c_str()));
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            // 根据平台筛选动态库后缀
#if defined(_WIN32)
            if (ext != ".dll") continue;
#else
            if (ext != ".so") continue;
#endif
            loadPlugin(entry.path().string().c_str());
        }
    }

	void PluginManager::updateAspectRatio(float aspectRatio)
	{
		for (auto& p : m_plugins) {
			p->onUpdateAspectRatio(aspectRatio);
		}
	}

	void PluginManager::keyEventUpdate(int key)
	{
		for (auto& p : m_plugins) {
			if (key >= 0 && key < 37 && p->keyCallbacks[key]) {
				p->keyCallbacks[key]();
			}
		}
	}
}