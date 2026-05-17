[English](./README_global/README_en.md)

# C-VEC 🚀

**A Modern, Type-Safe, Rust-inspired Dynamic Array for C (GCC/Clang)**

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![C](https://img.shields.io/badge/C-11%2F17%2F23-orange.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Compiler](https://img.shields.io/badge/Compiler-GCC%2FClang-green.svg)](#)

`C-VEC` 是一个极其轻量、单头文件的 C 语言动态数组库。它不仅在 API 上致敬了 Rust 的 `Vec`，更利用现代编译器特性解决了 C 语言动态数组长期以来的痛点：内存管理混乱、类型不安全以及难以对齐。

---

## ✨ 核心特性

*   **🛡️ 真正的 RAII 支持**: 利用 GNU 扩展 `__attribute__((cleanup))`，实现变量作用域结束自动释放。
*   **💉 原地指针更新**: 告别 `v = vec_push(v, x)`！宏会自动捕获地址并更新你的指针，防止 `realloc` 后的野指针问题。
*   **🔒 工业级安全性**: 
    *   **Realloc 保护**: 分配失败时保留原内存，绝不因失败导致内存泄漏。
    *   **零副作用**: 宏内部参数捕获，即使传入 `vec_push(v, i++)` 也不会导致逻辑崩溃。
*   **📐 智能对齐**: 自动处理内存对齐（Alignment），完美支持 SIMD 等对内存边界敏感的数据类型。
*   **🍎 函数式风格**: 内置 `filter`、`map`、`enumerate` 等迭代工具。
*   **📦 单头文件**: 零依赖，拷贝 `vec.h` 即可食用。

---

## 🚀 快速上手

### 1. 基础用法
```c
#include "vec.h"

int main() {
    // 像 Rust 一样初始化
    int* nums = vec(int, 1, 2, 3);

    // 原地推入元素，不需要重新赋值给 nums
    vec_push(nums, 4);
    vec_push(nums, 5);

    // 优雅的迭代
    vec_for_each(nums, x) {
        printf("%d ", x);
    }

    vec_free(nums);
    return 0;
}
```

### 2. 开启 RAII 模式 (自动释放)
再也不用担心函数分支过多导致的 `free` 遗漏：
```c
void heavy_logic() {
    // 作用域结束时自动调用 vec_free
    vec_auto(float*) data = vec(float, 1.0f, 2.0f);
    
    if (some_error) return; // 自动释放！
    
    vec_push(data, 3.0f);
} // 自动释放！
```

---

## 🛠 技术深度：内存布局

C-VEC 采用了一种改进的 **"Hidden Header with Buried Offset"** 设计，以支持严格的内存对齐：

```text
[ VecHeader | Padding | HeaderOffset (size_t) ] [ User Data (Aligned) ]
                                ^                     ^
                                |                     |
                          元数据回溯索引          用户持有的指针 (v)
```

当调用 `vec_push` 时，宏会通过 `&v` 获取你的指针地址。如果发生扩容，它会透明地修改你原本的指针变量，确保你持有的永远是有效的地址。

---

## 📖 API 概览

### 生命周期
| 宏 / 函数 | 描述 |
| :--- | :--- |
| `vec(type, ...)` | 创建并初始化 Vec |
| `vec_auto(type)` | (RAII) 声明一个自动释放的 Vec |
| `vec_free(v)` | 手动释放内存并置空 |
| `vec_clear(v)` | 清空元素，保持容量 |

### 动态操作
| 宏 / 函数 | 描述 |
| :--- | :--- |
| `vec_push(v, val)` | **(原地更新)** 末尾压入元素 |
| `vec_pop(v)` | 弹出末尾元素 |
| `vec_insert(v, idx, val)` | 在指定索引处插入 |
| `vec_remove(v, idx)` | 删除指定索引元素（保持顺序） |

### 迭代与函数式
| 宏 / 函数 | 描述 |
| :--- | :--- |
| `vec_for_each(v, elem)` | 遍历元素 |
| `vec_enumerate(v, i, elem)` | 带索引遍历 |
| `vec_filter(v, fn)` | 过滤元素并返回新 Vec |
| `vec_map(v, fn, out_type)` | 映射元素并返回新 Vec |

---

## ⚠️ 限制

*   **编译器**: 仅支持 **GCC** 或 **Clang**（依赖 `__extension__`, `typeof` 和 `cleanup` 属性）。
*   **标准**: 建议开启 `C11` 或更高版本。

---

## 📜 许可证

本项目采用 [Apache-2.0](LICENSE) 许可证。

---

**OxidizedSchale** 制作 
"Make C feel like Rust, one macro at a time."
