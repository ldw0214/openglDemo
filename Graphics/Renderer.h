#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include "Graphics/Camera.h"
#include "Core/Logger.h"

namespace MiniEngine {
	/*
	*正向渲染器：清屏、设置全屏渲染状态，提交绘制命令
	*当前完成设置投影矩阵、视图矩阵
	*/
	class Renderer
	{
	public:
		Renderer();
		void beginScene(const Camera& camera);//帧开始调用，清理画布，更新相机
		void endScene();//每帧结束时调用，TODO

		glm::mat4 getProjectionMatrix(float aspectRatio) const// 获取当前帧投影矩阵，外部提供宽高比
		{
			if (m_camera)
			{
				return m_camera->getProjectionMatrix(aspectRatio);
			}
			return glm::mat4(1.0f);
		}
		glm::mat4 getViewMatrix() const//获取当前帧视图矩阵
		{
			if (m_camera)
			{
				return m_camera->getViewMatrix();
			}
			return glm::mat4(1.0f);
		}

	private:
		const Camera* m_camera = nullptr;
	};



}// namespace MiniEngine