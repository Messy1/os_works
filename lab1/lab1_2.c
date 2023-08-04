#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(){
	pid_t pid;
	pid=fork();
	if(pid<0){
		fprintf(stderr, "Fork Failed");
		return 1;
	}else if(pid==0){
		execl("/bin/ls","ls","-1","-color", NULL);
		printf("exec fail!/n");
		exit(1);
	}else{
		wait(NULL);
		printf("is completed!/n");
		exit(0);
	}
}
