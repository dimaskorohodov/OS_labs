#pragma once
#include <iostream>
#include <list>
#include <Windows.h>
using namespace std;
class CppAlocator
{
	struct block_of_memory //this structure describes logical block of memory
	{
		unsigned char* ptr; //pointer to this block
		unsigned int size; // block size
		block_of_memory* next; // pointer to the next block
		block_of_memory* prev; // pointer to the previous block
	};


private:
	list<block_of_memory*>* free_blocks; // here are free bloks are stored
	list<block_of_memory*>* used_blocks; // here are used blocks are stored
	block_of_memory* first_block; //pointer to the first block
	block_of_memory* last_block; // pointer to the last block

	block_of_memory* find_block(unsigned int size) {
		for (block_of_memory* block : *free_blocks) // 
		{
			if (block->size < size)
			{
				continue;
			}
			return block;
		}
		return nullptr; 
	}

	 int align_size(unsigned int size)
	{
		return (size + sizeof(intptr_t) - 1) & ~(sizeof(intptr_t) - 1);
	}

	block_of_memory* alloc_block(unsigned int size) {
		 auto memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size); 

		if (memory != nullptr)
		{
			const auto block = new block_of_memory(); 
			block->next = nullptr;
			block->prev = nullptr;
			block->ptr = static_cast<uint8_t*>(memory);
			block->size = size;

			if ((free_blocks->size() == 0) && (used_blocks->size() == 0))
			{
				first_block = block;
			}

			free_blocks->push_back(block);

			if (last_block != nullptr)
			{
				last_block->next = block;
				block->prev = last_block;
			}

			last_block = block;

			return block;
		}
		else 
		{
			return nullptr;
		
		}

		
	}
	void split_block(block_of_memory* current_memory_block, unsigned int size) {
		free_blocks->remove(current_memory_block); 
		used_blocks->push_back(current_memory_block); 

		if (current_memory_block->size == size) return; 

		 auto size_diff = current_memory_block->size - size; 

		 auto splitted_block = new block_of_memory(); 
		splitted_block->ptr = current_memory_block->ptr + size;
		splitted_block->size = size_diff;
		splitted_block->prev = current_memory_block;
		splitted_block->next = current_memory_block->next;

		current_memory_block->next = splitted_block; 
		current_memory_block->size = size;

		free_blocks->push_back(splitted_block);
	}
	void join_block(block_of_memory* current_block_of_memory) {
		if (current_block_of_memory->prev != nullptr && (current_block_of_memory->prev + current_block_of_memory->prev->size == current_block_of_memory) && contains_block(free_blocks, current_block_of_memory->prev))
		{
			current_block_of_memory = join_blocks(current_block_of_memory->prev, current_block_of_memory);
		}

		if (current_block_of_memory->next != nullptr && (current_block_of_memory + current_block_of_memory->size == current_block_of_memory->next) && contains_block(free_blocks, current_block_of_memory->next))
		{
			current_block_of_memory = join_blocks(current_block_of_memory, current_block_of_memory->next);
		}
	}

	block_of_memory* join_blocks(block_of_memory* left, block_of_memory* right) {
		left->size += right->size;
		left->next = right->next;
		return left;
	}
	bool contains_block(list<block_of_memory*>* list, block_of_memory* block) {
		if (block == nullptr)
			return false;

		for (auto list_block : *list)
		{
			if (list_block->ptr == block->ptr)
				return true;
		}
		return false;
	}

	public:
		CppAlocator()
		{
			used_blocks = new std::list<block_of_memory*>();
			free_blocks = new std::list<block_of_memory*>();
			first_block = nullptr;
			last_block = nullptr;
		}

		void mem_dump() {
			cout << "Here you can see state of memory in the moment:" << endl;

			if (first_block == nullptr) {
				cout << "Memory was finished" << endl;
				return;
			}

			auto current_block = first_block;
			while (true) {

				cout << "- Size of the current block: " << current_block->size << "  Block is : ";

				//contains_block(free_blocks, current_block) ? cout << "free" : contains_block(used_blocks, current_block) ?

				if (contains_block(free_blocks, current_block))
				{
					cout << "free" << endl;
				}

				else if (contains_block(used_blocks, current_block))
				{
					cout << " used" << endl;
				}

				if (current_block->next == nullptr)
				{
					break;
				}
				current_block = current_block->next;
			}
		}
		void* mem_alloc(unsigned int size) {

			size = align_size(size); 

			block_of_memory* block = find_block(size); 

			if (block == nullptr) 
			{
				block = alloc_block(size);

				if (block == nullptr)
				{
					return nullptr;
				}
			}

			split_block(block, size); 

			return block->ptr; 
		}
		void* mem_realloc(void* addr, unsigned int size) {
			mem_free(addr); 
			return mem_alloc(size);
		}
		void mem_free(void* addr) {
			const auto ptr = reinterpret_cast<uint8_t*>(addr);

			for (auto block : *used_blocks)
			{
				if (block->ptr != ptr)
					continue;

				used_blocks->remove(block);
				free_blocks->push_back(block);
				join_block(block);
				return;
			}
		}



};

