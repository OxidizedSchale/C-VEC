[English](./README_global/README_en.md)

# C-VEC 🚀

**一个受 Rust 启发、具备工业级安全性的现代 C 语言动态数组库 (GCC/Clang)**

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![C](https://img.shields.io/badge/C-11%2F17%2F23-orange.svg)](#)
[![Compiler](https://img.shields.io/badge/Compiler-GCC%2FClang-green.svg)](#)

`C-VEC` 是一个轻量级、单头文件的动态数组库。它不仅在 API 上致敬了 Rust 的 `Vec`，更通过巧妙的宏设计解决了 C 语言动态数组开发中的三大痛点：**内存泄漏、繁琐的指针重分配赋值、以及复杂的内存对齐。**

---

## ✨ 核心特性

*   **🛡️ 真正的 RAII 支持**: 借助 GNU 扩展，提供 `vec_auto` 宏。声明的变量在作用域结束时自动释放，彻底告别 `free` 遗漏导致的内存泄漏。
*   **💉 原地指针更新 (Black Magic)**: 告别 `v = vec_push(v, x)`！`C-VEC` 自动捕获变量地址，在 `realloc` 发生后自动更新你的指针变量，确保指针永远有效。
*   **🔒 极高的健壮性**:
    *   **Realloc 保护**: 内存分配失败时自动保留原内存块，不会丢失数据指针。
    *   **副作用安全**: 宏内部严格控制参数计算次数，即使传入 `vec_push(v, i++)` 也能正常工作。
*   **📐 完美的对齐支持**: 自动处理 `alignof(T)`。无论是基础类型还是 32 字节对齐的 SIMD 数据结构，内存布局始终合法。
*   **🍎 函数式编程接口**: 内置 `vec_filter`、`vec_map`、`vec_extend`（拼接）等高级操作，让 C 语言代码更具表现力。
*   **📦 零依赖**: 单头文件设计，直接拷贝到项目中即可使用。

---

## 🚀 快速上手

### 1. 基础用法 (原地更新)
不需要将返回值赋给原变量，宏会自动处理一切。
```c
#include "vec.h"

int main() {
    int* nums = vec(int, 1, 2, 3);

    // 原地推入，nums 指针如果变动会自动更新
    vec_push(nums, 4);
    vec_push(nums, 5);

    vec_show(nums, "%d"); // 输出: Vec<5/8>: [1, 2, 3, 4, 5]
    
    vec_free(nums);
    return 0;
}
```

### 2. 自动释放 (RAII)
像 C++ 或 Rust 一样管理生命周期。
```c
void some_function() {
    // 作用域结束时自动释放内存
    vec_auto(float*) scores = vec(float, 95.0, 88.5);
    
    if (error_occurred) return; // 自动释放，无泄漏！
    
    vec_push(scores, 100.0);
} 
```

### 3. 向量拼接与函数式操作
```c
int* v1 = vec(int, 1, 2);
int* v2 = vec(int, 3, 4);

vec_extend(v1, v2); // 将 v2 拼接到 v1 末尾

// 过滤出所有偶数，生成新 Vec
int* evens = vec_filter(v1, is_even); 
```

---

## 🛠 技术深度：内存布局

`C-VEC` 使用了一种创新的 **“偏移量埋点 (Buried Offset)”** 设计来支持严格对齐：

```text
[ VecHeader | 填充区 | 头部偏移量 (size_t) ] [ 用户数据区 (严格对齐) ]
                                 ^                      ^
                                 |                      |
                        Header 快速寻址点          用户持有的指针 (v)
```

当调用 `vec_push(v, val)` 时，宏通过 `&v` 获取你的指针变量地址。如果 `realloc` 导致内存搬迁，宏会直接修改你的 `v` 变量，使其指向新的数据区。

---

## 📖 API 参考

### 生命周期与管理
| 宏 / 函数 | 说明 | 复杂度 |
| :--- | :--- | :--- |
| `vec(type, ...)` | 创建并初始化 Vec | $O(n)$ |
| `vec_auto(type)` | (RAII) 声明一个自动释放的 Vec | - |
| `vec_free(v)` | 手动释放内存并清空指针 | $O(1)$ |
| `vec_reserve(v, n)` | 预留指定大小的容量 | $O(1)$ |

### 动态操作
| 宏 / 函数 | 说明 | 复杂度 |
| :--- | :--- | :--- |
| `vec_push(v, val)` | **(原地更新)** 末尾压入元素 | 均摊 $O(1)$ |
| `vec_pop(v)` | 弹出并返回末尾元素 | $O(1)$ |
| `vec_extend(dst, src)` | **(原地更新)** 将 src 拼接到 dst | $O(n)$ |
| `vec_insert(v, idx, val)` | 在指定索引处插入元素 | $O(n)$ |
| `vec_remove(v, idx)` | 删除指定索引的元素 | $O(n)$ |

### 函数式与迭代
| 宏 / 函数 | 说明 | 复杂度 |
| :--- | :--- | :--- |
| `vec_for_each(v, var)` | 遍历所有元素 | $O(n)$ |
| `vec_enumerate(v, i, var)`| 带索引遍历 | $O(n)$ |
| `vec_filter(v, pred)` | 根据条件过滤并返回新 Vec | $O(n)$ |
| `vec_map(v, fn, type)` | 映射转换并返回新类型的 Vec | $O(n)$ |

---

## ⚠️ 注意事项

*   **编译器**: 必须使用 **GCC** 或 **Clang**。不支持 MSVC（因为它缺乏 `typeof` 和 `cleanup` 扩展）。
*   **迭代器失效**: 尽管 `vec_push` 会更新主指针，但在扩容后，任何指向 Vec 内部元素的**旧临时指针**都会失效，请务必小心。

---

## 📜 许可证

本项目基于 [Apache-2.0](LICENSE) 许可证开源。

---

**由 OxidizedSchale 倾力打造**
*"让 C 语言写起来像 Rust 一样顺滑。"*
