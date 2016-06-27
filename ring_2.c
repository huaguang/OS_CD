#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
int N=3;
int mailBox[2];	//id and content
int readCount=0;
pthread_mutex_t read_mutex;
pthread_mutex_t write_mutex;

void* ring(void*arg){
	int *id;
	int content;
	id=(int*)arg;
	char flag=0;// is my mail?
	int writeCount=2;
	while(writeCount>=0){
		pthread_mutex_lock(&read_mutex);
		if(readCount==0)
			pthread_mutex_lock(&write_mutex);
		readCount++;
		pthread_mutex_unlock(&read_mutex);
		if(*id==(mailBox[0]%N)){
			flag=1;
			content=mailBox[1];
		}
		pthread_mutex_lock(&read_mutex);
		readCount--;
		if(readCount==0) 
			pthread_mutex_unlock(&write_mutex);
		pthread_mutex_unlock(&read_mutex);
		if(flag==1){
			pthread_mutex_lock(&write_mutex);
			mailBox[0]=*id+1;	// to next thread
			mailBox[1]=content+1;	//
			pthread_mutex_unlock(&write_mutex);
			printf("this is thread_%d:Receive:%d;Send:%d\n",*id+1,content,content+1);
			flag=0;
			writeCount--;
		}
	}
}

int main()
{
	 N=3;
    pthread_t worker_id[N];
	int param[N];
	
	mailBox[0]=0;	//launch the cicle; T0(main) pass 0 to T1;
	mailBox[1]=0;
	pthread_mutex_init(&read_mutex,NULL);
	pthread_mutex_init(&write_mutex,NULL);

	int i;
	for(i=0;i<N;i++){
		param[i]=i;
		pthread_create(&worker_id[i],NULL,ring,&param[i]);
	}
	for(i=0;i<N;i++){
		pthread_join(worker_id[i],NULL);
	}
    return 0;
}
 

















