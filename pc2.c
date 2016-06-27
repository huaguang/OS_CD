#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define CAPACITY_1 4
#define CAPACITY_2 4

void put(int *item){
	if(count>=CAPACITY_1)
		pthread_cond_wait(notfull);
	buffer[in]=*item;
	in=(in+1)%CAPACITY_1;
	count++;
	pthread_cond_signal(notempty);
}
void get(int *item){
	if(count<=0)
		pthread_cond_wait(notempty);
	*item=buffer[out];
	out=(out+1)%CAPACITY;
	count--;
	pthread_cond_signal(notfull);
}

typedef void (*FunType)(int*);
typedef struct proCal{
	int buffer[CAPACITY_1];
	int in=0,out=0;
	pthread_cond_t notfull,notempty;
	int count=0;
	FunType pPut=&put;
	FunType pGet=&get;
	/*void init(){
		in=0;
		out=0;
		count=0;
	}*/
}proCal;

proCal PC;
void *producer(void*arg){
	int x=0;
	while(1){
		(*PC.pPut)(&x);
	}
}
void*consumer(void*arg){
	int x;
	while(1){
		(*PC.pGet)(&x);
	}

}


int main(){
	int x=0;
	int y=10;
	(*(PC.pPut))(&y);
	(*(PC.pGet))(&x);
	printf("%d",x);
}
/*
typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

void sema_init(sema_t *sema, int value)
{
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    sema->value--;
    while (sema->value < 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

int buffer1[CAPACITY],buffer2[CAPACITY];
int pIn,calIn;
int calOut,conOut;


int get_item(int *out,int*buffer)
{
    int item;
    item = buffer[*out];
    *out = ((*out) + 1) % CAPACITY;
    return item;
}

void put_item(int item,int *in,int*buffer)
{
    buffer[*in] = item;
    *in = ((*in) + 1) % CAPACITY;
}

sema_t empty_buffer1_sema;
sema_t full_buffer1_sema;
sema_t empty_buffer2_sema;
sema_t full_buffer2_sema;

#define ITEM_COUNT (CAPACITY * 2)
void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&full_buffer2_sema);

        item = get_item(&conOut,buffer2); 

        sema_signal(&empty_buffer2_sema);

        printf("    consume item: %c\n", item);
    }
    return NULL;
}


void *calculate(void *arg)
{
    int i;
    int item;
    for (i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&full_buffer1_sema);
        item = get_item(&calOut,buffer1); 
        sema_signal(&empty_buffer1_sema);
		item=item+'A'-'a';
        sema_wait(&empty_buffer2_sema);
        put_item(item,&calIn,buffer2); 
        sema_signal(&full_buffer2_sema);

        printf("    calculate item: %c\n", item);
    }
    return NULL;
}

void *produce(void*args)
{
    int i;
    int item;
    for (i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&empty_buffer1_sema);
        item = i + 'a';
        put_item(item,&pIn,buffer1);
        sema_signal(&full_buffer1_sema);
        printf("produce item: %c\n", item);
    }
}

// The program contains a bug. Can you find it?
int main()
{
    pthread_t consume_tid;
    pthread_t produce_tid;
    pthread_t calculate_tid;
	pIn=0;
	conOut=0;
	calIn=0;
	calOut=0;

    sema_init(&empty_buffer1_sema, CAPACITY);
    sema_init(&full_buffer1_sema, 0);
    sema_init(&empty_buffer2_sema, CAPACITY);
    sema_init(&full_buffer2_sema, 0);

    pthread_create(&produce_tid, NULL, produce, NULL);
    pthread_create(&consume_tid, NULL, consume, NULL);
    pthread_create(&calculate_tid, NULL,calculate, NULL);
	pthread_join(produce_tid,NULL);
	pthread_join(consume_tid,NULL);
	pthread_join(calculate_tid,NULL);


    return 0;
}
 */

















