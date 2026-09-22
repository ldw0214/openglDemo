#pragma once
//#include<string>
//#include<fstream>
//#include<sstream>
#include<filesystem>
#include<glad/glad.h>
#include<glm.hpp>
#include"Core/Logger.h"
#include"Core/String.h"

namespace MiniEngine {
#define SHARDER_FILE_PATH(filename) "assets/shaders/" filename
	/*
	*着色器程序封装、支持热重载
	*文件读取GLSL源码，修改自动重编
	*/

	extern const char* const errorVertSrc ;
	extern const char* const errorFragSrc ;

	class Shader
	{
	public:
		Shader(const String& verPath, const String& fragPath)
			:m_vertPath(verPath), m_fragPath(fragPath)
		{
			reload();
		};
		Shader() :m_vertPath(""), m_fragPath("") {};

		~Shader()
		{
			release();
		};

		void bind() const {
			glUseProgram(m_programID);
		}

		// Uniform 设置
		void setInt(const String& name, int value) const
		{
			glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
		};
		void setMat4(const String& name, const glm::mat4& mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void setVec3(const String& name, const glm::vec3& vec) const
		{
			glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, &vec[0]);
		}

		//着色器更新检查，主循环控制调用频率（release 版本主循环控制关闭）
		void checkAndReload()
		{
			auto ftimeV = std::filesystem::last_write_time(m_vertPath.c_str());
			auto ftimeF = std::filesystem::last_write_time(m_fragPath.c_str());
			if (ftimeV > m_lastWriteTime || ftimeF > m_lastWriteTime) {
				LOG_INFO("Reloading shader: " + m_vertPath + "," + m_fragPath);
				release();
				reload();
				m_lastWriteTime = std::max(ftimeV, ftimeF);
			}
		}
		bool createFromSource(const String& vertSrc, const String& fragSrc) {
			release(); // 释放之前的程序
			String vss = vertSrc;
			String fss = fragSrc;
			GLuint vs = compileShader(GL_VERTEX_SHADER, vss);
			GLuint fs = compileShader(GL_FRAGMENT_SHADER, fss);
			if (!vs || !fs) return false;
			linkProgram(vs, fs);
			glDeleteShader(vs);
			glDeleteShader(fs);
			return m_programID != 0;
		}


	private:
		//文件读取
		String readFile(const String& path)
		{
			std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
			if (!file.is_open())
			{
				LOG_INFO("Failed to open shader file: " + path);
				throw std::runtime_error(("Failed to open shader file: " + path).c_str());
			}
			return file;
		}
		//编译单个着色器
		GLuint compileShader(GLenum type, const String& source)
		{
			GLuint shader = glCreateShader(type);
			const char* src = m_errorState ? (type == GL_VERTEX_SHADER ? errorVertSrc : errorFragSrc) : source.c_str();
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);
			try
			{
				checkProgram(shader, 'c');
			}
			catch (const std::exception& e)
			{
				GLuint shader = glCreateShader(type);
				const char* srce = (type == GL_VERTEX_SHADER ? errorVertSrc : errorFragSrc);
				glShaderSource(shader, 1, &srce, nullptr);

				glCompileShader(shader);
			}
			

			return shader;
		}
		//链接程序
		void linkProgram(GLuint vert, GLuint frag)
		{
			m_programID = glCreateProgram();
			glAttachShader(m_programID, vert);
			glAttachShader(m_programID, frag);
			glLinkProgram(m_programID);
			try
			{
				checkProgram(m_programID, 'l');
			}
			catch (const std::exception& e)
			{
				compileShader(GL_VERTEX_SHADER, errorVertSrc);
				compileShader(GL_FRAGMENT_SHADER, errorFragSrc);
				linkProgram(vert, frag);
			}
			

		}
		//完整加载流程 文件io-编译-链接
		void reload()
		{
			String vertSrc = readFile(m_vertPath);
			String fragSrc = readFile(m_fragPath);

			GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertSrc);
			GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragSrc);

			linkProgram(vertShader, fragShader);

			glDeleteShader(vertShader);
			glDeleteShader(fragShader);

			m_lastWriteTime = std::max(
				std::filesystem::last_write_time(m_vertPath.c_str()),
				std::filesystem::last_write_time(m_fragPath.c_str()));
		}
		//释放当前程序
		void release() {
			if (m_programID) {
				glDeleteProgram(m_programID);
				m_programID = 0;
			}
			m_errorState = false;
		}
		//检查编译结果 type 'c' 编译查询，'l'连接查询
		void checkProgram(GLuint id, char type)
		{
			GLint success;
			String errhead;
			switch (type)
			{
			case 'c':
				glGetShaderiv(id, GL_COMPILE_STATUS, &success);
				break;
			case 'l':
				glGetProgramiv(id, GL_LINK_STATUS, &success);
				break;
			default:
				success = 0;
				break;
			}

			if (!success)
			{
				char infoLog[512];
				switch (type)
				{
				case 'c':
					errhead = "Shader compilation error: ";
					glGetShaderInfoLog(id, sizeof(infoLog), nullptr, infoLog);
					glDeleteShader(id);
					break;
				case 'l':
					errhead = "Shader linking error: ";
					glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog);
					glDeleteProgram(id);
					m_programID = 0;
					break;
				default:
					errhead = "Shader check mode error: check key not have -";
					infoLog[0] = type;
					infoLog[1] = '\0';
					break;
				}

				LOG_INFO((errhead + infoLog).c_str());
				throw std::runtime_error((errhead + infoLog).c_str());
				m_errorState = true;

			}
		}

		bool m_errorState = false;
		GLuint m_programID = 0;
		String m_vertPath, m_fragPath;
		std::filesystem::file_time_type m_lastWriteTime;
	};
	using SetShader = void* (*)(Shader *);
} // namespace MiniEngine