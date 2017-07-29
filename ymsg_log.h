#ifndef  YMSG_LOG_H 
#define  YMSG_LOG_H 
#define MAX_YMSGQSIZE 50
typedef struct ymsg_log
{
   char  *key;
   unsigned int saddr ;
   unsigned int src;
   unsigned int daddr;
   unsigned int dst;
    char *time;
   char *sql;
}YM;

typedef struct 
{

  YM *base;
  int front;
  int rear;
}YM_LOG;
extern YM_LOG *YM_LOG_Q ;
int Init_YMLOGQueue();
int En_YMLOGQueue(struct ymsg_log q);
struct ymsg_log  De_YMLOGQueue();
int YMLOG_QueueLength();
void* ym_consumer(void* arg);
#endif
