#include "Graphics/Camera.h"
#include <GLFW/glfw3.h>

MiniEngine::Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
	:m_position(position), m_worldUp(worldUp), m_yaw(yaw), m_pitch(pitch)
{
	updateCameraVectors();
}

void MiniEngine::Camera::processKeyBoard(GLFWwindow* window, float deltaTime)
{
	float speed = m_movementSpeed * deltaTime;

	// 计算水平前向（去除垂直分量）
	glm::vec3 horizontalFront = glm::normalize(glm::vec3(m_front.x, 0.0f, m_front.z));
	// 右向已经是水平的（因为 m_right 是 m_front 与 worldUp 的叉积，worldUp 是 (0,1,0)）
	glm::vec3 horizontalRight = m_right;  // m_right 本身就平行于地面

	glm::vec3 moveDelta(0.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		moveDelta += horizontalFront * speed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		moveDelta -= horizontalFront * speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		moveDelta -= horizontalRight * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		moveDelta += horizontalRight * speed;

	// 只更新 X 和 Z，Y 保持不变（重力会单独处理）
	m_position.x += moveDelta.x;
	m_position.z += moveDelta.z;

}

void MiniEngine::Camera::processMouseMovement(double xpos, double ypos)
{
	if (m_firstMouse)
	{
		m_lastX = xpos;
		m_lastY = ypos;
		m_firstMouse = false;
		return;
	}

	float xoffset = static_cast<float>(xpos - m_lastX) * m_mouseSensitivity * 0.5;
	float yoffset = static_cast<float>(m_lastY - ypos) * m_mouseSensitivity; // 反转Y轴
	m_lastX = xpos;
	m_lastY = ypos;
	
	m_yaw += xoffset;
	m_pitch += yoffset;

	//限制俯仰角，防止反转死锁
	if (m_pitch > 89.0f) m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;

	updateCameraVectors();
}

void MiniEngine::Camera::processMouseScroll(double yoffset)
{
	m_fov -= static_cast<float>(yoffset);
	if (m_fov < 1.0f) m_fov = 1.0f;
	if (m_fov > 120.0f) m_fov = 120.0f;
}

void MiniEngine::Camera::applyGravity(float deltaTime, float groundY)
{
	m_groundY = groundY;

	// 1. 应用重力：改变速度
	m_verticalVelocity += GRAVITY * deltaTime;

	// 2. 速度限制（绝对值上限）
	if (m_verticalVelocity > MAX_SPEED) m_verticalVelocity = MAX_SPEED;
	if (m_verticalVelocity < -MAX_SPEED) m_verticalVelocity = -MAX_SPEED;

	// 3. 记录旧位置，更新 Y 坐标
	float oldY = m_position.y;
	m_position.y += m_verticalVelocity * deltaTime;

	// 4. 统计滞空时间（只要不在地面就累加）
	if (!m_onGround) {
		m_airTime += deltaTime;
	}

	// 5. 统计下落距离（仅当速度向下且不在刚落地的那一帧）
	if (m_verticalVelocity < 0.0f && !m_onGround) {
		float downwardDelta = oldY - m_position.y; // 正值
		if (downwardDelta > 0.0f) {
			m_fallDistance += downwardDelta;
		}
	}

	// 6. 地面碰撞检测（如果穿透或低于地面）
	if (m_position.y <= m_groundY) {
		m_position.y = m_groundY;
		m_verticalVelocity = 0.0f;
		// 如果之前是离地状态，现在落地了
		if (!m_onGround) {
			m_onGround = true;
			m_airTime = 0.0f;
			m_fallDistance = 0.0f;
		}
	}
	else {
		// 在空中
		m_onGround = false;
	}
}

void MiniEngine::Camera::jump(float targetHeight)
{
	// 只有在地面上才能起跳
	if (!m_onGround) return;

	// 起跳前确保相机 Y 坐标与地面一致（避免浮点误差）
	m_position.y = m_groundY;

	// 计算所需初速度（向上为正）
	float requiredSpeed = std::sqrt(3.0f * std::abs(GRAVITY) * targetHeight);
	m_verticalVelocity = requiredSpeed;

	// 离开地面，重置统计
	m_onGround = false;
	m_airTime = 0.0f;
	m_fallDistance = 0.0f;
}

glm::vec3 MiniEngine::Camera::getPosition() const
{
	return m_position;
}

glm::vec3 MiniEngine::Camera::getFront() const
{
	return m_front;
}

void MiniEngine::Camera::updateCameraVectors()
{
	//欧拉角计算方向
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);

	//重新计算右轴和上轴
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
