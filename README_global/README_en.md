[简体中文](../README.md)
# C-VEC: C Dynamic Array Library

A single-header implementation of a dynamic array library for C, inspired by Rust's `Vec`. Zero dependencies, type-safe, and featuring an elegant API.

# ✨ Features

- **Single Header File**: Simply include `vec.h` in your project; no extra dependencies or build steps required.
- **Type Safety**: Achieves compile-time type checking through the use of C macros.
- **Automatic Memory Management**: Automatically expands and shrinks capacity as needed, abstracting away manual memory resizing.
- **Rich API**: Provides a comprehensive suite of operations including `push`, `pop`, `insert`, `remove`, `filter`, and `map`.
- **Multiple Iteration Methods**: Supports value-based iteration, indexed iteration, and pointer iteration.
- **Memory Alignment Optimization**: Supports `alignof` to correctly handle special or user-defined data types.
- **Rust-like API**: Designed to be intuitive for developers familiar with modern languages like Rust or Go.

# 🚀 Quick Start

### 1. Include the Header

```c
#include "vec.h"
```

### 2. Basic Usage Example

```c
#include "vec.h"
#include <stdio.h>

int main() {
    // 1. Create and initialize an array
    int* nums = vec(int, 1, 2, 3, 4, 5);
    
    // 2. Add elements
    vec_push(nums, 6);
    vec_push(nums, 7);
    
    // 3. Iterate and display
    printf("Array contents: ");
    vec_for_each(nums, x) {
        printf("%d ", x);
    }
    printf("\n");
    
    // 4. Find an element
    ssize_t idx = vec_find(nums, 4);
    if (idx >= 0) {
        printf("Found 4 at index: %zd\n", idx);
    }
    
    // 5. Free memory
    vec_free(nums);
    
    return 0;
}
```

# 📚 API Overview

### Creation & Destruction
- `vec(type, ...)`: Create an array from a list of values.
- `vec_with_capacity(type, cap)`: Create an empty array with a specified initial capacity.
- `vec_clone(v)`: Create a deep copy of an array.
- `vec_free(v)`: Safely release the array's memory.

### Basic Operations
- `vec_push(v, val)`: Append an element to the end.
- `vec_pop(v)`: Remove and return the last element.
- `vec_insert(v, idx, val)`: Insert an element at a specific index.
- `vec_remove(v, idx)`: Remove the element at a specific index.
- `vec_get(v, idx)`: Retrieve the value at an index.
- `vec_set(v, idx, val)`: Update the value at an index.

### Property Queries
- `vec_len(v)`: Get the current number of elements.
- `vec_cap(v)`: Get the current memory capacity.
- `vec_empty(v)`: Returns `true` if the array contains no elements.

### Advanced Operations
- `vec_sort(v, cmp)`: Sort the array using a custom comparison function.
- `vec_filter(v, filter_fn)`: Filter elements based on a predicate function.
- `vec_map(v, map_fn, out_type)`: Transform elements using a mapping function.
- `vec_reduce(v, init, reduce_fn)`: Reduce the array to a single value.
- `vec_slice(v, start, end)`: Extract a sub-array slice.
- `vec_reverse(v)`: Reverse the array in place.

### Iterators
- `vec_for_each(v, elem)`: Simple value-based iteration.
- `vec_enumerate(v, idx, elem)`: Iteration providing both index and value.
- `VecIter(type)`: A struct-based iterator for custom traversal logic.

### Display & Debugging
- `vec_show_int(v)`: Print an integer array.
- `vec_show(v, fmt)`: Print the array using a custom format string.
- `vec_show_custom(v, pfn)`: Print using a custom user-defined printing function.

# 🔧 Compilation Requirements

- **Compiler**: Requires a compiler supporting GNU C extensions (GCC or Clang).
- **Standard**: C99 or higher.
- **Dependencies**: Standard libraries (`stdlib.h`, `string.h`, `stddef.h`, `stdbool.h`).

# 📦 Memory Management Strategy

- **Initial Capacity**: Minimum of 4 elements.
- **Expansion Strategy**: Grows by a factor of 2 (2x) when full.
- **Shrinking Condition**: Capacity is halved when the utilization falls below 25%.
- **Minimum Capacity**: Always maintains at least 4 elements.

# ⚠️ Notes

- **Compiler Compatibility**: Only supports GCC/Clang due to the use of GNU extensions (e.g., `__typeof__` and statement expressions).
- **Thread Safety**: Not thread-safe; requires external synchronization for multi-threaded usage.
- **Error Checking**: It is recommended to check return values or lengths for critical operations.
- **Performance Note**: `vec_dedup` (de-duplication) is $O(n^2)$; for large datasets, sort the array before processing.

# 📄 License

This project is licensed under the **Apache-2.0 License**. See the `LICENSE` file for details.

# 🎯 Design Philosophy

C-VEC aims to provide C developers with a modern, safe, and easy-to-use dynamic array experience while maintaining zero dependencies and minimal runtime overhead. The API design is heavily inspired by Rust's `Vec`, allowing developers familiar with modern ecosystem ergonomics to feel right at home in C.

---

**Get Started**: Just copy `vec.h` into your project and enjoy the convenience of modern dynamic arrays!
