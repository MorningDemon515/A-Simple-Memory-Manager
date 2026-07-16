# A-Simpler-Memory-Manager

A minimalist C memory manager for storing heterogeneous data types in a single contiguous memory block using safe handles and dynamic byte offsets.

---

## Introduction

This project addresses classic heap fragmentation and dangling pointer issues in C by keeping track of the total bytes allocated and utilizing a **Handle-based Tracking System**. 

By combining `realloc` with byte-level copying via `memcpy` and memory shifting via `memmove`, it allows you to safely store, retrieve, and delete various structures of different sizes and layouts within a single contiguous heap memory block.

---

## How It Works

The core mechanism of this manager consists of three pillars: **byte-length-based physical offset addressing**, a **metadata registry**, and **handles**.

### 1. Track Total Bytes & Precise Reallocation
The `Manager` struct maintains a `total` variable to record the exact size (in bytes) currently occupied by the contiguous dynamic memory block. Each time a new element is added, the block is reallocated with a target size of `manager->total + sizeof(new_type)`.

### 2. Pointer Offset Writing
Since pointer arithmetic is not permitted on `void*`, we cast `manager->data` to a 1-byte addressable `char*` pointer and offset it by `currentTotal` bytes to find the exact target address:
```c
void* targetAddr = (char*)manager->data + currentTotal;
```

### 3. Handle System & Metadata Registry
To prevent dangling pointers caused by realloc shifting the memory block's base address, users are returned a lightweight Handle instead of a raw pointer.

- A registry table (ElementRecord) maps each unique Handle.id to its type and physical offset inside the block.
- When retrieving elements, the address is resolved dynamically:
```c
void* GetElements(Manager* manager, Handle handle) {
    for (int i = 0; i < manager->size; i++) {
        if (manager->records[i].id == handle.id) {
            return (char*)manager->data + manager->records[i].offset;
        }
    }
    return NULL;
}
```

### 4. Compact-on-Delete (Removal Shifting)
When removing an element mid-way via RemoveElements, the manager:

1. Calculates the remaining bytes behind the deleted element: `moveBytes = total - (targetOffset + targetSize)`.
2. Uses memmove to safely shift all subsequent elements forward, closing the memory gap.
3. Updates the registry table by shifting subsequent records and adjusting their `offsets (offset -= targetSize)`.
4. Shrinks the contiguous buffer via realloc to keep memory footprint perfectly compacted.
