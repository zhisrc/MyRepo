/*************************************************************************
	> File Name: client_analysis.c
	> Author: ganzhixiong
	> Mail: ahigzx@163.com 
	> Created Time: Wed 02 Dec 2015 07:48:51 PM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "http.h"
#include "tcp_queue.h"
#include "associate.h"
#include "logqueue.h"
#define MAXSIZE 30


extern pthread_mutex_t httpmutex;
extern pthread_cond_t http_fullcond; 
extern pthread_cond_t http_empty_cond;

extern MATCHENTRY *urlmatch;
extern MATCHENTRY *http_blockmatch;
extern MATCHENTRY *http_match;
extern SqQueue *httpqueue;
extern Httplog_Sq *httplog_Q;

//暂时简要注释，返回实体数据的值，记录需要修改的值。
char * client_data_analysis(char content[], int number, char *contype, int *cont_encoding, int *tran_encoding,int *datalen)
{
	char headinfo[MAXSIZE];  
	char *entity_data;
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int type_already_find = 0; //创建一个标记记录是否已经找到字段，下同
	int tran_already_find = 0;
	int cont_already_find = 0;
	*cont_encoding = 0;//第一个包先初始化 
	*tran_encoding = 0;
	for (i = 0; i < number; i++)
	{
		if (content[i] != '\n')
		{
			k++;  
			continue;
		}
		l = (k< MAXSIZE-1)?k:MAXSIZE-1; //如果行很长只需取前几十个字节
		for(j = 0; j < l; j++)
		{
			headinfo[j] = content[j + i - k];
		}
		headinfo[j] = '\0';
		if (!type_already_find)
		{   
			if (strstr(headinfo, "Content-Type"))
			{
				//	contype = (char*)malloc(6*sizeof(char));
				sscanf(headinfo,"%*s %4s",contype);  //取content-type前4个字符(不为text即可不对数据进行处理)
				printf("Content-Type:%s\n",contype);
				type_already_find = 1;
			}
		}

		if (!tran_already_find)
		{      
			if (strstr(headinfo, "Transfer-Encoding"))
			{     
				//sscanf(headinfo,"%*s %s",tran_encoding);
				*tran_encoding = 1;					//如果匹配到Transfer-Encoding只可能是chunked
				printf("transfer_encoding is chunked\n");
				tran_already_find = 1;
			}
		}

		if (!cont_already_find)
		{         
			if (strstr(headinfo, "Content-Encoding"))
			{
				//sscanf(headinfo,"%*s %s",cont_encoding);
				*cont_encoding = 1;					//如果匹配到Content-Encoding即为gzip
				printf("content_encoding is gzip\n");
				cont_already_find = 1;
			}
                       
		}

		if ((content[i] == '\n') && (content[i + 1] == '\r') &&(content[i + 2] == '\n'))
		{     
			if (i + 3 == number)
			{
				printf("无实体内容\n");
				return NULL;
			}
			entity_data = content + i + 3; //用指针指向实体数据部分的开头并返回,这里可用长度减(i+3)获得当前包内的实体数据长度
			*datalen = number - i - 3; //实体数据部分长度
			printf("实体部分长度为%d\n",*datalen);
			return entity_data;
		}
		k = 0; //k表示行的字符数重新置0
	}
			return NULL;

}

void *http_consumer(void *arg)
{
	while(1)
	{ 
		pthread_mutex_lock(&httpmutex);
		while(QueueLength(httpqueue) ==0)  
		{  printf("what happen http consumer\n");
			pthread_cond_wait(&http_fullcond,&httpmutex); 
			printf("what happen http here \n");
		}
		http_protocol_callback(DeQueue(httpqueue).tcp_connection,NULL);
                         
		pthread_mutex_unlock(&httpmutex);
		pthread_cond_signal(&http_empty_cond);
		usleep(50);
	}
  
}

void http_producer(struct tcp_stream *tcp_connection, void **arg)
{         
	struct tuple4 ip_and_port = tcp_connection->addr;
      if (Find_anode(ip_and_port.daddr))
      return;
	if(ip_and_port.dest == 80||ip_and_port.source == 80)
	{  //struct tuple4 ip_and_port = tcp_connection->addr;

		pthread_mutex_lock(&httpmutex);
		while(QueueLength(httpqueue) == MAXQSIZE)
		{ 
			printf("buffer is full,producer is waiting...\n");
			pthread_cond_wait(&http_empty_cond,&httpmutex);
		}
			struct Qnode q ;
			printf("I am producer..\n");
			q.tcp_connection = tcp_connection;
			EnQueue(q,httpqueue);
			pthread_mutex_unlock(&httpmutex);
			pthread_cond_signal(&http_fullcond);
			usleep(100);
	}
	return ;
}           

void http_protocol_callback(struct tcp_stream *tcp_http_connection, void **param)
{       printf("llllhttp\n");
	char address_content[1024];
	char content[65535]     = {0};
	char ungzip_data[65535] = {0};
	struct tuple4 ip_and_port = tcp_http_connection->addr;
	strcpy(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
    sprintf(address_content + strlen(address_content), " : %i", ip_and_port.source);
    strcat(address_content, " <----> ");
    strcat(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
    sprintf(address_content + strlen(address_content), " : %i", ip_and_port.dest);
    strcat(address_content, "\n");
     printf("cllaback http\n");
    if (tcp_http_connection->nids_state == NIDS_JUST_EST)
    {    
        if (tcp_http_connection->addr.dest != 80)
         /* 只捕获HTTP协议数据包 */
        {
            return ;
        }
        tcp_http_connection->client.collect++; /* 浏览器接收数据 */
        tcp_http_connection->server.collect++; /* WEB服务器端接收数据 */
        printf("\n\n\n==============================================\n");
        printf("%s 建立连接...ss\n", address_content);
          
        return ;
    }
    if (tcp_http_connection->nids_state == NIDS_CLOSE)
    {
        printf("--------------------------------\n");
        printf("%s连接正常关闭...\n", address_content);
        /* 连接正常关闭 */
        return ;
    }
    if (tcp_http_connection->nids_state == NIDS_RESET)
    {    
        printf("--------------------------------\n");
        printf("%s连接被RST关闭...\n", address_content);
        /* 连接被RST关闭 */
         printf("cllaback http1\n");
        return ;
    }
    if (tcp_http_connection->nids_state == NIDS_DATA)
    {           
		http_analysis(tcp_http_connection); 
               printf("cllaback http12\n");
	}
	return ;
}
