# C-VEC: C Dynamic Array Library | C-VEC: C 语言动态数组库

A single-header implementation of a dynamic array library for C, similar to Rust's 
"Vec". Zero dependencies, type-safe, elegant API.

一个用单头文件实现的、类似 Rust 
"Vec" 的 C 语言动态数组库。零依赖，类型安全，API 优雅。

# ✨ Features | ✨ 特性

- Single Header File: Just include 
"vec.h", no extra dependencies. | 单头文件：只需包含 
"vec.h"，无需额外依赖。
- Type Safety: Achieves compile-time type checking through macros. | 类型安全：通过宏实现编译时类型检查。
- Automatic Memory Management: Automatically expands and shrinks capacity, no manual management needed. | 自动内存管理：自动扩容/缩容，无需手动管理。
- Rich API: Provides complete operations like push/pop/insert/remove/filter/map. | 丰富 API：提供 push/pop/insert/remove/filter/map 等完整操作。
- Multiple Iteration Methods: Supports value iteration, indexed iteration, and pointer iteration. | 多种迭代方式：支持值迭代、索引迭代、指针迭代。
- Memory Alignment Optimization: Supports 
"alignof" for handling special data types. | 内存对齐优化：支持 
"alignof"，处理特殊数据类型。
- Rust-like API: Easy to pick up for developers familiar with modern languages. | 类似 Rust 的 API：熟悉现代语言开发的用户上手快。

# 🚀 Quick Start | 🚀 快速开始

1. Include the Header | 1. 引入头文件

#include "vec.h"

2. Basic Usage Example | 2. 基本使用示例

#include "vec.h"

#include <stdio.h>

int main() {

    // Create an array | 创建数组
    
    int* nums = vec(int, 1, 2, 3, 4, 5);
    
    // Add elements | 添加元素
    
    vec_push(nums, 6);
    
    vec_push(nums, 7);
    
    // Iterate and display | 遍历显示
    
    printf("Array contents: "); // 数组内容:
    
    vec_for_each(nums, x) {
    
        printf("%d ", x);
        
    }
    
    printf("\n");
    
    // Find an element | 查找元素
    
    ssize_t idx = vec_find(nums, 4);
    
    if (idx >= 0) {
    
        printf("Found 4 at position: %zd\n", idx); // 找到 4 在位置: %zd\n
        
    }
    
    
    // Free memory | 释放内存
    
    vec_free(nums);
    
    return 0;
}

# 📚 API Overview | 📚 API 概览

Creation & Destruction | 创建与销毁

"vec(type, ...)" - Create from a value list. | 从值列表创建。

"vec_with_capacity(type, cap)" - Create with specified capacity. | 指定容量创建。

"vec_clone(v)" - Clone an array. | 克隆数组。

"vec_free(v)" - Safely free memory. | 安全释放。

# Basic Operations | 基本操作

"vec_push(v, val)" - Append an element. | 添加元素。

"vec_pop(v)" - Remove and return the last element. | 弹出元素。

"vec_insert(v, idx, val)" - Insert an element at index. | 插入元素。

"vec_remove(v, idx)" - Remove the element at index. | 删除元素。

"vec_get(v, idx)" - Get the element at index. | 获取元素。

"vec_set(v, idx, val)" - Set the element at index. | 设置元素。

# Property Queries | 查询属性

"vec_len(v)" - Get the length (number of elements). | 获取长度。

"vec_cap(v)" - Get the capacity. | 获取容量。

"vec_empty(v)" - Check if the array is empty. | 是否为空。

# Advanced Operations | 高级操作

"vec_sort(v, cmp)" - Sort the array using a comparison function. | 排序。

"vec_filter(v, filter_fn)" - Filter elements based on a predicate function. | 条件过滤。

"vec_map(v, map_fn, out_type)" - Transform elements using a mapping function. | 映射变换。

"vec_reduce(v, init, reduce_fn)" - Reduce the array to a single value. | 归约操作。

"vec_slice(v, start, end)" - Create a slice (sub-array). | 获取切片。

"vec_reverse(v)" - Reverse the array in place. | 反转数组。

# Iterators | 迭代器

"vec_for_each(v, elem)" - Value-based iteration. | 值迭代。

"vec_enumerate(v, idx, elem)" - Iteration with index. | 带索引迭代。

"VecIter(type)" - Struct-based iterator. | 结构体迭代器。

# Display & Debugging | 显示与调试

"vec_show_int(v)" - Display an integer array. | 显示整数数组。

"vec_show(v, fmt)" - Display with a custom format string. | 自定义格式显示。

"vec_show_custom(v, pfn)" - Display using a custom print function. | 自定义打印函数。

# 🔧 Compilation Requirements | 🔧 编译要求
A compiler supporting GNU C extensions (GCC or Clang). | 支持 GNU C 扩展的编译器（GCC 或 Clang）。

C99 standard or higher. | C99 或更高标准。

Standard libraries: | 标准库：

"stdlib.h", 

"string.h", 

"stddef.h", 

"stdbool.h". 

"stdlib.h", 

"string.h", 

"stddef.h", 

"stdbool.h"。

# 📦 Memory Management Strategy | 📦 内存管理策略
Initial Capacity: Minimum of 4 elements. | 初始容量：最小 4 个元素。

Expansion Strategy: Grows by a factor of 2. | 扩容策略：2 倍增长。

Shrinking Condition: When utilization falls below 25%. | 缩容条件：使用率低于 25%。

Minimum Capacity: 4 elements. | 最小容量：4 个元素。

# ⚠️ Notes | ⚠️ 注意事项

 Compiler Compatibility: Only supports GCC/Clang (uses GNU extensions). | 编译器兼容性：仅支持 GCC/Clang（使用 GNU 扩展）。

 Thread Safety: Not thread-safe; requires external synchronization for multi-threaded use. | 线程安全：非线程安全，多线程需外部同步。

 Error Checking: Check return values for critical operations. | 错误检查：重要操作需检查返回值。

 Performance Note: 
 "vec_dedup" is O(n²); for large datasets, sort first. | 性能注意：
"vec_dedup" 为 O(n²)，大数据集建议先排序。

# 📄 License | 📄 许可证

This project is licensed under the Apache-2.0 License. See the "LICENSE" (LICENSE) file for details. | 本项目采用 Apache-2.0 许可证。详见 "LICENSE" (LICENSE) 文件。

# 🤝 Contributing | 🤝 贡献

Issues and Pull Requests are welcome! | 欢迎提交 Issue 和 Pull Request！

# 🎯 Design Philosophy | 🎯 设计理念

C-VEC aims to provide C developers with a modern, safe, and easy-to-use dynamic array experience while maintaining zero dependencies and minimal runtime overhead. The API design is inspired by Rust's 
"Vec", allowing developers familiar with modern languages to get started quickly.

C-VEC 旨在为 C 语言开发者提供现代、安全、易用的动态数组体验，同时保持零依赖和最小运行时开销。API 设计参考 Rust 的 
"Vec"，让熟悉现代语言的开发者能够快速上手。

# Get Started: Just copy 
"vec.h" into your project and enjoy the convenience of modern dynamic arrays! | 开始使用：只需将 
"vec.h" 复制到你的项目中，即可享受现代动态数组的便利！
