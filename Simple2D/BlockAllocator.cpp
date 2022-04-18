#include "BlockAllocator.h"

#include <cstdarg>
#include <Windows.h>
#include <iostream>

namespace Simple2D
{
	int BlockAllocator::blockTypeSizeTable[BLOCK_TYPE_COUNT] = {
		12,		// 2 个索引： 4  * 2 = 8   + 4 = 12
		16,		// 3 个索引： 4  * 3 = 12  + 4 = 16
		28,		// 6 个索引： 4  * 6 = 24  + 4 = 28
		36,		// 8 个索引： 4  * 8 = 32  + 4 = 36
		60,		// 1 条线段： 28 * 2 = 56  + 4 = 60
		76,		// 18个索引： 4 * 18 = 72  + 4 = 76
		88,		// 1 三角形： 28 * 4 = 84  + 4 = 88
		116,	// 1 个矩形： 28 * 4 = 112 + 4 = 116
		148,	// 1 张纹理： 36 * 4 = 144 + 4 = 148
		196,	// 48个索引： 4 * 48 = 192 + 4 = 196
		260,	// 9 个顶点： 28 * 9 = 256 + 4 = 260
		292,	// 72个索引： 4 * 72 = 288 + 4 = 292
		436,	// 108 索引： 4 * 108 = 432 + 4 = 436
		676,	// 24个顶点： 28 * 24 = 672 + 4 = 676
		868,	// 216 顶点： 4 * 216 = 864 + 4 = 868
		1068,	// 38个顶点： 28 * 37 = 1064 + 4 = 1068
		2076	// 74个顶点： 28 * 74 = 2072 + 4 = 2076
	};

	int BlockAllocator::chunkSizeTable[BLOCK_TYPE_COUNT] = {
		12 * 1024,
		16 * 1024,
		28 * 585,
		36 * 455,
		60 * 273,
		76 * 215,
		88 * 186,
		116 * 141,
		148 * 110,
		196 * 83,
		260 * 63,
		292 * 56,
		436 * 37,
		676 * 24,
		868 * 18,
		1068 * 15,
		2076 * 7
	};

	char BlockAllocator::blockTypeSizeLookup[MAX_BLOCK_SIZE + 1] = { 0 };
	bool BlockAllocator::blockTypeSizeLookipInitialized = false;


	BlockAllocator BlockAllocator::allocator;
	BlockAllocator* pAllocator = nullptr;

	BlockAllocator* BlockAllocator::getInstance()
	{
		return &allocator;
	}

	BlockAllocator::BlockAllocator()
	{
		pAllocator = this;

		nChunkSpace = CHUNK_ARRAY_INCREMENT;
		nChunkCount = 0;
		pChunks = ( Chunk* ) malloc(nChunkSpace * sizeof(Chunk));

		/* 初始化内存 */
		memset(pChunks, 0, nChunkSpace * sizeof(Chunk));
		memset(pFreeLists, 0, sizeof(pFreeLists));

		/* 设置索引数组 blockTypeSizeLookup 的 block 类型索引值 */
		if ( blockTypeSizeLookipInitialized == false ) {
			blockTypeSizeLookipInitialized = true;

			int blockTypeSizeIndex = 0;
			for ( int i = 0; i <= MAX_BLOCK_SIZE; i++ ) {
				if ( i <= blockTypeSizeTable[blockTypeSizeIndex] ) {
					blockTypeSizeLookup[i] = blockTypeSizeIndex;
				}
				else {
					blockTypeSizeIndex++;
					blockTypeSizeLookup[i] = blockTypeSizeIndex;
				}
			}
		}
	}

	BlockAllocator::~BlockAllocator()
	{
		/* 释放所有申请的形成 block 链表的块内存 */
		for ( int i = 0; i < nChunkCount; i++ ) {
			::free(pChunks[i].blocks);
		}
		::free(pChunks);
	}

	void* BlockAllocator::allocate(int size)
	{
		if ( size == 0 ) return 0;
		assert(size > 0);

		/* 使用四个字节记录 block 的类型索引，free 是使用 */
		size += sizeof(int);

		/* 申请的空间大于规定的最大值，直接申请，不放到块的链表中去 */
		if ( size > MAX_BLOCK_SIZE ) {
			int* data = ( int* ) malloc(size);
			/* -1 表示这是直接分配的内存 */
			data[0] = UNKNOWN_MEMORY;		
			return (data + 1);
		}

		int index = blockTypeSizeLookup[size];
		assert(0 <= index && index < BLOCK_TYPE_COUNT);

		/* 存在同类型的未被使用的内存块？返回内存块 */
		if ( pFreeLists[index] ) {
			/* 使块头指针指向新的未被使用的 block */
			Block* block = pFreeLists[index];
			pFreeLists[index] = block->next;
			return (( int* ) block + 1);
		}
		else {
			/* 扩展 chunk 数组 */
			if ( nChunkCount == nChunkSpace ) {
				Chunk* oldChunks = pChunks;
				nChunkSpace += CHUNK_ARRAY_INCREMENT;
				pChunks = ( Chunk* ) malloc(nChunkSpace * sizeof(Chunk));
				memcpy(pChunks, oldChunks, nChunkCount * sizeof(Chunk));
				memset(pChunks + nChunkCount, 0, CHUNK_ARRAY_INCREMENT * sizeof(Chunk));
				::free(oldChunks);
			}
			int chunkSize = chunkSizeTable[index];
			/* 获取一个未被使用的可以用来分配内存的 chunk */
			Chunk* chunk = pChunks + nChunkCount;
			chunk->blocks = ( Block* ) malloc(chunkSize);

			/* 获取当前申请的 block 类型大小 */
			int blockSize = blockTypeSizeTable[index];

			/* 计算一块 chunk 内存能够分割成 block 的数量 */
			int blockCount = chunkSize / blockSize;
			assert(blockCount * blockSize <= chunkSize);

			/* 将 chunk 分割出许多 block，再将 block 以链表的形式串起来 */
			for ( int i = 0; i < blockCount - 1; i++ ) {
				Block* block = ( Block* ) (( char* ) chunk->blocks + blockSize * i);
				Block* next  = ( Block* ) (( char* ) chunk->blocks + blockSize * (i + 1));

				block->sizeIndex = index;
				block->next  = next;
			}
			/* 将最后一个 block 的 next 指向空结点，表示这是最后一个 block */
			Block* lastBlock = ( Block* ) (( char* ) chunk->blocks + blockSize * (blockCount - 1));
			lastBlock->sizeIndex = index;
			lastBlock->next = nullptr;

			/* 将刚申请的 block 链表的第二块 block 保存到 pFreeLists 对应类型的数组中 */
			pFreeLists[index] = chunk->blocks->next;
			nChunkCount++;

			/* 返回刚申请的 block 链表的第一块 block */
			return (( int* ) chunk->blocks + 1);
		}
	}

	void BlockAllocator::free(void* ptr)
	{
		int* data = ( int* ) ptr - 1;
		int index = data[0];

		if ( index == UNKNOWN_MEMORY ) {
			::free(data);
			return;
		}

		/* 根据内存大小获取 block 类型的索引值，并判断是否有效 */
		assert(0 <= index && index < BLOCK_TYPE_COUNT);
		int size = blockTypeSizeTable[index];

		/* 用头插法将 block 插到 pFreeLists[index] 指向的 block 链表中去 */
		Block* block = ( Block* ) data;
		block->next = pFreeLists[index];
		pFreeLists[index] = block;
	}

	void BlockAllocator::clear()
	{
		/* 释放所有申请的形成 block 链表的块内存 */
		for ( int i = 0; i < nChunkCount; i++ ) {
			::free(pChunks[i].blocks);
		}

		/* 置零清空所有数据 */
		nChunkCount = 0;
		memset(pChunks, 0, nChunkSpace * sizeof(Chunk));
		memset(pFreeLists, 0, sizeof(pFreeLists));
	}



	//--------------------------------------------------------------------
	// StackAllocator
	//--------------------------------------------------------------------
	//const int StackAllocator::stackSize = 500 * 1024;	/* 500 k */
	//const int StackAllocator::maxStackEntries = 32;

	StackAllocator::StackAllocator()
	{
		nIndex = 0;
		nStackEntryCount = 0;
	}

	StackAllocator::~StackAllocator()
	{
		assert(nIndex == 0);
		assert(nStackEntryCount == 0);
	}

	void* StackAllocator::allocate(size_t size)
	{
		assert(nStackEntryCount < maxStackEntries);

		StackEntry* entry = stackEntries + nStackEntryCount;
		entry->size = size;

		/* 内存池不足于分配需要的空间大小 */
		if ( nIndex + size > stackSize ) {
			entry->data = ( char* ) malloc(size);
			entry->useMalloc = true;
		}
		else {
			/* 从内存池中划出需要的空间 */
			entry->data = data + nIndex;
			entry->useMalloc = false;
			nIndex += size;
		}

		nStackEntryCount++;
		return entry->data;
	}

	void StackAllocator::free(void* ptr)
	{
		assert(nStackEntryCount > 0);

		StackEntry* entry = stackEntries + nStackEntryCount - 1;
		assert(ptr == entry->data);

		if ( entry->useMalloc ) {
			::free(ptr);
		}
		else {
			nIndex -= entry->size;
		}

		nStackEntryCount--;
		ptr = nullptr;
	}
}