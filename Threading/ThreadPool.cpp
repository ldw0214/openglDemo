#include"ThreadPool.h"
using namespace MiniEngine;

MiniEngine::ThreadPool::ThreadPool(size_t numThreads)
{
	// 创建指定数量的工作线程
	for (size_t i = 0; i < numThreads; i++)
	{
		m_workers.emplace_back([this] {
			while (true)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					//等待任务或者线程关闭
					m_cv.wait(lock, [this] {
						return m_stop || !m_tasks.empty();
						});
					//如果标志触发且任务队列空，退出线程。
					if (m_stop && m_tasks.empty())return;

					task = std::move(m_tasks.front());
				}
				m_activeCount.fetch_add(1, std::memory_order_relaxed);
				task();
				m_activeCount.fetch_sub(1, std::memory_order_relaxed);
			}
			});
	}
}

MiniEngine::ThreadPool::~ThreadPool()
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_stop = true;
	}
	m_cv.notify_all();			//唤醒所有线程
	for(auto& worker : m_workers)
	{
		worker.join();			//等待线程销毁
	}
}

size_t MiniEngine::ThreadPool::activeCount() const
{
	return m_activeCount.load(std::memory_order_relaxed);
}

template<typename F, typename... Args>
auto MiniEngine::ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<ReturnType<F, Args...>>
{
	// 使用packaged_task 包装对象为future任务
	auto task = std::make_shared<std::packaged_task<ReturnType()>>(
		std::bind(std::forward<F>(f),std::forward<Args>(args)...)
	);

	std::future<ReturnType> result = task->get_future();
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_stop)
		{
			throw std::runtime_error("enqueue on stopped ThreadPool");
		}
		//进一步包装为function<void()>装入队列
		m_tasks.emplace([task]() {(*task)(); });
	}
	m_cv.notify_one();
	return result;
}