#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

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
	if(sema->value<=0)
		pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

#define CAPACITY 4
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
sema_t buffer1_mutex_sema;
sema_t buffer2_mutex_sema;

#define ITEM_COUNT (CAPACITY * 2)
void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&full_buffer2_sema);
		sema_wait(&buffer2_mutex_sema);
        item = get_item(&conOut,buffer2); 
		sema_signal(&buffer2_mutex_sema);
        sema_signal(&empty_buffer2_sema);
        printf("          consume item: %c\n", item);
    }
}


void *calculate(void *arg)
{
    int i;
    int item;
    for (i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&full_buffer1_sema);
        sema_wait(&buffer1_mutex_sema);
        item = get_item(&calOut,buffer1); 
        sema_signal(&buffer1_mutex_sema);
        sema_signal(&empty_buffer1_sema);
		item=item+'A'-'a';
        sema_wait(&empty_buffer2_sema);
        sema_wait(&buffer2_mutex_sema);
        put_item(item,&calIn,buffer2); 
        sema_signal(&buffer2_mutex_sema);
        sema_signal(&full_buffer2_sema);
        printf("    calculate item: %c\n", item);
    }
}

void *produce(void*args)
{
    int i;
    int item;
    for (i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&empty_buffer1_sema);
        sema_wait(&buffer1_mutex_sema);
        item = i + 'a';
        put_item(item,&pIn,buffer1);
        sema_signal(&buffer1_mutex_sema);
        sema_signal(&full_buffer1_sema);
        printf("produce item: %c\n", item);
    }
}

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
    sema_init(&buffer1_mutex_sema, 1);
    sema_init(&buffer2_mutex_sema, 1);

    pthread_create(&produce_tid, NULL, produce, NULL);
    pthread_create(&consume_tid, NULL, consume, NULL);
    pthread_create(&calculate_tid, NULL,calculate, NULL);
	pthread_join(produce_tid,NULL);
	pthread_join(consume_tid,NULL);
	pthread_join(calculate_tid,NULL);
    return 0;
}
 

















