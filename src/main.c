#include "vec.h"
#include <stdio.h>
#include <stdlib.h>

/* 比较函数 */
int compare_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int compare_int_desc(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia < ib) - (ia > ib);
}

/* 映射函数 */
void* double_int(const void* elem) {
    static int result;
    result = *(const int*)elem * 2;
    return &result;
}

void* int_to_string(const void* elem) {
    static char buffer[20];
    sprintf(buffer, "%d", *(const int*)elem);
    return buffer;
}

/* 过滤函数 */
bool is_even(const void* elem) {
    return *(const int*)elem % 2 == 0;
}

bool is_positive(const void* elem) {
    return *(const int*)elem > 0;
}

/* 自定义打印函数 */
void print_person(const void* elem) {
    typedef struct { char name[20]; int age; } Person;
    const Person* p = (const Person*)elem;
    printf("{%s, %d}", p->name, p->age);
}

int main() {
    printf("=== Vec 扩展功能测试 ===\n\n");
    
    /* 1. 排序功能测试 */
    printf("1. 排序功能测试:\n");
    int* nums = vec(int, 5, 2, 8, 1, 9, 3, 7, 4, 6);
    printf("   原始: ");
    vec_show_int(nums);
    
    vec_sort(nums, compare_int);
    printf("   升序: ");
    vec_show_int(nums);
    
    vec_sort(nums, compare_int_desc);
    printf("   降序: ");
    vec_show_int(nums);
    
    /* 2. 缩容功能测试 */
    printf("\n2. 缩容功能测试:\n");
    printf("   原始容量: %zu\n", vec_cap(nums));
    for (int i = 0; i < 5; i++) {
        vec_remove(nums, 0);
    }
    printf("   删除5个后: ");
    vec_show_int(nums);
    printf("   容量(自动缩容后): %zu\n", vec_cap(nums));
    
    vec_shrink_to_fit(nums);
    printf("   手动缩容后容量: %zu\n", vec_cap(nums));
    
    /* 3. 切片功能测试 */
    printf("\n3. 切片功能测试:\n");
    int* slice = vec_slice(nums, 1, 3);
    printf("   nums[1:3]: ");
    vec_show_int(slice);
    
    vec_free(slice);
    
    printf("   范围删除 nums[0:2]: ");
    vec_drain(nums, 0, 2);
    vec_show_int(nums);
    
    /* 4. 映射功能测试 */
    printf("\n4. 映射功能测试:\n");
    int* doubled = vec_map(nums, double_int, int);
    printf("   加倍: ");
    vec_show_int(doubled);
    
    char** strings = vec_map(nums, int_to_string, char*);
    printf("   转字符串: ");
    vec_show_string(strings);
    
    /* 5. 过滤功能测试 */
    printf("\n5. 过滤功能测试:\n");
    int* evens = vec_filter(nums, is_even);
    printf("   偶数: ");
    if (evens) {
        vec_show_int(evens);
    } else {
        printf("(无偶数)\n");
    }
    
    /* 6. 重复初始化 */
    printf("\n6. 重复初始化测试:\n");
    int* repeated = vec_repeat(int, 42, 5);
    printf("   5个42: ");
    vec_show_int(repeated);
    
    /* 7. 去重功能测试 */
    printf("\n7. 去重功能测试:\n");
    int* with_duplicates = vec(int, 1, 2, 2, 3, 1, 4, 5, 4, 4, 6);
    printf("   有重复: ");
    vec_show_int(with_duplicates);
    
    int* deduped = vec_dedup(with_duplicates);
    printf("   去重(保持顺序): ");
    vec_show_int(deduped);
    
    vec_sort(with_duplicates, compare_int);
    printf("   排序后: ");
    vec_show_int(with_duplicates);
    
    vec_dedup_sorted(with_duplicates);
    printf("   排序去重: ");
    vec_show_int(with_duplicates);
    
    /* 8. vec_show 功能测试 */
    printf("\n8. vec_show 功能测试:\n");
    
    // 不同类型显示
    float* floats = vec(float, 1.1, 2.2, 3.3, 4.4, 5.5);
    printf("   float向量:\n");
    vec_show(floats, "%.2f");
    
    char* chars = vec(char, 'a', 'b', 'c', 'd', 'e');
    printf("\n   char向量:\n");
    vec_show_char(chars);
    
    const char** str_vec = vec(const char*, "hello", "world", "test");
    printf("\n   字符串向量:\n");
    vec_show_string(str_vec);
    
    // 自定义结构体显示
    typedef struct { char name[20]; int age; } Person;
    Person* people = vec(Person, {"Alice", 25}, {"Bob", 30}, {"Charlie", 35});
    printf("\n   自定义结构体向量:\n");
    printf("   使用自定义打印函数:\n");
    vec_show_custom(people, print_person);
    
    printf("\n   使用通用格式(指针):\n");
    vec_show_pointer(people);
    
    /* 9. 自动缩容测试 */
    printf("\n9. 自动缩容测试:\n");
    int* auto_shrink = vec_with_capacity(int, 100);
    printf("   初始容量: %zu\n", vec_cap(auto_shrink));
    
    for (int i = 0; i < 80; i++) {
        vec_push(auto_shrink, i);
    }
    printf("   添加80个元素后容量: %zu\n", vec_cap(auto_shrink));
    
    for (int i = 0; i < 60; i++) {
        vec_remove(auto_shrink, 0);
    }
    printf("   删除60个后容量(自动缩容): %zu\n", vec_cap(auto_shrink));
    
    vec_clear(auto_shrink);
    printf("   清空后容量: %zu\n", vec_cap(auto_shrink));
    
    /* 清理内存 */
    vec_free(nums);
    vec_free(doubled);
    vec_free(strings);
    vec_free(evens);
    vec_free(repeated);
    vec_free(with_duplicates);
    vec_free(deduped);
    vec_free(floats);
    vec_free(chars);
    vec_free(str_vec);
    vec_free(people);
    vec_free(auto_shrink);
    
    printf("\n=== 所有测试完成 ===\n");
    system("pause");
    return 0;
}

