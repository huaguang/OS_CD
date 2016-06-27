#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<pthread.h>
#define N 7

typedef struct parameter{
	int* array;
	int start;
	int end;
}parameter;

void chooseSort(int *a,int beg,int end){
	int i=0;
	int j=0;
	int pos=0;
	int t;
	for(i=beg;i<end-1;i++){
		t=a[i];
		pos=i;
		for(j=i+1;j<end;j++){
			if(t>a[j]){
				t=a[j];
				pos=j;
			}
		}
		a[pos]=a[i];
		a[i]=t;
	}
}

//
void Merge(int *a, int beg, int n, int m)
{
	int *b=(int*)malloc((n-beg)*sizeof(int));
	int i = beg, j = beg + m;
	int k = beg;
	for (; (i < beg + m)  && (j < n);){
		if (a[i] < a[j])
		{
			b[k++] = a[i++];
		}
		else{
			b[k++] = a[j++];
		}
	}
	while (i < beg + m){
		b[k++] = a[i++];
	}
	while (j < n){
		b[k++] = a[j++];
	}
	for (i = beg; i < k; i++){
		a[i] = b[i];
	}
	free(b);
}

void *childThread(void*arg){
	parameter *param;
	param=(parameter*)arg;
	chooseSort(param->array,param->start,param->end);
}

int main(){
	int a[]={9,4,1,10,2,24,5};
	pthread_t worker_id;	
	parameter param;
	param.array=a;
	param.start=N/2;
	param.end=N;
	pthread_create(&worker_id,NULL,childThread,(void*)&param);
    
	chooseSort(a,0,N/2);


	pthread_join(worker_id,NULL);
	

	Merge(a,0,N,N/2);
	int i=0;
	for(i=0;i<N;i++){
		printf("%d\t",a[i]);
	}
	printf("\n");
}



