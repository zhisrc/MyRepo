#ifndef CONN_H 
#define CONN_H 
#include "ymsg_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oci.h>
#include"keyword_match.h"
#include "write_orcl_circular_queue.h"
#include "telnet_log.h"
#include "logqueue.h"
typedef struct {
    OCIEnv        *EnvHP;        //OCI环境句柄
    OCIServer    *SrvHP;        //OCI服务器句柄
    OCISvcCtx    *SvcHP;        //OCI;服务器上下文句柄
    OCIError    *ErrHP;        //OCI错误句柄
    OCISession    *SessionHP;    //OCI对话句柄
    OCIStmt        *StmtHP;  
                               //OCI语句句柄
    OCIBind      **bindpp; 
} MY_OCI_CONTEXT_T;
int oci_init(MY_OCI_CONTEXT_T* ociCtx_p);
//static 
MATCHENTRY*  select_data(MY_OCI_CONTEXT_T* ociCtx_p,char *sql);
int select_data_temp(MY_OCI_CONTEXT_T* ociCtx_p,char *sql);
int insert_data(MY_OCI_CONTEXT_T* ociCtx_p,struct ymsg_log q);
int insert_httpdata(MY_OCI_CONTEXT_T* ociCtx_p,char *sql,log_t q);
int insert_ftpdata(MY_OCI_CONTEXT_T* ociCtx_p,char *sql ,struct Log q);
int insert_telnetdata(MY_OCI_CONTEXT_T* ociCtx_p,struct telnet_log q);
int insert_ipdata(MY_OCI_CONTEXT_T* ociCtx_p,struct telnet_log q);
void oci_clean(MY_OCI_CONTEXT_T* ociCtx_p);
#endif
