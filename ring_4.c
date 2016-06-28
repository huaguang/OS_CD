#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include<assert.h>
#include<sys/time.h>
#include<math.h>
typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}sema_t;
int N=3;
double sleepTime;
int mailBox[2];	//id and content
sema_t read_sema;
sema_t write_sema;
int readCount=0;
int circleCount;
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
    if(sema->value < 0)
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


void* ring(void*arg){
	int *id;
	int content;
	id=(int*)arg;
	char flag=0;// is my mail?
	int circle=circleCount;
	while(circle>0){
		sema_wait(&read_sema);
		if(readCount==0)
			sema_wait(&write_sema);
		readCount++;
		sema_signal(&read_sema);
		if(*id==((mailBox[0]-1)%N+1)){
			flag=1;
			content=mailBox[1];
		}
		sema_wait(&read_sema);
		readCount--;
		if(readCount==0) 
			sema_signal(&write_sema);
		sema_signal(&read_sema);
		if(flag==1){
			sema_wait(&write_sema);
			mailBox[0]=*id+1;
			mailBox[1]=content+1;
			sema_signal(&write_sema);
			int i;
		/*	for(i=0;i<*id-1;i++)
				printf("\t");
			printf("this is thread_%d:Receive:%d;Send:%d\n",*id,content,content+1);

		*/
			assert(*id==content%N+1);

			flag=0;
			circle--;
		//	if(circle%10==0)
		//		printf("%d\n",circle);
		}
/*		else{
			usleep(sleepTime);
		}
*/	}
}

int main()
{
    pthread_t worker_id[N];
	int i,j;
	int param[N];
    sema_init(&read_sema, 1);
    sema_init(&write_sema, 1);
	struct timeval start,end;
/*	printf("core number:");
	scanf("%d",&N);
	printf("circlrCount:");
	scanf("%d",&circleCount);
	printf("sleePTime:");
	scanf("%lf",&sleepTime);
	*/
	N=1;
	circleCount=40000;
	for(j=0;j<3;j++){
		sleepTime=0.1*pow(0.01,j+1);
		mailBox[0]=1;	//launch the cicle; T0(main) pass 0 to T1;
		mailBox[1]=0;
		gettimeofday(&start,NULL);
		for(i=0;i<N;i++){
			param[i]=i+1;
			pthread_create(&worker_id[i],NULL,ring,&param[i]);
		}
		for(i=0;i<N;i++){
			pthread_join(worker_id[i],NULL);
		}
		gettimeofday(&end,NULL);
		int timeuse=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
		printf("timeuse:%d us,core number:%d,circleCount:%d,sleepTime:%.9lf\n",timeuse,N,circleCount,sleepTime);
	}

	N=4;
	circleCount=10000;

	for(j=0;j<3;j++){

		sleepTime=0.1*pow(0.01,j+1);
		mailBox[0]=1;	//launch the cicle; T0(main) pass 0 to T1;
		mailBox[0]=1;	//launch the cicle; T0(main) pass 0 to T1;
		mailBox[1]=0;
		gettimeofday(&start,NULL);
		for(i=0;i<N;i++){
			param[i]=i+1;
			pthread_create(&worker_id[i],NULL,ring,&param[i]);
		}
		for(i=0;i<N;i++){
			pthread_join(worker_id[i],NULL);
		}
		gettimeofday(&end,NULL);
		int timeuse=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
		printf("timeuse:%d us,core number:%d,circleCount:%d,sleepTime:%.9lf\n",timeuse,N,circleCount,sleepTime);

	}

	N=2;
	circleCount=20000;
	for(j=0;j<3;j++){

		sleepTime=0.1*pow(0.01,j+1);
		mailBox[0]=1;	//launch the cicle; T0(main) pass 0 to T1;
		mailBox[0]=1;	//launch the cicle; T0(main) pass 0 to T1;
		mailBox[1]=0;
		gettimeofday(&start,NULL);
		for(i=0;i<N;i++){
			param[i]=i+1;
			pthread_create(&worker_id[i],NULL,ring,&param[i]);
		}
		for(i=0;i<N;i++){
			pthread_join(worker_id[i],NULL);
		}
		gettimeofday(&end,NULL);
		int timeuse=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
		printf("timeuse:%d us,core number:%d,circleCount:%d,sleepTime:%.9lf\n",timeuse,N,circleCount,sleepTime);

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














