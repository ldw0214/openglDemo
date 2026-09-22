#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include "Core/Logger.h"
#include "App/Window.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"
#include "Graphics/SceneNode.h"
#include "Graphics/Mesh.h"
#include "Graphics/ParticleSystem.h"
#include "Threading/ThreadPool.h"
#include "Plugin/PluginManager.h"



namespace MiniEngine {

    // 头文件解耦
    class Texture;

    /**
     * 应用程序主类
     * 负责窗口、渲染器、场景、粒子系统等所有子系统的初始化与主循环调度。
     */
    class Application {
    public:
        Application();

        int run();

    private:
        // ---------- 初始化 ----------
        void initWindow();

        void initOpenGL();

        void initGraphics();

        void initScene();

        void initParticleSystem();

        void initPlugins();

        // ---------- 每帧操作 ----------
        void processInput(float dt);

        /** 简单的粒子发射函数：在相机前方随机位置生成粒子 */
        void emitParticles(float dt);

        void updateApp(float dt);

		// 更新屏幕比例
		void updateAspectRatio(int width, int height);

        void renderApp();

        void shutdown();

        static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
            GLenum severity, GLsizei length,
            const GLchar* message, const void* userParam) {
            if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
            String msg = "OpenGL Debug: ";
            LOG_INFO(msg + message);
        }

        // ---------- 成员变量 ----------
        std::unique_ptr<Window> m_window;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Shader> m_particleShader;   // 专用于粒子渲染的着色器
        std::unique_ptr<Camera> m_camera;
        std::unique_ptr<Renderer> m_renderer;
        std::shared_ptr<SceneNode> m_root;
        std::shared_ptr<Texture> m_texture;
        std::unique_ptr<ParticleSystem> m_particleSystem;
        std::unique_ptr<ThreadPool> m_threadPool;
        std::unique_ptr<PluginManager> m_pluginManager;

        // 屏幕比例
		float m_aspectRatio = 16.0f / 9.0f;

		bool m_isMouseCaptured = false; // 鼠标是否被捕获（用于相机控制）

        struct keyBufferItme
        {
            int key = 0;
            int action = 0;
            int mods = 0;
        };

        //命令缓存队列，即时响应（回调），每帧处理
        std::queue<struct keyBufferItme> m_keyBufferQueue{};
    };

} // namespace MiniEngine