#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>
#include<iostream> 
#define Thread_Num 20

using namespace std;

pthread_mutex_t Device_mutex;

struct VirtualPCB
{
    int tid;
    int priority;
    int waittime;
    int runtime;
    int arrivetime;
    int visited;
    int tempruntime;
    public:
        int gettid()
        {
            return tid; 
        }
        int getwaittime()
        {
            return waittime;    
        }
        int getpriority()
        {
            return priority;
        }
        int getruntime()
        {
            return runtime;
        }
        int getarrivetime()
        {
            return arrivetime;
        }
        void setvisit(int a)
        {
            visited=a;
        }
        int getvisit()
        {
            return visited;
        }
        int gettempruntime()
        {
            return tempruntime;
        }
        void setwaittime(int n)
        {
            waittime = n;   
        }
        void settempruntime(int n)
        {
            tempruntime = tempruntime - n;
        }
}TCB[Thread_Num]; 

//PCB��ʼ������ 
void t_init()
{
    int n;
    srand(time(NULL));
    for(n =0;n<Thread_Num;n++)
    {
    TCB[n].tid = n + 1;//���̴߳��������Ϊ����id
    //������������������PCB��ֵ
    TCB[n].priority = 1 + rand()%19;
    TCB[n].runtime = 1 + rand()%19;
    TCB[n].arrivetime = rand()%201;//�ٶ������߳���0~500ʱ������̾��� 
    TCB[n].waittime = 0;
    TCB[n].visited =0;
    TCB[n].tempruntime = TCB[n].runtime;
    }
}

//�߳��������� 
void *t_print(void *arg)
{
    int n = *(int *)arg;//��ȡ��ǰ�߳�id 
    while(1)
    {
            pthread_mutex_lock(&Device_mutex);
            printf("Thread_%-2d: ", n);
            printf("tid:%-2d priority:%-2d arrivetime:%-2d runtime:%-2d \n",TCB[n-1].gettid(), TCB[n-1].priority, TCB[n-1].arrivetime, TCB[n-1].runtime);
            pthread_mutex_unlock(&Device_mutex);
            sleep(1);
        break;
    }
    pthread_exit(0);
}

//FIFO
void FIFO()
{
    cout<<"-----------FIFO:"<<endl;
    int i,j;
    int start = 0;
    float waittime = 0;
    float avwait = 0;
    for(i=0;i<201;i++)
    {
        for(j=0;j<Thread_Num;j++){
            if(TCB[j].getarrivetime()==i && TCB[j].getvisit()==0){
            	if(start<TCB[j].getarrivetime()) start = TCB[j].getarrivetime();//��ת��� 
                printf("Thread: %-2d  Arrive: %-3d   Start: %-3d   Runtime: %-2d\n",
					TCB[j].gettid(),TCB[j].getarrivetime(),start,TCB[j].getruntime());
				TCB[j].waittime=(float)start - TCB[j].getarrivetime();
				waittime+=TCB[j].getwaittime();
                start = start + TCB[j].getruntime();
                TCB[j].setvisit(1);
            }
        }
    }
    avwait = waittime / (float)Thread_Num;
    printf("Total waitting time : %f\n",waittime);
    printf("Average waitting time : %f\n",avwait);
}

//�����̺߳��� 
void *Children(void*)
{
    int ret[Thread_Num];
    t_init();
    pthread_t tid[Thread_Num];
    pthread_mutex_init(&Device_mutex,NULL);
    int i,j;
	for(i=0;i<Thread_Num;i++)
	{
        int k =i+1;
        ret[i] = pthread_create(&tid[i],NULL,&t_print, &k);
        if(ret[i] == 0) {
            sleep(1);
        }
        else{ 
        	printf("Thread_%-2d failed!\n",i+1);
        }   
    }
    for(j=0;j<Thread_Num;j++)
    pthread_join (tid[i], NULL);
    pthread_mutex_destroy(&Device_mutex);
    pthread_exit(0);
}

int main()
{
    int ret1;
    pthread_t tid1;//���߳� 
    ret1 = pthread_create(&tid1,NULL,&Children,NULL);
    
    if(ret1 == 0) 
    {
        printf("Main Thread done\n");
        sleep(20);
    }
        else{ 
        printf("Thread failed!\n");

    }
    FIFO();
    return 0;
}

