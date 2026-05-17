[English](./README_global/README_en.md)
# C-VEC: C 语言动态数组库

一个用单头文件实现的、类似 Rust "Vec" 的 C 语言动态数组库。零依赖，类型安全，API 设计优雅。

# ✨ 特性

- **单头文件**：只需包含 `vec.h`，无需额外依赖，方便集成。
- **类型安全**：通过宏实现编译时类型检查，减少运行时错误。
- **自动内存管理**：自动扩容/缩容，开发者无需手动管理容量细节。
- **丰富 API**：提供 push/pop/insert/remove/filter/map 等完整操作。
- **多种迭代方式**：支持值迭代、索引迭代、和指针迭代。
- **内存对齐优化**：支持 `alignof`，能够正确处理特殊数据类型。
- **类似 Rust 的 API**：熟悉现代编程语言（如 Rust, Go）的开发者可快速上手。

# 🚀 快速开始

### 1. 引入头文件

将 `vec.h` 复制到你的项目中并包含：
```c
#include "vec.h"
```

### 2. 基本使用示例

```c
#include "vec.h"
#include <stdio.h>

int main() {
    // 1. 创建数组并初始化
    int* nums = vec(int, 1, 2, 3, 4, 5);
    
    // 2. 添加元素
    vec_push(nums, 6);
    vec_push(nums, 7);
    
    // 3. 遍历显示
    printf("数组内容: ");
    vec_for_each(nums, x) {
        printf("%d ", x);
    }
    printf("\n");
    
    // 4. 查找元素
    ssize_t idx = vec_find(nums, 4);
    if (idx >= 0) {
        printf("找到数字 4，位置在索引: %zd\n", idx);
    }
    
    // 5. 释放内存
    vec_free(nums);
    
    return 0;
}
```

# 📚 API 概览

### 创建与销毁
- `vec(type, ...)`：从初始值列表创建数组。
- `vec_with_capacity(type, cap)`：指定初始容量创建空数组。
- `vec_clone(v)`：克隆一个现有的数组。
- `vec_free(v)`：安全释放数组内存。

### 基本操作
- `vec_push(v, val)`：在末尾添加一个元素。
- `vec_pop(v)`：弹出并返回最后一个元素。
- `vec_insert(v, idx, val)`：在指定索引处插入元素。
- `vec_remove(v, idx)`：删除指定索引处的元素。
- `vec_get(v, idx)`：获取指定索引的元素值。
- `vec_set(v, idx, val)`：设置指定索引的元素值。

### 属性查询
- `vec_len(v)`：获取数组当前的元素个数。
- `vec_cap(v)`：获取数组当前的内存容量。
- `vec_empty(v)`：判断数组是否为空。

### 高级操作
- `vec_sort(v, cmp)`：使用自定义比较函数进行排序。
- `vec_filter(v, filter_fn)`：根据谓词函数过滤元素。
- `vec_map(v, map_fn, out_type)`：通过映射函数转换元素。
- `vec_reduce(v, init, reduce_fn)`：将数组归约为单一值。
- `vec_slice(v, start, end)`：获取子数组切片。
- `vec_reverse(v)`：原地反转数组。

### 迭代器
- `vec_for_each(v, elem)`：值迭代。
- `vec_enumerate(v, idx, elem)`：带索引迭代。
- `VecIter(type)`：结构体迭代器，用于更复杂的遍历逻辑。

### 显示与调试
- `vec_show_int(v)`：打印整数数组内容。
- `vec_show(v, fmt)`：根据自定义格式化字符串打印。
- `vec_show_custom(v, pfn)`：使用自定义打印函数显示。

# 🔧 编译要求

- **编译器**：需支持 GNU C 扩展的编译器（如 GCC 或 Clang）。
- **标准**：C99 或更高标准。
- **标准库依赖**：`stdlib.h`, `string.h`, `stddef.h`, `stdbool.h`。

# 📦 内存管理策略

- **初始容量**：最小为 4 个元素。
- **扩容策略**：当空间不足时，容量翻倍（2x）。
- **缩容策略**：当空间利用率低于 25% 时自动缩容。
- **最小限制**：缩容后最小保留 4 个元素的空间。

# ⚠️ 注意事项

- **编译器兼容性**：由于使用了 GNU 扩展（如 `__typeof__` 和语句表达式），仅支持 GCC 和 Clang。
- **线程安全**：本库非线程安全。若在多线程环境下使用，需自行加锁。
- **错误检查**：对于关键操作，建议通过 `vec_len` 等检查结果。
- **性能提示**：`vec_dedup`（去重）操作的时间复杂度为 $O(n^2)$，对于大数据集建议先排序。

# 📄 许可证

本项目采用 **Apache-2.0** 许可证。详见 `LICENSE` 文件。

# 🎯 设计理念

C-VEC 旨在为 C 语言开发者提供一种现代、安全、易用的动态数组体验，同时保持零依赖和极小的运行时开销。API 设计深度参考了 Rust 的 `Vec` 容器，旨在让习惯了现代语言开发的程序员在编写底层 C 代码时也能感受到便利。

---

**开始使用**：只需将 `vec.h` 复制到你的项目中，即可开启现代动态数组的开发之旅！
