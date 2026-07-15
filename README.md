# A-Simpler-Memory-Manager

A minimalist C memory manager for storing heterogeneous data types in a single contiguous memory block using raw pointers and byte offsets.

---

## Introduction

This project addresses these issues by keeping track of the total bytes allocated. By combining `realloc` with byte-level copying via `memcpy`, it allows you to safely store various structures of different sizes and layouts within a single contiguous heap memory block.

---

## How It Works

The core mechanism of this manager is **byte-length-based physical offset addressing**.

1. **Track Total Bytes**: The `Manager` struct maintains a `total` variable to record the exact size (in bytes) currently occupied by the dynamic memory block.
2. **Precise Reallocation**: Each time a new element is added, the block is reallocated with a target size of `manager->total + sizeof(new_type)`.
3. **Pointer Offset Writing**:
   Since pointer arithmetic is not permitted on `void*`, we cast `manager->data` to a 1-byte addressable `char*` pointer and offset it by `currentTotal` bytes:
   ```c
   void* targetAddr = (char*)manager->data + currentTotal;
