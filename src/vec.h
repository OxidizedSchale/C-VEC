/**
 * vec.h
 * 功能完整的C语言动态数组库
 * 只支持 GCC 与 Clang 编译器
 * license Apache-2.0
 * 
 * 特性：
 * - 类似Rust Vec的API设计
 * - 自动内存管理（扩容/缩容）
 * - 丰富的操作：push/pop/insert/remove/filter/map等
 * - 多种迭代方式
 * - 类型安全的宏接口
 * - 单头文件，零依赖
 * - 内存对齐优化
 * 
 * 示例：
 * 
 * int* nums = vec(int, 1, 2, 3);
 * vec_push(nums, 4);
 * vec_for_each(nums, x) printf("%d ", x);
 * vec_free(nums);
 * 
 */

#ifndef VEC_H
#define VEC_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdalign.h>

/* --- 元数据结构 --- */
typedef struct {
    size_t len;
    size_t cap;
    size_t stride;
    size_t align;
    size_t header_size; /* 存储从底层块起始到数据区起始的字节偏移 */
} VecHeader;

/* --- 内部辅助函数 --- */

/**
 * 获取 Header 的指针
 * 逻辑：数据指针 v 的前一个 size_t 存储了 header_size 的值
 */
#define _vec_h(v) ((VecHeader*)((char*)(v) - *((size_t*)(v) - 1)))

/**
 * 计算对齐后的起始偏移
 * 必须保证存储 VecHeader 后，剩下的起始地址符合 align 要求，
 * 且额外留出一个 size_t 空间给 _vec_h 快速寻址。
 */
static inline size_t _vec_calculate_h_size(size_t align) {
    size_t min_h = sizeof(VecHeader);
    /* 必须留出至少一个 size_t 来存偏移量，且满足 align */
    size_t offset = (min_h + align - 1) & ~(align - 1);
    if (offset - min_h < sizeof(size_t)) {
        offset += align;
    }
    return offset;
}

static inline void* _vec_grow(void* v, size_t inc, size_t stride, size_t align) {
    size_t cur_len = v ? _vec_h(v)->len : 0;
    size_t cur_cap = v ? _vec_h(v)->cap : 0;
    if (cur_len + inc <= cur_cap) return v;

    size_t new_cap = cur_cap == 0 ? (inc > 4 ? inc : 4) : cur_cap * 2 + inc;
    size_t h_size = _vec_calculate_h_size(align);
    size_t total_size = h_size + new_cap * stride;

    void* base = v ? (char*)v - _vec_h(v)->header_size : NULL;
    void* new_ptr = realloc(base, total_size);
    if (!new_ptr) return NULL;

    VecHeader* h = (VecHeader*)new_ptr;
    h->cap = new_cap;
    h->stride = stride;
    h->align = align;
    h->header_size = h_size;
    if (!v) h->len = 0;

    void* data_ptr = (char*)new_ptr + h_size;
    /* 在数据区前一个 size_t 位置埋下偏移量 */
    *((size_t*)data_ptr - 1) = h_size;

    return data_ptr;
}

static inline void* _vec_shrink(void* v, size_t stride, size_t align) {
    if (!v) return NULL;
    VecHeader* h = _vec_h(v);
    if (h->len * 4 >= h->cap || h->cap <= 4) return v;

    size_t new_cap = h->len > 0 ? h->len : 1;
    size_t h_size = h->header_size;
    void* new_ptr = realloc((char*)v - h_size, h_size + new_cap * stride);
    if (!new_ptr) return v;

    ((VecHeader*)new_ptr)->cap = new_cap;
    void* data_ptr = (char*)new_ptr + h_size;
    *((size_t*)data_ptr - 1) = h_size;
    return data_ptr;
}

/* --- 公共 API 宏 --- */

#define vec_len(v) ((v) ? _vec_h(v)->len : 0)
#define vec_cap(v) ((v) ? _vec_h(v)->cap : 0)

#define vec_free(v) __extension__({ \
    typeof(v) _v = (v); \
    if (_v) free((char*)_v - _vec_h(_v)->header_size); \
    (v) = NULL; \
})

#define vec_push(v, val) __extension__({ \
    typeof(v) _v = (v); \
    bool _ok = false; \
    void* _res = _vec_grow(_v, 1, sizeof(*_v), alignof(*_v)); \
    if (_res) { \
        (v) = (typeof(v))_res; \
        (v)[_vec_h(v)->len++] = (val); \
        _ok = true; \
    } \
    _ok; \
})

#define vec_pop(v) __extension__({ \
    typeof(v) _v = (v); \
    assert(_v && _vec_h(_v)->len > 0); \
    _v[--_vec_h(_v)->len]; \
})

#define vec_insert(v, idx, val) __extension__({ \
    typeof(v) _v = (v); \
    size_t _i = (idx); \
    bool _ok = false; \
    if (_i <= vec_len(_v)) { \
        void* _res = _vec_grow(_v, 1, sizeof(*_v), alignof(*_v)); \
        if (_res) { \
            (v) = (typeof(v))_res; \
            size_t _l = _vec_h(v)->len; \
            if (_i < _l) \
                memmove(&(v)[_i + 1], &(v)[_i], (_l - _i) * sizeof(*(v))); \
            (v)[_i] = (val); \
            _vec_h(v)->len++; \
            _ok = true; \
        } \
    } \
    _ok; \
})

#define vec_remove(v, idx) __extension__({ \
    typeof(v) _v = (v); \
    size_t _i = (idx); \
    if (_v && _i < _vec_h(_v)->len) { \
        size_t _l = _vec_h(_v)->len; \
        if (_i < _l - 1) \
            memmove(&_v[_i], &_v[_i + 1], (_l - _i - 1) * sizeof(*_v)); \
        _vec_h(_v)->len--; \
        (v) = (typeof(v))_vec_shrink(_v, sizeof(*_v), _vec_h(_v)->align); \
    } \
})

/* 迭代器逻辑：修正副作用 */
#define vec_for_each(v, elem) \
    for (size_t _i = 0, _once = 1; _once && (v); _once = 0) \
        for (size_t _len = _vec_h(v)->len; _i < _len && ((elem) = (v)[_i], 1); ++_i)

#define vec_enumerate(v, idx, elem) \
    for (size_t idx = 0, _once = 1; _once && (v); _once = 0) \
        for (size_t _len = _vec_h(v)->len; idx < _len && ((elem) = (v)[idx], 1); ++idx)

/* 初始化宏：修复副作用并支持对齐 */
#define vec(type, ...) __extension__({ \
    type _tmp[] = { __VA_ARGS__ }; \
    size_t _n = sizeof(_tmp)/sizeof(type); \
    type* _v_ptr = (type*)_vec_grow(NULL, _n, sizeof(type), alignof(type)); \
    if (_v_ptr) { \
        memcpy(_v_ptr, _tmp, sizeof(_tmp)); \
        _vec_h(_v_ptr)->len = _n; \
    } \
    _v_ptr; \
})

#define vec_clear(v) do { \
    if (v) { _vec_h(v)->len = 0; (v) = _vec_shrink((v), sizeof(*(v)), _vec_h(v)->align); } \
} while(0)

/* --- 函数式操作 --- */

#define vec_filter(v, filter_fn) __extension__({ \
    typeof(v) _v = (v); \
    typeof(v) _res = NULL; \
    if (_v) { \
        _res = (typeof(v))_vec_grow(NULL, _vec_h(_v)->len, sizeof(*_v), _vec_h(_v)->align); \
        if (_res) { \
            size_t _nl = 0; \
            for(size_t _i=0; _i < _vec_h(_v)->len; ++_i) \
                if(filter_fn(&_v[_i])) _res[_nl++] = _v[_i]; \
            _vec_h(_res)->len = _nl; \
            _res = (typeof(v))_vec_shrink(_res, sizeof(*_res), _vec_h(_res)->align); \
        } \
    } \
    _res; \
})

#define vec_map(v, map_fn, out_type) __extension__({ \
    typeof(v) _v = (v); \
    out_type* _res = NULL; \
    if (_v) { \
        size_t _l = _vec_h(_v)->len; \
        _res = (out_type*)_vec_grow(NULL, _l, sizeof(out_type), alignof(out_type)); \
        if (_res) { \
            for(size_t _i=0; _i<_l; ++_i) _res[_i] = map_fn(&_v[_i]); \
            _vec_h(_res)->len = _l; \
        } \
    } \
    _res; \
})

/* --- 调试显示 --- */
#define vec_show(v, fmt) do { \
    typeof(v) _v_dbg = (v); \
    if (_v_dbg) { \
        printf("Vec<%zu/%zu>: [", _vec_h(_v_dbg)->len, _vec_h(_v_dbg)->cap); \
        for(size_t _i=0; _i < _vec_h(_v_dbg)->len; ++_i) { \
            printf(fmt, _v_dbg[_i]); if(_i < _vec_h(_v_dbg)->len-1) printf(", "); \
        } \
        printf("]\n"); \
    } else printf("Vec: (null)\n"); \
} while(0)

#endif /* VEC_H */
