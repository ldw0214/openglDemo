#include "Graphics/Renderer.h"
#include <gtc/matrix_transform.hpp>

namespace MiniEngine {
	Renderer::Renderer()
	{
		//开始深度测试和面剔除
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void Renderer::beginScene(const Camera& camera)
	{
		glClearColor(0.1f,0.1f,0.15f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_camera = &camera;
	}

	void Renderer::endScene()
	{
		m_camera = nullptr;
		//TODO
	}

}
