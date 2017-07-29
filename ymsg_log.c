#include "ymsg_log.h"
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"conn.h"

extern pthread_mutex_t ymlog_mutex; // 互斥信号锁
extern pthread_cond_t ymlogfull_cond ;
extern pthread_cond_t ymlogempty_cond;

extern MY_OCI_CONTEXT_T  ociCtx_p;

extern YM_LOG *YM_LOG_Q ;
int Init_YMLOGQueue()
{ 
  YM_LOG_Q = (YM_LOG *) malloc(sizeof(YM_LOG));
  YM_LOG_Q->base = (struct ymsg_log *)malloc(MAX_YMSGQSIZE*sizeof(struct ymsg_log));
  if(!(YM_LOG_Q->base))
  {
    printf("分配错误\n");
     return 0;
  }
  YM_LOG_Q->front = YM_LOG_Q->rear = 0;
  return 1;
}
//int EnQueue(struct tcp_stream *tcp_connection)
int En_YMLOGQueue(struct ymsg_log q)
{ 
  printf("YMSG_LOG INSERT QUEUE \n");
  if((YM_LOG_Q->rear +1)% MAX_YMSGQSIZE == YM_LOG_Q->front )
    return 0;
   printf("E\n");
  YM_LOG_Q->base[YM_LOG_Q->rear] = q;
  YM_LOG_Q->rear = (YM_LOG_Q->rear+1)%MAX_YMSGQSIZE;
  printf("YMSGfronr %d \n",YM_LOG_Q->front);
  printf(" YMSGrear %d \n",YM_LOG_Q->rear);
    return 1;
}
//struct tcp_stream * DeQueue ( )
struct ymsg_log  De_YMLOGQueue()
{  
  struct ymsg_log  log;
  
  if(YM_LOG_Q->front == YM_LOG_Q->rear)
    {
    return ;}
  
  log = YM_LOG_Q->base[YM_LOG_Q->front];
  
  YM_LOG_Q->front = (YM_LOG_Q->front +1)%MAX_YMSGQSIZE;
  return log;
}
int YMLOG_QueueLength()
{
  return (YM_LOG_Q->rear - YM_LOG_Q->front + MAX_YMSGQSIZE )%MAX_YMSGQSIZE;
}

void* ym_consumer(void* arg)
{
   while(1)
   { 
     pthread_mutex_lock(&ymlog_mutex);
     while(YMLOG_QueueLength() ==0)  
     {  printf("this ymlog \n");
        pthread_cond_wait(&ymlogfull_cond,&ymlog_mutex); 
        printf("this ym_log \n");
       }       
          struct ymsg_log  q = De_YMLOGQueue();
        
       printf("this is log consumerrcharurrrr \n");
       insert_data( &ociCtx_p,q);
         free(q.time);
            free(q.sql);
     pthread_mutex_unlock(&ymlog_mutex); 
     pthread_cond_signal(&ymlogempty_cond);
      usleep(50);
     
  }
  
}

