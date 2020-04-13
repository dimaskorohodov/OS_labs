// ConsoleApplication2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <list>
#include <Windows.h>
#include"CppAlocator.h"

using namespace std;

int main()
{
	auto allocator = new CppAlocator();

	cout << "Two blocks with int values were crested" << endl;
	auto mem1 = allocator->mem_alloc(sizeof(int));
	auto mem2 = allocator->mem_alloc(sizeof(int));
	auto var1 = new (mem1) int(1);
	auto var2 = new (mem2) int(2);
	allocator->mem_dump();

	cout << endl << "reallocating mem2 block to custom size of 7. Expect to get 3rd block with size of 8" << endl;
	auto mem3 = allocator->mem_realloc(mem2, 7);
	allocator->mem_dump();

	cout << endl << "Free block 1" << endl;
	allocator->mem_free(mem1);
	allocator->mem_dump();

	system("pause");
}

