#include <nids.h>
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<libnet.h>
#include<pcap.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include "tcp_queue.h"
#include<pthread.h>
#include"ymsg_log.h"
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "nids.h"
#include <oci.h>
#include <time.h>
#include "data_hash.h"
#include "control_hash.h"
#include "update.h"
#include "write_orcl_circular_queue.h"
#include "conn.h"
#include "ftp_other_func.h"
#include "telnet_link.h"
#include "http.h"
#include <sys/types.h>
#define BACKLOG 10
#define SERVPORT 5002
extern SqQueue *Q ;
extern MATCHENTRY *ftp_entry;
extern MATCHENTRY *ftp_entry_1;
extern SqQueue *FTPQ;
//extern MY_OCI_CONTEXT_T  ociCtx_p1;
extern MY_OCI_CONTEXT_T  ociCtx_p;
extern YM_LOG *YM_LOG_Q ;
extern MATCHENTRY *entry;
extern  MATCHENTRY *entry_ym1;
//TEL
extern packet_telnet * telnet_head;

extern MATCHENTRY *telnet_entry;
extern  SqQueue * TelQ;
//HTTP
extern MY_OCI_CONTEXT_T ociCtx_p;
extern MATCHENTRY *urlmatch;
extern MATCHENTRY *http_blockmatch;
extern MATCHENTRY *http_match;
extern SqQueue *httpqueue;
extern int R;
//IP
extern MATCHENTRY *ip_entry;
extern int pid;
void SYS_INIT()
{
telnet_head = create_telnet_head(); //链表创建空的头节点
  TelQ = InitQueue(TelQ);
  Init_TELLOGQueue();
  FTPQ = InitQueue(FTPQ);
  orcl_create_queue();
  pthread_t ftpid;
  pthread_t orclid;
    //http
  httpqueue = InitQueue(httpqueue);
  InitlogQueue();
  IniturlQueue();
  pthread_t http_tid;
  pthread_t httplog_tid;
  pthread_t httpurl_tid;
//YMSG
  Q = InitQueue(Q);
  Init_YMLOGQueue();
   //nids_params.device = "1";//此处可以定义全局参数
  int i;
  int errNo = 0;
  text errInfo[1024];  
  i = oci_init(&ociCtx_p);
   //IP
   char *sql_ip ="select KEYWORD from IP_CONF";
  ip_entry =select_data(&ociCtx_p,sql_ip);  
  char *sql=" select keyword from ymsg_conf";
  entry=select_data(&ociCtx_p,sql); 
   char *sql_ftp ="select keyword from ftp_conf";
  ftp_entry = select_data(&ociCtx_p,sql_ftp);
  telnet_entry =select_data(&ociCtx_p,"select keyword from telnet_conf");
   
  char *sql_url = "select keyword from url_conf"; 
  char *sql_http = "select keyword from http_conf";
  char *sql_block = "select keyword from http_block_conf";
  http_blockmatch = select_data(&ociCtx_p,sql_block);
  http_match = select_data(&ociCtx_p,sql_block);
  urlmatch = select_data(&ociCtx_p,sql_url);
  pthread_t lsn;
  pthread_t up_d;
  pthread_t id2;
  pthread_t ym_con;
  pthread_t pcap;
  int ret_1;
  pthread_t tel_con;
  pthread_t tel_log;
  ret_1 = pthread_create(&pcap, NULL, pcap_tcp, NULL);
   if(ret_1 != 0)
    {
    printf("pcap_tcp%d creation failed \n", ret_1);
    exit(1);
    }
   ret_1 = pthread_create(&tel_con, NULL, telQ_consumer, NULL);
   if(ret_1 != 0)
   {
	printf("consumer%d creation failed \n", ret_1);
	exit(1);
   }
  ret_1 = pthread_create(&tel_log, NULL, tel_consumer, NULL);
  if(ret_1 != 0) 
  {
	printf("Tel_log%d creation failed \n", ret_1);
	exit(1);
  }
  ret_1 = pthread_create(&lsn, NULL,listen_up, NULL);
   if(ret_1 != 0)
   {
    printf("consumer%d creation failed \n", ret_1);
    exit(1);
   }
   ret_1 = pthread_create(&up_d, NULL,upd, NULL);
   if(ret_1 != 0)
   {
    printf("consumer%d creation failed \n", ret_1);
    exit(1);
   }

   ret_1 = pthread_create(&id2, NULL, consumer, NULL);
   if(ret_1 != 0)
   {
    printf("consumer%d creation failed \n", ret_1);
    exit(1);
   }
   ret_1 = pthread_create(&ym_con, NULL, ym_consumer, NULL);
   if(ret_1 != 0)
   {
    printf("consumer%d creation failed \n", ret_1);
    exit(1);
   }
 
   ret_1 = pthread_create(&ftpid,NULL,fetch_ftp_queue,NULL);
   if(ret_1!= 0)
   {
	printf("ftp consumer %d creation failed\n",ret_1);
	exit(1);
   }
	
  ret_1 = pthread_create(&orclid,NULL,fetch_orcl_queue,NULL);
  if(ret_1 != 0)
  {
	printf("orcl consumer %d creation failed\n",ret_1);
	exit(1);
  }
    /* Libnids进入循环捕获数据包状态 */
  ret_1 = pthread_create(&http_tid, NULL, http_consumer, NULL);
  if(ret_1 != 0)
  {
	printf("consum%d creation failed \n", ret_1);
	exit(1);
  }

  ret_1 = pthread_create(&httplog_tid, NULL, httplog_consumer, NULL);
  if(ret_1 != 0)
  {
	printf("consumer%d creation failed \n", ret_1);
	exit(1);
  }

  ret_1 = pthread_create(&httpurl_tid, NULL, httpurl_consumer, NULL);
  if(ret_1 != 0)
  {
	printf("consumer%d creation failed \n", ret_1);
	exit(1);
  }

   pthread_join(id2,NULL);
   pthread_join(lsn,NULL);
   pthread_join(up_d,NULL);
   pthread_join(ym_con,NULL);
   pthread_join(pcap,NULL);
   pthread_join(ftpid,NULL);
   pthread_join(orclid,NULL);
   pthread_join(http_tid,NULL);
   pthread_join(httplog_tid,NULL);
   pthread_join(httpurl_tid,NULL);
   pthread_join(tel_con,NULL);
   pthread_join(tel_log,NULL);
}
 

  
int main()
{  


  char *p;
  int i=0;
  char buf1[2048];
  long size;
  int sin_size;
  int sock_fd,client_fd;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  int opt = TRUE;
  
  if((sock_fd=socket(AF_INET,SOCK_STREAM,0))==-1)
  {
    printf("socket built fail\n");
    exit(1);
  }
  if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))!=0)
  {
   printf("set Error\n");
   exit(0);
   }
  my_addr.sin_family=AF_INET;
  my_addr.sin_port=htons(SERVPORT);
  my_addr.sin_addr.s_addr=INADDR_ANY;
  bzero(&(my_addr.sin_zero),8);
  if(bind(sock_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
  { int b;
  
      b = bind(sock_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr));
      if(b == -1)
        printf("upd Bind is error\n"); 
      
       else
      {printf("upd Bind is errorss\n");exit(1);}
  }
  if(listen(sock_fd,BACKLOG) ==-1)
  {
    printf("listen fail\n");
    exit(1);
  }
  while(1)
  {
    sin_size=sizeof(struct sockaddr_in);
    if((client_fd=accept(sock_fd,(struct sockaddr *)&remote_addr,&sin_size))==-1)    
    {
      printf("accept fail\n");
    }
   int   n=read(client_fd,buf1,2048);close(client_fd);

      p=strtok(buf1,"\r\n");
      if(strcmp(p,"9")==0)
      {  
         pid_t id=fork();
         if(id==0)
         {   close(sock_fd);
             execv("/home/cat/Desktop/restart1.1/ncms/ncms",NULL);           
           
         }
      }    
  } 


}

 

   

