#include<stdio.h>
#include<dirent.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#define FILE_NAME_SIZE 256
char curPath[4096];
int getComNum(char**args,short,int*comBegPos);
void getComNumAndRedirectedPos(char **args,int *pComNum,int*pRedirectedPos);
int isRedirected(char **args,short*);
int child(int,int,char**);
void myCd(char*arg);
void myPwd(int out,char*arg);
void execCom(int in,int out,char** args);
void getCurPath(char*currentPath);
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
	int comBegPos[20];
	char **args;	
	short argNum=0;
	pid_t child_id;
	int redirectPos=0;
	char*redirectedFileName;
	int fd[2];
	int readPipe;
	int comNum=0;
	int lastOut[2];
	getCurPath(curPath);
	printf("%s$",curPath);
	gets(command);
	while(strcmp(command,"exit")!=0){
		getComAndArgs(command,&args,&argNum);
		comNum=getComNum(args,argNum,comBegPos);
		getComNumAndRedirectedPos(args,&comNum,&redirectPos);
		printf("command:%s,redirectPos:%d\n",command,redirectPos);
		printf("\n\ncommandInfo\ncomNum=%d,isRedirected:%d\n",comNum,redirectPos);
		for(i=0;i<argNum;i++){
			if(args[i]==NULL){
				puts("NULL");
			}else{
				puts(args[i]);
			}
		}
		for(i=0;i<comNum;i++){
			printf("pos:%d\t",comBegPos[i]);
		}
		printf("\n\n\ncontent below:\n");
		lastOut[1]=1;
		if(redirectPos!=0)
		{
			pipe(lastOut);
			redirectedFileName=args[comBegPos[comNum-1]+redirectPos]+1;
			args[comBegPos[comNum-1]+redirectPos]=NULL;
		}
		if(comNum==1){
		//just one command and no pipe
			execCom(0,lastOut[1],args);			
		}else{
			//Given that if pipe(fd) ,fd[0]>0&&fd[1]>0;
			for(i=0;i<comNum;i++){
				if(i==0){
					//the first one
					pipe(fd);
					readPipe=0;
					execCom(readPipe,fd[1],args+comBegPos[i]);
				}else if(i==comNum-1){
					//the last one
					close(fd[1]);
					if(readPipe!=0)
						close(readPipe);
					readPipe=fd[0];
					execCom(readPipe,lastOut[1],args+comBegPos[i]);
				}
				else{
					//not first nor last
					close(fd[1]);
					if(readPipe!=0)
						close(readPipe);
					readPipe=fd[0];
					pipe(fd);
					execCom(readPipe,fd[1],args+comBegPos[i]);
				}
			}
			if(readPipe!=0)
				close(readPipe);
		}
		//has redirectPos and the result must in lastOut[0];
		if(redirectPos!=0){
			int file=open(redirectedFileName,O_RDWR|O_CREAT,S_IRWXU);
			char buffer[10000];
			int len;
			len=read(lastOut[0],buffer,10000);
			write(file,buffer,len);
			close(file);
			close(lastOut[0]);
			close(lastOut[1]);
		}
		int k=0;
		for(k=0;k<argNum;k++){
			//	puts(args[k]);	
			if(args[k]!=NULL);
				free(args[k]);
		}
		free(args);
		printf("%s$",curPath);
		gets(command);
	}	
	puts("sh3.c结束运行");
}

int  child(int in,int out,char**args){
	if(in!=0){
		dup2(in,0);
		close(in);
	}
	if(out!=1){
		dup2(out,1);
		close(out);
	}
	return execvp(args[0],args);
}

void execCom(int in,int out,char** args){
		pid_t child_id;
		if(strcmp(args[0],"pwd")==0){
			myPwd(out,args[1]);
		}else if(strcmp(args[0],"cd")==0){
			myCd(args[1]);
		}else{
			child_id=fork();
			if(child_id==0){
				int error=child(in,out,args);
				if(error<0){
					printf("execCom:%s  failed!\n",args[0]);
				}
			}
			wait(NULL);
		}
}
void getCurPath(char*currentPath){
	currentPath[0]='\0';
	getPath(getFileInoByName("."),currentPath);
	int error=chdir(currentPath);
	if(error!=0){
		perror("getCurPath chdir error");
	}
}


void myPwd(int out,char*arg){
	char* fileName=(char*)malloc(4096);
	fileName[0]='\0';
	if(arg==NULL){
		getPath(getFileInoByName("."),fileName);
	}else{
		getPath(getFileInoByName(arg),fileName);
	}
	/*if(out!=1){
	}else{
		puts(fileName);
	}*/
	write(out,fileName,strlen(fileName));
	write(out,"\n",1);
	int error=chdir(fileName);
	if(error!=0){
		perror("run pwd chdir error");
	}
	free(fileName);
}

void myCd(char*arg){
	int error;
	if(arg==NULL){
		error=chdir("..");
	}else{
		error=chdir(arg);
	}
	if(error!=0){
		perror("run cd error");
	}else{
		getCurPath(curPath);
	}
}
//preprocessing command  to make sure that there is no more than one space between two arguments and not end whth space;
void prepareCommand(char*command){
	int i=0;
	int pos=0;
	int distance=0;
	for(i=0;command[i]!='\0';i++){
		if(command[i]==' '){
			distance=0;
			pos=i+1;
			while(command[pos++]==' ')
				distance++;
			if(distance>0){
				pos=i+1;
				while(command[pos+distance]!='\0'){
					command[pos]=command[pos+distance];
					pos++;
				}
				command[pos]='\0';
			}
		}
	}
	if(i>0&&command[i]=='\0'&&command[i-1]==' '){
		command[i-1]='\0';
	}
}
//命令处理，将命令与参数分开
void getComAndArgs(char*command,char ***pargs,short*pArgNum){
	prepareCommand(command);
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
		strcat(fileName,"/\0");
		strcat(fileName,fileNameBuffer);
	}
}

int getComNum(char **args,short argNum,int*comBegPos){
	int i=0;
	int comNum=1;
	comBegPos[0]=0;
	for(i=1;i<argNum-1;i++){
		if(strcmp("|",args[i])==0){
			comBegPos[comNum]=i+1;
			comNum++;
			free(args[i]);
			args[i]=NULL;
		}
	}
	return comNum;
}
void getComNumAndRedirectedPos(char **args,int *pComNum,int*pRedirectedPos){
	int i;
	int pos=0;
	short totalLen=0;
	short comLen=0;
	*pRedirectedPos=0;
	for(i=0;i<*pComNum;i++){
		pos=isRedirected(args+totalLen,&comLen);
		if(pos!=0){
			*pComNum=i+1;
			*pRedirectedPos=pos;
			return;
		}
		totalLen+=comLen;
	}

}

int isRedirected(char **args,short *comLen){
	int i=0;
	for(i=1;args[i]!=NULL;i++){
		if(args[i][0]=='>'){
			return i;
		}
	}
	*comLen=i+1;
	return 0;
}



