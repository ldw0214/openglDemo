#include "Graphics/VertexArray.h"
using namespace MiniEngine;

MiniEngine::VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &m_id);
}

MiniEngine::VertexArray::~VertexArray()
{
	if (m_id)
	{
		glDeleteVertexArrays(1, &m_id);
	}
}

MiniEngine::VertexArray::VertexArray(VertexArray&& other) noexcept
	:m_id(other.m_id)
{
	other.m_id = 0;
}

VertexArray& MiniEngine::VertexArray::operator=(VertexArray&& other) noexcept
{
	if (this != &other)
	{
		if (m_id) {
			glDeleteVertexArrays(1, &m_id);
		}
		m_id = other.m_id;
		other.m_id = 0;
	}

	return *this;
}

GLuint MiniEngine::VertexArray::id() const
{
	return m_id;
}
