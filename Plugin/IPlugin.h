namespace MiniEngine {
	/*
	*插件接口：所有插件必须实现此抽象类
	*/

	class Renderer;
	class Shader;

	class IPlugin
	{
	public:
		virtual ~IPlugin() = default;
		virtual const char* getName() const = 0;
		virtual void onUpdate(float deltaTime) = 0;
		virtual void onRender(Renderer& renderer, Shader& shader) = 0;
		virtual void onUpdateAspectRatio(float aspectRatio) = 0;
		void(*keyCallbacks[37])() = {nullptr};
	};
}