#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
//#define TOTAL_NUM 3
//#define CPU_NUM 2
typedef struct result{
	long double sum;
}result;
typedef struct parameter{
	unsigned long end;
	unsigned long start;	
}parameter;


void *compute(void*arg){
	parameter *param;
	param=(parameter*)arg;
    result *res;
	res=malloc(sizeof(result));
	unsigned long i=0;
	res->sum=0;

	for(i=param->start;i<param->end;i++){

		if(i%2==1){	//negative
			 res->sum-=(double)1/(double)(i*2+1);
		}else{
			res->sum+=(double)1/(double)(i*2+1);
		}

	}
	return res;
}
int main(){
	unsigned long TOTAL_NUM=0;
	unsigned int CPU_NUM=0;
	printf("enter the TOTAL_NUM:");
	scanf("%lu",&TOTAL_NUM);
	printf("enter the CPU_NUM:");
	scanf("%u",&CPU_NUM);

	result *res;
	long double sum=0;
	unsigned long i=0;
	unsigned long average=(TOTAL_NUM+CPU_NUM-1)/CPU_NUM;
	pthread_t worker_id[CPU_NUM];
	parameter param[CPU_NUM];
	for(i=0;i<CPU_NUM;i++){
		param[i].start=i*average;
		param[i].end=(i+1)*average;
		if(param[i].end>TOTAL_NUM)
			param[i].end=TOTAL_NUM;
		pthread_create(&worker_id[i],NULL,&compute,&param[i]);
	}
	
	
	for(i=0;i<CPU_NUM;i++){
		pthread_join(worker_id[i],(void**)&res);
		sum+=res->sum;
		printf("sum=%.12Lf,res->sum=%.12Lf\n",sum,res->sum);
	}
	printf("pi/4 is %.12Lf",sum);
	return 0;
}


