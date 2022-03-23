#include "ngx_palloc.hpp"
#include <cstring>
#include <cstdio>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>

typedef struct Data stData;
struct Data {
	char *ptr;
	int   fd;
};

void func1(void *p1) //void (*) (void*)
{
	char *p = (char*)p1;
	printf("free ptr mem! content: %s\n", p);
	free(p);
}
//void func2(void *pf1)
//{
//	FILE *pf = (FILE*)pf1;
//	printf("close file!");
//	fclose(pf);
//}

void func2(void *p2) {
    int* p = (int*) p2;
    printf("close file! fd is %d\n", *p);
    close(*p);
}

int main() {
	ngx_mem_pool mempool; 
	if (nullptr == mempool.ngx_create_pool(512)) {
		printf("ngx_create_pool fail...\n");
		return -1;
	}
    ngx_pool_s *p = mempool.getPoolPtr();
    assert(p->max == 512 - sizeof(ngx_pool_s));

	void *p1 = mempool.ngx_palloc(128);
	if (p1 == nullptr) {
		printf("ngx_palloc 128 bytes fail...\n");
		return -1;
	}

	stData *p2 = (stData *)mempool.ngx_palloc(512);
	if (p2 == nullptr) {
		printf("ngx_palloc 512 bytes fail...\n");
		return -1;
	}
	p2->ptr = (char*)malloc(12);
	strcpy(p2->ptr, "hello world");
	/*FILE *fp;
	p2->pfile = (FILE*)fopen_s(&fp,"data.txt", "w");*/
    int fd = open("Makefile", O_RDONLY);
    p2->fd = fd;
    

	ngx_pool_cleanup_s *c1 = mempool.ngx_pool_cleanup_add(sizeof(char*));
	c1->handler = func1;
	c1->data = p2->ptr;

	/*ngx_pool_cleanup_s *c2 = mempool.ngx_pool_cleanup_add(sizeof(*));
	c2->handler = func2;
	c2->data = p2->pfile;*/
    ngx_pool_cleanup_s *c2 = mempool.ngx_pool_cleanup_add(sizeof(int*));
    c2->handler = func2;
    c2->data = &(p2->fd);

	mempool.ngx_destroy_pool();

	return 0;
}