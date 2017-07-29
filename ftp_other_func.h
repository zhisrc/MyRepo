
/********************************************************************
** Copyright(c) 2015 ,哈尔滨工程大学信息安全研究中心
** All rights reserved
**
** 文件名：ftp_other_func.h
** 创建人：赵旭
** 描  述：
** 注  意：c语言访问数据库
** 
** 当前版本：1.0
** 作    者：赵恒
** 完成日期：2015-12-017
** 取代版本：1.x
** 作    者：输入原作者（或修改者）名字
** 完成日期：*********

** ********************************************************************/
#ifndef __FTP_OTHER_FUNC_H
#define __FTP_OTHER_FUNC_H
#include <nids.h>
#include "write_orcl_circular_queue.h"
int getPasvServerDataPort(char *content);
void determineFileSuffixToDo(struct tuple4 tul4,char *content);
void ftp_protocol_manage(struct tcp_stream *ftp_connection);
void write_log_to_orcl(struct orclQnode q);
void ftp_protocol_callback(struct tcp_stream* ftp_connection,void **arg); 
void *fetch_ftp_queue(void* arg);
void *fetch_orcl_queue(void* arg);
#endif
