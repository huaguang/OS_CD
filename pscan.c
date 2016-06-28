#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include      <stdlib.h>    //exit()
#include      <string.h>    //strerror()
#include      <errno.h>     //errno
#include      <sys/types.h> //stat()
#include      <sys/stat.h>
#include<pthread.h>
pthread_mutex_t lineMutex;

int lineCount=0;
char strContain(char*childstr,char*str);
char strContainDEFINE(char*str);
int countDefine(char*fileName);
int find(char * dirName);
typedef struct task{
	struct task * next;
	char* dir;
}task;
typedef struct threadPool{
	int num;
	int shutdown;
	pthread_t* workThread;
	task* taskHead;
	task* taskTail;
	pthread_mutex_t mutex;
	pthread_cond_t notEmptyCond;

}threadPool;
threadPool *tp;
void* workProcess(void *arg){
	int lineNum;
	pthread_mutex_lock(&tp->mutex);
	while(1){
		while(!tp->shutdown&&tp->taskHead==NULL){
			pthread_cond_wait(&tp->notEmptyCond,&tp->mutex);
		}
		if(tp->shutdown==1){
			pthread_mutex_unlock(&tp->mutex);
			pthread_exit(NULL);
		}
		task *t;
		t=tp->taskHead;
		tp->taskHead=tp->taskHead->next;
		if(tp->taskTail==t){
			tp->taskTail=NULL;
		}
		pthread_mutex_unlock(&tp->mutex);
		lineNum=0;
		printf("way to fine:%s             taskNumber:%d\n",t->dir,getTaskNum());
		lineNum=find(t->dir);
		if(lineNum!=0){
			pthread_mutex_lock(&lineMutex);
			lineCount+=lineNum;
			pthread_mutex_unlock(&lineMutex);
		} 
		free(t->dir);
		free(t);
	}
}

int getTaskNum(){
	int i=0;
	pthread_mutex_lock(&tp->mutex);
	task*p=tp->taskHead;
	while(p!=tp->taskTail){
		i++;
		p=p->next;
	}
	pthread_mutex_unlock(&tp->mutex);
	return i;
}
void createThreadPool(int num){
	int errorno;
	if(num<=0)
		exit(1);
	tp=(threadPool*)malloc(sizeof(threadPool));
	if(tp==NULL){
		exit(1);
	} 
	tp->num=num;
	tp->shutdown=0;
	tp->workThread=(pthread_t*)malloc(num*sizeof(pthread_t));
	if (tp->workThread==NULL){
		free(tp);
		exit(1);
	}
	int i=0;
	for(i=0;i<tp->num;i++){
		errorno=pthread_create(&tp->workThread[i],NULL,workProcess,NULL);
		if(errorno!=0){
			exit(1);
	 	}
	} 
	tp->taskHead=NULL;
	tp->taskTail=NULL;
	pthread_mutex_init(&tp->mutex,NULL);
	pthread_cond_init(&tp->notEmptyCond,NULL);
}

void addTask(char *dir){
	task *t=(task*)malloc(sizeof(task));
	if(t==NULL){
		exit(1);
	} 
	t->dir=(char*)malloc(strlen(dir));
	strcpy(t->dir,dir);
	t->next=NULL;
	pthread_mutex_lock(&tp->mutex);
	if(tp->taskTail!=NULL){
		tp->taskTail->next=t;
		tp->taskTail=t;
	}else{
		tp->taskHead=t;
		tp->taskTail=t;
	}  
	pthread_cond_broadcast(&tp->notEmptyCond);
	pthread_mutex_unlock(&tp->mutex);

}
void destroyThreadPool(){

}

char strContain(char*childstr,char*str){
	int clen=strlen(childstr);
	int len=strlen(str);
	int i=0;
	int j=0;
	for(i=0;i+clen<=len;i++){
		for(j=0;j<clen;j++){
			if(childstr[j]!=str[i+j]){
				break;
			}
		}
		if(j==clen)
			return 1;
	}
	return 0;
}

char strContainDEFINE(char*str){
	int len=strlen(str);
	int i=0;
	for(i=0;i+6<=len;i++){
		if((str[i]=='d')&&(str[i+1]=='e')&&(str[i+2]=='f')&&(str[i+3]=='i')&&(str[i+4]=='n')&&(str[i+5]=='e'))
			return 1;
	}
	return 0;
}

int countDefine(char*fileName){
	int lineNum=0;
	FILE *fp=fopen(fileName,"r");
	if(fp==NULL){
		perror(fileName);
	}
	char content[200];
	while(fgets(content,200,fp)){
		if(strContainDEFINE(content)==1){
			lineNum++;
		}
	} 
	fclose(fp);
	return lineNum;
}

int find(char * dirName){
	char fileName[1000];
	int lineNum=0;
	int dirLen=strlen(dirName);
	int error;
	DIR *dir;
	struct dirent entry;
	struct dirent *result;
	struct stat fstat;
	strcpy(fileName,dirName);
	fileName[dirLen]='/';
	dir = opendir(dirName);
	int len;
	for (;;) {
		fileName[dirLen+1]='\0';
		error = readdir_r(dir, &entry, &result);
		if (error != 0) {
			perror("readdir");
			return ;//EXIT_FAILURE;
	 	}
		if (result == NULL)
			break;
		strcat(fileName,result->d_name);
		stat(fileName,&fstat);

	 	if(S_ISDIR(fstat.st_mode)){
			if((strcmp(".",result->d_name)==0)||(strcmp("..",result->d_name)==0))
				continue;
			//printf("DIR :%s/%s**\n",dirName, result->d_name);
			//find(fileName);
			addTask(fileName);
		}else{
//			len=strlen(result->d_name);
//			if(len<2)
//				continue;
//			if((result->d_name[len-1]!='h')||(result->d_name[len-2]!='.'))
//				continue;	
	//		printf("FILE:%s/%s**\n",dirName, result->d_name);
			lineNum+=countDefine(fileName);
 	 	}
 	}  
	closedir(dir);
	return lineNum;
}




int main()
{
	pthread_mutex_init(&lineMutex,NULL);
//	find("/home/zgh/submit/test");
//	find("/usr/include");
	createThreadPool(2);
//	addTask("/home/zgh/submit/test");
	addTask("/usr/include");
	sleep(3);
	printf("%d\n",lineCount);
	return 0;
}

