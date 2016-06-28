#include<stdio.h>
#include<assert.h>
#include<stdio.h>
#define POINTERS 1000
#define REGION_SIZE (4000 + 1000*16)
#define MIN_SIZE 0

char region[REGION_SIZE];
 
typedef struct block{
	struct block *next;
	struct block *prev;
	int size;
	char* startAddr;	
}block,*freeList;

typedef struct usedBlock{
	char* startAddr;
	int size;
}UB;

freeList fl;
void my_malloc_init(){
	block *b=(block*)malloc(sizeof(block));		//if malloc should be used here??
	fl=b;
	b->prev=NULL;
	b->next=NULL;
	b->size=REGION_SIZE;
	b->startAddr=region;
}
void *my_malloc(int size){
	block*p;
	p=fl;
	UB *result;
	while(p!=NULL){
	 	if(p->size-size>=MIN_SIZE){
			result=(UB*)malloc(sizeof(UB));
		    result->startAddr=(void*)p->startAddr;
			result->size=size;
			p->startAddr+=size;
			p->size-=size;
			if(p->size==0){
				if(p->prev==NULL){
					fl=p->next;
					if(p->next!=NULL){
						p->next->prev=NULL;
					}
				}else{
					p->prev->next=p->next;
					
				}
				free(p);
			}
			return (void*)result;
		}else{
			p=p->next;
		}
	}
	return NULL;
}
void my_free(void*waste){
	UB *w =(UB*)waste;
	block *p;
	p=fl;
	char preflag=0;
	char nextflag=0;
	block *pos=NULL;
	while(p!=NULL){
		if((w->startAddr+w->size)<=p->startAddr){
			pos=p;
			break;
		}
		assert((p->startAddr+p->size)<=w->startAddr);
		p=p->next;
	}
	preflag=nextflag=0;
 	if(pos!=NULL){
		if(pos->startAddr==(w->startAddr+w->size)){
			nextflag=1;
		}
		p=pos->prev;
		if(p!=NULL){
			if((p->startAddr+p->size)==w->startAddr){
				preflag=1;
			}
		}
		if((preflag==1)&&(nextflag==1)){
			p->size=p->size+w->size+pos->size;
			if(pos->next!=NULL){
				pos->next->prev=p;
			}
			p->next=pos->next;
			free(w);
			free(pos);			
		}else if((preflag==1)&&(nextflag==0)){
			p->size+=w->size;
			free(w);
		}else if((preflag==0)&&(nextflag==1)){
			pos->startAddr=w->startAddr;
			pos->size+=w->size;
		}else{
			block *r=malloc(sizeof(block));
			if(p!=NULL){
				p->next=r;
			}
			r->next=pos;
			r->prev=pos->prev;
			pos->prev=r;
			r->startAddr=w->startAddr;
			r->size=w->size;
			free(w);
			if(fl==pos)
				fl=r;
		} 
	}else{
		p=fl;
		if(fl==NULL){
			block *b=(block*)malloc(sizeof(block));		
			fl=b;
			b->prev=NULL;
			b->next=NULL;
			b->size=w->size;
			b->startAddr=w->startAddr;
			free(w);
			return;
		} 
		while(p!=NULL&&p->next!=NULL){
			p=p->next;
		} 
 		if((p->startAddr+p->size)==w->startAddr){
			p->size+=w->size;
			free(w);
		}else{
			block *q;
			q=(block*)malloc(sizeof(block));
			p->next=q;
			q->startAddr=w->startAddr;
			q->size=w->size;
			q->next=NULL;
			q->prev=p;
			free(w);
		}
	}
}

			
	
void print_free_memory(){
	block *p=fl;
	int i=0;
	while(p!=NULL ){
		printf("block_%d\tstartAddr:0x%lx\tsize:%d\n",i,(unsigned long)p->startAddr,p->size);
		p=p->next;
		i++;
	} 
	printf("\n\n");
}
void print_free_memory_1(){
	block *p=fl;
	int i=0;
	while(p!=NULL){
		p=p->next;
		i++;
	} 
	printf("%d\n",i);
}

void test0()
{
	void *p1, *p2;

	puts("Test0");

	p1 = my_malloc(10);
	print_free_memory();

	p2 = my_malloc(20);
	print_free_memory();

	my_free(p1);
	print_free_memory();
	my_free(p2);
	print_free_memory();

}

void test1()
{
	void *array[POINTERS];
	int i;
	void *p;

	puts("Test1");
	for (i = 0; i < POINTERS; i++) {
		p = my_malloc(4);
		array[i] = p;

	}

	for (i = 0; i < POINTERS; i++) {
		p = array[i];
		my_free(p);
	}

	print_free_memory();
}
void test2()
{
	void *array[POINTERS];
	int i;
	void *p;

	puts("Test2");
	for (i = 0; i < POINTERS; i++) {
		p = my_malloc(4);
		array[i] = p;
	}

	for (i = POINTERS - 1; i >= 0; i--) {
		p = array[i];
		my_free(p);
	}

	print_free_memory();
}
void test3()
{
	void *array[POINTERS];
	int i;
	void *p;

	puts("Test3");
	for (i = 0; i < POINTERS; i++) {
		p = my_malloc(4);
		array[i] = p;
		if(i==40)
			print_free_memory();
	}

	for (i = 0; i < POINTERS; i += 2) {
		p = array[i];
		my_free(p);
	}

	for (i = 1; i < POINTERS; i += 2) {
		p = array[i];
		my_free(p);
			
	}

	print_free_memory();
}
void main(){
	my_malloc_init();
	test0();
	test1();
	test2();
	test3();
}
