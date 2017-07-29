#ifndef __WRITE_ORCL_CIRCULAR_QUEUE_H
#define __WRITE_ORCL_CIRCULAR_QUEUE_H
#define MAX_SIZE 65536
//#include <nids.h>
typedef struct Log
{
  char saddr[20];
  int src;
  char daddr[20];
  int dst;
  char text[100];
  char time[100];
}logstru;
typedef struct orclQnode
{
  char sql[200];
  struct Log log;
  struct orclQnode *next;
}orclnode;
typedef struct 
{
	orclnode *base;
	int front;//指向队列第一个元素
	int rear;//指向队列最后一个元素的下一个元素
}orclQueue;
int orcl_create_queue();
int orclEnqueue(struct orclQnode q);  
struct orclQnode orclDequeue(); 	
#endif
