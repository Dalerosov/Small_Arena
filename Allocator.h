// I know #pragma once isnt standardised, but should be enough in this case
#pragma once

#include <cstddef>
#include <cassert>
#include <cstdint>
struct Arena
{
    std::byte* buffer;
    std::size_t capacity;
    std::size_t offset;
    bool owns_buffer;


    // Allows the programmer to decide if they want to own the buffer themselves, so they can store it on the stack if it isnt too big, but also they should align it themselves
    Arena(std::byte* buffer, std::size_t cap)
        : buffer(buffer), capacity(cap), offset(0), owns_buffer(false)
    {}

    Arena(std::size_t cap)
        : capacity(cap), offset(0), owns_buffer(true)
    {
        buffer = static_cast<std::byte*>(
            ::operator new(cap, std::align_val_t(64))
            );
    }

    // delete copy constructors so we dont get a double free
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;


    Arena(Arena&& o) noexcept : buffer(o.buffer), capacity(o.capacity), offset(o.offset), owns_buffer(o.owns_buffer) {
        o.reset_ownership();
    }

    ~Arena()
    {
        if (owns_buffer)
        {
            ::operator delete(buffer, std::align_val_t(64));
        }
    }

    void reset_ownership()
    {
        if (owns_buffer) {
            buffer = nullptr;
        }
        capacity = 0;
        offset = 0;
        owns_buffer = false;
    }

    static std::uintptr_t align_forward(std::uintptr_t ptr, std::size_t alignment)
    {
        assert((alignment & (alignment - 1)) == 0 && "alignment must be a power of two");
        std::uintptr_t mod = ptr & (alignment - 1);
        return mod == 0 ? ptr : ptr + (alignment - mod);
    }

    void* alloc(std::size_t size, std::size_t alignment = alignof(std::max_align_t))
    {
        std::uintptr_t current = reinterpret_cast<std::uintptr_t>(buffer) + offset;

        std::uintptr_t aligned = align_forward(current, alignment);
        std::uintptr_t new_offset = aligned - reinterpret_cast<std::uintptr_t>(buffer) + size;

        if (new_offset > capacity) return nullptr;

        offset = new_offset;
        return reinterpret_cast<void*>(aligned);
    }


    // together with used() can be used to snapshot offset
    void restore(std::size_t mark) { offset = mark; }

    // doesnt clear memory, only resets offset
    void reset()
    {
        offset = 0;
    }

    std::size_t used() const
    {
        return offset;
    }

    std::size_t remaining() const
    {
        return capacity > offset ? capacity - offset : 0;
    }
};
