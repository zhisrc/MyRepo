#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<pthread.h>
#include"conn.h"
#include"update.h"
 #include <sys/wait.h>

#define SERVPORT 5001
#define BACKLOG 10
#define LMAXQSIZE 100
 extern MY_OCI_CONTEXT_T  ociCtx_p;
   //Ac
extern int YM_FLAG;
extern int FTP_FLAG;
extern int TEL_FLAG;
extern MATCHENTRY *entry;
 extern MATCHENTRY *entry_ym1;
 extern  MATCHENTRY *telnet_entry_1;
extern MATCHENTRY *ftp_entry_1;
//http
extern int HTTP_FLAG;
extern int URL_FLAG;
extern int BLOCK_FLAG;
MATCHENTRY *urlmatch_1;
MATCHENTRY *http_blockmatch_1;
MATCHENTRY *http_match_1;
//IP
extern MATCHENTRY *ip_entry;
extern MATCHENTRY *ip_entry_1;
extern  IP_FLAG ;
extern int pid;
extern int R;
 pthread_mutex_t lmutex; // 互斥信号锁
 pthread_cond_t lfull_cond = PTHREAD_COND_INITIALIZER;
  pthread_cond_t lempty_cond = PTHREAD_COND_INITIALIZER;
 int lfd[LMAXQSIZE];
int lfront = 0;
int lrear = 0;
void *listen_up( void *arg )
{
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
  //  while(1)
    //{ 
      b = bind(sock_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr));
      if(b == -1)
        printf("upd Bind is error\n"); 
      //else
       // break;
      
     //}
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
      pthread_mutex_lock(&lmutex);
      int size_now = ( lrear - lfront + LMAXQSIZE )% LMAXQSIZE;
      while(size_now == LMAXQSIZE)
    
     { 
       printf("allbuffer is full,produceris waiting...\n");
       pthread_cond_wait(&lempty_cond,&lmutex);
       
     }
     printf("I am is producer..\n");
     lfd[lrear] = client_fd;
     lrear = (lrear+1) % LMAXQSIZE;
     pthread_mutex_unlock(&lmutex);
     pthread_cond_signal(&lfull_cond); 
     
     usleep(50);
      
  } 
 
 
}
void *upd(void *arg)
{ 
  char *p,*p1,*p2; 
  int client_fd;
  int i=0;
  int n;
  char buf1[2048];
  while(1)
   { 
     pthread_mutex_lock(&lmutex);
     while(((lrear - lfront + LMAXQSIZE )%LMAXQSIZE) == 0)
     {  printf("what happen\n");
        pthread_cond_wait(&lfull_cond,&lmutex); 
        printf("what happen here \n");
        usleep(50);
      }
     
     client_fd = lfd[lfront];
     lfront = (lfront +1)%LMAXQSIZE;
     
     pthread_mutex_unlock(&lmutex);
     pthread_cond_signal(&lempty_cond);
     
     n=read(client_fd,buf1,2048);close(client_fd);

      p=strtok(buf1,"\r\n");
    if(strcmp(p,"3")==0)
     {  
        telnet_entry_1 =select_data(&ociCtx_p,"select keyword from telnet_conf");  
        TEL_FLAG = 1;
     }
    else if(strcmp(p,"5")==0)
    {
     char *sql = " select keyword from ymsg_conf";
     entry_ym1 = select_data(&ociCtx_p,sql);
       YM_FLAG = 1;
    }
    else if(strcmp(p,"4")==0)
     {
      ftp_entry_1 = select_data(&ociCtx_p,"select keyword from ftp_conf");
      FTP_FLAG =1;
     }
    else if(strcmp(p,"6")==0)
     {
       ip_entry_1 = select_data(&ociCtx_p,"select keyword from ip_conf");
       IP_FLAG = 1;
     }
     else if(strcmp(p,"7")==0)
      {
       char *sql_url = "select keyword from url_conf"; 
        urlmatch_1 = select_data(&ociCtx_p,sql_url);
	  URL_FLAG = 1;
      }
      else if(strcmp(p,"1")==0)
       {
        char *sql_http = "select keyword from http_conf";
        http_match_1 = select_data(&ociCtx_p,sql_http);
     	  HTTP_FLAG = 1;
       }
      else if(strcmp(p,"2")==0)
      {
      char *sql_block = "select keyword from http_block_conf";
	http_blockmatch_1 = select_data(&ociCtx_p,sql_block);
	BLOCK_FLAG = 1;
      }   
      else if(strcmp(p,"0")==0)
      {  pid = getpid();
         printf("%d",pid); 
         kill(pid,SIGKILL);
         

      }  
       
     printf("\nHAVE UPDATE\n");
    
     usleep(50);
  }  
}
   



