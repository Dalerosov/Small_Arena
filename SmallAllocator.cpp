#include <iostream>
#include <cstddef>
#include "Allocator.h"

int main()
{
    alignas(64) std::byte buffer[1024];
    Arena a(buffer, 1024);
    int* p = static_cast<int*>(a.alloc(sizeof(int)));
    *p = 42;

    
    Arena b = std::move(a);

    std::cout << *p << "\n" << a.remaining() << "\n" << b.remaining() << "\n";
    std::cout << "Hello World!\n";
    return 0;
}

