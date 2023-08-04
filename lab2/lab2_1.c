#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/msg.h> 
#include <sys/ipc.h> 

#define SVKEY 318

struct msgform {//消息结构
   long mtype; 
   char mtext[250];
}msg; 

int msgqid,pid,*pint,i; 

void client() { 
   msgqid=msgget(SVKEY,0777);//打开75#消息队列
   pid=getpid();
   pint=(int *)msg.mtext;//把正文的内容传给 pint，并强制转换类型
   *pint=pid;//pint指针指向client进程标识符
   msg.mtype=1;//消息类型为1
   msgsnd(msgqid,&msg,sizeof(int),0);//发送消息msg入msgqid消息队列
   msgrcv(msgqid,&msg,250,pid,0);//从队列msgqid接收消息msg 
   printf("(client):receive reply from pid=%d\n",*pint);//显示 server进程标识数
   exit(0); 
} 

void server( ) 
{ 
   msgqid=msgget(SVKEY,0777|IPC_CREAT);//创建75#消息队列 
   msgrcv(msgqid,&msg,250,1,0);//接收client进程标识数消息 
   pint=(int *)msg.mtext;//把正文的内容传给 pint，并强制转换类型 
   pid=*pint;//获得 cilent 进程标识数
   printf("(server):serving for client pid=%d\n",pid); 
   msg.mtype=pid;//消息类型为 client 进程标识数
   *pint=getpid();//获取 server 进程标识数 
   msgsnd(msgqid,&msg,sizeof(int),0);//发送消息 
   exit(0); 
} 

int main() 
{ 
   while((i=fork())==-1);//创建进程 1 
   if(!i)server(); 
   while((i=fork())==-1);//创建进程 2 
   if(!i) client(); 
   wait(0); 
   wait(0); 
   return 0;
}
