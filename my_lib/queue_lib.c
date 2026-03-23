#include "queue_lib.h"
#include "string.h"

//==============队列数据结构==============↓
/**
* @brief 初始化队列
* @param queue: 队列结构体指针
* @param arr: 存放消息的数组指针
* @param size: 队列大小
*/
void QueueInit(S_Queue *queue, uint8_t *arr, uint16_t size)
{
	queue->pArr = arr;
	queue->size = size;
	queue->pArr[queue->front] = 0;
	queue->pArr[queue->rear] = 0;
}

/**
* @brief 读取队列长度
* @param queue: 队列结构体指针
* @return 返回队列剩余数据长度
*/
uint16_t QueueLength(S_Queue *queue)
{
			//通用队列计算长度，rear > front 或 rear < front都可用
	return (queue->rear + (queue->size - queue->front)) % queue->size;
}

/**
* @brief 将数据存入队列中
* @param queue: 队列结构体指针
* @param data: 要存放的数据
* @return	0, 成功；1, 失败
*/
uint8_t EnQueue(S_Queue *queue, uint8_t data)
{
	if((queue->rear + 1) % queue->size == queue->front){		//判断队列是否已满
		return 1;
	}
	
	queue->pArr[queue->rear] = data;		//将数据插入队尾
	queue->rear = (queue->rear + 1) % queue->size;		//队尾后移，若到数组尾部，则转到数组头部
	
	return 0;
}

/**
* @brief 将数据从队列读出中
* @param queue: 队列结构体指针
* @param data: 存放读出数据的指针
* @return	0, 成功；1, 失败
*/
uint8_t DeQueue(S_Queue *queue, uint8_t *data)
{
	if(queue->front == queue->rear){		//判断队列是否为空
		return 1;
	}
	
	*data = queue->pArr[queue->front];		//读取元素
	queue->pArr[queue->front] = 0;		//清空当前空间
	queue->front = (queue->front + 1) % queue->size;		//front指针后移，若到数组尾部，则转到数组头部
	
	return 0;
}

//============================================
/**
* @brief 在EnQueue()基础上封装，用于一次存放大量数据
* @param queue: 队列结构体指针
* @param arr: 存放数据首地址
* @param size: 存放数据长度
* @return	0, 成功；1, 失败
*/
uint8_t EnQueueLarge(S_Queue *queue, uint8_t *arr, uint16_t size)
{
	if((queue->size - QueueLength(queue)) < size){		//判断剩余空间是否足够
		return 1;
	} else {        //足够则往队列里存入数据
		for(int i = 0; i < size; i++){
			EnQueue(queue, arr[i]);     //入队
		}
	}
	
	return 0;
}

/**
* @brief 在DeQueue()基础上封装，用于一次取出大量数据
* @param queue: 队列结构体指针
* @param arr: 取出数据首地址
* @param size: 取出数据长度
* @return	0, 成功；1, 失败
*/
uint8_t DeQueueLarge(S_Queue *queue, uint8_t *arr, uint16_t size)
{
	if(QueueLength(queue) < size){		//判断队列内的数据长度是否足够
		return 1;
	} else {
		for(int i = 0; i < size; i++){
			DeQueue(queue, &arr[i]);		//出队
		}
	}
	
	return 0;
}
