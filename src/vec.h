/**
 * C-VEC: A modern, type-safe dynamic array for C (GCC/Clang)
 * Features: RAII, In-place updates, Auto-alignment, Functional APIs
 * License: Apache-2.0
 */

#ifndef VEC_H
#define VEC_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdalign.h>
#include <stdio.h>

/* --- Metadata Structure --- */
typedef struct {
    size_t len;
    size_t cap;
    size_t stride;
    size_t align;
    size_t header_size; 
} VecHeader;

/* --- Internal Implementation --- */

static inline VecHeader* _vec_get_header(void* v) {
    if (!v) return NULL;
    size_t offset = *((size_t*)v - 1);
    return (VecHeader*)((char*)v - offset);
}

static inline size_t _vec_calculate_offset(size_t align) {
    size_t min_h = sizeof(VecHeader);
    // Ensure header is followed by a size_t for the offset, and data is aligned
    size_t offset = (min_h + align - 1) & ~(align - 1);
    if (offset - min_h < sizeof(size_t)) offset += align;
    return offset;
}

static inline void* _vec_grow_internal(void* v, size_t inc, size_t stride, size_t align) {
    VecHeader* h = _vec_get_header(v);
    size_t cur_len = h ? h->len : 0;
    size_t cur_cap = h ? h->cap : 0;
    if (cur_len + inc <= cur_cap) return v;

    size_t new_cap = cur_cap == 0 ? (inc > 4 ? inc : 4) : (cur_cap * 2 + inc);
    size_t h_size = _vec_calculate_offset(align);
    void* base = h ? (void*)h : NULL;
    void* new_ptr = realloc(base, h_size + new_cap * stride);
    if (!new_ptr) return NULL;

    h = (VecHeader*)new_ptr;
    h->cap = new_cap;
    h->stride = stride;
    h->align = align;
    h->header_size = h_size;
    if (!v) h->len = 0;

    void* data_ptr = (char*)new_ptr + h_size;
    *((size_t*)data_ptr - 1) = h_size; // Bury the offset
    return data_ptr;
}

static inline void _vec_free_internal(void* v_ptr) {
    void** v = (void**)v_ptr;
    if (v && *v) {
        free(_vec_get_header(*v));
        *v = NULL;
    }
}

/* --- Public API Macros --- */

/**
 * @brief RAII support: automatically frees the vector when scope ends.
 * Usage: vec_auto(int*) nums = vec(int, 1, 2);
 */
#define vec_auto(type) __attribute__((cleanup(_vec_free_internal))) type

// Basic Properties
#define vec_len(v) (__extension__({ VecHeader* _h = _vec_get_header(v); _h ? _h->len : 0; }))
#define vec_cap(v) (__extension__({ VecHeader* _h = _vec_get_header(v); _h ? _h->cap : 0; }))
#define vec_free(v) _vec_free_internal(&(v))

/**
 * @brief Push an element. Automatically updates the pointer if reallocated.
 */
#define vec_push(v, val) __extension__({ \
    typeof(&(v)) _v_ptr = &(v); \
    void* _res = _vec_grow_internal(*_v_ptr, 1, sizeof(**_v_ptr), alignof(**_v_ptr)); \
    bool _ok = false; \
    if (_res) { \
        *_v_ptr = (typeof(*_v_ptr))_res; \
        (*_v_ptr)[_vec_get_header(*_v_ptr)->len++] = (val); \
        _ok = true; \
    } \
    _ok; \
})

#define vec_pop(v) __extension__({ \
    VecHeader* _h = _vec_get_header(v); \
    assert(_h && _h->len > 0); \
    (v)[--_h->len]; \
})

#define vec_insert(v, idx, val) __extension__({ \
    typeof(&(v)) _v_p = &(v); \
    size_t _i = (idx); \
    bool _ok = false; \
    size_t _cl = vec_len(*_v_p); \
    if (_i <= _cl) { \
        void* _r = _vec_grow_internal(*_v_p, 1, sizeof(**_v_p), alignof(**_v_p)); \
        if (_r) { \
            *_v_p = (typeof(*_v_p))_r; \
            VecHeader* _h = _vec_get_header(*_v_p); \
            if (_i < _h->len) \
                memmove(&(*_v_p)[_i + 1], &(*_v_p)[_i], (_h->len - _i) * sizeof(**_v_p)); \
            (*_v_p)[_i] = (val); \
            _h->len++; \
            _ok = true; \
        } \
    } \
    _ok; \
})

#define vec_remove(v, idx) __extension__({ \
    typeof(v) _v = (v); \
    VecHeader* _h = _vec_get_header(_v); \
    size_t _i = (idx); \
    if (_h && _i < _h->len) { \
        if (_i < _h->len - 1) \
            memmove(&_v[_i], &_v[_i + 1], (_h->len - _i - 1) * sizeof(*_v)); \
        _h->len--; \
    } \
})

/**
 * @brief Concatenates src vector to dst vector.
 * Both must be compatible vector types.
 */
#define vec_extend(dst, src) __extension__({ \
    typeof(&(dst)) _d_ptr = &(dst); \
    typeof(src) _s = (src); \
    bool _ok = false; \
    if (_s) { \
        size_t _slen = vec_len(_s); \
        void* _res = _vec_grow_internal(*_d_ptr, _slen, sizeof(**_d_ptr), alignof(**_d_ptr)); \
        if (_res) { \
            *_d_ptr = (typeof(*_d_ptr))_res; \
            VecHeader* _dh = _vec_get_header(*_d_ptr); \
            memcpy(&(*_d_ptr)[_dh->len], _s, _slen * sizeof(**_d_ptr)); \
            _dh->len += _slen; \
            _ok = true; \
        } \
    } \
    _ok; \
})

#define vec(type, ...) __extension__({ \
    type _tmp[] = { __VA_ARGS__ }; \
    size_t _n = sizeof(_tmp)/sizeof(type); \
    type* _v_ptr = (type*)_vec_grow_internal(NULL, _n, sizeof(type), alignof(type)); \
    if (_v_ptr) { \
        memcpy(_v_ptr, _tmp, sizeof(_tmp)); \
        _vec_get_header(_v_ptr)->len = _n; \
    } \
    _v_ptr; \
})

#define vec_reserve(v, n) __extension__({ \
    typeof(&(v)) _v_ptr = &(v); \
    void* _res = _vec_grow_internal(*_v_ptr, n, sizeof(**_v_ptr), alignof(**_v_ptr)); \
    if (_res) *_v_ptr = (typeof(*_v_ptr))_res; \
    _res != NULL; \
})

/* --- Functional APIs --- */

#define vec_filter(v, pred_fn) __extension__({ \
    typeof(v) _res = NULL; \
    if (v) { \
        _res = (typeof(v))_vec_grow_internal(NULL, vec_len(v), sizeof(*v), alignof(*v)); \
        if (_res) { \
            size_t _new_len = 0; \
            for (size_t _i = 0; _i < vec_len(v); ++_i) { \
                if (pred_fn((v)[_i])) _res[_new_len++] = (v)[_i]; \
            } \
            _vec_get_header(_res)->len = _new_len; \
        } \
    } \
    _res; \
})

#define vec_map(v, map_fn, out_type) __extension__({ \
    out_type* _res = NULL; \
    if (v) { \
        _res = (out_type*)_vec_grow_internal(NULL, vec_len(v), sizeof(out_type), alignof(out_type)); \
        if (_res) { \
            for (size_t _i = 0; _i < vec_len(v); ++_i) { \
                _res[_i] = map_fn((v)[_i]); \
            } \
            _vec_get_header(_res)->len = vec_len(v); \
        } \
    } \
    _res; \
})

/* --- Iterators --- */

#define vec_for_each(v, elem) \
    for (size_t _i = 0, _once = 1; _once && (v); _once = 0) \
        for (VecHeader* _h = _vec_get_header(v); _i < _h->len && ((elem) = (v)[_i], 1); ++_i)

#define vec_enumerate(v, idx, elem) \
    for (size_t idx = 0, _once = 1; _once && (v); _once = 0) \
        for (VecHeader* _h = _vec_get_header(v); idx < _h->len && ((elem) = (v)[idx], 1); ++idx)

/* --- Debugging --- */

#define vec_show(v, fmt) do { \
    typeof(v) _v_dbg = (v); \
    VecHeader* _h = _vec_get_header(_v_dbg); \
    if (_h) { \
        printf("Vec<%zu/%zu>: [", _h->len, _h->cap); \
        for(size_t _i=0; _i < _h->len; ++_i) { \
            printf(fmt, _v_dbg[_i]); if(_i < _h->len-1) printf(", "); \
        } \
        printf("]\n"); \
    } else printf("Vec: (null)\n"); \
} while(0)

#endif // VEC_H
