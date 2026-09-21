#pragma once
#include <glad/glad.h>

namespace MiniEngine {
	/*
	*VAOµÄRAII·â×°£¬½ûÖ¹¿½±´ºÍÉ¾³ý
	*/
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		VertexArray(const VertexArray&) = delete;
		VertexArray& operator=(const VertexArray&) = delete;

		VertexArray(VertexArray&& other) noexcept;

		VertexArray& operator=(VertexArray&& other) noexcept;

		void bind() const
		{
			glBindVertexArray(m_id);
		}

		void unbind() const
		{
			glBindVertexArray(0);
		}
		GLuint id() const;


	private:
		GLuint m_id = 0;
	};



} // namespace MiniEngine
