/*************************************************************************
	> File Name: logqueue.c
	> Author: ganzhixiong
	> Mail: ahigzx@163.com 
	> Created Time: Mon 21 Dec 2015 03:06:23 PM CST
 ************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include "logqueue.h"
#include "nids.h"
#define MAXQSIZE 100
#include "conn.h"

extern MY_OCI_CONTEXT_T ociCtx_p;
extern Httplog_Sq *httplog_Q;
extern pthread_mutex_t httplogmtx;
extern pthread_cond_t httplog_fullcond;
extern pthread_cond_t httplog_emptycond;

extern pthread_mutex_t httpurlmtx;
extern pthread_cond_t httpurl_fullcond;
extern pthread_cond_t httpurl_emptycond;
extern urllog_Sq *urllog_Q;

//int InitQueue(Httplog_Sq **Q)
//{ 
//	*Q = (Httplog_Sq *) malloc(sizeof(Httplog_Sq));
//	*Q ->base = (struct http_log *)malloc(MAXQSIZE*sizeof(struct http_log));
//	if(!(*Q ->base))
//	{
//		printf("分配错误\n");
//		return 0;
//	}
//	*Q->front = *Q->rear = 0;
//	return 1;
//}

int InitlogQueue()
{
	httplog_Q = (Httplog_Sq *) malloc(sizeof(Httplog_Sq));
	httplog_Q ->base = (http_log *)malloc(MAXQSIZE*sizeof(http_log));
	if(!(httplog_Q ->base))
	{
		printf("分配错误\n");
		return 0;
	}
	httplog_Q->front = httplog_Q->rear = 0;
	return 1;
}

int EnlogQueue(http_log q, Httplog_Sq *Q)
{ 
	printf("http1En\n");
	if((Q->rear +1)% MAXQSIZE == Q->front )
		return 0;
	printf("httpEn\n");
	Q->base[Q->rear] = q;
	Q->rear = (Q->rear+1)%MAXQSIZE;
	printf("httpfronr %d \n",Q->front);
	printf(" httprear %d \n",Q->rear);
	return 1;
}
//struct tcp_stream * DeQueue ( )
http_log DelogQueue(Httplog_Sq *Q)
{  
	http_log log;

	if(Q->front == Q->rear)
	{
		return ;
	}

		log = Q->base[Q->front];

		Q->front = (Q->front +1)%MAXQSIZE;
		return log;
}

int logQueueLength(Httplog_Sq *Q)
{
	return (Q->rear - Q->front + MAXQSIZE )%MAXQSIZE;
}



void httplog_producer(char *findword, struct tuple4 ip_and_port, int is_block, void **arg)
{ 
	if(findword)
	{  //struct tuple4 ip_and_port = tcp_connection->addr;
		pthread_mutex_lock(&httplogmtx);
		while(logQueueLength(httplog_Q) == MAXQSIZE)
		{ 
			//printf("buffer is full,producer is waiting...\n");
			pthread_cond_wait(&httplog_emptycond,&httplogmtx);
		}
			http_log q ;
			//printf("I am producer..\n");
			
			strcpy(q.keyword,findword);
			q.saddr = ip_and_port.saddr;
			q.daddr = ip_and_port.daddr;
			q.source = ip_and_port.source;
			q.dest   = ip_and_port.dest;
			q.is_block = is_block;

			EnlogQueue(q,httplog_Q);
			pthread_mutex_unlock(&httplogmtx);
			pthread_cond_signal(&httplog_fullcond);
			usleep(100);
	}
	return ;
}           

void *httplog_consumer(void *arg)
{
	while(1)
	{ 
		pthread_mutex_lock(&httplogmtx);
		while(logQueueLength(httplog_Q) ==0)  
		{  
			//printf("what happen\n");
			pthread_cond_wait(&httplog_fullcond,&httplogmtx); 
			//printf("what happen here \n");
		}
		//http_protocol_callback(DeQueue(httplog_Q).tcp_connection,NULL);
		//写日志
		http_log q;
		log_t log;
		q = DelogQueue(httplog_Q);

		strcpy(log.key,q.keyword);
		log.saddr = q.saddr;
		log.daddr = q.daddr;
		log.src = q.source;
		log.dst = q.dest;

		if(q.is_block)
		{
			char *sql = "insert into http_blog values(httpseq_b.nextval,:1,:2,:3,:4,:5,:6)";
			insert_httpdata(&ociCtx_p, sql, log);
			//写封堵日志
		}
		else
		{
			char *sql = "insert into http_slog values(httpseq_s.nextval,:1,:2,:3,:4,:5,:6)";
			insert_httpdata(&ociCtx_p, sql, log);
			//写监测日志
		}
		pthread_mutex_unlock(&httplogmtx);
		pthread_cond_signal(&httplog_emptycond);
		usleep(50);
	}
  
}

int IniturlQueue()
{
	urllog_Q = (urllog_Sq *) malloc(sizeof(urllog_Sq));
	urllog_Q ->base = (log_t *)malloc(MAXQSIZE*sizeof(log_t));
	if(!(urllog_Q ->base))
	{
		printf("分配错误\n");
		return 0;
	}
	urllog_Q->front = urllog_Q->rear = 0;
	return 1;
}

int EnurlQueue(log_t q, urllog_Sq *Q)
{ 
	printf("1En\n");
	if((Q->rear +1)% MAXQSIZE == Q->front )
		return 0;
	printf("En\n");
	Q->base[Q->rear] = q;
	Q->rear = (Q->rear+1)%MAXQSIZE;
	printf("fronr %d \n",Q->front);
	printf(" rear %d \n",Q->rear);
	return 1;
}
//struct tcp_stream * DeQueue ( )
log_t DeurlQueue(urllog_Sq *Q)
{  
	log_t log;

	if(Q->front == Q->rear)
	{
		return ;
	}

		log = Q->base[Q->front];

		Q->front = (Q->front +1)%MAXQSIZE;
		return log;
}

int urlQueueLength(urllog_Sq *Q)
{
	return (Q->rear - Q->front + MAXQSIZE )%MAXQSIZE;
}
void httpurl_producer(char *findword, struct tuple4 ip_and_port, void **arg)
{ 
	if(findword)
	{  //struct tuple4 ip_and_port = tcp_connection->addr;
		pthread_mutex_lock(&httpurlmtx);
		while(urlQueueLength(urllog_Q) == MAXQSIZE)
		{ 
			printf("buffer is full,producer is waiting...\n");
			pthread_cond_wait(&httpurl_emptycond,&httpurlmtx);
		}
			log_t q ;
			//printf("I am producer..\n");
			
			strcpy(q.key,findword);
			q.saddr = ip_and_port.saddr;
			q.daddr = ip_and_port.daddr;
			q.src = ip_and_port.source;
			q.dst = ip_and_port.dest;

			EnurlQueue(q,urllog_Q);
			pthread_mutex_unlock(&httpurlmtx);
			pthread_cond_signal(&httpurl_fullcond);
			usleep(100);
	}
	return ;
}           

void *httpurl_consumer(void *arg)
{
	while(1)
	{ 
		pthread_mutex_lock(&httpurlmtx);
		while(urlQueueLength(urllog_Q) ==0)  
		{  
			printf("what happen http\n");
			pthread_cond_wait(&httpurl_fullcond,&httpurlmtx); 
			printf("what happen http \n");
		}
		//http_protocol_callback(DeQueue(httplog_Q).tcp_connection,NULL);
		//写日志
		log_t q;
		q = DeurlQueue(urllog_Q);
                printf("\nssssssssssxxx\n");
		char *sql = "insert into http_urlog values(httpseq_url.nextval,:1,:2,:3,:4,:5,:6)";
		insert_httpdata(&ociCtx_p, sql, q);

		pthread_mutex_unlock(&httpurlmtx);
		pthread_cond_signal(&httpurl_emptycond);
		usleep(50);
	}
  
}
