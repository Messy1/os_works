#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>

#define SHMKEY 318

int main(){
    int id;//共享内存标识符
    char *addr;//指向共享内存
    char message[512];
    id=shmget(SHMKEY,512,0666|IPC_CREAT);//创建共享内存
    if(fork()==0){//子进程
        sprintf(message,"%d wrote to parent",getpid());
        printf("%s\n",message);
        addr=shmat(id,0,0);//将共享内存连接到当前进程的地址空间
        strcpy(addr,message);//将内容放入共享内存
        shmdt(addr);//将共享内存从当前进程中分离
     }else{
        wait(NULL);//等待子进程执行完成
        addr=shmat(id,0,0);//将共享内存连接到当前进程的地址空间
        printf("%s\n",addr);//将共享内存中的内容输出
        printf("parent copy.\n");
        shmdt(addr);//将共享内存从当前进程中分离
        shmctl(id,IPC_RMID,0);//删除共享内存段
      }
    return 0;
}
