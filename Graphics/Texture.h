#pragma once
#include <vector>
#include <future>
#include <chrono>
#include <glad/glad.h>
#include "Core/Logger.h"
#include "Core/String.h"

namespace MiniEngine {
#define TEXTURES_FILE_PATH(filename) "assets/textures/" filename
	/*
	*纹理封装，异步加载纹理数据。防止大纹理卡顿主线程
	*/
	class Texture
	{
	public:
		struct ImageData
		{
			int width = 0, height = 0, channels = 0;
			std::vector<unsigned char> pixels;

			ImageData() = default;
			ImageData(int w, int h, int c, unsigned char* data);
		};
		
		//异步加载，tryUpload检查
		void loadAsync(const String& path);
		
		//每帧调用，如果加载完成，需要更新纹理
		bool tryUpload();

		void bind(unsigned unit = 0) const;

		static Texture CreateCheckerboard(int size = 512, int squares = 8);

	private:
		GLuint m_id = 0;
		std::future<ImageData> m_future;
		bool m_loaded = false;
	};


}// namespace MiniEngine
