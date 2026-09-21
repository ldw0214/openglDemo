#pragma once
#include <glad/glad.h>

namespace MiniEngine {
	/*
	*VBOµÄRAII·â×°£¬½ûÖ¹¿½±´ºÍÉ¾³ý
	*/

	class Buffer
	{
	public:
		Buffer();
		Buffer(const void* data, size_t size);
		~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		Buffer(Buffer&& other) noexcept;
		Buffer& operator=(Buffer&& other) noexcept;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void setData(const void* data, size_t size) = 0;
		

	protected:
		GLuint m_id = 0;
	};

	
	class VertexBuffer :public Buffer
	{
	public:
		using Buffer::Buffer;
		void bind() const override;
		void unbind() const override;
		void setData(const void* data, size_t size)  override;
	private:
		using Buffer::m_id;
	};

	class IndexBuffer :public Buffer
	{
	public:
		using Buffer::Buffer;
		void bind() const override;
		void unbind() const override;
		void setData(const void* data, size_t size)  override;
	private:
		using Buffer::m_id;
	};



} // namespace MiniEngin