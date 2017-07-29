#ifndef  TELNET_LOG_H 
#define  TELNET_LOG_H 
#define MAX_TELNETQSIZE 50
typedef struct telnet_log
{
   char * key;
   unsigned int saddr ;
   unsigned int src;
   unsigned int daddr;
   unsigned int dst;
    char *time;
}TEL;

typedef struct 
{

  TEL *base;
  int front;
  int rear;
}TELNET_LOG;
extern TELNET_LOG *TEL_LOG_Q ;
int Init_TELLOGQueue();
int En_TELLOGQueue(struct telnet_log q);
struct telnet_log  De_TELLOGQueue();
int TELLOG_QueueLength();
void* tel_consumer(void* arg);
#endif
