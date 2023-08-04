#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	pid_t pid;
	pid = fork();
	if(pid<0){//error
		fprintf(stderr, "Fork Failed");
		return 1;
	}else if(pid==0){//child process
		printf("I'm b."); 
	}else{//parent process
		pid = fork();
		if(pid<0){
			fprintf(stderr, "Fork Failed");
			return 1;
		}else if(pid==0){//another child process
			printf("I'm c.");
		}else{
			printf("I'm a.");
			wait(NULL);
		}
	}
	return 0;
} 
