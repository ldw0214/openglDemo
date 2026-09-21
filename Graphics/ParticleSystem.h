#pragma once

#include<vector>
#include<random>
#include<glm.hpp>
#include"Graphics/Mesh.h"
#include"Graphics/Shader.h"
#include"Core/MemoryPool.h"
#include"Graphics/VertexArray.h"
#include"Graphics/Buffer.h"

namespace MiniEngine {
	// 单个粒子节点
	struct Particle
	{
		glm::vec3 position;
		glm::vec3 velocity;
		float life = 0.0f;
		float maxLife = 0.0f;
		glm::vec4 color{ 1.0f };
	};

	/*
	*简单粒子系统：生成、更新和渲染粒子。
	*使用MemoryPool提高分配效率。
	*并行更新TODO
	*/

	class ParticleSystem
	{
	public:
		ParticleSystem(size_t maxParticles = 10000);
		
		// 发射一个粒子
		void emit(const glm::vec3& pos, const glm::vec3& dir, float speed, float life, const glm::vec4& color);

		//更新所有粒子状态（每帧处理）
		void update(float dt);

		//渲染粒子（点精灵）
		void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

	private:
		MemoryPool<Particle> m_pool;
		std::vector<Particle*> m_activeParticles;	//活跃粒子，池化生成
	};




}//	namespace MiniEngine