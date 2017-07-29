/********************************************************************
** Copyright(c) 2015 ,哈尔滨工程大学信息安全研究中心
** All rights reserved
**
** 文件名：control_hash
** 创建人：赵旭
** 描  述：哈希表
** 注  意：用于tcp重组
** 
** 当前版本：1.0
** 作    者：赵旭
** 完成日期：2015-12-04
** 取代版本：1.x
** 作    者：输入原作者（或修改者）名字
** 完成日期：*********

** ********************************************************************/

#ifndef  CONTROL_HASH_H 
#define  CONTROL_HASH_H 
#define MAX_SIZE 65536
#include <nids.h>

typedef struct controlhashNode
{
  struct tuple4  tup4;//四元组
  int port;//存储服务器端的数据端口
  struct controlhashNode* next;
}controllist;
controllist * controlhashtable[MAX_SIZE]; //哈希表
void control_hash_init();
/*unsigned int  hash_map( unsigned int saddr, unsigned short sport, unsigned int daddr, unsigned short dport);//哈希算法
unsigned int hash_map(int key);
/*int  tuplecmp(struct tuple4 a, struct tuple4 b);
*/
//void hashtable_insert(struct tuple4 ip_and_port, char *s,char *data)
//void control_hash_insert(int key ,char *s ,int port);
void control_hash_insert(struct tuple4 ip_and_port,int port);
void control_hash_print();
int control_find_node(struct tuple4 ip_and_port);
int control_hash_delete(struct tuple4 ip_and_port);
int control_hash_release();
#endif
