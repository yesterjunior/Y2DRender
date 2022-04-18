#pragma once
#include "Common.h"

#include <queue>

#define MAX_BLOCK_SIZE			2076	/* 块节点 block 的大小最大值 */
#define BLOCK_TYPE_COUNT		17		/* 可以申请块节点的类型数量 */
#define CHUNK_ARRAY_INCREMENT	128		/* 块空间增量 */

#define UNKNOWN_MEMORY			-1

namespace Simple2D
{
	struct Block
	{
		int sizeIndex;
		Block* next;
	};

	struct Chunk
	{
		Block*	blocks;
	};

	struct CustomChunk
	{
		int size;
		char* data;
	};

	class BlockAllocator
	{
		BlockAllocator();
		~BlockAllocator();

	public:
		static BlockAllocator* getInstance();

		template<typename T>
		T* allocateWithType(size_t count = 1)
		{
			return ( T* ) allocate(count * sizeof(T));
		}

		void* allocate(int size);
		void  free(void* ptr);
		void  clear();

	private:
		static BlockAllocator allocator;
		Chunk* pChunks;

		/* 当前已使用的块空间节点总数 */
		int nChunkCount;

		/* 当前已申请的块空间节点总数 */
		int nChunkSpace;

		/* 未被使用的内存块链表类型数组，保存了其不同类型链表的头指针 */
		Block* pFreeLists[BLOCK_TYPE_COUNT];

		static int blockTypeSizeTable[BLOCK_TYPE_COUNT];
		static int chunkSizeTable[BLOCK_TYPE_COUNT];

		/* 根据要申请块 block 的大小索引出其对应 block 类型的索引数组 */
		static char blockTypeSizeLookup[MAX_BLOCK_SIZE + 1];
		static bool blockTypeSizeLookipInitialized;
	};

	//--------------------------------------------------------------------
	// StackAllocator
	//--------------------------------------------------------------------
	class StackAllocator
	{
		static const int stackSize = 500 * 1024;
		static const int maxStackEntries = 32;

		/* 栈元素实体 */
		struct StackEntry
		{
			char* data;
			int   size;
			bool  useMalloc;
		};

	public:
		StackAllocator();
		~StackAllocator();

		void* allocate(size_t size);
		void  free(void* ptr);

	private:
		/* 内存池 */
		char data[stackSize];

		/* 已经使用的内存大小 */
		int nIndex;

		/* 栈元素实体数组 */
		StackEntry stackEntries[maxStackEntries];

		/* 栈元素的数量 */
		int nStackEntryCount;
	};
}