#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

namespace MiniEngine {
	/*
	*通用线程池，提交任意可调用对象并返回future。
	*使用 packaged_task 封装，支持返回值和无返回值。
	*/
	//invoke_result_t的c++低版本兼容
#if __cplusplus >= 201703L  // C++17 或更高

	// 直接使用标准库的实现
	template<typename F, typename... Args>
	using ReturnType = invoke_result_t<F, Args...>;

#else  // C++14 或更低 使用result_of模拟
	template <typename F, typename... Args>
	using my_invoke_result_t = typename std::result_of<F(Args...)>::type;
	template<typename F, typename... Args>
	using ReturnType = my_invoke_result_t<F, Args...>;
#endif


	class ThreadPool
	{
	public:
		explicit ThreadPool(size_t numThreads);

		~ThreadPool();
	
		// 提交一个任务，返回该任务结果关联的future。auto res = pool.enqueue(func,10);
		template<typename F, typename... Args>
		auto enqueue(F&& f, Args&&... args) -> std::future<ReturnType<F, Args...>>;
		

		//获取当前任务数
		size_t activeCount() const;

	private:
		std::vector<std::thread> m_workers;			//工作线程
		std::queue<std::function<void()>> m_tasks;	//任务队列
		std::mutex m_mutex;							//互斥锁
		std::condition_variable m_cv;				//条件变量
		bool m_stop = false;						//停止标志
		std::atomic<size_t> m_activeCount{ 0 };		//活跃计数

	};
}// namespace MiniEngine