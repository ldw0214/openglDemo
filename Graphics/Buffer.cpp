#include "Buffer.h"

namespace MiniEngine {
	Buffer::Buffer()
	{
		glCreateBuffers(1, &m_id);
	}

	Buffer::Buffer(const void* data, size_t size)
		:Buffer()
	{
		glNamedBufferData(m_id, size, data, GL_STATIC_DRAW);
	}

	Buffer::~Buffer()
	{
		if (m_id)
		{
			glDeleteBuffers(1, &m_id);
		}
	}

	Buffer::Buffer(Buffer&& other) noexcept
		:m_id(other.m_id)
	{
		other.m_id = 0;
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (this != &other)
		{
			if (m_id)
				glDeleteBuffers(1, &m_id);
			m_id = other.m_id;
			other.m_id = 0;
		}
		return *this;
	}

	void VertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void VertexBuffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::setData(const void* data, size_t size)
	{
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	void IndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void IndexBuffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IndexBuffer::setData(const void* data, size_t size)
	{
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

}


