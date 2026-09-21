#include <memory>
#include <chrono>
#include <random>

#include "App/Application.h"
#include "Graphics/Texture.h"
#include "Core/ResourceManager.h"

namespace MiniEngine {

    //模型矩阵，静态，通过在绘图前更新，控制插件渲染位置。
    glm::mat4 pluginsModelMat(1.0);


	Application::Application()
    {
        initWindow();
        initOpenGL();
        initGraphics();
        initScene();
        initParticleSystem();      // 粒子系统初始化
        m_threadPool = std::make_unique<ThreadPool>(4);
        initPlugins();
    }
    int Application::run()
    {
        auto lastTime = std::chrono::high_resolution_clock::now();

        while (!m_window->shouldClose()) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            processInput(dt);
            updateApp(dt);
            renderApp();

            m_window->swapBuffers();
            m_window->pollEvents();
        }
        shutdown();
        return 0;
    }
    void Application::initWindow()
    {
        m_window = std::make_unique<Window>(1280, 720, "MiniEngine");

        // 鼠标移动 → 相机视角旋转
        m_window->setMouseCallback([this](double xpos, double ypos) {
            m_camera->processMouseMovement(xpos, ypos);
            });

        // 滚轮 → 相机缩放
        m_window->setScrollCallback([this](double, double yoffset) {
            m_camera->processMouseScroll(yoffset);
            });

        // 窗口尺寸变化 → 调整视口
        m_window->setResizeCallback([](int width, int height) {
            glViewport(0, 0, width, height);
            });

        // 键盘事件回调，操作响应
        m_window->setKeyCallback([this](int key, int scancode, int action, int mods) {

            if (m_keyBufferQueue.empty())
            {
                m_keyBufferQueue.push(keyBufferItme{ key, action, mods });
                return;
            }
            
            keyBufferItme& tempLastKey = m_keyBufferQueue.back();

            if (tempLastKey.key != key || tempLastKey.action != action || tempLastKey.mods != mods)
            {
                m_keyBufferQueue.push(keyBufferItme{ key, action, mods });
                if (m_keyBufferQueue.size() > 10)
                {
                    m_keyBufferQueue.pop();
                }
            }
            }
        );

        glfwSetInputMode(m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    void Application::initOpenGL()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            LOG_INFO("Failed to initialize GLAD");
            std::exit(EXIT_FAILURE);
        }
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debugCallback, nullptr);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE); // 允许在着色器中控制点精灵大小
        //glfwSwapInterval(0);
    }
    void Application::initGraphics()
    {
        m_shader = std::make_unique<Shader>(SHARDER_FILE_PATH("basic.vert"),
            SHARDER_FILE_PATH("basic.frag"));
        m_particleShader = std::make_unique<Shader>("assets/shaders/particle.vert",
            "assets/shaders/particle.frag");
        m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 2.0f, 5.0f));
        m_renderer = std::make_unique<Renderer>();
    }
    void Application::initScene()
    {
        m_root = std::make_shared<SceneNode>();

        // 地面或立方体作为静态场景
        auto cube = std::make_shared<SceneNode>();
        cube->setMesh(std::make_shared<Mesh>(Mesh::createCube()));
        cube->setLocalTransform(glm::mat4(1.0f));
        m_root->addChild(cube);

        // 纯色纹理（示例）
        //m_texture = std::make_shared<Texture>(Texture::CreateCheckerboard());
        // 异步加载纹理（示例）
        m_texture = std::make_shared<Texture>();
        m_texture->loadAsync(TEXTURES_FILE_PATH("wood.png"));
        // 注册全屏四边形（NDC 坐标）
        {
            std::vector<float> quadVertices = {
                -1.0f, -1.0f,  0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // 位置+法线+UV
                 1.0f, -1.0f,  0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
                 1.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
                 1.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
                -1.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f
            };
            std::vector<unsigned int> quadIndices = { 0,1,2, 3,4,5 };
            auto quadMesh = std::make_shared<Mesh>(quadVertices, quadIndices, std::vector<int>{3, 3, 2});
            ResourceManager::instance().registerMesh("quad", quadMesh);
        }

        // 注册立方体
        ResourceManager::instance().registerMesh("cube", std::make_shared<Mesh>(Mesh::createCube()));
    }
    void Application::initParticleSystem()
    {
        // 最多容纳 10,000 个粒子
        m_particleSystem = std::make_unique<ParticleSystem>(10000);
    }
    void Application::initPlugins()
    {
        m_pluginManager = std::make_unique<PluginManager>();
        m_pluginManager->setGlfunCallBack([](const char * name)->void *{
            return (void*)glfwGetProcAddress(name);
        });
        m_pluginManager->setModelCallBack([]()->glm::mat4 {
            return pluginsModelMat;
            });
        m_pluginManager->scanAndLoadPlugins("plugins/"); // 自动加载 plugins 目录下所有插件
        m_pluginManager->setPluginMesh(&ResourceManager::instance());
    }
    void Application::processInput(float dt)
    {
        GLFWwindow* native = m_window->getNativeWindow();

        //指令队列处理
        while (!m_keyBufferQueue.empty())
        {
            keyBufferItme& tempKey = m_keyBufferQueue.front();
            switch (tempKey.key)
            {
            case GLFW_KEY_SPACE:
                if(tempKey.action == GLFW_PRESS)
                {
                    m_camera->jump();
                }
                break;
            default:
                break;
            }
            m_keyBufferQueue.pop();
        }

        m_camera->applyGravity(dt,1.0);

        // 相机移动
        m_camera->processKeyBoard(native, dt);

        // 按空格键发射粒子（每帧发射少量，演示用）
        if (glfwGetMouseButton(native, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            emitParticles(dt);
        }

        if (glfwGetKey(native, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            m_window->setShouldClose(true);
    }
    void Application::emitParticles(float dt)
    {
        static std::default_random_engine rng;
        static std::uniform_real_distribution<float> offset(-0.5f, 0.5f);
        static std::uniform_real_distribution<float> life(1.0f, 3.0f);
        static std::uniform_real_distribution<float> speed(1.0f, 3.0f);

        // 每帧发射约 100 个粒子（dt 控制数量，避免帧率波动影响）
        int count = static_cast<int>(100.0f * dt);
        if (count < 1) count = 1;

        glm::vec3 front = m_camera->getFront();
        for (int i = 0; i < count; ++i) {
            glm::vec3 pos = m_camera->getPosition() + front * 2.0f
                + glm::vec3(offset(rng), offset(rng), offset(rng));
            glm::vec3 dir = front + glm::vec3(offset(rng) * 0.5f, offset(rng) * 0.5f, 0);
            glm::vec4 color(1.0f, 0.6f, 0.2f, 1.0f); // 橙色粒子
            m_particleSystem->emit(pos, glm::normalize(dir), speed(rng), life(rng), color);
        }
    }
    void Application::updateApp(float dt)
    {
        // 着色器热重载
        m_shader->checkAndReload();
        if (m_particleShader) m_particleShader->checkAndReload();

        // 纹理异步上传
        if (m_texture) m_texture->tryUpload();

        // 场景树更新
        m_root->update(glm::mat4(1.0f));

        // 粒子系统更新（可以使用线程池并行，这里保持简单串行）
        m_particleSystem->update(dt);
    }
    void Application::renderApp()
    {
        glClearColor(.1f, .1f, .15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        // ---- 场景渲染 ----
        m_renderer->beginScene(*m_camera);
        m_shader->bind();
        m_shader->setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        m_shader->setVec3("viewPos", m_camera->getPosition());

        if (m_texture) {
            m_texture->bind(0);
            m_shader->setInt("texture1", 0);
        }
        
        m_root->render(*m_renderer, *m_shader);
        m_pluginManager->renderAll(*m_renderer, *m_shader);
        // ---- 粒子渲染 ----
        if (m_particleShader && m_particleSystem) {
            m_particleShader->bind();
            // 传递统一的视图和投影矩阵
            m_particleSystem->render(
                *m_particleShader,
                m_camera->getViewMatrix(),
                m_camera->getProjectionMatrix(m_window->getAspectRatio())
            );
        }
        m_renderer->endScene();
    }
    void Application::shutdown()
    {
        // 先释放 OpenGL 相关资源，再销毁窗口
        m_root.reset();
        m_particleSystem.reset();
        m_shader.reset();
        m_particleShader.reset();
        m_renderer.reset();
        m_threadPool.reset();
        // Window 析构会自动清理 GLFW
    }
} // namespace MiniEngine