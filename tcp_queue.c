#include <nids.h>
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<libnet.h>
#include<pcap.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include"conn.h"
#include"update.h"
#include "tcp_hash.h"
#include "tcp_queue.h"
#include<pthread.h>
#include "ftp_other_func.h"
#include"associate.h"
#include "apue.h"
#include "telnet.h"
#include "http.h"

extern pthread_mutex_t mutex; // 互斥信号锁
extern pthread_cond_t full_cond ;
extern pthread_cond_t empty_cond ;
extern pthread_mutex_t ymlog_mutex; // 互斥信号锁
extern pthread_cond_t ymlogfull_cond ;
extern pthread_cond_t ymlogempty_cond ;


extern MY_OCI_CONTEXT_T  ociCtx_p;
extern SqQueue * Q;
SqQueue * InitQueue(SqQueue *X)
{  SqQueue *QQ = X ;
  QQ = (SqQueue *) malloc(sizeof(SqQueue));
  QQ->base = (struct Qnode *)malloc(MAXQSIZE*sizeof(struct Qnode));
  if(!(QQ->base))
  {
    printf("分配错误\n");
     return NULL;
  }
  QQ->front = QQ->rear = 0;
  return QQ;
}
//int EnQueue(struct tcp_stream *tcp_connection)
int EnQueue(struct Qnode q,SqQueue *X)
{ SqQueue *QQ = X ;
  printf("1En\n");
  if((QQ->rear +1)% MAXQSIZE == QQ->front )
    return 0;
   
  QQ->base[QQ->rear] = q;printf(" UQUU wrong\n");
  QQ->rear = (QQ->rear+1)%MAXQSIZE;
  printf("fronr %d \n",QQ->front);
  printf(" rear %d \n",QQ->rear);
    return 1;
}
//struct tcp_stream * DeQueue ( )
struct Qnode  DeQueue(SqQueue *X)
{  SqQueue *QQ = X ;
  struct Qnode  tcp_connection;
  
  if(QQ->front == QQ->rear)
    {
    return ;}
  
  tcp_connection = QQ->base[QQ->front];
  
  QQ->front = (QQ->front +1)%MAXQSIZE;
   printf("xxxxxxDEQUEUE\n");
  return tcp_connection;
}
int QueueLength(SqQueue *X)
{SqQueue *QQ = X;
  return (QQ->rear - QQ->front + MAXQSIZE )%MAXQSIZE;
}


  void tcp_protocol_callback(struct tcp_stream *tcp_connection, void **arg)
{  struct tuple4 ip_and_port = tcp_connection->addr;
   if(Find_anode(ip_and_port.daddr)||ip_and_port.dest == 5050||ip_and_port.source == 5050)
  {  
    pthread_mutex_lock(&mutex);
    while(QueueLength(Q) == MAXQSIZE)
    
     { 
      printf("allbuffer is full,produceris waiting...\n");
      pthread_cond_wait(&empty_cond,&mutex);
     }
    struct Qnode q ;
    printf("I am is producer..\n"); 
    q.tcp_connection = tcp_connection;
    EnQueue(q,Q);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&full_cond);
    usleep(50);

    }
    return ;
}           
void tcp_protocol_callback_1(struct tcp_stream *tcp_connection, void **arg)
{   int ip;
    char buf_filename[128];
    char buf_ip[20];
    char buf_length[64]; 
    
    //bzero(buf_filename,128);
   // bzero(buf_ip,20);
   // bzero(buf_length,64);
    //bzero(buf,2048);
    char *a_filename;
   
    FILE *fp;//保存聊天文件
    int i;
    int k;
    char address_string[1024];
    
    char content[65535];
    FILE * fp_file;//保存传输文件
   // char content_urgent[65535];
    struct tuple4 ip_and_port = tcp_connection->addr;
    if(ip_and_port.dest == 5050||ip_and_port.source == 5050)
    {
    switch (tcp_connection->nids_state) /* 判断LIBNIDS的状态 */
    {
        case NIDS_JUST_EST:
            /* 表示TCP客户端和TCP服务器端建立连接状态 */
            tcp_connection->client.collect++;
            /* 客户端接收数据 */
            tcp_connection->server.collect++;
            /* 服务器接收数据 */
            tcp_connection->server.collect_urg++;
            /* 服务器接收紧急数据 */
            tcp_connection->client.collect_urg++;
            /* 客户端接收紧急数据 */
            printf("%sTCP连接建立\n", address_string);
            return ;
        case NIDS_CLOSE:
            /* 表示TCP连接正常关闭 */
            printf("--------------------------------\n");
            printf("%sTCP连接正常关闭\n", address_string);
            return ;
        case NIDS_RESET:
            /* 表示TCP连接被RST关闭 */
            printf("--------------------------------\n");
            printf("%sTCP连接被RST关闭\n", address_string);
            return ;
        case NIDS_DATA:
            /* 表示有新的数据到达 */
            {     struct half_stream *hlf;
                /* 表示TCP连接的一端的信息，可以是客户端，也可以是服务器端 */
                if (tcp_connection->server.count_new_urg)
                {  
                  
                    return ;
                }
                if (tcp_connection->client.count_new_urg)
                {
                    
                    return ;
                }
                if (tcp_connection->client.count_new)
                {              
                    hlf = &tcp_connection->client;
                    
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                    printf("一号客户端接收数据\n");
                    if( content[10] == 0 && content[11] == 6)//解析聊天
                    {
                       analy_chat(content,hlf->count_new,ip_and_port);
                    }
                      else if(content[10]== 0 && content[11]==-36)
                    {    
                      analy_fileinfo(content,hlf->count_new);
                    }
                      else if(content[10]== 0 && content[11]==-35)//文件信息
                    {
                      analy_fileinfo_2(content,hlf->count_new);
                    }
                    else  if(content[10]== 0 && content[11]== -34)
                    {
                       analy_uri(content,hlf->count_new);//解析出uri
                    }

                }
                else//我发送的才能收到！！！！ 
                {
                    /* 表示服务器端接收到新的数据 */
                    hlf = &tcp_connection->server;
                    /* 此时hlf表示服务器端的TCP连接信息 */
                   
                    memcpy(content, hlf->data, hlf->count_new);
                    content[hlf->count_new] = '\0';
                     
                    if( content[10] == 0 && content[11] == 6)//解析聊天
                    {
                       analy_chat(content,hlf->count_new,ip_and_port);
                    }
                    else if(content[10]== 0 && content[11]==-36)
                    {    
                      analy_fileinfo(content,hlf->count_new);
                    }
                    else if(content[10]== 0 && content[11]==-35)//文件信息
                    {
                      analy_fileinfo_2(content,hlf->count_new);
                    }
                    else if(content[10]== 0 && content[11]== -34)
                    {
                       analy_uri(content,hlf->count_new);//解析出uri
                    }//文件传输information
                }//else data_state
             }//case
      default :
      break ;
      }//SWITCH
      
     //hashtable_print(0);
    return ;
   } //else iftansfer file tcp ip
     //

  
   else
    { 
      switch (tcp_connection->nids_state) /* 判断LIBNIDS的状态 */
     {
        case NIDS_JUST_EST:
            /* 表示TCP客户端和TCP服务器端建立连接状态 */
            tcp_connection->client.collect++;
            /* 客户端接收数据 */
            tcp_connection->server.collect++;
            /* 服务器接收数据 */
            tcp_connection->server.collect_urg++;
            /* 服务器接收紧急数据 */
            tcp_connection->client.collect_urg++;
            /* 客户端接收紧急数据 */
            printf("%sTCP连接建立\n", address_string);
            return ;
        case NIDS_CLOSE:
            /* 表示TCP连接正常关闭 */
            
            hashdelete(&ip_and_port);
            return ;
        case NIDS_RESET:
            /* 表示TCP连接被RST关闭 */
            printf("--------------------------------\n");
            printf("%sTCP连接被RST关闭\n", address_string);
                 hashdelete(&ip_and_port);
            return ;
        case NIDS_DATA:
            /* 表示有新的数据到达 */
            {
                struct half_stream *hlf;
                /* 表示TCP连接的一端的信息，可以是客户端，也可以是服务器端 */
                if (tcp_connection->server.count_new_urg)
                {
                  
                    return ;
                }
                if (tcp_connection->client.count_new_urg)
                {     
                    
                    return ;
                }
                if (tcp_connection->client.count_new)//
                {  
                   { hlf = &tcp_connection->client;
                  /* 此时hlf表示服务器端的TCP连接信息 */
                  strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                  sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                  strcat(address_string, " ---> ");
                  strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                  sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                  strcat(address_string, "\n");
                  printf("------------浏览器接受------------------\n");
                  printf("the size is  hlg->datta%d  %i\n",hlf->count_new,ip_and_port.saddr);
                  printf("%s", address_string);
                  memcpy(content, hlf->data, hlf->count_new);
                  if(hlf->count_new !=0)
                  {        printf("zhelishi\n");
                           if(content[0] =='H' && content[1] =='T' && content[2] =='T' && content[3] =='P')
                                printf("\n我不要HTTP\n");
                           else
                           {
                             char *file_content = (char*)malloc(hlf->count_new);
                             memcpy(file_content,content,hlf->count_new);
                             hashtable_insert(&ip_and_port,NULL,file_content,0,hlf->count_new);
                           }

                  }
	          }
                }
                else//我发送的文件的TCP/* 表示服务器端接收到新的数据 */
                {   
                   hlf = &tcp_connection->server;
                  /* 此时hlf表示服务器端的TCP连接信息 */
                  strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
                  sprintf(address_string + strlen(address_string), ":%i", ip_and_port.source);
                  strcat(address_string, " ---> ");
                  strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
                  sprintf(address_string + strlen(address_string), ":%i", ip_and_port.dest);
                  strcat(address_string, "\n");
                  printf("--------------------------------\n");
                  printf("%s", address_string);
                  memcpy(content, hlf->data, hlf->count_new);
                  printf("the size is  hlg->datta%d \n",hlf->count_new);
                  
                  content[hlf->count_new] = '\0';
	          
                    if(content[0]=='P'&&content[1]=='O'&&content[2]=='S')
                    { 
                      if(hlf->count_new !=0)
                        analy_filecontent(content,hlf->count_new,ip_and_port);
                    }
                   else if (content[0]=='G'&&content[1]=='E'&&content[2]=='T')                   
                   { 
                     if(hlf->count_new !=0)  
                        analy_stocfilecontent(content,hlf->count_new,ip_and_port);  
                   }
                   
                   else   
                   {       
                      char *file_content = (char*)malloc(hlf->count_new);
                      printf("\ngetssssssssssssssssssssssssssssssssss\n"); 
                      memcpy(file_content,content,hlf->count_new);
                      hashtable_insert(&ip_and_port,NULL,file_content,0,hlf->count_new);
                  }
                   //serve发来的                 
                }//接受文件的 
            }//case
      default :
      break ;
      }//SWITCH    
    return ;
   }   
 return ;  
}
void* consumer(void* arg)
{
   while(1) 
   { 
     pthread_mutex_lock(&mutex);
     while(QueueLength(Q) ==0)  
     {  printf("what happen YMSG consumer\n");
        pthread_cond_wait(&full_cond,&mutex); 
         
           
     }
     tcp_protocol_callback_1(DeQueue(Q).tcp_connection,NULL);     
     pthread_mutex_unlock(&mutex);
     pthread_cond_signal(&empty_cond);
     
     
  }
  
}
void* pcap_tcp(void* arg)
{
   if (!nids_init())
     /* Libnids初始化 */
    {
        printf("出现错误：%s\n", nids_errbuf);
        exit(1);
    }      
   printf("lalla \n");  
   nids_register_tcp(telnet_protocol_callback);  
    nids_register_tcp(tcp_protocol_callback);//可以过滤端口号的，所有的回调汉函数都返回，tcpstream才会释放。   
    nids_register_tcp(ftp_protocol_callback);
     
       nids_register_tcp(http_producer);
    // nids_register_tcp(tcp_protocol_callback_3);
     nids_run();
  
}


