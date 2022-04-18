#pragma once
#include "Common.h"

#include <queue>

#define MAX_BLOCK_SIZE			2076	/* ��ڵ� block �Ĵ�С���ֵ */
#define BLOCK_TYPE_COUNT		17		/* ���������ڵ���������� */
#define CHUNK_ARRAY_INCREMENT	128		/* ��ռ����� */

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

		/* ��ǰ��ʹ�õĿ�ռ�ڵ����� */
		int nChunkCount;

		/* ��ǰ������Ŀ�ռ�ڵ����� */
		int nChunkSpace;

		/* δ��ʹ�õ��ڴ�������������飬�������䲻ͬ���������ͷָ�� */
		Block* pFreeLists[BLOCK_TYPE_COUNT];

		static int blockTypeSizeTable[BLOCK_TYPE_COUNT];
		static int chunkSizeTable[BLOCK_TYPE_COUNT];

		/* ����Ҫ����� block �Ĵ�С���������Ӧ block ���͵��������� */
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

		/* ջԪ��ʵ�� */
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
		/* �ڴ�� */
		char data[stackSize];

		/* �Ѿ�ʹ�õ��ڴ��С */
		int nIndex;

		/* ջԪ��ʵ������ */
		StackEntry stackEntries[maxStackEntries];

		/* ջԪ�ص����� */
		int nStackEntryCount;
	};
}