#include "telnet_log.h"
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"conn.h"

extern pthread_mutex_t tellog_mutex; // 互斥信号锁
extern pthread_cond_t tellogfull_cond ;
extern pthread_cond_t tellogempty_cond;

extern MY_OCI_CONTEXT_T  ociCtx_p;

extern TELNET_LOG *TEL_LOG_Q ;
int Init_TELLOGQueue()
{ 
  TEL_LOG_Q = (TELNET_LOG*) malloc(sizeof(TELNET_LOG));
  TEL_LOG_Q->base = (struct telnet_log *)malloc(MAX_TELNETQSIZE*sizeof(struct telnet_log));
  if(!(TEL_LOG_Q->base))
  {
    printf("分配错误\n");
     return 0;
  }
  TEL_LOG_Q->front = TEL_LOG_Q->rear = 0;
  return 1;
}
//int EnQueue(struct tcp_stream *tcp_connection)
int En_TELLOGQueue(struct telnet_log q)
{ 
  printf("TEL_LOG_Q INSERT QUEUE \n");
  if((TEL_LOG_Q->rear +1)% MAX_TELNETQSIZE == TEL_LOG_Q->front )
    return 0;
   printf("E\n");
  TEL_LOG_Q->base[TEL_LOG_Q->rear] = q;
  TEL_LOG_Q->rear = (TEL_LOG_Q->rear+1)%MAX_TELNETQSIZE;
  printf("TELNETfronr %d \n",TEL_LOG_Q->front);
  printf(" TELNETrear %d \n",TEL_LOG_Q->rear);
    return 1;
}
//struct tcp_stream * DeQueue ( )
struct telnet_log  De_TELLOGQueue()
{  
  struct telnet_log  log;
  
  if(TEL_LOG_Q->front == TEL_LOG_Q->rear)
    {
    return ;}
  
  log = TEL_LOG_Q->base[TEL_LOG_Q->front];
  
  TEL_LOG_Q->front = (TEL_LOG_Q->front +1)%MAX_TELNETQSIZE;
  return log;
}
int TELLOG_QueueLength()
{
  return (TEL_LOG_Q->rear - TEL_LOG_Q->front + MAX_TELNETQSIZE )%MAX_TELNETQSIZE;
}

void* tel_consumer(void* arg)
{
   while(1)
   { 
     pthread_mutex_lock(&tellog_mutex);
     while(TELLOG_QueueLength() ==0)  
     {  printf("this tellog consumer\n");
        pthread_cond_wait(&tellogfull_cond,&tellog_mutex); 
        printf("this tel_log \n");
       }       
          struct telnet_log  q = De_TELLOGQueue();
        if(!strcmp(q.key,"ip"))
        {printf("this is IPconsumerrcharurrrr \n");
            insert_ipdata( &ociCtx_p,q);
           }
           
        else
          { 
         insert_telnetdata( &ociCtx_p,q);
           }
     pthread_mutex_unlock(&tellog_mutex); 
     pthread_cond_signal(&tellogempty_cond);
      usleep(50);
     
  }
  
}

