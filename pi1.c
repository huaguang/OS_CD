#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define total 100000000
typedef struct result{
	double sum;
}result;
void *compute(void*arg){
    result *res;
	int i=0;
	int *num;
	num=(int*)arg;
	res=malloc(sizeof(result));
	res->sum=0;
	for(i=0;i<(*num);i++){
		res->sum+=(double)1/(double)(i*4+3);
	}
	res->sum=-res->sum;
	res->sum*=4;
	return res;
}
int main(){
	result *res;
	result *res_m;
	double sum;
	int num=total/2;
	int i=0;
	pthread_t worker_id;
	pthread_create(&worker_id,NULL,&compute,&num);

	res_m=malloc(sizeof(result));
	for(i=0;i<num;i++){
		res_m->sum+=(double)1/(double)(i*4+1);
	}
	res_m->sum*=4;
	pthread_join(worker_id,(void**)&res);

	sum=res_m->sum+res->sum;
	printf("sum=%.12lf,master=%.12lf,worker=%.12lf\n",sum,res_m->sum,res->sum);
	return 0;
}


