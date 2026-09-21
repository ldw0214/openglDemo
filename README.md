# MiniEngine 入门指引

> 一个基于现代 C++ 的简易 3D 渲染引擎，用于学习图形编程、多线程与插件架构。

---

## 📖 项目简介

MiniEngine 是一个使用 **C++17/20** 编写的轻量级 3D 渲染引擎。它通过可运行的代码展示了从底层资源管理到上层渲染管线的完整实现，适合作为图形编程、并发编程和软件架构的学习参考。

项目涵盖了以下技术方向：

- **传统 C++**：手写 `String`、内存池、智能指针、虚表模拟
- **现代 C++**：移动语义、完美转发、Lambda、`std::optional`、`std::variant`、范围库
- **多线程**：无锁队列、线程池、异步纹理加载、内存序
- **OpenGL**：RAII 资源封装、着色器热重载、粒子系统、场景图
- **插件系统**：动态库自动加载、预制资源复用、热插拔

---

## ✨ 核心特性

| 模块 | 功能 |
|------|------|
| **Core** | 手写 String、内存池、异步日志、单例 |
| **Threading** | 无锁 SPSC 队列、线程池、`std::future` |
| **Graphics** | VAO/VBO 封装、Shader 热重载、异步纹理、FPS 相机、场景图、粒子系统 |
| **Plugin** | 动态库扫描加载、`IPlugin` 接口、`ResourceManager` 预制资源 |
| **App** | GLFW 窗口封装、主循环、ImGui 调试面板（todo） |

---

## 📁 目录结构

```
MiniEngine/
├── Core/               # 基础工具
│   ├── String.h        # 手写字符串类
│   ├── MemoryPool.h    # 固定大小内存池
│   ├── Logger.h        # 异步日志
│   ├── ResourceManager.h # 预制资源管理
│   └── Singleton.h
├── Threading/          # 多线程设施
│   ├── SPSCQueue.h     # 单生产者单消费者无锁队列
│   └── ThreadPool.h    # 可返回 future 的线程池
├── Graphics/           # OpenGL 封装
│   ├── VertexArray.h   # VAO RAII
│   ├── Buffer.h        # VBO/EBO RAII
│   ├── Shader.h        # 着色器程序 + 热重载
│   ├── Texture.h       # 纹理 + 异步加载
│   ├── Mesh.h          # 网格数据 + 工厂方法
│   ├── Camera.h        # FPS 相机控制器
│   ├── Renderer.h      # 正向渲染器
│   ├── SceneNode.h     # 场景图节点
│   └── ParticleSystem.h # 粒子系统
├── Plugin/             # 插件系统
│   ├── IPlugin.h       # 插件抽象接口
│   └── PluginManager.h # 动态库加载与管理
├── App/                # 应用层
│   ├── Window.h        # GLFW 窗口 RAII
│   └── Application.h   # 主循环与模块集成
├── assets/             # 资源文件
│   ├── shaders/        # GLSL 着色器
│   └── textures/       # 纹理图片
├── plugins/            # 编译后的插件动态库
├── CMakeLists.txt
└── main.cpp
```

---

## 🛠 环境要求

- **操作系统**：Windows 10+ / Linux / macOS
- **编译器**：支持 C++17 及以上
  - MSVC 2019+
  - GCC 9+
  - Clang 10+
- **构建工具**：CMake 3.15+
- **第三方库**：
  - GLFW（窗口与输入）
  - GLAD（OpenGL 函数加载）
  - GLM（数学库）
  - stb_image（图片加载）
  - ImGui（todo）

## 🚀 快速开始

### 操作说明

| 按键 | 功能 |
|------|------|
| `W/A/S/D` | 相机前后左右移动 |
| `Q/E` | 相机垂直升降 |
| 鼠标移动 | 旋转视角（需锁定光标） |
| 鼠标滚轮 | 缩放视野 |
| `空格` | 发射粒子 |
| `ESC` | 退出程序 |

---

## 🧩 项目架构

MiniEngine 采用四层架构，自底向上依次为：

1. **Core**：基础工具，无外部图形依赖
2. **Threading**：并发基础设施，可独立测试
3. **Graphics**：OpenGL 资源封装与渲染
4. **App**：应用层，整合所有模块

每层仅依赖下层，模块间通过接口解耦，便于替换与扩展。

---

## 🔍 核心模块详解

### String（`Core/String.h`）
- 手写字符串类，演示深拷贝、移动语义、copy-and-swap。

### MemoryPool（`Core/MemoryPool.h`）
- 固定大小内存池，使用 `union` 实现空闲链表。
- 为粒子系统提供高效分配，避免频繁 `new/delete`。

### Logger（`Core/Logger.h`）
- Meyer's 单例 + `SPSCQueue` 实现异步日志。
- 后台线程写入文件，主线程非阻塞。

### SPSCQueue（`Threading/SPSCQueue.h`）
- 单生产者单消费者无锁环形队列。
- 使用 `std::atomic` 与 `acquire-release` 内存序，缓存行对齐避免伪共享。

### ThreadPool（`Threading/ThreadPool.h`）
- 固定线程数，提交任务返回 `std::future`。
- 基于 `packaged_task` 和条件变量，支持任意可调用对象。

### Shader（`Graphics/Shader.h`）
- 封装着色器编译、链接、uniform 设置。
- 热重载：检测文件修改时间，运行时重新编译。

### Texture（`Graphics/Texture.h`）
- 异步加载：`std::async` 后台读取图片，主线程 `tryUpload` 创建 GL 纹理。
- 异常安全，使用 `std::optional` 避免隐式异常崩溃。

### Camera（`Graphics/Camera.h`）
- FPS 风格相机，支持键盘移动、鼠标旋转、滚轮缩放。
- 可调节鼠标灵敏度、移动速度。

### SceneNode（`Graphics/SceneNode.h`）
- 树形场景图，递归更新世界变换。
- 支持挂载 `Mesh` 和颜色，便于快速搭建场景。

### ParticleSystem（`Graphics/ParticleSystem.h`）
- 基于 `MemoryPool` 的粒子系统，支持发射、更新、回收。
- 使用点精灵渲染，可结合线程池并行更新。

### PluginManager（`Plugin/PluginManager.h`）
- 扫描指定目录（如 `plugins/`）下的动态库（`.dll`/`.so`）。
- 通过 `dlopen`/`dlsym` 加载 `createPlugin` 函数，实例化插件。
- 引擎内置 `ResourceManager`，提供预制网格（如 `quad`、`cube`）。

---

## 🔌 插件开发指南

### 1. 插件接口

```cpp
// IPlugin.h
namespace MiniEngine {
    class Renderer;
    class Shader;

    class IPlugin {
    public:
        virtual ~IPlugin() = default;
        virtual const char* getName() const = 0;
        virtual void onUpdate(float deltaTime) = 0;
        virtual void onRender(Renderer& renderer, Shader& shader) = 0;
    };
}
```

### 2. 创建插件

- 实现 `IPlugin` 接口。
- 在 `onRender` 中使用 `ResourceManager::instance().getMesh("quad")` 获取预制网格。
- 可使用 `Shader` 类创建自己的着色器（支持从源码字符串创建）。
- 导出 `extern "C" IPlugin* createPlugin()` 函数。

**示例：背景渐变插件**

```cpp
#include "IPlugin.h"
#include "Core/ResourceManager.h"
#include "Graphics/Shader.h"
#include <glad/glad.h>

class BackgroundPlugin : public MiniEngine::IPlugin {
public:
    const char* getName() const override { return "Background"; }
    void onUpdate(float) override {}
    void onRender(MiniEngine::Renderer&, MiniEngine::Shader&) override {
        if (!m_init) {
            m_shader = std::make_unique<MiniEngine::Shader>();
            m_shader->createFromSource(vertSrc, fragSrc);
            m_quad = MiniEngine::ResourceManager::instance().getMesh("quad");
            m_init = true;
        }
        m_shader->bind();
        m_quad->bind();
        glDrawElements(GL_TRIANGLES, m_quad->indexCount(), GL_UNSIGNED_INT, 0);
    }
private:
    std::unique_ptr<MiniEngine::Shader> m_shader;
    std::shared_ptr<MiniEngine::Mesh> m_quad;
    bool m_init = false;
};

extern "C" MiniEngine::IPlugin* createPlugin() {
    return new BackgroundPlugin();
}
```

### 3. 编译插件为动态库

```bash
# Linux / macOS
g++ -std=c++17 -shared -fPIC -I/path/to/MiniEngine BackgroundPlugin.cpp -o BackgroundPlugin.so -lGL

# Windows (MSVC)
cl /std:c++17 /LD /I MiniEngine\include BackgroundPlugin.cpp /FeBackgroundPlugin.dll opengl32.lib
```

### 4. 加载插件

将生成的动态库放入 `plugins/` 目录，引擎启动时会自动扫描并加载。

----

---

**希望 MiniEngine 能帮助你更好地理解现代 C++ 与图形编程。**
