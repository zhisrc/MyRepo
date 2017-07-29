/*************************************************************************
	> File Name: logqueue.h
	> Author: ganzhixiong
	> Mail: ahigzx@163.com 
	> Created Time: Tue 22 Dec 2015 01:41:29 PM CST
 ************************************************************************/
#ifndef LOG_QUEUE_H
#define LOG_QUEUE_H
#include "nids.h"
typedef struct httplog_node
{
	char keyword[200];
	int is_block;
	//char *keyword;
	unsigned int saddr;
	unsigned int daddr;
	unsigned short source;
	unsigned short dest;
	
}http_log;


typedef struct httplog_inode
{
	//char *keyword;
	unsigned int saddr;
	unsigned int daddr;
	unsigned short source;
	unsigned short dest;
	char keyword[200];
	
}httplog_insert;   //这块结构体可以应该都是一样的 到时候改个名字就可以了

typedef struct 
{
	http_log *base;
	int front;
	int rear;
}Httplog_Sq;

typedef struct log_node
{
	unsigned int saddr;
	int src;
	unsigned int daddr;
	int dst;
	char key[200];
}log_t; //跟httplog_in一样。。先这样不改

typedef struct 
{
	log_t *base;
	int front;
	int rear;
}urllog_Sq;


void httplog_producer(char *findword, struct tuple4 ip_and_port, int is_block, void **arg);
int QueuelogLength(Httplog_Sq *Q);
int InitlogQueue();
int EnlogQueue(http_log q, Httplog_Sq *Q);
http_log DelogQueue(Httplog_Sq *Q);
void *httplog_consumer(void *arg);
int IniturlQueue();
int EnurlQueue(log_t q, urllog_Sq *Q);
log_t DeurlQueue(urllog_Sq *Q);
int urlQueueLength(urllog_Sq *Q);
void httpurl_producer(char *findword, struct tuple4 ip_and_port, void **arg);
void *httpurl_consumer(void *arg);
#endif
