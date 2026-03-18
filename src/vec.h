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
} VecHeader;

/* --- 底层核心辅助 --- */
static inline size_t _vec_h_size(size_t align) {
    return (sizeof(VecHeader) + align - 1) & ~(align - 1);
}

#define _vec_h(v) ((VecHeader*)((char*)(v) - _vec_h_size(((VecHeader*)((char*)(v) - sizeof(VecHeader)))->align)))

static inline void* _vec_grow(void* v, size_t inc, size_t stride, size_t align) {
    size_t cur_len = v ? _vec_h(v)->len : 0;
    size_t cur_cap = v ? _vec_h(v)->cap : 0;
    if (cur_len + inc <= cur_cap) return v;
    size_t new_cap = cur_cap == 0 ? inc : cur_cap * 2 + inc;
    size_t h_size = _vec_h_size(align);
    void* new_ptr = realloc(v ? (char*)v - h_size : NULL, h_size + new_cap * stride);
    if (!new_ptr) return NULL;
    VecHeader* h = (VecHeader*)new_ptr;
    if (!v) h->len = 0;
    h->cap = new_cap; h->stride = stride; h->align = align;
    return (char*)new_ptr + h_size;
}

static inline void* _vec_shrink(void* v, size_t stride, size_t align) {
    if (!v) return NULL;
    VecHeader* h = _vec_h(v);
    if (h->len * 4 >= h->cap || h->cap <= 4) return v;
    size_t new_cap = h->len > 0 ? h->len : 1;
    size_t h_size = _vec_h_size(align);
    void* new_ptr = realloc((char*)v - h_size, h_size + new_cap * stride);
    if (!new_ptr) return v;
    ((VecHeader*)new_ptr)->cap = new_cap;
    return (char*)new_ptr + h_size;
}

/* --- 基础 API --- */
#define vec_len(v) ((v) ? _vec_h(v)->len : 0)
#define vec_cap(v) ((v) ? _vec_h(v)->cap : 0)
#define vec_stride(v) ((v) ? _vec_h(v)->stride : 0)
#define vec_align(v) ((v) ? _vec_h(v)->align : 0)

#define vec_free(v) do { \
    if (v) { free((char*)(v) - _vec_h_size(_vec_h(v)->align)); (v) = NULL; } \
} while(0)

#define vec_push(v, val) __extension__({ \
    void* _tmp = _vec_grow((v), 1, sizeof(*(v)), alignof(*(v))); \
    if (_tmp) { (v) = _tmp; (v)[_vec_h(v)->len++] = (val); } \
    _tmp != NULL; \
})

#define vec_pop(v) ((v) && vec_len(v) > 0 ? (v)[--_vec_h(v)->len] : (typeof(*(v))){0})

#define vec_insert(v, idx, val) do { \
    if ((v) && (idx) <= vec_len(v)) { \
        void* _tmp = _vec_grow((v), 1, sizeof(*(v)), _vec_h(v)->align); \
        if (_tmp) { \
            (v) = _tmp; \
            if ((idx) < vec_len(v)) \
                memmove((v)+(idx)+1, (v)+(idx), (vec_len(v)-(idx))*sizeof(*(v))); \
            (v)[idx] = (val); \
            _vec_h(v)->len++; \
        } \
    } \
} while(0)

#define vec_remove(v, idx) do { \
    size_t _i = (idx); \
    if ((v) && _i < vec_len(v)) { \
        memmove((v) + _i, (v) + _i + 1, (vec_len(v) - _i - 1) * sizeof(*(v))); \
        _vec_h(v)->len--; \
        (v) = _vec_shrink((v), sizeof(*(v)), _vec_h(v)->align); \
    } \
} while(0)

#define vec_set(v, idx, val) do { \
    if ((v) && (idx) < vec_len(v)) (v)[idx] = (val); \
} while(0)

#define vec_get(v, idx) ((v) && (idx) < vec_len(v) ? (v)[idx] : (typeof(*(v))){0})

#define vec_get_ptr(v, idx) ((v) && (idx) < vec_len(v) ? &(v)[idx] : NULL)

#define vec_clear(v) do { \
    if (v) { _vec_h(v)->len = 0; (v) = _vec_shrink((v), sizeof(*(v)), _vec_h(v)->align); } \
} while(0)

#define vec_swap(v, i, j) do { \
    if ((v) && (i) < vec_len(v) && (j) < vec_len(v)) { \
        typeof(*(v)) _tmp = (v)[i]; \
        (v)[i] = (v)[j]; \
        (v)[j] = _tmp; \
    } \
} while(0)

#define vec_reverse(v) do { \
    if (v) { \
        size_t _len = vec_len(v); \
        for (size_t _i = 0; _i < _len / 2; ++_i) { \
            typeof(*(v)) _tmp = (v)[_i]; \
            (v)[_i] = (v)[_len - _i - 1]; \
            (v)[_len - _i - 1] = _tmp; \
        } \
    } \
} while(0)

#define vec_find(v, val) __extension__({ \
    ssize_t _result = -1; \
    if (v) for (size_t _i = 0; _i < vec_len(v); ++_i) \
        if ((v)[_i] == (val)) { _result = _i; break; } \
    _result; \
})

#define vec_contains(v, val) (vec_find(v, val) >= 0)

/* --- 高级操作 --- */
#define vec_sort(v, cmp) do { \
    if ((v) && vec_len(v) > 1) qsort((v), vec_len(v), sizeof(*(v)), cmp); \
} while(0)

#define vec_shrink_to_fit(v) do { \
    if (v) { \
        size_t h_size = _vec_h_size(_vec_h(v)->align); \
        void* _tmp = realloc((char*)(v) - h_size, h_size + vec_len(v) * sizeof(*(v))); \
        if (_tmp) { ((VecHeader*)_tmp)->cap = vec_len(v); (v) = (char*)_tmp + h_size; } \
    } \
} while(0)

#define vec_slice(v, start, end) __extension__({ \
    typeof(v) _res = NULL; \
    size_t _s = (start), _e = (end), _l = vec_len(v); \
    if ((v) && _s < _e && _s < _l) { \
        if (_e > _l) _e = _l; \
        size_t _new_len = _e - _s; \
        _res = _vec_grow(NULL, _new_len, sizeof(*(v)), _vec_h(v)->align); \
        if (_res) { memcpy(_res, (v) + _s, _new_len * sizeof(*(v))); _vec_h(_res)->len = _new_len; } \
    } \
    _res; \
})

#define vec_drain(v, start, end) do { \
    size_t _s = (start), _e = (end), _l = vec_len(v); \
    if ((v) && _s < _e && _s < _l) { \
        if (_e > _l) _e = _l; \
        size_t _count = _e - _s; \
        memmove((v) + _s, (v) + _e, (_l - _e) * sizeof(*(v))); \
        _vec_h(v)->len -= _count; \
        (v) = _vec_shrink((v), sizeof(*(v)), _vec_h(v)->align); \
    } \
} while(0)

#define vec_map(v, map_fn, out_type) __extension__({ \
    out_type* _res = NULL; \
    if (v) { \
        size_t _l = vec_len(v); \
        _res = _vec_grow(NULL, _l, sizeof(out_type), alignof(out_type)); \
        if (_res) { \
            for(size_t _i=0; _i<_l; ++_i) _res[_i] = *(out_type*)map_fn(&(v)[_i]); \
            _vec_h(_res)->len = _l; \
        } \
    } \
    _res; \
})

#define vec_filter(v, filter_fn) __extension__({ \
    typeof(v) _res = NULL; \
    if (v) { \
        _res = _vec_grow(NULL, vec_len(v), sizeof(*(v)), _vec_h(v)->align); \
        if (_res) { \
            size_t _nl = 0; \
            for(size_t _i=0; _i<vec_len(v); ++_i) \
                if(filter_fn(&(v)[_i])) _res[_nl++] = (v)[_i]; \
            _vec_h(_res)->len = _nl; \
            _res = _vec_shrink(_res, sizeof(*(v)), _vec_h(_res)->align); \
        } \
    } \
    _res; \
})

#define vec_repeat(type, val, count) __extension__({ \
    type* _v = _vec_grow(NULL, (count), sizeof(type), alignof(type)); \
    if (_v) { \
        for(size_t _i=0; _i<(count); ++_i) _v[_i] = (val); \
        _vec_h(_v)->len = (count); \
    } \
    _v; \
})

#define vec_dedup(v) __extension__({ \
    typeof(v) _res = NULL; \
    if (v) { \
        _res = _vec_grow(NULL, vec_len(v), sizeof(*(v)), _vec_h(v)->align); \
        size_t _nl = 0; \
        for(size_t _i=0; _i<vec_len(v); ++_i) { \
            bool _exists = false; \
            for(size_t _j=0; _j<_nl; ++_j) \
                if(memcmp(&(v)[_i], &_res[_j], sizeof(*(v))) == 0) { _exists = true; break; } \
            if(!_exists) _res[_nl++] = (v)[_i]; \
        } \
        _vec_h(_res)->len = _nl; \
        _res = _vec_shrink(_res, sizeof(*(v)), _vec_h(_res)->align); \
    } \
    _res; \
})

#define vec_dedup_sorted(v) do { \
    if ((v) && vec_len(v) > 1) { \
        size_t _nl = 1; \
        for(size_t _i=1; _i<vec_len(v); ++_i) \
            if(memcmp(&(v)[_i], &(v)[_nl-1], sizeof(*(v))) != 0) (v)[_nl++] = (v)[_i]; \
        _vec_h(v)->len = _nl; \
        (v) = _vec_shrink((v), sizeof(*(v)), _vec_h(v)->align); \
    } \
} while(0)

#define vec_unique(v, cmp) do { \
    if (v) { \
        vec_sort(v, cmp); \
        vec_dedup_sorted(v); \
    } \
} while(0)

/* --- 迭代器宏 --- */
#define vec_for_each(v, elem) \
    for (size_t _i = 0, _len = vec_len(v); _i < _len && ((elem) = (v)[_i], 1); ++_i)

#define vec_enumerate(v, idx, elem) \
    for (size_t idx = 0, _len = vec_len(v); idx < _len && ((elem) = (v)[idx], 1); ++idx)

#define VecIter(type) struct { type* ptr; size_t idx; size_t len; }
#define vec_iter(v) { (v), 0, vec_len(v) }
#define iter_next(it) ((it).idx < (it).len ? &(it).ptr[(it).idx++] : NULL)
#define iter_has_next(it) ((it).idx < (it).len)
#define iter_reset(it) ((it).idx = 0)

/* --- 显示功能 --- */
#define vec_show(v, fmt) do { \
    if (v) { \
        printf("Vec<%zu/%zu>: [", vec_len(v), vec_cap(v)); \
        for(size_t _i=0; _i<vec_len(v); ++_i) { \
            printf(fmt, (v)[_i]); if(_i < vec_len(v)-1) printf(", "); \
        } \
        printf("]\n"); \
    } else printf("Vec: (null)\n"); \
} while(0)

#define vec_show_custom(v, pfn) do { \
    if (v) { \
        printf("Vec<%zu/%zu>: [", vec_len(v), vec_cap(v)); \
        for(size_t _i=0; _i<vec_len(v); ++_i) { \
            pfn(&(v)[_i]); if(_i < vec_len(v)-1) printf(", "); \
        } \
        printf("]\n"); \
    } else printf("Vec: (null)\n"); \
} while(0)

#define vec_show_int(v) vec_show(v, "%d")
#define vec_show_char(v) vec_show(v, "'%c'")
#define vec_show_string(v) vec_show(v, "\"%s\"")
#define vec_show_pointer(v) vec_show(v, "%p")
#define vec_show_float(v) vec_show(v, "%.2f")
#define vec_show_double(v) vec_show(v, "%.4f")

/* --- 初始化器 --- */
#define vec(type, ...) __extension__({ \
    type _tmp[] = { __VA_ARGS__ }; \
    size_t _n = sizeof(_tmp)/sizeof(type); \
    type* _v = _vec_grow(NULL, _n, sizeof(type), alignof(type)); \
    if (_v) { memcpy(_v, _tmp, sizeof(_tmp)); _vec_h(_v)->len = _n; } \
    _v; \
})

#define vec_with_capacity(type, cap) \
    (type*)_vec_grow(NULL, (cap), sizeof(type), alignof(type))

#define vec_clone(v) __extension__({ \
    typeof(v) _res = NULL; \
    if (v) { \
        size_t _len = vec_len(v); \
        _res = _vec_grow(NULL, _len, sizeof(*(v)), _vec_h(v)->align); \
        if (_res) { \
            memcpy(_res, v, _len * sizeof(*(v))); \
            _vec_h(_res)->len = _len; \
        } \
    } \
    _res; \
})

#endif /* VEC_H */
