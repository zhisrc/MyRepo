﻿/********************************************************************
** Copyright(c) 2015 ,哈尔滨工程大学信息安全研究中心
** All rights reserved
**
** 文件名：data_hash
** 创建人：赵旭
** 描  述：哈希表
** 注  意：用于tcp重组
** 
** 当前版本：1.0
** 作    者：赵旭
** 完成日期：2015-12-08
** 取代版本：1.x
** 作    者：输入原作者（或修改者）名字
** 完成日期：*********

** ********************************************************************/
#ifndef  DATA_HASH_H 
#define  DATA_HASH_H 
#define MAX_SIZE 65536
#include <nids.h>
//#include "keyword_match.h"
typedef struct datanode
{
	char * data_stream; //half_stream->data
	int eachdatalength;//每一个data的长度
	struct datanode * next;
}datatcp;
typedef struct tuple3 
{
	unsigned int serverip;//
	short int serverport;
	unsigned int clientip;
}datatul3;
typedef struct datahashNode
{
	datatul3 tul3;
	char filename[100];//该文件传输的文件名
	unsigned int source;
	datatcp* tcp_list;//每个四元组对应的data
	struct datahashNode* next;
}datalist;
datalist * datahashtable[MAX_SIZE]; //哈希表
void data_hash_init();
//unsigned int data_hash_map();
//void  data_tcp_insert(tcp *head, char *s);
void data_hash_insert_one(struct tuple4 ip_and_port,char *filename,int port);
void data_hash_insert_two(struct tuple4 ip_and_port,char *data,int length,int flag);
void data_hash_print();
//int data_find_node(struct tuple4 ip_and_port);
//void fetch_data_to_manage(struct tuple4 ip_and_port,int port,MATCHENTRY *entry);
//void fetch_data_to_manage(struct tuple4 ip_and_port,int port);
int data_hash_delete(struct tuple4 ip_and_port,int port);
//int data_hash_realase();
#endif

