# A-Simpler-Memory-Manager

A minimalist, type-safe Handle Management System in C designed to store and manage heterogeneous structures using an indirection table and stable IDs.

---

## Introduction

This project addresses classic dangling pointer and unpredictable memory lifecycle issues in C by utilizing a Handle-based Tracking System.

By introducing an indirection layer (`ElementRecord`) between application logic and raw memory, it allows you to dynamically allocate, retrieve, and delete various independent structures (`Vector`, `Book`) without exposing their actual heap addresses directly to the caller.

---

## How It Works

The core mechanism of this manager consists of three pillars: independent element tracking, a metadata registry, and stable handles.

### 1. Independent Heap Allocation & Registry Expansion
Unlike managing a single raw byte buffer, every time `CreateVector` or `CreateBook` is called, a separate chunk of heap memory is allocated for that individual object. The manager then uses `realloc` to expand its central registry array (`ElementRecord* records`) to accommodate the new object's metadata.

### 2. Handle System & Indirection Mapping
To prevent application code from holding onto unstable raw pointers that might become wild or hard to trace, the system wraps an internal lookup token into a lightweight `Handle`:

- A central registry table maps each unique `Handle.id` to its object `Type` (e.g., `VECTOR`, `BOOK`) and its actual heap address pointer (`void* ptr`).

- Users hold onto the `Handle`, while the manager retains ownership of the raw pointers.

### 3. Dynamic Pointer Resolution
When an element needs to be accessed, the raw pointer is resolved dynamically on-the-fly by scanning the registry table:

```c
void* GetElements(Manager* manager, Handle handle) {
    for (int i = 0; i < manager->size; i++) {
        if (manager->records[i].id == handle.id) {
            return manager->records[i].ptr; // Return the exact heap address
        }
    }
    return NULL;
}
```

### 4. Registry Compaction-on-Delete
When removing an element via `RemoveElements`, the manager:

1. Locates the targeted item in the registry, reads its type, and safely `free()`s its independent heap memory.
2. Shifts all subsequent metadata records forward in the registry array using a loop to close the slot gap.
3. Shrinks the central registry array via `realloc` to keep the metadata footprint perfectly compacted.

---

## Quick Start & Usage Pattern
⚠️Best Practice Rule: Always adopt a "Fetch-and-Use" pattern. Never cache the resolved raw pointers long-term, as elements can be explicitly deleted or freed from the system under the hood, rendering cached pointers instantly invalid. Rely strictly on the `Handle`.

```c
// 1. Initialize the Manager
Manager* manager = InitManager();[cite: 1]

// 2. Create Heterogeneous Objects & Receive Stable Handles
Handle hVector = CreateVector(manager); // Allocates a Vector on heap[cite: 1]
Handle hBook   = CreateBook(manager);   // Allocates a Book on heap[cite: 1]

// 3. Safe Dynamic Access (Temporary pointer resolution)
Vector* v = (Vector*)GetElements(manager, hVector);[cite: 2]
if (v) {
    printf("Vector coordinates: %f, %f\n", v->x, v->y);[cite: 1]
}

// 4. Safe Removal (Frees the object memory and compacts the registry table)
RemoveElements(manager, hVector);[cite: 1]

// 5. Cleanup Everything safely at once
ReleaseManager(manager);[cite: 1]
```

---

## 📋 License
This project is open-source and available under the [MIT License](./LICENSE).
