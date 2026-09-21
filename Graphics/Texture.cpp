#include "Graphics/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <Graphics/stb_image.h>

namespace MiniEngine
{


	void Texture::loadAsync(const String& path)
	{
		m_future = std::async(std::launch::async, [path]()->ImageData {
			int w, h, c;
			unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);
			if (!data)
				throw std::runtime_error(("Failed to load texture: " + path + "," + stbi_failure_reason()).c_str());
			return MiniEngine::Texture::ImageData(w, h, c, data);
			});
	}
	bool Texture::tryUpload()
	{
		if (!m_future.valid()) return false;
		if (m_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
			return false;

		ImageData img;
		try
		{
			img = m_future.get();
		}
		catch (const std::exception& e)
		{
			String eout = e.what();
			LOG_INFO(eout);
		}
		

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);

		GLenum format = img.channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, 
			img.width, img.height, 0, format, 
			GL_UNSIGNED_BYTE, img.pixels.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		m_loaded = true;
		LOG_INFO("Texture uploaded successfully");
		return true;
	}
	void Texture::bind(unsigned unit) const
	{
		if (m_loaded) {
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, m_id);
		}
	}
	Texture::ImageData::ImageData(int w, int h, int c, unsigned char* data)
		: width(w), height(h), channels(c), pixels(data, data + w * h * c) {
		stbi_image_free(data);
	}

	Texture Texture::CreateCheckerboard(int size, int squares) {
		Texture tex;
		tex.m_loaded = true;

		std::vector<unsigned char> pixels(size * size * 4);
		for (int y = 0; y < size; ++y) {
			for (int x = 0; x < size; ++x) {
				bool white = ((x / (size / squares)) + (y / (size / squares))) % 2 == 0;
				unsigned char c = white ? 255 : 128;
				int idx = (y * size + x) * 4;
				pixels[idx + 0] = c;
				pixels[idx + 1] = c;
				pixels[idx + 2] = c;
				pixels[idx + 3] = 255;
			}
		}

		glGenTextures(1, &tex.m_id);
		glBindTexture(GL_TEXTURE_2D, tex.m_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		return tex;
	}
}
