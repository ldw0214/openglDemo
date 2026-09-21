#pragma once
#include<array>
#include<atomic>
#include<cstddef>


namespace MiniEngine {
	/*
	*单生产消费者无锁定长环形队列
	*日志、渲染命令高频，避免锁竞争
	*要求Capacity是2的幂
	*/
	template<typename T, size_t Capacity>
	class SPSCQueue
	{
		static_assert((Capacity & (Capacity-1)) == 0,"Capacity must be a power of 2");
		std::array<T, Capacity> m_buffer;				//环形缓冲区
		alignas(64) std::atomic<size_t> m_head{ 0 };	//生产者索引
		alignas(64) std::atomic<size_t> m_tail{ 0 };	//消费者索引
	public:
		// 生产者填充队列，返回：成功true、满队false
		bool push(const T& itme);
		// 消费者从队列取出，返回：成功true、队空flase
		bool pop(T& item);
		// 判断空队
		bool empty() const;

	};

	template<typename T, size_t Capacity>
	bool MiniEngine::SPSCQueue<T, Capacity>::empty() const
	{
		return m_head.load(std::memory_order_acquire) == m_tail.load(std::memory_order_acquire);
	}

	template<typename T, size_t Capacity>
	bool MiniEngine::SPSCQueue<T, Capacity>::pop(T& item)
	{
		const size_t tail = m_tail.load(std::memory_order_relaxed);

		//如果头尾相接，队空
		if (tail == m_head.load(std::memory_order_acquire))
		{
			return false;
		}

		item = m_buffer[tail];

		//发布读取，释放同步生产者
		m_tail.store((tail + 1) & (Capacity - 1), std::memory_order_release);
		return true;
	}
	template<typename T, size_t Capacity>
	inline bool MiniEngine::SPSCQueue<T, Capacity>::push(const T& item)
	{
		const size_t head = m_head.load(std::memory_order_relaxed);
		const size_t next = (head + 1) & (Capacity - 1);

		// 如果下一个位置位于尾部，则队列满
		if (next == m_tail.load(std::memory_order_acquire))
		{
			return false;
		}
		m_buffer[head] = item;
		//发布写入，确保通知消费者
		m_head.store(next, std::memory_order_release);
		return true;
	}

}	// namespace MiniEngine