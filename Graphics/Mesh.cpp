#include "Graphics/Mesh.h"

namespace MiniEngine {
	Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<int>& attributeSizes)
	{
		create(vertices, indices, attributeSizes);
	}
	
	
	Mesh Mesh::createCube()
	{
		// 顶点布局：位置(3) + 法线(3) + 纹理坐标(2)
		std::vector<float> vertices = {
			// 前面
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
			// 后面
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
			// 上面
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			// 下面
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
			// 右面
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			// 左面
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f
		};

		std::vector<unsigned int> indices = {
			0, 1, 2, 2, 3, 0,       // 前
			4, 5, 6, 6, 7, 4,       // 后
			8, 9, 10, 10, 11, 8,    // 上
			12, 13, 14, 14, 15, 12, // 下
			16, 17, 18, 18, 19, 16, // 右
			20, 21, 22, 22, 23, 20  // 左
		};

		// 顶点属性尺寸：位置3，法线3，纹理坐标2
		return Mesh(vertices, indices, {3,3,2});
	}
	void Mesh::create(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<int>& attributeSizes)
	{
		m_indexCount = indices.size();

		//创建缓冲区并填充数据
		VertexBuffer vbo(vertices.data(), vertices.size() * sizeof(float));
		IndexBuffer ebo(indices.data(), indices.size() * sizeof(unsigned int));

		m_vao.bind();
		vbo.bind();
		ebo.bind();

		// 设置顶点属性指针
		size_t stride = 0;
		for (int size : attributeSizes) stride += size * sizeof(float);

		size_t offset = 0;
		for (size_t i = 0; i < attributeSizes.size(); i++)
		{
			glEnableVertexAttribArray(static_cast<GLuint>(i));
			glVertexAttribPointer(static_cast<GLuint>(i),
				attributeSizes[i],
				GL_FLOAT,
				GL_FALSE,
				static_cast<GLsizei>(stride),
				reinterpret_cast<void*>(offset));
			offset += attributeSizes[i] * sizeof(float);
		}

		m_vao.unbind();
	}
}