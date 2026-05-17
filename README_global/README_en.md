[简体中文](../README.md)

# C-VEC 🚀

**A Modern, Type-Safe, Rust-inspired Dynamic Array for C (GCC/Clang)**

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![C](https://img.shields.io/badge/C-11%2F17%2F23-orange.svg)](#)
[![Compiler](https://img.shields.io/badge/Compiler-GCC%2FClang-green.svg)](#)

`C-VEC` is a lightweight, single-header dynamic array library. It goes beyond simple `std::vector` clones by leveraging modern compiler extensions to eliminate the most common pitfalls of C development: **memory leaks, manual pointer re-assignment, and alignment headaches.**

---

## ✨ Key Features

*   **🛡️ True RAII Support**: Via the `vec_auto` macro. Arrays are automatically deallocated when they go out of scope, eliminating memory leaks in complex functions with multiple return paths.
*   **💉 In-place Pointer Updates**: Forget the tedious `v = vec_push(v, x)` syntax! `C-VEC` automatically captures your variable's address and updates the pointer during `realloc`, ensuring your handle is always valid.
*   **🔒 Industrial-Grade Safety**: 
    *   **Realloc Protection**: Preserves the original memory block if allocation fails, preventing data loss and leaks.
    *   **Side-Effect Safe**: Macros are carefully designed to evaluate arguments exactly once (safe for `vec_push(v, i++)`).
*   **📐 SIMD-Ready Alignment**: Automatically respects `alignof(T)`. Whether you are storing `char` or 32-byte aligned AVX-512 structures, the memory layout remains strictly valid.
*   **🍎 Functional Paradigms**: Built-in support for `vec_filter`, `vec_map`, and `vec_extend` (concatenation) for expressive, modern C code.
*   **📦 Zero Dependencies**: Header-only design. Just drop `vec.h` into your project.

---

## 🚀 Quick Start

### 1. Basic Usage (In-place Updates)
No need to re-assign the return value. The macro handles the "Black Magic" for you.
```c
#include "vec.h"

int main() {
    int* nums = vec(int, 1, 2, 3);

    // In-place push: 'nums' is updated automatically if realloc occurs
    vec_push(nums, 4);
    vec_push(nums, 5);

    vec_show(nums, "%d"); // Output: Vec<5/8>: [1, 2, 3, 4, 5]
    
    vec_free(nums);
    return 0;
}
```

### 2. Automatic Memory Management (RAII)
Manage lifecycles like C++ or Rust.
```c
void process_data() {
    // Automatically freed at the end of the scope
    vec_auto(float*) scores = vec(float, 95.0, 88.5);
    
    if (some_error_occurred) return; // No memory leak!
    
    vec_push(scores, 100.0);
} 
```

### 3. Concatenation & Functional Ops
```c
int* v1 = vec(int, 1, 2);
int* v2 = vec(int, 3, 4);

vec_extend(v1, v2); // Appends v2 to v1 (In-place)

// Filter even numbers into a NEW vector
int* evens = vec_filter(v1, is_even); 
```

---

## 🛠 Technical Deep Dive: Memory Layout

`C-VEC` utilizes a unique **"Buried Offset"** design to ensure strict alignment even with metadata:

```text
[ VecHeader | Padding | HeaderOffset (size_t) ] [ User Data (Aligned) ]
                                 ^                      ^
                                 |                      |
                          Metadata Lookup        User Pointer (v)
```

When calling `vec_push(v, val)`, the macro uses `&v` to capture the address of your pointer variable. If `realloc` moves the memory block, the macro directly modifies your variable `v` to point to the new data segment.

---

## 📖 API Reference

### Lifecycle & Management
| Macro / Function | Description | Complexity |
| :--- | :--- | :--- |
| `vec(type, ...)` | Create and initialize a new Vec | $O(n)$ |
| `vec_auto(type)` | (RAII) Declare a Vec that auto-frees at end of scope | - |
| `vec_free(v)` | Manually free memory and nullify the pointer | $O(1)$ |
| `vec_reserve(v, n)` | Pre-allocate capacity for at least `n` elements | $O(1)$ |

### Dynamic Operations
| Macro / Function | Description | Complexity |
| :--- | :--- | :--- |
| `vec_push(v, val)` | **(In-place)** Append an element to the end | Amortized $O(1)$ |
| `vec_pop(v)` | Remove and return the last element | $O(1)$ |
| `vec_extend(dst, src)` | **(In-place)** Append `src` vector to `dst` | $O(n)$ |
| `vec_insert(v, idx, val)` | Insert element at specific index | $O(n)$ |
| `vec_remove(v, idx)` | Remove element at index (maintains order) | $O(n)$ |

### Functional & Iteration
| Macro / Function | Description | Complexity |
| :--- | :--- | :--- |
| `vec_for_each(v, var)` | Iterate through all elements | $O(n)$ |
| `vec_enumerate(v, i, var)`| Iterate with index and value | $O(n)$ |
| `vec_filter(v, pred)` | Filter elements into a **new** Vec | $O(n)$ |
| `vec_map(v, fn, type)` | Map elements to a **new** Vec of specific type | $O(n)$ |

---

## ⚠️ Caveats

*   **Compiler Support**: Requires **GCC** or **Clang** (depends on `typeof`, `cleanup`, and `__extension__`).
*   **Iterator Invalidation**: While `vec_push` updates the *main* pointer, any **secondary raw pointers** pointing into the vector elements will be invalidated if a `realloc` occurs.

---

## 📜 License

Licensed under the [Apache-2.0](LICENSE) License.

---

**Crafted with ❤️ by OxidizedSchale**  
*"Making C feel like Rust, one macro at a time."*
