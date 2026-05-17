[简体中文](../README.md)
# C-VEC 🚀

**A Modern, Type-Safe, Rust-inspired Dynamic Array for C (GCC/Clang)**

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![C](https://img.shields.io/badge/C-11%2F17%2F23-orange.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Compiler](https://img.shields.io/badge/Compiler-GCC%2FClang-green.svg)](#)

`C-VEC` is a lightweight, single-header dynamic array library for C. While its API is inspired by Rust's `Vec`, it goes beyond simple imitation by leveraging modern compiler extensions to solve the most painful issues in C memory management: manual deallocation, pointer invalidation, and strict memory alignment.

---

## ✨ Key Features

*   **🛡️ True RAII Support**: Using the GNU `__attribute__((cleanup))` extension, `C-VEC` provides `vec_auto`, allowing arrays to be automatically freed when they go out of scope.
*   **💉 In-place Pointer Updates**: Forget `v = vec_push(v, x)`. Our macros capture the address of your pointer, automatically updating it during `realloc`. This eliminates a major source of "dangling pointer" bugs.
*   **🔒 Industrial-Grade Safety**: 
    *   **Realloc Protection**: If memory allocation fails, `C-VEC` preserves the original pointer, preventing memory leaks common in naive `realloc` usage.
    *   **Side-Effect Safe**: Macros are designed to evaluate arguments exactly once. Operations like `vec_push(v, i++)` are perfectly safe.
*   **📐 Smart Alignment**: Automatically handles memory alignment (e.g., for SIMD/AVX types) by calculating offsets dynamically at runtime.
*   **🍎 Functional Paradigms**: Built-in macros for `filter`, `map`, and `enumerate` for a more expressive coding style.
*   **📦 Zero Dependencies**: A single `vec.h` file is all you need.

---

## 🚀 Quick Start

### 1. Basic Usage
```c
#include "vec.h"

int main() {
    // Initialize with values
    int* nums = vec(int, 1, 2, 3);

    // In-place push: no need to re-assign 'nums'
    vec_push(nums, 4);
    vec_push(nums, 5);

    // Elegant iteration
    vec_for_each(nums, x) {
        printf("%d ", x);
    }

    vec_free(nums);
    return 0;
}
```

### 2. Automatic Memory Management (RAII)
No more worrying about memory leaks in complex functions with multiple return paths:
```c
void process_data() {
    // 'data' will be automatically freed when the function returns
    vec_auto(float*) data = vec(float, 10.5f, 20.0f);
    
    if (check_error()) return; // Automatically freed!
    
    vec_push(data, 30.5f);
} // Automatically freed!
```

---

## 🛠 Technical Deep Dive: Memory Layout

`C-VEC` utilizes an advanced **"Hidden Header with Buried Offset"** design to support strict alignment requirements:

```text
[ VecHeader | Padding | HeaderOffset (size_t) ] [ User Data (Aligned) ]
                                ^                     ^
                                |                     |
                          Metadata Lookup        User Pointer (v)
```

When `vec_push` is called, the macro captures the address of your pointer (`&v`). If the capacity needs to grow, it performs the `realloc` and transparently updates your local pointer variable to the new valid address.

---

## 📖 API Reference

### Lifecycle
| Macro / Function | Description |
| :--- | :--- |
| `vec(type, ...)` | Creates and initializes a new Vec with optional elements. |
| `vec_auto(type)` | (RAII) Declares a Vec that auto-frees at end of scope. |
| `vec_free(v)` | Manually releases memory and sets pointer to NULL. |
| `vec_clear(v)` | Resets length to zero without deallocating capacity. |

### Dynamic Operations
| Macro / Function | Description |
| :--- | :--- |
| `vec_push(v, val)` | **(In-place)** Appends an element to the end. |
| `vec_pop(v)` | Removes and returns the last element. |
| `vec_insert(v, idx, val)` | Inserts an element at the specified index. |
| `vec_remove(v, idx)` | Removes the element at index (preserves order). |

### Iteration & Functional
| Macro / Function | Description |
| :--- | :--- |
| `vec_for_each(v, var)` | Iterates over elements. |
| `vec_enumerate(v, i, var)` | Iterates with index and value. |
| `vec_filter(v, predicate)` | Returns a new Vec containing elements that pass the test. |
| `vec_map(v, fn, out_t)` | Returns a new Vec by applying a function to each element. |

---

## ⚠️ Requirements & Limitations

*   **Compiler**: **GCC** or **Clang** is required (uses `__extension__`, `typeof`, and `cleanup` attributes).
*   **Standard**: `C11` or later is recommended.
*   **Safety**: While we provide safety macros, users should still avoid keeping raw pointers to internal `vec` elements as they may be invalidated during a `push`.

---

## 📜 License

Licensed under the [Apache-2.0](LICENSE) License.

---

**Designed with ❤️ by OxidizedSchale**  
*"Making C feel like Rust, one macro at a time."*
