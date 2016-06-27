#include<stdio.h>
#include<dirent.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#define FILE_NAME_SIZE 256
int isRedirected(char **args,short argNum);
char curPath[4096];
ino_t getFileInoByName(char*);
void getFileNameByIno(ino_t ino,char*);
void getPath(ino_t,char*);
void getComAndArgs(char*,char*** args,short*); 
int main(int argc,char* argv[]){
	//获取当前路径
	int i=0;
	for(i=0;i<argc;i++){
		puts(argv[i]);
	}
	char command[1000];	//限制命令长度为999个字符
	char **args;	
	short argNum=0;
	gets(command);
	pid_t child_id;
	int redirectPos=0;
	int fd=-1;
	
	while(strcmp(command,"exit")!=0){
		getComAndArgs(command,&args,&argNum);
		if(strcmp(args[0],"echo")==0){
			redirectPos=isRedirected(args,argNum);
			if(redirectPos!=0){
				fd=open(args[redirectPos]+1,O_RDWR | O_CREAT);
				if(fd==-1){
					printf("create file failed:fileName=%s",args[redirectPos]+1);
				}else{
					args[redirectPos]=NULL;
				}
			}
			child_id=fork();
			if(child_id==0){
				//child
				if(redirectPos!=0&&fd!=-1){
					dup2(fd,1);
				}
				int error=execvp("echo",args);
				if(error<0){
					perror("run echo error");
				}
				puts("this is childThread:");
			}
			int status;
			wait(&status);
			if(!WIFEXITED(status)){
				printf("echo child thread exit failed");
			}
			if(redirectPos!=0&&fd!=-1){
				close(fd);
			}
		}else if(strcmp(args[0],"ls")==0){
			child_id=fork();
			if(child_id==0){
				//child
				int error=execvp("ls",args);
				if(error<0){
					perror("run ls error");
				}
			}
		}else if(strcmp(args[0],"pwd")==0){
			char* fileName=(char*)malloc(4096);
			fileName[0]='\0';
			if(args[1]==NULL){
				getPath(getFileInoByName("."),fileName);
			}else{
				getPath(getFileInoByName(args[1]),fileName);
			}
			puts(fileName);
			int error=chdir(fileName);
			if(error!=0){
				perror("run pwd chdir error");
			}
			free(fileName);
		}else if(strcmp(args[0],"cd")==0){
			int error=chdir(args[1]);
			if(error!=0){
				perror("run cd error");
			}
		}else{
			//puts("no this file!");
			child_id=fork();
			if(child_id==0){
				//child
				int error=execvp(args[0],args);
				if(error<0){
					perror("1");
				}
			}
		}
		/*puts(args[0]);
		  free(args[0]);*/
		int k=0;
		for(k=0;k<argNum;k++){
			//	puts(args[k]);	
			free(args[k]);
		}
		free(args);
		gets(command);
	}	
	puts("sh1.c结束运行");
}


//命令处理，将命令与参数分开
void getComAndArgs(char*command,char ***pargs,short*pArgNum){
	int commandLen=strlen(command);
	int i=0;
	int j=0;
	int pos=0; 	//记录每次断点
	int len=0;	//命令或者参数长度
	*pArgNum=0;
	for(i=0;i<commandLen;i++)
		if(command[i]==' ')
			len++;
	(*pargs)=(char**)malloc(sizeof(char*)*(len+2));
	for(i=0;i<=commandLen;i++){
		if(command[i]==' '||command[i]=='\0'){
				((*pargs)[*pArgNum])=malloc(i-pos+1);
				j=0;
				while(pos<i){
					((*pargs)[*pArgNum][j++])=command[pos++];
				} 
				((*pargs)[*pArgNum][j])='\0';
				pos++;
				(*pArgNum)++;
		}		
	}	
	((*pargs)[len+1])=NULL;
	*pArgNum=len+2;
}

ino_t getFileInoByName(char* fileName){
	struct stat buf;
	if((stat(fileName,&buf))!=0){
		perror("getFileInoByName() fail");
		return 1;
	}
	return buf.st_ino;
}

void getFileNameByIno(ino_t ino,char*fileNameBuffer){
	DIR *d;
	struct dirent *dp;
	if((d=opendir("."))==NULL){
		perror("Can't open dir");
		exit(1);
	}
	while((dp=readdir(d))!=NULL){
		if(dp->d_ino==ino){

			strcpy(fileNameBuffer,dp->d_name);
			closedir(d);
			return;
		}
	}
	perror("Can't find this dirtory!");
	exit(2);
}
void getPath(ino_t ino,char *fileName){
	char fileNameBuffer[FILE_NAME_SIZE];
	if(getFileInoByName("..")!=ino){
		chdir("..");
		getFileNameByIno(ino,fileNameBuffer);
		getPath(getFileInoByName("."),fileName);
//		printf("/%s",fileNameBuffer);
		strcat(fileName,"/\0");
		strcat(fileName,fileNameBuffer);
	}
}
int isRedirected(char **args,short argNum){
	int i=0;
	for(i=1;i<argNum-1;i++){
		if(args[i][0]=='>'){
			return i;
		}
	}
	return 0;
}



