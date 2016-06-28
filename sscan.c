#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include      <stdlib.h>    //exit()
#include      <string.h>    //strerror()
#include      <errno.h>     //errno
#include      <sys/types.h> //stat()
#include      <sys/stat.h>
int lineCount=0;
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
	FILE *fp=fopen(fileName,"r");
	if(fp==NULL){
		perror(fileName);
	}
	char content[200];
	while(fgets(content,200,fp)){
		if(strContainDEFINE(content)==1)
			lineCount++;
	} 
	fclose(fp);
}

void find(char * dirName){
	char fileName[1000];
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
			printf("DIR :%s/%s**\n",dirName, result->d_name);
			find(fileName);
		}else{
			len=strlen(result->d_name);
			if(len<2)
				continue;
			if((result->d_name[len-1]!='h')||(result->d_name[len-2]!='.'))
				continue;	
			printf("FILE:%s/%s**\n",dirName, result->d_name);
			countDefine(fileName);
 	 	}
 	}  
	closedir(dir);
}


int main()
{
//	find("/home/zgh/submit/test");
	find("/usr/include");
	printf("%d\n",lineCount);
	return 0;
}

