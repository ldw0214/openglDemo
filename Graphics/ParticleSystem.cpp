#include "Graphics/ParticleSystem.h"

namespace MiniEngine {
	ParticleSystem::ParticleSystem(size_t maxParticles)
	{
		// 此处应该进行预分配池化空间，但当前内存池采用自动扩展的形式的懒汉模式，暂不处理。
	}
	void ParticleSystem::emit(const glm::vec3& pos, const glm::vec3& dir, float speed, float life, const glm::vec4& color)
	{
		Particle* p = m_pool.allocate();
		if (!p) return; //池满

		p->position = pos;
		p->velocity = dir * speed;
		p->life = life;
		p->maxLife = life;
		p->color = color;

		m_activeParticles.push_back(p);
	}
	void ParticleSystem::update(float dt)
	{
		for (auto it = m_activeParticles.begin(); it != m_activeParticles.end();)
		{
			Particle* p = *it;
			p->life -= dt;

			if (p->life <= 0.0f)
			{
				m_pool.deallocate(p); //回收内存
				it = m_activeParticles.erase(it);
			}
			else
			{
				p->position += p->velocity * dt;
				++it;
			}
		}
	}
	void ParticleSystem::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection)
	{
		if (m_activeParticles.empty()) return;

		// 构建临时定点数据，每个粒子一个点
		std::vector<float> vertices;
		for(const Particle* p : m_activeParticles)
		{
			vertices.push_back(p->position.x);
			vertices.push_back(p->position.y);
			vertices.push_back(p->position.z);
			vertices.push_back(p->color.r);
			vertices.push_back(p->color.g);
			vertices.push_back(p->color.b);
			vertices.push_back(p->color.a);
			vertices.push_back(p->life / p->maxLife); // 剩余生命比例，可在着色器中调整动效
		}

		static VertexArray vao;
		static VertexBuffer vbo{};
		static bool initialized = false;

		// 第一次执行，初始化vao
		if (!initialized)
		{
			vao.bind();
			vbo.bind();
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(7 * sizeof(float)));
			glEnableVertexAttribArray(2);
			initialized = true;
		}

		vbo.setData(vertices.data(),vertices.size() * sizeof(decltype(vertices)::value_type));
		vao.bind();

		shader.bind();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_activeParticles.size()));

		vao.unbind();
	}
}