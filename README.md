# Small Arena

A single-header arena allocator in C++. Drop `Allocator.h` into your project and use it.

\---

## What it does

An arena allocator hands out memory by bumping a pointer forward through a fixed-size buffer. Individual allocations are not freed — instead the entire arena is reset at once, dropping the offset back to zero. This makes allocation essentially free (a pointer comparison and an addition), and deallocation free too.

This implementation supports:

* **Owned or borrowed buffer** — the arena can allocate its own heap buffer, or you can hand it one you manage yourself (stack-allocated, static, etc.)
* **Alignment** — each allocation is aligned to any power-of-two boundary; defaults to `alignof(std::max\_align\_t)`. The internal buffer is allocated with 64-byte alignment (cache-line boundary).
* **Snapshot / restore** — save the current offset with `used()` and restore it later with `restore(mark)`, giving you scoped temporary allocation without a separate allocator.
* **Move semantics, no copies** — the copy constructor and copy assignment are deleted to prevent double-free. The move constructor transfers ownership and nulls out the source.

\---

## Usage

### Owned buffer (heap)

```cpp
#include "Allocator.h"

Arena arena(1024 \* 1024); // 1 MB, allocated internally

auto\* data = static\_cast<int\*>(arena.alloc(sizeof(int) \* 100, alignof(int)));
// use data...

arena.reset(); // drop everything at once, no individual frees needed
```

### Borrowed buffer (stack or static)

```cpp
alignas(64) std::byte buf\[4096];
Arena arena(buf, sizeof(buf)); // arena does not own or free this buffer

auto\* vec = static\_cast<float\*>(arena.alloc(sizeof(float) \* 3, alignof(float)));
```

### Snapshot / restore (temporary scratch space)

```cpp
Arena arena(65536);

std::size\_t mark = arena.used();       // save position
auto\* tmp = arena.alloc(1024);         // temporary allocation
// ... do work with tmp ...
arena.restore(mark);                   // reclaim that memory
```

\---

## API

```cpp
// Constructors
Arena(std::size\_t cap);                          // owns the buffer
Arena(std::byte\* buffer, std::size\_t cap);       // borrows the buffer

// Allocation
void\* alloc(std::size\_t size, std::size\_t alignment = alignof(std::max\_align\_t));

// Reset / inspect
void   reset();                  // reset offset to 0 (does not clear memory)
void   restore(std::size\_t mark); // restore offset to a saved position
std::size\_t used() const;        // bytes used
std::size\_t remaining() const;   // bytes remaining
```

