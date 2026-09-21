#pragma once
#include <vector>
#include <cstddef>
#include <new>

namespace MiniEngine {
	/*
	固定大小内存池，频繁创建销毁小对象使用
	union结构，减少开销
	协约：禁止使用非池内对象析构，会导致池外内存污染池内存
	*/
template<typename T,size_t BlockSize = 256>
class MemoryPool
{
	//节点定义，对象节点形式存活，链表节点形式回收
	union Node
	{
		T data;
		Node* next;
	};
public:
	MemoryPool() { allocateBlock(); }

	T* allocate();

	void deallocate(T* ptr);


private:
	void allocateBlock();// 内存申请、内存块构造


	std::vector<std::vector<char>> m_blocks; //内存块生命周期保证
	Node* m_freeList = nullptr;				//空闲节点
};
template<typename T, size_t BlockSize>
void MiniEngine::MemoryPool<T, BlockSize>::allocateBlock()
{
	m_blocks.emplace_back(BlockSize * sizeof(Node));
	Node* blockStart = reinterpret_cast<Node*>(m_blocks.back().data());
	for (size_t i = 0; i < BlockSize; i++)
	{
		blockStart[i].next = &blockStart[i + 1];
	}blockStart[BlockSize - 1].next = nullptr;
	m_freeList = blockStart;
}

template<typename T, size_t BlockSize>
void MiniEngine::MemoryPool<T, BlockSize>::deallocate(T* ptr)
{
	ptr->~T();
	Node* node = reinterpret_cast<Node*>(ptr);
	node->next = m_freeList;
	m_freeList = node;
}

template<typename T, size_t BlockSize>
inline T* MiniEngine::MemoryPool<T, BlockSize>::allocate()
{
	if (!m_freeList)
	{
		allocateBlock();
	}
	Node* node = m_freeList;
	m_freeList = m_freeList->next;
	return new(&node->data)T();
}

} // namespace MiniEngine