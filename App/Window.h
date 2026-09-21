#pragma once

#include <functional>
#include <GLFW/glfw3.h>
#include "Core/String.h"


namespace MiniEngine {
	/*
	*GLFW窗口的RAII封装
	*负责创建销毁窗口，管理事件回调提供尺寸和关闭状态
	*/
	class Window
	{
	public:
		//回调函数定义
		using KeyCallback = std::function<void(int key, int scancode, int action, int mods)>;
		using MouseCallback = std::function<void(double xpos, double ypos)>;
		using ScrollCallback = std::function<void(double xoffset,double yoffset)>;
		using ResizeCallback = std::function<void(int width, int height)>;

		//构造函数，创建窗口并设置用户指针
		Window(int width, int height, const String& title);

		~Window();

		//禁止拷贝
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		//移动语义
		Window(Window&& other) noexcept;

		//查询状态
		bool shouldClose() const;
		void setShouldClose(bool value);
		
		void swapBuffers();
		void pollEvents();

		int getWidth() const;
		int getHeight() const;

		float getAspectRatio()const;
		
		GLFWwindow* getNativeWindow()const;

		// 事件回调注册

		void setKeyCallback(KeyCallback cb) { m_keyCallback = std::move(cb); }
		void setMouseCallback(MouseCallback cb) { m_mouseCallback = std::move(cb); }
		void setScrollCallback(ScrollCallback cb) { m_scrollCallback = std::move(cb); }
		void setResizeCallback(ResizeCallback cb) { m_resizeCallback = std::move(cb); }

	private:
		// 静态回调，c风格，供glfw使用
		static void keyCallbackStatic(GLFWwindow* win, int key, int scancode, int action, int mods);
		static void cursorPosCallbackStatic(GLFWwindow* win, double x, double y);
		static void scrollCallbackStatic(GLFWwindow* win, double xoff, double yoff);
		static void framebufferSizeCallbackStatic(GLFWwindow* win, int w, int h);

		GLFWwindow* m_window = nullptr;

		//自定义回调
		KeyCallback m_keyCallback;
		MouseCallback m_mouseCallback;
		ScrollCallback m_scrollCallback;
		ResizeCallback m_resizeCallback;
	};



}// namespace MiniEngin
