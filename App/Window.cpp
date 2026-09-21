#include"App/Window.h"
#include "Core/Logger.h"
#include "Core/MathTool.h"

namespace MiniEngine{
	void Window::keyCallbackStatic(GLFWwindow* win, int key, int scancode, int action, int mods)
	{
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
		if (self && self->m_keyCallback)
			self->m_keyCallback(key, scancode, action, mods);
	}
	void Window::cursorPosCallbackStatic(GLFWwindow* win, double x, double y)
	{
		//LOG_INFO("MOVE X:" + String::toString(x) + " Y:" + String::toString(y));
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
		if (self && self->m_mouseCallback)
			self->m_mouseCallback(x, y);
	}
	void Window::scrollCallbackStatic(GLFWwindow* win, double xoff, double yoff)
	{
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
		if (self && self->m_scrollCallback)
			self->m_scrollCallback(xoff, yoff);
	}
	void Window::framebufferSizeCallbackStatic(GLFWwindow* win, int w, int h)
	{
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(win));
		glViewport(0, 0, w, h);
		if (self && self->m_resizeCallback)
			self->m_resizeCallback(w, h);
	}
	Window::Window(int width, int height, const String& title)
	{
		// 初始化 GLFW（如果尚未初始化）
		if (!glfwInit()) {
			LOG_INFO("Failed to initialize GLFW");
			return;
		}

		// 设置 OpenGL 版本和核心模式
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);

		m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (!m_window) {
			LOG_INFO("Failed to create GLFW window");
			glfwTerminate();
			return;
		}

		// 将当前对象指针存到 GLFW 窗口用户数据中，便于在静态回调函数里获取 this
		glfwSetWindowUserPointer(m_window, this);

		// 设置各种回调（使用静态函数 + 通过用户指针派发）
		glfwSetKeyCallback(m_window, keyCallbackStatic);
		glfwSetCursorPosCallback(m_window, cursorPosCallbackStatic);
		glfwSetScrollCallback(m_window, scrollCallbackStatic);
		glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallbackStatic);

		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(1); // 开启垂直同步

		LOG_INFO("Window created: " + String::toString(width) + "x" + String::toString(height));
	}
	Window::~Window()
	{
		if (m_window) {
			glfwDestroyWindow(m_window);
			m_window = nullptr;
		}
		glfwTerminate();
	}
	Window::Window(Window&& other) noexcept
		:m_window(other.m_window)
	{
		other.m_window = nullptr;
		//移动后重设用户指针
		if (m_window)
		{
			glfwSetWindowUserPointer(m_window, this);
		}
	}
	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(m_window);
	}
	void Window::setShouldClose(bool value)
	{
		glfwSetWindowShouldClose(m_window, value);
	}
	void Window::swapBuffers()
	{
		glfwSwapBuffers(m_window);
	}
	void Window::pollEvents()
	{
		glfwPollEvents();
	}
	int Window::getWidth() const
	{
		int w, h;
		glfwGetFramebufferSize(m_window, &w, &h);
		return w;
	}
	int Window::getHeight() const
	{
		int w, h;
		glfwGetFramebufferSize(m_window, &w, &h);
		return h;
	}
	float Window::getAspectRatio() const
	{
		int w, h;
		glfwGetFramebufferSize(m_window, &w, &h);
		return TOF(w) / TOF(h);
	}
	GLFWwindow* Window::getNativeWindow() const
	{
		return m_window;
	}
}