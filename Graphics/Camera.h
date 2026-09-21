#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
struct GLFWwindow;

namespace MiniEngine {

	
	/*
	*FPS风格相机控制器
	*键盘wasd移动
	*鼠标控制欧拉角
	*滚轮控制fov
	*/
	class Camera
	{
	public:
		Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,3.0f),
				glm::vec3 worldUp = glm::vec3(0.0f,1.0f,0.0f),
				float yaw = -90.0f,float pitch = 0.0f);

		// 处理输入
		void processKeyBoard(GLFWwindow* window, float deltaTime);	//键盘输入
		void processMouseMovement(double xpos, double ypos);		//鼠标移动
		void processMouseScroll(double yoffset);					//鼠标滚轮

		// 重力模拟，提供时间和地面信息。
		void applyGravity(float deltaTime, float groundY);
		// 跳跃
		void jump(float targetHeight = 2.0f);


		float getVerticalVelocity() const { return m_verticalVelocity; }
		bool isOnGround() const { return m_onGround; }
		float getAirTime() const { return m_airTime; }          // 连续滞空时间（上升+下落）
		float getFallDistance() const { return m_fallDistance; }// 仅下落阶段累计的距离

		//矩阵获取
		glm::mat4 getViewMatrix() const
		{
			return glm::lookAt(m_position, m_position + m_front, m_up);
		}

		glm::mat4 getProjectionMatrix(float aspectRatio) const
		{
			return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
		}

		//属性获取
		glm::vec3 getPosition() const;
		glm::vec3 getFront() const;

	private:

		//更新相机视图
		void updateCameraVectors();

		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldUp;

		float m_yaw;
		float m_pitch;
		float m_movementSpeed = 2.5f;
		float m_mouseSensitivity = 0.05f;
		float m_fov = 45.0f;
		
		//鼠标初始状态
		bool m_firstMouse = true;
		double m_lastX = 0.0, m_lastY = 0.0;

		// ========== 速度驱动的垂直运动 ==========
		float m_verticalVelocity = 0.0f;   // 正=向上，负=向下
		bool m_onGround = true;            // 是否站在地面上

		float m_airTime = 0.0f;            // 离地后的总时间（无论上下）
		float m_fallDistance = 0.0f;       // 下落阶段累计的向下移动距离（速度负时累加）
		float m_groundY = 0.0f;            // 最近一次已知的地面高度（内部辅助）

		// 重力常量
		static constexpr float GRAVITY = -9.8f;          // 重力加速度 (m/s²)
		static constexpr float MAX_SPEED = 30.0f;        // 最大速率（向上或向下）绝对值
	};


} // namesepce MiniEngine
