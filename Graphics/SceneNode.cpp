#include "Graphics/SceneNode.h"
namespace MiniEngine {
	void SceneNode::addChild(Ptr child)
	{
		child->m_parent = this;
		m_children.push_back(std::move(child));
	}
	void SceneNode::removeChild(const Ptr& child)
	{
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
		{
			(*it)->m_parent = nullptr;
			m_children.erase(it);
		}
	}
	void SceneNode::setLocalTransform(const glm::mat4& transform)
	{
		m_localTransform = transform;
	}
	glm::mat4 SceneNode::getLocalTransform() const
	{
		return m_localTransform;
	}
	glm::mat4 SceneNode::getWorldTransform() const
	{
		return m_worldTransform;
	}
	void SceneNode::setMesh(std::shared_ptr<Mesh> mesh)
	{
		m_mesh = std::move(mesh);
	}
	void SceneNode::setColor(const glm::vec3& color)
	{
		m_color = color;
	}
	void SceneNode::update(const glm::mat4& parentTransform)
	{
		m_worldTransform = parentTransform * m_localTransform;
		for (auto& child : m_children) {
			child->update(m_worldTransform);
		}
	}
	void SceneNode::render(Renderer& renderer, Shader& shader)
	{
		if (m_mesh)
		{
			shader.bind();
			shader.setMat4("model", m_worldTransform);
			shader.setMat4("view", renderer.getViewMatrix());
			shader.setMat4("projection", renderer.getProjectionMatrix(m_aspectRatio));
			shader.setVec3("objectColor", m_color);
			m_mesh->bind();
			glDrawElements(GL_TRIANGLES,static_cast<GLsizei>(m_mesh->indexCount()),GL_UNSIGNED_INT,0);
			m_mesh->unbind();
		}
		for (auto& children: m_children)
		{
			children->render(renderer, shader);
		}
	}
	void SceneNode::updateAspectRatio(float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
		for (auto& child : m_children) {
			child->updateAspectRatio(m_aspectRatio);
		}
	}
}