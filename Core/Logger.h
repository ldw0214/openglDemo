#pragma once
#include "String.h"
#include "Threading/SPSCQueue.h"
#include <thread>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <atomic>

namespace MiniEngine{
	/*
	*单生产消费者异步日志系统。
	*Logger::instance().Log()保证线程安全的日志写入。
	*内部通过无锁队列传递数据。
	*/
	class Logger
	{
	public:
		// 单例，懒汉，c++11特性保证安全
		static Logger& instance()
		{
			static Logger logger;
			return logger;
		};
		
		//记录一条日志（附带时间）
		void log(const String& message)
		{
			// 消息格式化处理
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			//std::tm tm = *std::localtime(&time);
			std::tm tm{};
			localtime_s(&tm, &time);

			char timeBuffer[32];
			std::strftime(timeBuffer, sizeof(timeBuffer), "[%H:%M:%S]", &tm);

			String fullMsg = timeBuffer + message + String("\n");
			//无锁队列日志填充（队列满会丢失）
			m_queue.push(fullMsg);
		}

	private:
		//禁止移动和拷贝
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		Logger()
			:m_file("engine.log", std::ios::out | std::ios::app)
		{
			if (!m_file.is_open()) {
				throw;
			}
			m_worker = std::thread(&Logger::processLoop, this);
		}

		~Logger()
		{
			m_running.store(false, std::memory_order_relaxed);
			// 析构前等待写入完成
			if (m_worker.joinable())
				m_worker.join();
		}

		// 后台线程，处理io
		void processLoop()
		{
			String msg;
			while (m_running.load(std::memory_order_relaxed) || !m_queue.empty())
			{
				if (m_queue.pop(msg))
				{
					m_file.write(msg.c_str(), msg.size());
				}
				else
				{
					// 无日志让出cpu
					std::this_thread::yield();
				}
			}
		}


		SPSCQueue<String, 1024> m_queue;	// 无锁队列 大小1024（修改保证2^n）
		std::thread m_worker;				// 文件io线程
		std::ofstream m_file;				// 文件流
		std::atomic<bool> m_running{ true };// 运行标志
	};

//日志标注宏
#define LOG_INFO(msg) MiniEngine::Logger::instance().log(msg)
}
