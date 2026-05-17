# C-VEC: C 语言动态数组库
一个用单头文件实现的、类似 Rust "Vec" 的 C 语言动态数组库。零依赖，类型安全，API 优雅。

✨ 特性

  - 单头文件：只需包含 vec.h，无需额外依赖。
  - 类型安全：通过宏实现编译时类型检查。
  - 自动内存管理：自动扩容/缩容，无需手动管理。
  - 丰富 API：提供 push/pop/insert/remove/filter/map 等完整操作。
  - 多种迭代方式：支持值迭代、索引迭代、指针迭代。
  - 内存对齐优化：支持 alignof，处理特殊数据类型。
  - 类似 Rust 的 API：熟悉现代语言开发的用户上手快。

🚀 快速开始

1. 引入头文件

#include "vec.h"

2. 基本使用示例

#include "vec.h"
#include <stdio.h>

int main() {
    // 创建数组
    int* nums = vec(int, 1, 2, 3, 4, 5);
    
    // 添加元素
    vec_push(nums, 6);
    vec_push(nums, 7);
    
    // 遍历显示
    printf("数组内容: ");
    vec_for_each(nums, x) {
        printf("%d ", x);
    }
    printf("\n");
    
    // 查找元素
    ssize_t idx = vec_find(nums, 4);
    if (idx >= 0) {
        printf("找到 4 在位置: %zd\n", idx);
    }
    
    // 释放内存
    vec_free(nums);
    
    return 0;
}

📚 API 概览

创建与销毁

  - vec(type, ...) - 从值列表创建。
  - vec_with_capacity(type, cap) - 指定容量创建。
  - vec_clone(v) - 克隆数组。
  - vec_free(v) - 安全释放内存。

基本操作

  - vec_push(v, val) - 添加元素。
  - vec_pop(v) - 弹出并返回最后一个元素。
  - vec_insert(v, idx, val) - 在指定索引插入元素。
  - vec_remove(v, idx) - 删除指定索引的元素。
  - vec_get(v, idx) - 获取指定索引的元素。
  - vec_set(v, idx, val) - 设置指定索引的元素。

查询属性

  - vec_len(v) - 获取长度（元素个数）。
  - vec_cap(v) - 获取容量。
  - vec_empty(v) - 是否为空。

高级操作

  - vec_sort(v, cmp) - 使用比较函数进行排序。
  - vec_filter(v, filter_fn) - 根据谓词函数过滤元素。
  - vec_map(v, map_fn, out_type) - 使用映射函数转换元素。
  - vec_reduce(v, init, reduce_fn) - 将数组规约为单个值。
  - vec_slice(v, start, end) - 获取切片（子数组）。
  - vec_reverse(v) - 原地反转数组。

迭代器

  - vec_for_each(v, elem) - 值迭代。
  - vec_enumerate(v, idx, elem) - 带索引迭代。
  - VecIter(type) - 结构体迭代器。

显示与调试

  - vec_show_int(v) - 显示整数数组。
  - vec_show(v, fmt) - 使用自定义格式字符串显示。
  - vec_show_custom(v, pfn) - 使用自定义打印函数显示。

🔧 编译要求

  - 支持 GNU C 扩展的编译器（如 GCC 或 Clang）。
  - C99 或更高标准。
  - 依赖的标准库：stdlib.h, string.h, stddef.h, stdbool.h。

📦 内存管理策略

  - 初始容量：最小 4 个元素。
  - 扩容策略：2 倍增长。
  - 缩容条件：当利用率低于 25% 时。
  - 最小容量：保持 4 个元素。

⚠️ 注意事项

1.  编译器兼容性：仅支持 GCC/Clang（使用了 GNU 扩展）。
2.  线程安全：非线程安全，多线程使用需外部同步。
3.  错误检查：重要操作建议检查返回值。
4.  性能注意：vec_dedup 复杂度为 O(n²)；对于大数据集，建议先排序再处理。

📄 许可证

本项目采用 Apache-2.0 许可证。详见 LICENSE 文件。

🤝 贡献

欢迎提交 Issue 和 Pull Request！

🎯 设计理念

C-VEC 旨在为 C 语言开发者提供现代、安全、易用的动态数组体验，同时保持零依赖和最小运行时开销。API 设计参考 Rust 的
Vec，让熟悉现代语言的开发者能够快速上手。

开始使用：只需将 vec.h 复制到你的项目中，即可享受现代动态数组的便利！
