#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define CAPACITY_1 4
#define CAPACITY_2 4
#define CAPACITY 4

int buffer1[CAPACITY_1],buffer2[CAPACITY_2];
int pIn,calIn;
int calOut,conOut;
int count1,count2;

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

pthread_cond_t wait_notempty_buffer1;
pthread_cond_t wait_notfull_buffer1;
pthread_cond_t wait_notempty_buffer2;
pthread_cond_t wait_notfull_buffer2;
pthread_mutex_t buffer1_mutex;
pthread_mutex_t buffer2_mutex;


#define ITEM_COUNT (CAPACITY* 2)
void *consume(void *arg)
{
    int i;
    int item;
    for (i = 0; i < ITEM_COUNT; i++) {
		pthread_mutex_lock(&buffer2_mutex);
		while(count2<=0)
			pthread_cond_wait(&wait_notempty_buffer2,&buffer2_mutex);
        item=get_item(&conOut,buffer2);
		count2--;
		pthread_cond_signal(&wait_notfull_buffer1);
        printf("\t\t\tconsume item: %c\n", item);
		pthread_mutex_unlock(&buffer2_mutex);
    }
}


void *calculate(void *arg)
{
    int i;
    int item;
    for (i = 0; i < ITEM_COUNT; i++) {
		pthread_mutex_lock(&buffer1_mutex);
		while(count1<=0)
			pthread_cond_wait(&wait_notempty_buffer1,&buffer1_mutex);
        item=get_item(&calOut,buffer1);
		count1--;
		pthread_cond_signal(&wait_notfull_buffer1);
        printf("\tcalculate item before: %c\n", item);
		pthread_mutex_unlock(&buffer1_mutex);
		item=item+'A'-'a';
		pthread_mutex_lock(&buffer2_mutex);
		while(count2>=CAPACITY_2)
			pthread_cond_wait(&wait_notfull_buffer2,&buffer2_mutex);
        put_item(item,&calIn,buffer2);
		count2++;
		pthread_cond_signal(&wait_notempty_buffer2);
        printf("\tcalculate item after: %c\n", item);
		pthread_mutex_unlock(&buffer2_mutex);
    } 
}

void *produce(void*args)
{
    int i;
    int item;
    for (i = 0; i < ITEM_COUNT; i++) {
		pthread_mutex_lock(&buffer1_mutex);
		while(count1>=CAPACITY_1)
			pthread_cond_wait(&wait_notfull_buffer1,&buffer1_mutex);
        item = i + 'a';
        put_item(item,&pIn,buffer1);
		count1++;
		pthread_cond_signal(&wait_notempty_buffer1);
        printf("produce item: %c\n", item);
		pthread_mutex_unlock(&buffer1_mutex);
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
	count1=count2=0;
	pthread_cond_init(&wait_notempty_buffer1,NULL);
	pthread_cond_init(&wait_notfull_buffer1,NULL);
	pthread_cond_init(&wait_notempty_buffer2,NULL);
	pthread_cond_init(&wait_notfull_buffer2,NULL);
	pthread_mutex_init(&buffer1_mutex,NULL);
	pthread_mutex_init(&buffer2_mutex,NULL);

    pthread_create(&produce_tid, NULL, produce, NULL);
    pthread_create(&consume_tid, NULL, consume, NULL);
    pthread_create(&calculate_tid, NULL,calculate, NULL);

	pthread_join(produce_tid,NULL);
	pthread_join(consume_tid,NULL);
	pthread_join(calculate_tid,NULL);
    return 0;
}
