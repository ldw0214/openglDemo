#pragma once
#include<cstring>
#include<utility>
#include<filesystem>

namespace MiniEngine{
	/**
	* 自建字符串类，演示拷贝移动、cas。
	*/


	class String
	{
	public:
		// 成员访问
		const char* c_str() const
		{
			return m_data ? m_data : "";
		}
		size_t size() const {
			return m_size;
		}
		bool empty() const;
		
		//cas赋值
		String& operator=(String other) noexcept
		{
			swap(*this, other);
			return *this;
		}

		//交换
		friend void swap(String& a, String& b) noexcept;
		friend String operator+(const String& lhs, const String& rhs);
		// 普通构造
		String() = default;
		String(const char* str)
		{
			if (str) {
				m_size = std::strlen(str);
				m_data = new char[m_size + 1];
				strcpy_s(m_data, m_size + 1, str);
			}
		}
		
		//拷贝构造
		String(const String& other) :String(other.c_str()) {};

		//移动构造
		String(String&& other) noexcept : m_data(other.m_data), m_size(other.m_size)
		{
			other.m_data = nullptr;
			other.m_size = 0;
		}

		//文件流构造,减少string容器的文件流拷贝流程
		String(std::ifstream& file);
		
		//析构
		~String()
		{
			delete[] m_data;
		}

		template<typename T>
		static String toString(T targ) {

			static_assert(std::is_arithmetic_v<T>, "myFunction only accepts numeric types");

			String singl;
			long double arg = targ;
			if (arg < 0)
			{
				singl = "-";
				arg = -arg;
			}
			long left = arg;
			double right = (arg - left)*10;
			String ls, rs;
			char num[2] = "0";
			while (left > 0)
			{
				num[0] = '0' + left % 10;
				ls = num + ls;
				left /= 10;
			}

			while (right > 0.0001)
			{
				num[0] = '0' + (char)right;
				right -= (char)right;
				if ((int)(right*1000) == 999)
				{
					num[0]++;
					rs = rs + num;
					break;
				}
				rs = rs + num;
				right *= 10;
			}
			return singl+ls +"."+rs;
		}

	private:
		char* m_data = nullptr;
		size_t m_size = 0;

		//服务加法重载
		String add(const String& other) const
		{
			String temp;
			temp.m_size = this->size() + other.size();
			temp.m_data = new char[temp.m_size + 1];
			strcpy_s(temp.m_data, temp.size() + 1, this->c_str());
			strcpy_s(temp.m_data + this->size(), temp.size() - this->size() + 1, other.c_str());
			return temp;
		}

	};

	void static swap(String& a, String& b) noexcept
	{
		using std::swap;
		swap(a.m_data, b.m_data);
		swap(a.m_size, b.m_size);
	}
	inline String MiniEngine::operator+(const String& lhs, const String& rhs)
	{
		return lhs.add(rhs);
	}

	//string兼容宏
	#define STR_TO_MYSTR(str) String(str.c_str()) 

}	// namespace MiniEngine