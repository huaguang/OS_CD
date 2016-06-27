#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
	int id;
} sema_t;
int mailBox[2];	//id and content
sema_t read_sema;
sema_t write_sema;
int readCount=0;
int circleCount;
void sema_init(sema_t *sema, int value,int identifier)
{
	sema->id=identifier;
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema,int processId)
{
    pthread_mutex_lock(&sema->mutex);
    sema->value--;
	printf("processId:%d\tid=%d;oldvalue:%d,newValue:%d\n",processId,sema->id,sema->value+1,sema->value);
    while (sema->value < 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema,int processId)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
	printf("processId:%d\tid=%d;oldvalue:%d,newValue:%d\n",processId,sema->id,sema->value-1,sema->value);
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}


void* ring(void*arg){
	int *id;
	id=(int*)arg;
	char flag=0;// is my mail?
	int circle=circleCount;
	while(circle>0){
		sema_wait(&read_sema,*id);
		if(readCount==0)
			sema_wait(&write_sema,*id);
		readCount++;
		sema_signal(&read_sema,*id);
		if(*id==mailBox[0]){
			flag=1;
		}
		sema_wait(&read_sema,*id);
		readCount--;
		if(readCount==0) 
			sema_signal(&write_sema,*id);
		sema_signal(&read_sema,*id);
		if(flag==1){
			sema_wait(&write_sema,*id);
			mailBox[0]=*id+1;
			mailBox[1]=*id;
			sema_signal(&write_sema,*id);
			printf("this is thread_%d:Receive:%d;Send:%d\n",*id,*id,*id+1);
			flag=0;
			circle--;
		}
	}
}

int main()
{
	int N=3;
	circleCount=1;
    pthread_t worker_id[N];
	int param[N];
    sema_init(&read_sema, 1,0);
    sema_init(&write_sema, 1,1);
	
	mailBox[0]=1;	//launch the cicle; T0(main) pass 0 to T1;
	mailBox[1]=0;

	int i;
	for(i=0;i<N;i++){
		param[i]=i+1;
		pthread_create(&worker_id[i],NULL,ring,&param[i]);
	}
	for(i=0;i<N;i++){
		pthread_join(worker_id[i],NULL);
	}
    return 0;
}
 



/*
processId:3	id=0;oldvalue:1,newValue:0
processId:3	id=1;oldvalue:1,newValue:0
processId:3	id=0;oldvalue:0,newValue:1
processId:3	id=0;oldvalue:1,newValue:0
processId:3	id=1;oldvalue:0,newValue:1
processId:3	id=0;oldvalue:0,newValue:1
processId:3	id=0;oldvalue:1,newValue:0
processId:3	id=1;oldvalue:1,newValue:0
processId:2	id=0;oldvalue:0,newValue:-1
processId:1	id=0;oldvalue:-1,newValue:-2
processId:3	id=0;oldvalue:-2,newValue:-1
processId:3	id=0;oldvalue:-1,newValue:-2*/

//when thread 3 locked read_sema->mutex using sema_wait,
//why can thread 2  enter sema_wait and  change the  sema->value?
//thread 2 and thread 3 use  the same read_sema. 














