#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 10

#define OVER -1

struct Buffer{
	int data[BUFFER_SIZE]; //缓冲区数组 
	int readPos; //读的位置 
	int writePos;//写的位置
	pthread_mutex_t lock; //互斥锁  
	pthread_cond_t notEmpty; //非空信号 
	pthread_cond_t notFull; //非满信号   
}buffer;

void writeData(struct Buffer* buffer, int data);
int readData(struct Buffer* buffer);
void* ProducerThread();
void* ConsumerThread();
void initBuffer(struct Buffer* buffer);

int main(){
    	pthread_t producerThread;
    	pthread_t consumerThread;
    	void *retval;
    
    	initBuffer(&buffer);
    	pthread_create(&producerThread, NULL, ProducerThread, NULL);
    	pthread_create(&consumerThread, NULL, ConsumerThread, NULL);
    	pthread_join(producerThread, &retval);
    	pthread_join(consumerThread, &retval);
        
	printf("Task Over.\n");
}

void writeData(struct Buffer* buffer, int data){
	pthread_mutex_lock(&buffer->lock); //获取互斥锁
	while((buffer->writePos + 1) % BUFFER_SIZE == buffer->readPos){
		pthread_cond_wait(&buffer->notFull, &buffer->lock); /*等待状态变量 b->notFull，不满则跳出阻塞*/
    }
	/*写入数据*/
	buffer->data[buffer->writePos] = data;
	if(data!=OVER){
		printf("Producer: Put data %d to buffer.\n", data);
	}else printf("Producer: end of input.\n");	
	buffer->writePos = (buffer->writePos+1)%BUFFER_SIZE;
	pthread_cond_signal(&buffer->notEmpty); //设置缓冲区非空信号
        pthread_mutex_unlock(&buffer->lock);  //释放互斥锁
}

int readData(struct Buffer* buffer){
	int data;
	pthread_mutex_lock(&buffer->lock); //获取互斥锁	
	while(buffer->writePos == buffer->readPos){
		pthread_cond_wait(&buffer->notEmpty, &buffer->lock); /*等待状态变量 b->notEmpty，不空则跳出阻塞*/
	}	
	/*提取数据*/
	data = buffer->data[buffer->readPos];	
	if(data!=OVER){
		printf("Consumer: Read data %d from buffer.\n", data);
	}else printf("Consumer: end of read.\n");
	buffer->readPos = (buffer->readPos+1)%BUFFER_SIZE;	
	pthread_cond_signal(&buffer->notFull); //设置缓冲区非信号      
	pthread_mutex_unlock(&buffer->lock);  //释放互斥锁
	return data;
}

void* ProducerThread(){
	int i;
	for(i=0; i<1000; i++){
			
		//生产者写入随机数
        	writeData(&buffer, i);
	}
	writeData(&buffer, OVER);//结束写入	
}

void* ConsumerThread(){
	int data;
	while(1){
		data=readData(&buffer);
		if(data == OVER) break;
	}
}

void initBuffer(struct Buffer* buffer){
	memset(buffer->data, 0, sizeof(buffer->data));
	buffer->readPos=0;
	buffer->writePos=0;
	pthread_mutex_init(&buffer->lock, NULL);
	pthread_cond_init(&buffer->notEmpty, NULL);
	pthread_cond_init(&buffer->notFull, NULL);
}
