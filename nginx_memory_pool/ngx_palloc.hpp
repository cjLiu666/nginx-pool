#ifndef NGX_PALLOC_H_
#define NGX_PALLOC_H_

// 利用oop来实现nginx的内存池
#include <iostream>
#include <cstdlib>
#include <memory>
using namespace std;

// 类型重定义
using u_char = unsigned char;
using ngx_uint_t = unsigned int;

// 类型前置声明
struct ngx_pool_s;

// 清理函数的类型，回调函数
typedef void(*ngx_pool_cleanup_pt)(void* data);
struct ngx_pool_cleanup_s {
    // 保存预先设置的回调函数
    ngx_pool_cleanup_pt handler;
    // 释放资源时资源的地址
    void *data;
    // 链表
    ngx_pool_cleanup_s *next;
};

// 大块内存的内存头信息
struct ngx_pool_large_s {
    // 指向下一个同类型内存块
    ngx_pool_large_s *next;
    // 大块内存的起始地址
    void *alloc;
};

// 小块内存的内存池的头部信息，这个数据头每个内存块都有
struct ngx_pool_data_t {
    // 内存池可用内存的起始地址
    u_char *last;
    // 内存池可用内存的末尾地址
    u_char *end;
    // 指向下一个开辟的内存池地址，初始值为空
    ngx_pool_s *next;
    // 在当前内存池中分配内存失败的次数
    ngx_uint_t failed;
};

// 内存池的头部信息和管理成员信息
struct ngx_pool_s {
    ngx_pool_data_t     d;
    // 当前内存块能用于小内存分配的最大内存大小
    // 如果当前内存池中剩余的内存size<4095,那就只能记作size，否则记作4095
    size_t              max;
    // 指向当前可分配内存的内存块
    ngx_pool_s          *current;
    // 大内存链表
    ngx_pool_large_s    *large;
    // 清理函数handler的入口函数
    ngx_pool_cleanup_s  *cleanup;
};

// 把数值d调整为临近的a的倍数
#define ngx_align(d, a)     (((d) + (a - 1)) & ~(a - 1))

//能从池里分配的小块内存的最大内存，一个页面，4K,4096
const int ngx_pagesize = 4096;//默认一个物理页面的大小
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;

//默认的开辟的ngx内存池的大小
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;

//内存分配的匹配对齐的字节数
const int NGX_POOL_ALIGNMENT = 16;

//能够定义的最小的池的大小包含一个数据头信息大小和2*2个指针类型大小
//再将其转化为16的倍数
const int NGX_MIN_POOL_SIZE = \
			ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), \
			NGX_POOL_ALIGNMENT);


class ngx_mem_pool {
private:
    // 指向nginx内存池的入口指针
    ngx_pool_s      *pool_;
    // 从池中分配小内存
    void *ngx_palloc_small(size_t size, ngx_uint_t align);
    // 分配大内存
    void *ngx_palloc_large(size_t size);
    // 开辟新的内存池
    void *ngx_palloc_block(size_t size);

public:
    // 创建指定size的内存池
    void *ngx_create_pool(size_t size);
    
    //考虑内存字节对齐，从内存池中为小块内存申请size大小的内存
	void *ngx_palloc(size_t size);

	//不考虑内存字节对齐，从内存池中申请size大小的内存
	void *ngx_pnalloc(size_t size);

	//将内存初始化为0的内存申请操作
	void *ngx_pcalloc(size_t size);

	//提供释放大块内存的函数,注（小块内存由于其通过指针偏移来分配内存，所以连续内存不能只释放中间的）
	void ngx_pfree(void *p);

	//重置内存池
	void ngx_reset_pool();
	
	//销毁内存池
	void ngx_destroy_pool();
	
	//添加回调清理操作函数
	ngx_pool_cleanup_s *ngx_pool_cleanup_add(size_t size);

    // 获取内存池指针
    ngx_pool_s *getPoolPtr();
};


#endif