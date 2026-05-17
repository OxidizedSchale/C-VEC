#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- 1. 辅助函数定义 --- */

// 比较函数 (用于 qsort)
int compare_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// 映射函数 (注意：为了匹配宏实现，直接接收值)
int double_int(int x) {
    return x * 2;
}

// 过滤函数
bool is_even(int x) {
    return x % 2 == 0;
}

// 自定义结构体
typedef struct {
    char name[20];
    int age;
} Person;

void print_person(Person p) {
    printf("{%s, %d} ", p.name, p.age);
}

/* --- 2. 宏辅助定义 (简化打印) --- */
#define vec_show_int(v) vec_show(v, "%d")
#define vec_show_float(v) vec_show(v, "%.2f")
#define vec_show_str(v) vec_show(v, "\"%s\"")

/* --- 3. 核心演示函数 --- */

// 演示 RAII 自动释放功能
void test_raii_feature() {
    printf("\n>>> [RAII] 自动生命周期管理测试:\n");
    // 使用 vec_auto，函数返回时自动调用 free
    vec_auto(int*) temp_vec = vec(int, 100, 200, 300);
    vec_push(temp_vec, 400);
    printf("   temp_vec 长度: %zu (将在函数结束时自动释放)\n", vec_len(temp_vec));
}

int main() {
    printf("===========================================\n");
    printf("       C-VEC 工业级功能全解演示             \n");
    printf("===========================================\n");

    /* 1. 初始化与原地更新 (In-place Push) */
    printf("\n1. 初始化与原地更新测试:\n");
    int* nums = vec(int, 10, 20, 30);
    // 注意：这里不需要 nums = vec_push(nums, 40)，指针会自动重映射
    vec_push(nums, 40); 
    vec_push(nums, 50);
    printf("   添加元素后: ");
    vec_show_int(nums);

    /* 2. 拼接功能测试 (Concatenation) */
    printf("\n2. 向量拼接 (Extend) 测试:\n");
    int* extra = vec(int, 60, 70, 80);
    printf("   待拼接向量: ");
    vec_show_int(extra);
    vec_extend(nums, extra); // 原地将 extra 拼接到 nums
    printf("   拼接结果: ");
    vec_show_int(nums);
    vec_free(extra);

    /* 3. 函数式编程 (Map & Filter) */
    printf("\n3. 函数式 API (Map & Filter) 测试:\n");
    // Filter: 只要偶数
    int* evens = vec_filter(nums, is_even);
    printf("   过滤(偶数): ");
    vec_show_int(evens);

    // Map: 所有元素乘以 2
    int* doubled = vec_map(evens, double_int, int);
    printf("   映射(翻倍): ");
    vec_show_int(doubled);

    /* 4. 排序与弹出 */
    printf("\n4. 排序与操作测试:\n");
    vec_push(nums, 5); // 插入一个小数字
    qsort(nums, vec_len(nums), sizeof(int), compare_int); // 直接使用标准库 qsort
    printf("   排序后: ");
    vec_show_int(nums);
    
    int popped = vec_pop(nums);
    printf("   弹出末尾: %d, 剩余长度: %zu\n", popped, vec_len(nums));

    /* 5. 插入与删除 (有序性) */
    printf("\n5. 插入与删除测试:\n");
    vec_insert(nums, 0, 999); // 在头部插入
    printf("   头部插入 999: ");
    vec_show_int(nums);
    
    vec_remove(nums, 1); // 删除索引 1 的元素
    printf("   删除索引 1 : ");
    vec_show_int(nums);

    /* 6. 结构体与对齐测试 */
    printf("\n6. 自定义结构体与内存对齐测试:\n");
    vec_auto(Person*) people = vec(Person, {"Alice", 25}, {"Bob", 30});
    vec_push(people, (Person){"Charlie", 35});
    
    printf("   People 列表: ");
    vec_for_each(people, p) {
        print_person(p);
    }
    printf("\n   Person 结构体对齐要求: %zu 字节\n", alignof(Person));

    /* 7. RAII 演示 */
    test_raii_feature();

    /* 8. 边界与容量管理 */
    printf("\n8. 容量管理测试:\n");
    printf("   nums 当前容量: %zu, 长度: %zu\n", vec_cap(nums), vec_len(nums));
    vec_reserve(nums, 100);
    printf("   预留 100 空间后容量: %zu\n", vec_cap(nums));

    /* 清理非 RAII 变量 */
    vec_free(nums);
    vec_free(evens);
    vec_free(doubled);

    printf("\n===========================================\n");
    printf("           所有功能测试顺利完成!            \n");
    printf("===========================================\n");

    return 0;
}
