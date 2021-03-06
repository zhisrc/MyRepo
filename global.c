#include"conn.h"
#include "telnet_log.h"
#include "telnet_link.h"
#include "tcp_queue.h"
#include "associate.h"
#include "ymsg_log.h"
#include "logqueue.h"
 MY_OCI_CONTEXT_T  ociCtx_p;
 MATCHENTRY *entry;
 MATCHENTRY *entry_ym1;
 int YM_FLAG = 0;
 SqQueue *Q ;
 YM_LOG *YM_LOG_Q =NULL;
 ass_node *table=NULL;
 pthread_mutex_t ymlog_mutex; // 互斥信号锁
 pthread_cond_t ymlogfull_cond = PTHREAD_COND_INITIALIZER;
 pthread_cond_t ymlogempty_cond = PTHREAD_COND_INITIALIZER;
 
pthread_mutex_t mutex; // 互斥信号锁
pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty_cond = PTHREAD_COND_INITIALIZER;



//ftp互斥锁 
int FTP_FLAG = 0;
SqQueue *FTPQ;

//orcl互斥锁 
pthread_mutex_t orclmutex;
pthread_cond_t orcl_full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t orcl_empty_cond = PTHREAD_COND_INITIALIZER;
//用于自动机
MATCHENTRY *ftp_entry;
MATCHENTRY *ftp_entry_1;
 pthread_mutex_t ftpmutex;
 pthread_cond_t ftpfull_cond = PTHREAD_COND_INITIALIZER;
 pthread_cond_t ftpempty_cond = PTHREAD_COND_INITIALIZER; 



//TELNET
int TEL_FLAG = 0;
 SqQueue TelQ;

 MATCHENTRY *telnet_entry;
 MATCHENTRY *telnet_entry_1;
 packet_telnet *telnet_head;
 //int TEL_FLAG = 0;
 //SqQueue *Q = NULL;
 TELNET_LOG *TEL_LOG_Q=NULL;
 //ass_node *table=NULL;
 pthread_mutex_t tellog_mutex; // 互斥信号锁
 pthread_cond_t tellogfull_cond = PTHREAD_COND_INITIALIZER;
 pthread_cond_t tellogempty_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t telbuffer_mutex; // 互斥信号锁
pthread_cond_t telbufferfull_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t telbufferempty_cond = PTHREAD_COND_INITIALIZER;

//HTTP
MATCHENTRY *urlmatch;
MATCHENTRY *http_blockmatch;
MATCHENTRY *http_match;
MATCHENTRY *urlmatch_1;
MATCHENTRY *http_blockmatch_1;
MATCHENTRY *http_match_1;
int URL_FLAG = 0;
int HTTP_FLAG = 0;
int BLOCK_FLAG = 0;
pthread_mutex_t httpmutex;
pthread_cond_t http_fullcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t http_empty_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t httplogmtx;
pthread_cond_t httplog_fullcond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t httplog_emptycond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t httpurlmtx;
pthread_cond_t httpurl_fullcond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t httpurl_emptycond = PTHREAD_COND_INITIALIZER;
SqQueue *httpqueue = NULL;
Httplog_Sq *httplog_Q = NULL;
urllog_Sq *urllog_Q = NULL;
//IP
MATCHENTRY *ip_entry;
MATCHENTRY *ip_entry_1;
int IP_FLAG = 0;
//
int pid =-1;
int R =0;

