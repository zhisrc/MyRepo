#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oci.h>
#include"conn.h"
#include "telnet_log.h"
#include <time.h>
#include "ymsg_log.h"
#include<sys/socket.h>
#include<arpa/inet.h>
//tns名
 extern MY_OCI_CONTEXT_T  ociCtx_p;
 extern MY_OCI_CONTEXT_T  ociCtx_p1;
 extern MATCHENTRY *entry;
 extern MATCHENTRY *entry_ym1;
 char*   pConnectChar    = "orcl";

//用户名和密码

char*    pUsernameChar   = "orcl";
char*    pPasswordChar   = "orcl";
  
int insert_ftpdata(MY_OCI_CONTEXT_T* ociCtx_p,char *sql ,struct Log q)
{  
	 int p_bvi;
    char sqlcmd[256]="";
    sword retcode;
    OCIDefine *defcolp[3];
    OCIBind * p_bndp1[6];//六个参数
    sword indp[3];
    char pointId[100];
    char pointId1 [50];
    char pointName[30];
    char pointDesc[60];
    //生成sql语句
 
    strcpy(sqlcmd,sql);//刚开始语法错乱无''
    
    //准备SQL语句
    retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)sqlcmd,
                      (ub4)strlen((char *)sqlcmd),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   
    printf("%s\n",sql);
     char saddr[40];
     strcpy(saddr,q.saddr);
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[0],ociCtx_p->ErrHP, (ub4)1,&saddr,(sb4)(sizeof(saddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
    unsigned int src = q.src;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[1],ociCtx_p->ErrHP, (ub4)2,(dvoid *)&src,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char daddr[40];
  strcpy(daddr,q.daddr);
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[2],ociCtx_p->ErrHP, (ub4)3,&daddr,(sb4)(sizeof(daddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     unsigned int dst = q.dst;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[3],ociCtx_p->ErrHP, (ub4)4,(dvoid *)&dst,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char ym_time[40];
        strcpy(ym_time,q.time);

retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[4],ociCtx_p->ErrHP, (ub4)5,&ym_time,(sb4)(sizeof(ym_time)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
   char  id[40] ;
    strcpy(id,q.text);
   retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[5],ociCtx_p->ErrHP, (ub4)6,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
   printf("bind inss  error\n");
  /*  //sql语句输入有几列，有几列就定义几次绑定字符和整形不宜样赋值
    //定义输入变量0OCIBindByPos(p_sql,&p_bndp2,p_err,2,&getName,(sb4)sizeof(getName),SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)
    retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1,ociCtx_p->ErrHP, (ub4)1,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
    
    if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
*/
     //sql语句有两列输出，有几列就定义几次
    //定义输出变量0(dvoid *)&pointId,zhengxingbind
   retcode=OCIDefineByPos(ociCtx_p->StmtHP, &defcolp[0], ociCtx_p->ErrHP, (ub4)1, &pointId, (sb4)(sizeof(pointId)), SQLT_STR , (dvoid *)&indp[0], (ub2 *)0, (ub2 *)0, (ub4) OCI_DEFAULT);
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIDefineByPos 0 \n");
    }

  
    pointName[29] = '\0';
    //执行SQL语句
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    //查错
    if(retcode!=OCI_SUCCESS&&retcode!=OCI_NO_DATA)
    {
        int errNo = 0;
        retcode = OCIErrorGet ((dvoid*) ociCtx_p->ErrHP, (ub4) 1, (text *) NULL, &errNo,
                                (text*)pointName, (ub4)sizeof(pointName)-1, (ub4) OCI_HTYPE_ERROR);
        printf("error OCIStmtExecute No=%d Info=%s \n", errNo, pointName);
        return -1;
    }
     char comm[20];
     strcpy(comm,"commit");
      retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)comm,
                      (ub4)strlen((char *)comm),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
	printf("******FTP insert sucess****** \n");

}
int insert_httpdata(MY_OCI_CONTEXT_T* ociCtx_p,char *sql,log_t q)
{   
    int p_bvi;
    char sqlcmd[256]="";
    sword retcode;
    OCIDefine *defcolp[3];
    OCIBind * p_bndp1[6];//六个参数
    sword indp[3];
    char pointId[100];
    char pointId1 [50];
    char pointName[30];
    char pointDesc[60];
    //生成sql语句
 
    strcpy(sqlcmd,sql);//刚开始语法错乱无''
    
    //准备SQL语句
    retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)sqlcmd,
                      (ub4)strlen((char *)sqlcmd),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   
    printf("%s\n",sql);
     char saddr[40];
     strcpy(saddr,inet_ntoa(*((struct in_addr*) &(q.saddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[0],ociCtx_p->ErrHP, (ub4)1,&saddr,(sb4)(sizeof(saddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
    unsigned int src = q.src;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[1],ociCtx_p->ErrHP, (ub4)2,(dvoid *)&src,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char daddr[40];
  strcpy(daddr,inet_ntoa(*((struct in_addr*) &(q.daddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[2],ociCtx_p->ErrHP, (ub4)3,&daddr,(sb4)(sizeof(daddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     unsigned int dst = q.dst;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[3],ociCtx_p->ErrHP, (ub4)4,(dvoid *)&dst,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");

                   time_t timep;  
                time(&timep);  
                  char ym_time[40];
                strcpy(ym_time,ctime(&timep)); 
     
        

retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[4],ociCtx_p->ErrHP, (ub4)5,&ym_time,(sb4)(sizeof(ym_time)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
   char  id[40] ;
    strcpy(id,q.key);
   retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[5],ociCtx_p->ErrHP, (ub4)6,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
   printf("bind inss  error\n");
  /*  //sql语句输入有几列，有几列就定义几次绑定字符和整形不宜样赋值
    //定义输入变量0OCIBindByPos(p_sql,&p_bndp2,p_err,2,&getName,(sb4)sizeof(getName),SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)
    retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1,ociCtx_p->ErrHP, (ub4)1,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
    
    if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
*/
     //sql语句有两列输出，有几列就定义几次
    //定义输出变量0(dvoid *)&pointId,zhengxingbind
   retcode=OCIDefineByPos(ociCtx_p->StmtHP, &defcolp[0], ociCtx_p->ErrHP, (ub4)1, &pointId, (sb4)(sizeof(pointId)), SQLT_STR , (dvoid *)&indp[0], (ub2 *)0, (ub2 *)0, (ub4) OCI_DEFAULT);
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIDefineByPos 0 \n");
    }

  
    pointName[29] = '\0';
    //执行SQL语句
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    //查错
    if(retcode!=OCI_SUCCESS&&retcode!=OCI_NO_DATA)
    {
        int errNo = 0;
        retcode = OCIErrorGet ((dvoid*) ociCtx_p->ErrHP, (ub4) 1, (text *) NULL, &errNo,
                                (text*)pointName, (ub4)sizeof(pointName)-1, (ub4) OCI_HTYPE_ERROR);
        printf("error OCIStmtExecute No=%d Info=%s \n", errNo, pointName);
        return -1;
    }
     char comm[20];
     strcpy(comm,"commit");
      retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)comm,
                      (ub4)strlen((char *)comm),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   printf("******HTTP insert sucess****** \n");
}
 int insert_telnetdata(MY_OCI_CONTEXT_T* ociCtx_p,struct telnet_log q)
{   int p_bvi;
    char sqlcmd[256]="";
    sword retcode;
    OCIDefine *defcolp[3];
    OCIBind * p_bndp1[6];//六个参数
    sword indp[3];
    char pointId[100];
    char pointId1 [50];
    char pointName[30];
    char pointDesc[60];
    //生成sql语句
 
    strcpy(sqlcmd,"insert into telnet_log values(N_YMSG_CHATLOG_ID.nextval,:1,:2,:3,:4,:5,:6)");//刚开始语法错乱无''
    
    //准备SQL语句
    retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)sqlcmd,
                      (ub4)strlen((char *)sqlcmd),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   
    
     char saddr[40];
     strcpy(saddr,inet_ntoa(*((struct in_addr*) &(q.saddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[0],ociCtx_p->ErrHP, (ub4)1,&saddr,(sb4)(sizeof(saddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
    unsigned int src = q.src;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[1],ociCtx_p->ErrHP, (ub4)2,(dvoid *)&src,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char daddr[40];
  strcpy(daddr,inet_ntoa(*((struct in_addr*) &(q.daddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[2],ociCtx_p->ErrHP, (ub4)3,&daddr,(sb4)(sizeof(daddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     unsigned int dst = q.dst;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[3],ociCtx_p->ErrHP, (ub4)4,(dvoid *)&dst,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char ym_time[40];
        strcpy(ym_time,q.time);

retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[4],ociCtx_p->ErrHP, (ub4)5,&ym_time,(sb4)(sizeof(ym_time)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
   char  id[40] ;
    strcpy(id,q.key);
   retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[5],ociCtx_p->ErrHP, (ub4)6,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
   printf("bind inss  error\n");
  /*  //sql语句输入有几列，有几列就定义几次绑定字符和整形不宜样赋值
    //定义输入变量0OCIBindByPos(p_sql,&p_bndp2,p_err,2,&getName,(sb4)sizeof(getName),SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)
    retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1,ociCtx_p->ErrHP, (ub4)1,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
    
    if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
*/
     //sql语句有两列输出，有几列就定义几次
    //定义输出变量0(dvoid *)&pointId,zhengxingbind
   retcode=OCIDefineByPos(ociCtx_p->StmtHP, &defcolp[0], ociCtx_p->ErrHP, (ub4)1, &pointId, (sb4)(sizeof(pointId)), SQLT_STR , (dvoid *)&indp[0], (ub2 *)0, (ub2 *)0, (ub4) OCI_DEFAULT);
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIDefineByPos 0 \n");
    }

  
    pointName[29] = '\0';
    //执行SQL语句
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    //查错
    if(retcode!=OCI_SUCCESS&&retcode!=OCI_NO_DATA)
    {
        int errNo = 0;
        retcode = OCIErrorGet ((dvoid*) ociCtx_p->ErrHP, (ub4) 1, (text *) NULL, &errNo,
                                (text*)pointName, (ub4)sizeof(pointName)-1, (ub4) OCI_HTYPE_ERROR);
        printf("error OCIStmtExecute tel No=%d Info=%s \n", errNo, pointName);
        return -1;
    }
     char comm[20];
     strcpy(comm,"commit");
      retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)comm,
                      (ub4)strlen((char *)comm),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   printf("******TELNET insert sucess****** \n");
   
}

int oci_init(MY_OCI_CONTEXT_T* ociCtx_p)
{
    sword sr;
    //OCI环境初始化 在8i以后，可用OCIEnvCreate一个函数就可以初始化环境了，相当于OCIInitialize+ OCIEnvInit
    sr=OCIEnvInit( (OCIEnv **) &ociCtx_p->EnvHP, OCI_DEFAULT, (size_t) 0,
             (dvoid **) 0 );
    //创建OCI环境        
    sr=OCIEnvCreate(&ociCtx_p->EnvHP,OCI_DEFAULT,0,0,0,0,0,0);
    //判断是否成功
    if(sr!=OCI_SUCCESS)
    {
        return -1;
    }
    //申请错误句柄ErrHP
    sr= OCIHandleAlloc( (dvoid *) ociCtx_p->EnvHP, (dvoid **) &ociCtx_p->ErrHP, OCI_HTYPE_ERROR,
           (size_t) 0, (dvoid **) 0); /* server contexts */
    if(sr!=OCI_SUCCESS)
    {
        return -1;
    }
    //申请服务器句柄SrvHP
    sr=OCIHandleAlloc( (dvoid *)ociCtx_p->EnvHP, (dvoid **) &ociCtx_p->SrvHP, OCI_HTYPE_SERVER,
           (size_t) 0, (dvoid **) 0);
    if(sr!=OCI_SUCCESS)
    {
        return -1;
    }
    //申请服务器上下文句柄SvcHP
    sr=OCIHandleAlloc( (dvoid *)ociCtx_p->EnvHP, (dvoid **) &ociCtx_p->SvcHP, OCI_HTYPE_SVCCTX,
           (size_t) 0, (dvoid **) 0);
    if(sr!=OCI_SUCCESS)
    {
        return -1;
    }

    //多用户方式连接数据库服务器,设置链接的服务器名pConnectChar
    sr= OCIServerAttach(ociCtx_p->SrvHP,ociCtx_p->ErrHP, (text *)pConnectChar,
           strlen(pConnectChar), 0);
    if(sr!=OCI_SUCCESS)
    {
        printf("connect erro!!!!!!!!!!!!\n");
        return -1;
    }

    /* set attribute server context in the service context */
    //设置SvcHP句柄属性为：OCI_HTYPE_SVCCTX
    (void) OCIAttrSet( (dvoid *)ociCtx_p->SvcHP, OCI_HTYPE_SVCCTX, (dvoid *)ociCtx_p->SrvHP,
        (ub4) 0, OCI_ATTR_SERVER, (OCIError *) ociCtx_p->ErrHP);
    //申请对话句柄SessionHP  
    sr= OCIHandleAlloc((dvoid *)ociCtx_p->EnvHP, (dvoid **)&ociCtx_p->SessionHP,
        (ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0);
    if(sr!=OCI_SUCCESS)
    {
        return -1;
    }
    //设置用户名pUsernameChar
    (void) OCIAttrSet((dvoid *)ociCtx_p->SessionHP, (ub4) OCI_HTYPE_SESSION,
        (dvoid *)pUsernameChar, (ub4) strlen((char *)pUsernameChar),
        (ub4) OCI_ATTR_USERNAME, ociCtx_p->ErrHP);



    //设置SessionHP句柄用户登录密码pPasswordChar
    (void) OCIAttrSet((dvoid *)ociCtx_p->SessionHP, (ub4) OCI_HTYPE_SESSION,
        (dvoid *) pPasswordChar, (ub4) strlen((char *)pPasswordChar),
        (ub4) OCI_ATTR_PASSWORD, ociCtx_p->ErrHP);
    //建立一个会话连接
    sr=OCISessionBegin (ociCtx_p->SvcHP, ociCtx_p->ErrHP, ociCtx_p->SessionHP, OCI_CRED_RDBMS,
        (ub4) OCI_DEFAULT);
    if(sr!=OCI_SUCCESS)
    {
        return -1;
    }
    //设置SvcHP句柄属性为：OCI_HTYPE_SVCCTX
    (void) OCIAttrSet((dvoid *)ociCtx_p->SvcHP, (ub4) OCI_HTYPE_SVCCTX,
           (dvoid *)ociCtx_p->SessionHP, (ub4) 0,
           (ub4) OCI_ATTR_SESSION, ociCtx_p->ErrHP);

    //申请语句句柄StmtHP
    sr=OCIHandleAlloc( (dvoid *)ociCtx_p->EnvHP, (dvoid **) &ociCtx_p->StmtHP,
                   OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
    if(sr!=OCI_SUCCESS)
    {
        return -1;
    }

    printf("#############Connect db sccessfully!################\n");
  
    return 1;
}
  

 MATCHENTRY* select_data(MY_OCI_CONTEXT_T* ociCtx_p,char *sql)
{  
	MATCHENTRY *tp_entry =  MatchEntryInit(1);
	
	printf("#############selectdb start !################\n");
	int p_bvi;
	char sqlcmd[128]="";
	sword retcode;
	OCIDefine *defcolp[3];
	OCIBind * p_bndp1;
	sword indp[3];
	char pointId[100];
	char pointId1 [50];
	char pointName[30];
	char pointDesc[60];
	//靠sql靠
	strcpy(sqlcmd,sql);
	printf("hh\n");
	//strcpy(sqlcmd," select ym_key from ymsg_key");//靠靠靠靠''

	//靠SQL靠
	retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)sqlcmd,
				(ub4)strlen((char *)sqlcmd),
				(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
				);                   
	if(retcode!=OCI_SUCCESS)
	{
		printf("error OCIStmtPrepare \n");
	} 
	/*char  id[10] = "177";

	//sql靠靠靠靠靠靠靠靠靠靠靠靠靠靠
	//靠靠靠0OCIBindByPos(p_sql,&p_bndp2,p_err,2,&getName,(sb4)sizeof(getName),SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)
	retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1,ociCtx_p->ErrHP, (ub4)1,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);

	if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
	printf("bind inss  error\n");
	*/
	//sql靠靠靠靠靠靠靠靠
	//靠靠靠0(dvoid *)&pointId,zhengxingbind
	retcode=OCIDefineByPos(ociCtx_p->StmtHP, &defcolp[0], ociCtx_p->ErrHP, (ub4)1, &pointId, (sb4)(sizeof(pointId)), SQLT_STR , (dvoid *)&indp[0], (ub2 *)0, (ub2 *)0, (ub4) OCI_DEFAULT);
	if(retcode!=OCI_SUCCESS)
	{
		printf("error OCIDefineByPos 0 \n");
	}
	printf("exe ovwer\n");

	//靠靠靠靠靠靠
	//retcode=OCIDefineByPos(ociCtx_p->StmtHP, &defcolp[0], ociCtx_p->ErrHP, (ub4)2, (dvoid *)&pointId1,
	//            (sb4)sizeof(int), SQLT_INT , (dvoid *)&indp[1], (ub2 *)0,
	//            (ub2 *)0, (ub4) OCI_DEFAULT);
	// if(retcode!=OCI_SUCCESS)
	// {
	//     printf("error OCIDefineByPos 1 \n");
	// }  

	pointName[29] = '\0';
	//靠SQL靠
	retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
				(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
	//靠
	if(retcode!=OCI_SUCCESS&&retcode!=OCI_NO_DATA)
	{
		int errNo = 0;
		retcode = OCIErrorGet ((dvoid*) ociCtx_p->ErrHP, (ub4) 1, (text *) NULL, &errNo,
					(text*)pointName, (ub4)sizeof(pointName)-1, (ub4) OCI_HTYPE_ERROR);
		printf("error OCIStmtExecute No=%d Info=%s \n", errNo, pointName);
		return NULL;
	}
	printf("exe ovwer\n");
	void * ptr;
	int size_1;



	//靠靠靠
		Addkeyword(pointId,strlen(pointId),tp_entry, ptr);
	printf("have insert %s\n",pointId);
	while(
				OCIStmtFetch(ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4)1, (ub4)OCI_FETCH_NEXT, (ub4)OCI_DEFAULT)!= OCI_NO_DATA )
	{    
		Addkeyword(pointId,strlen(pointId),tp_entry, ptr);
		printf("have insert %s\n",pointId);
	}


	size_1 = Buildentry(0,tp_entry);
	return tp_entry;

	
}




int insert_data(MY_OCI_CONTEXT_T* ociCtx_p,struct ymsg_log q)
{  int p_bvi;
    char sqlcmd[256]="";
    sword retcode;
    OCIDefine *defcolp[3];
    OCIBind * p_bndp1[6];//六个参数
    sword indp[3];
    char pointId[100];
    char pointId1 [50];
    char pointName[30];
    char pointDesc[60];
    //生成sql语句
 
    strcpy(sqlcmd,q.sql);//刚开始语法错乱无''
    
    //准备SQL语句
    retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)sqlcmd,
                      (ub4)strlen((char *)sqlcmd),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   
    
     char saddr[40];
     strcpy(saddr,inet_ntoa(*((struct in_addr*) &(q.saddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[0],ociCtx_p->ErrHP, (ub4)1,&saddr,(sb4)(sizeof(saddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
    unsigned int src = q.src;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[1],ociCtx_p->ErrHP, (ub4)2,(dvoid *)&src,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char daddr[40];
  strcpy(daddr,inet_ntoa(*((struct in_addr*) &(q.daddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[2],ociCtx_p->ErrHP, (ub4)3,&daddr,(sb4)(sizeof(daddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     unsigned int dst = q.dst;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[3],ociCtx_p->ErrHP, (ub4)4,(dvoid *)&dst,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char ym_time[40];
        strcpy(ym_time,q.time);

retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[4],ociCtx_p->ErrHP, (ub4)5,&ym_time,(sb4)(sizeof(ym_time)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
   char  id[40] ;
    strcpy(id,q.key);
   retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[5],ociCtx_p->ErrHP, (ub4)6,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
   printf("bind inss  error\n");
  /*  //sql语句输入有几列，有几列就定义几次绑定字符和整形不宜样赋值
    //定义输入变量0OCIBindByPos(p_sql,&p_bndp2,p_err,2,&getName,(sb4)sizeof(getName),SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)
    retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1,ociCtx_p->ErrHP, (ub4)1,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
    
    if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
*/
     //sql语句有两列输出，有几列就定义几次
    //定义输出变量0(dvoid *)&pointId,zhengxingbind
   retcode=OCIDefineByPos(ociCtx_p->StmtHP, &defcolp[0], ociCtx_p->ErrHP, (ub4)1, &pointId, (sb4)(sizeof(pointId)), SQLT_STR , (dvoid *)&indp[0], (ub2 *)0, (ub2 *)0, (ub4) OCI_DEFAULT);
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIDefineByPos 0 \n");
    }

  
    pointName[29] = '\0';
    //执行SQL语句
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    //查错
    if(retcode!=OCI_SUCCESS&&retcode!=OCI_NO_DATA)
    {
        int errNo = 0;
        retcode = OCIErrorGet ((dvoid*) ociCtx_p->ErrHP, (ub4) 1, (text *) NULL, &errNo,
                                (text*)pointName, (ub4)sizeof(pointName)-1, (ub4) OCI_HTYPE_ERROR);
        printf("error OCIStmtExecute No=%d Info=%s \n", errNo, pointName);
        return -1;
    }
     char comm[20];
     strcpy(comm,"commit");
      retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)comm,
                      (ub4)strlen((char *)comm),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   printf("******YMSG insert sucess****** \n");
   
}
void oci_clean(MY_OCI_CONTEXT_T* ociCtx_p)
{
    printf("\n ########## clean up ############ \n");
    //结束于数据库服务器的对话
    if (OCISessionEnd(ociCtx_p->SvcHP, ociCtx_p->ErrHP,
              ociCtx_p->SessionHP, (ub4) 0))
        printf("FAILED: OCISessionEnd()\n");

    //断开与数据库服务器链接
    if (OCIServerDetach(ociCtx_p->SrvHP, ociCtx_p->ErrHP, (ub4) OCI_DEFAULT))
        printf("FAILED: OCIServerDetach()\n");

    printf("Detached from server.\n");
                          
    printf("Freeing handles ...\n");
    if (ociCtx_p->StmtHP)
        OCIHandleFree((dvoid *) ociCtx_p->StmtHP, (ub4) OCI_HTYPE_STMT);
    if (ociCtx_p->ErrHP)
        OCIHandleFree((dvoid *) ociCtx_p->ErrHP, (ub4) OCI_HTYPE_ERROR);
    if (ociCtx_p->SrvHP)
        OCIHandleFree((dvoid *) ociCtx_p->SrvHP, (ub4) OCI_HTYPE_SERVER);
    if (ociCtx_p->SvcHP)
        OCIHandleFree((dvoid *) ociCtx_p->SvcHP, (ub4) OCI_HTYPE_SVCCTX);
    if (ociCtx_p->SessionHP)
        OCIHandleFree((dvoid *) ociCtx_p->SessionHP, (ub4) OCI_HTYPE_SESSION);
    if (ociCtx_p->EnvHP)
        OCIHandleFree((dvoid *) ociCtx_p->EnvHP, (ub4) OCI_HTYPE_ENV);
}
int insert_ipdata(MY_OCI_CONTEXT_T* ociCtx_p,struct telnet_log q)
{   int p_bvi;
    char sqlcmd[256]="";
    sword retcode;
    OCIDefine *defcolp[3];
    OCIBind * p_bndp1[6];//六个参数
    sword indp[3];
    char pointId[100];
    char pointId1 [50];
    char pointName[30];
    char pointDesc[60];
    //生成sql语句
 
    strcpy(sqlcmd,"insert into ip_log values(N_YMSG_CHATLOG_ID.nextval,:1,:2,:3,:4,:5,:6)");//刚开始语法错乱无''
    
    //准备SQL语句
    retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)sqlcmd,
                      (ub4)strlen((char *)sqlcmd),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   
    
     char saddr[40];
     strcpy(saddr,inet_ntoa(*((struct in_addr*) &(q.saddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[0],ociCtx_p->ErrHP, (ub4)1,&saddr,(sb4)(sizeof(saddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
    unsigned int src = q.src;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[1],ociCtx_p->ErrHP, (ub4)2,(dvoid *)&src,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char daddr[40];
  strcpy(daddr,inet_ntoa(*((struct in_addr*) &(q.daddr))));
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[2],ociCtx_p->ErrHP, (ub4)3,&daddr,(sb4)(sizeof(daddr)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     unsigned int dst = q.dst;
retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[3],ociCtx_p->ErrHP, (ub4)4,(dvoid *)&dst,(sb4)(sizeof(int)),SQLT_INT,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
     char ym_time[40];
        strcpy(ym_time,q.time);

retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[4],ociCtx_p->ErrHP, (ub4)5,&ym_time,(sb4)(sizeof(ym_time)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
   char  id[40] ;
    strcpy(id,q.key);
   retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1[5],ociCtx_p->ErrHP, (ub4)6,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
   if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
   printf("bind inss  error\n");
  /*  //sql语句输入有几列，有几列就定义几次绑定字符和整形不宜样赋值
    //定义输入变量0OCIBindByPos(p_sql,&p_bndp2,p_err,2,&getName,(sb4)sizeof(getName),SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)
    retcode = OCIBindByPos(ociCtx_p->StmtHP,&p_bndp1,ociCtx_p->ErrHP, (ub4)1,&id,(sb4)(sizeof(id)),SQLT_STR,NULL,NULL,NULL,0,NULL,OCI_DEFAULT);
    
    if(retcode != OCI_SUCCESS && retcode != OCI_SUCCESS_WITH_INFO)
 printf("bind inss  error\n");
*/
     //sql语句有两列输出，有几列就定义几次
    //定义输出变量0(dvoid *)&pointId,zhengxingbind
   retcode=OCIDefineByPos(ociCtx_p->StmtHP, &defcolp[0], ociCtx_p->ErrHP, (ub4)1, &pointId, (sb4)(sizeof(pointId)), SQLT_STR , (dvoid *)&indp[0], (ub2 *)0, (ub2 *)0, (ub4) OCI_DEFAULT);
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIDefineByPos 0 \n");
    }

  
    pointName[29] = '\0';
    //执行SQL语句
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    //查错
    if(retcode!=OCI_SUCCESS&&retcode!=OCI_NO_DATA)
    {
        int errNo = 0;
        retcode = OCIErrorGet ((dvoid*) ociCtx_p->ErrHP, (ub4) 1, (text *) NULL, &errNo,
                                (text*)pointName, (ub4)sizeof(pointName)-1, (ub4) OCI_HTYPE_ERROR);
        printf("error OCIStmtExecute No=%d Info=%s \n", errNo, pointName);
        return -1;
    }
     char comm[20];
     strcpy(comm,"commit");
      retcode = OCIStmtPrepare (ociCtx_p->StmtHP, ociCtx_p->ErrHP, (unsigned char *)comm,
                      (ub4)strlen((char *)comm),
                      (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT
                      );                   
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
    retcode=OCIStmtExecute(ociCtx_p->SvcHP, ociCtx_p->StmtHP, ociCtx_p->ErrHP, (ub4) 1, (ub4) 0,
                  (CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT );
    if(retcode!=OCI_SUCCESS)
    {
        printf("error OCIStmtPrepare \n");
    } 
   printf("******IP insert sucess****** \n");
   
}

