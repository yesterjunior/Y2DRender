#include "BlockAllocator.h"

#include <cstdarg>
#include <Windows.h>
#include <iostream>

namespace Simple2D
{
	int BlockAllocator::blockTypeSizeTable[BLOCK_TYPE_COUNT] = {
		12,		// 2 �������� 4  * 2 = 8   + 4 = 12
		16,		// 3 �������� 4  * 3 = 12  + 4 = 16
		28,		// 6 �������� 4  * 6 = 24  + 4 = 28
		36,		// 8 �������� 4  * 8 = 32  + 4 = 36
		60,		// 1 ���߶Σ� 28 * 2 = 56  + 4 = 60
		76,		// 18�������� 4 * 18 = 72  + 4 = 76
		88,		// 1 �����Σ� 28 * 4 = 84  + 4 = 88
		116,	// 1 �����Σ� 28 * 4 = 112 + 4 = 116
		148,	// 1 ������ 36 * 4 = 144 + 4 = 148
		196,	// 48�������� 4 * 48 = 192 + 4 = 196
		260,	// 9 �����㣺 28 * 9 = 256 + 4 = 260
		292,	// 72�������� 4 * 72 = 288 + 4 = 292
		436,	// 108 ������ 4 * 108 = 432 + 4 = 436
		676,	// 24�����㣺 28 * 24 = 672 + 4 = 676
		868,	// 216 ���㣺 4 * 216 = 864 + 4 = 868
		1068,	// 38�����㣺 28 * 37 = 1064 + 4 = 1068
		2076	// 74�����㣺 28 * 74 = 2072 + 4 = 2076
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

		/* ��ʼ���ڴ� */
		memset(pChunks, 0, nChunkSpace * sizeof(Chunk));
		memset(pFreeLists, 0, sizeof(pFreeLists));

		/* ������������ blockTypeSizeLookup �� block ��������ֵ */
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
		/* �ͷ�����������γ� block ����Ŀ��ڴ� */
		for ( int i = 0; i < nChunkCount; i++ ) {
			::free(pChunks[i].blocks);
		}
		::free(pChunks);
	}

	void* BlockAllocator::allocate(int size)
	{
		if ( size == 0 ) return 0;
		assert(size > 0);

		/* ʹ���ĸ��ֽڼ�¼ block ������������free ��ʹ�� */
		size += sizeof(int);

		/* ����Ŀռ���ڹ涨�����ֵ��ֱ�����룬���ŵ����������ȥ */
		if ( size > MAX_BLOCK_SIZE ) {
			int* data = ( int* ) malloc(size);
			/* -1 ��ʾ����ֱ�ӷ�����ڴ� */
			data[0] = UNKNOWN_MEMORY;		
			return (data + 1);
		}

		int index = blockTypeSizeLookup[size];
		assert(0 <= index && index < BLOCK_TYPE_COUNT);

		/* ����ͬ���͵�δ��ʹ�õ��ڴ�飿�����ڴ�� */
		if ( pFreeLists[index] ) {
			/* ʹ��ͷָ��ָ���µ�δ��ʹ�õ� block */
			Block* block = pFreeLists[index];
			pFreeLists[index] = block->next;
			return (( int* ) block + 1);
		}
		else {
			/* ��չ chunk ���� */
			if ( nChunkCount == nChunkSpace ) {
				Chunk* oldChunks = pChunks;
				nChunkSpace += CHUNK_ARRAY_INCREMENT;
				pChunks = ( Chunk* ) malloc(nChunkSpace * sizeof(Chunk));
				memcpy(pChunks, oldChunks, nChunkCount * sizeof(Chunk));
				memset(pChunks + nChunkCount, 0, CHUNK_ARRAY_INCREMENT * sizeof(Chunk));
				::free(oldChunks);
			}
			int chunkSize = chunkSizeTable[index];
			/* ��ȡһ��δ��ʹ�õĿ������������ڴ�� chunk */
			Chunk* chunk = pChunks + nChunkCount;
			chunk->blocks = ( Block* ) malloc(chunkSize);

			/* ��ȡ��ǰ����� block ���ʹ�С */
			int blockSize = blockTypeSizeTable[index];

			/* ����һ�� chunk �ڴ��ܹ��ָ�� block ������ */
			int blockCount = chunkSize / blockSize;
			assert(blockCount * blockSize <= chunkSize);

			/* �� chunk �ָ����� block���ٽ� block ���������ʽ������ */
			for ( int i = 0; i < blockCount - 1; i++ ) {
				Block* block = ( Block* ) (( char* ) chunk->blocks + blockSize * i);
				Block* next  = ( Block* ) (( char* ) chunk->blocks + blockSize * (i + 1));

				block->sizeIndex = index;
				block->next  = next;
			}
			/* �����һ�� block �� next ָ��ս�㣬��ʾ�������һ�� block */
			Block* lastBlock = ( Block* ) (( char* ) chunk->blocks + blockSize * (blockCount - 1));
			lastBlock->sizeIndex = index;
			lastBlock->next = nullptr;

			/* ��������� block ����ĵڶ��� block ���浽 pFreeLists ��Ӧ���͵������� */
			pFreeLists[index] = chunk->blocks->next;
			nChunkCount++;

			/* ���ظ������ block ����ĵ�һ�� block */
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

		/* �����ڴ��С��ȡ block ���͵�����ֵ�����ж��Ƿ���Ч */
		assert(0 <= index && index < BLOCK_TYPE_COUNT);
		int size = blockTypeSizeTable[index];

		/* ��ͷ�巨�� block �嵽 pFreeLists[index] ָ��� block ������ȥ */
		Block* block = ( Block* ) data;
		block->next = pFreeLists[index];
		pFreeLists[index] = block;
	}

	void BlockAllocator::clear()
	{
		/* �ͷ�����������γ� block ����Ŀ��ڴ� */
		for ( int i = 0; i < nChunkCount; i++ ) {
			::free(pChunks[i].blocks);
		}

		/* ��������������� */
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

		/* �ڴ�ز����ڷ�����Ҫ�Ŀռ��С */
		if ( nIndex + size > stackSize ) {
			entry->data = ( char* ) malloc(size);
			entry->useMalloc = true;
		}
		else {
			/* ���ڴ���л�����Ҫ�Ŀռ� */
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