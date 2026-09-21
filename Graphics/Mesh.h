#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm.hpp>
#include "Graphics/VertexArray.h"
#include "Buffer.h"

namespace MiniEngine {

	/*
	*网格数据：封装VAO\VBO\EBO,并提供常用几何体生成
	*使用RAII资源管理
	*/
	class Mesh
	{
	public:
		Mesh(const std::vector<float>& vertices,
			const std::vector<unsigned int>& indices,
			const std::vector<int>& attributeSizes = { 3,3,2 });

		//禁止拷贝，允许移动
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = default;
		Mesh& operator=(Mesh&&) noexcept = default;

		void bind() const
		{
			m_vao.bind();
		}
		void unbind() const
		{
			m_vao.unbind();
		}
		size_t indexCount() const
		{
			return m_indexCount;
		}

		//工厂方法，内置几何体
		static Mesh createCube();

	private:

		void create(const std::vector<float>& vertices,
					const std::vector<unsigned int>& indices,
					const std::vector<int>& attributeSizes);


		VertexArray m_vao;
		size_t m_indexCount = 0;
	};



} // namespace MiniEngine
