#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Graphics/Mesh.h"
#include "Core/Logger.h"

namespace MiniEngine {

    /**
     * 引擎内置资源管理器（单例）。
     * 提供预制网格的注册与获取，供插件和内部模块使用。
     */
    class ResourceManager {
    public:
        static ResourceManager& instance() {
            static ResourceManager mgr;
            return mgr;
        }

        // 注册一个预制网格（通常在引擎初始化时调用）
        void registerMesh(const std::string& name, std::shared_ptr<Mesh> mesh) {
            m_meshes[name] = std::move(mesh);
        }

        // 获取预制网格，若不存在返回 nullptr
        std::shared_ptr<Mesh> getMesh(const std::string& name) const {
            auto it = m_meshes.find(name);
            if (it != m_meshes.end()) return it->second;
            LOG_INFO("ResourceManager: mesh '" + String(name.c_str()) + "' not found");
            return nullptr;
        }

        String shaderCodeV;
        String shaderCodeF;

    private:
        ResourceManager() = default;
        std::unordered_map<std::string, std::shared_ptr<Mesh>> m_meshes;
    };

    // dll传递借口，提供预制形状api
    using SetMeshSingleton = void * (*)(const MiniEngine::ResourceManager*);
} // namespace MiniEngine