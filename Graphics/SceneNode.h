#pragma once

#include <memory>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"

#ifdef WINDOWS_WIDTH
#ifdef WINDOWS_HEIGTH
#define ASPECT_RATIO (WINDOWS_WIDTH/WINDOWS_HEIGTH)
#endif
#endif


#ifndef ASPECT_RATIO

#define ASPECT_RATIO (1280.0f/720.0f)

#endif // 未指定屏幕比例，自动使用比例


namespace MiniEngine {
	/*
	*场景图节点：构成树形结构，负责递归更新和渲染
	*每个节点包含一个网格饮用和材质颜色
	*/
	class SceneNode : public std::enable_shared_from_this<SceneNode>
	{
	public:
		using Ptr = std::shared_ptr<SceneNode>;
		SceneNode() = default;

		// 父子关系
		void addChild(Ptr child);
		void removeChild(const Ptr& child);

		// 变换
		void setLocalTransform(const glm::mat4& transform);
		glm::mat4 getLocalTransform() const;
		glm::mat4 getWorldTransform() const;

		// 可渲染对象
		void setMesh(std::shared_ptr<Mesh> mesh);
		void setColor(const glm::vec3& color);

		//递归更新世界变换 （从上到下）
		virtual void update(const glm::mat4& parentTransform);

		//递归渲染：传递渲染器和着色器，绘制自身并递归子节点
		virtual void render(Renderer& renderer, Shader& shader);

		void updateAspectRatio(float aspectRatio);

	private:
		glm::mat4 m_localTransform{ 1.0f };
		glm::mat4 m_worldTransform{ 1.0f };
		std::shared_ptr<Mesh> m_mesh;
		glm::vec3 m_color{ 1.0f,0.9f,0.7f };

		//屏幕比例
		float m_aspectRatio = ASPECT_RATIO;

		SceneNode* m_parent = nullptr;
		std::vector<Ptr> m_children;
	};

} // namespace MiniEngine