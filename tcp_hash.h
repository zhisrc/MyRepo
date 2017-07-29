/********************************************************************
** Copyright(c) 2015 ,哈尔滨工程大学信息安全研究中心
** All rights reserved
**
** 文件名：tcp_hash
** 创建人：赵恒
** 描  述：哈希表
** 注  意：用于tcp重组
** 
** 当前版本：1.0
** 作    者：赵恒
** 完成日期：2015-12-02
** 取代版本：1.x
** 作    者：输入原作者（或修改者）名字
** 完成日期：*********

** ********************************************************************/


#ifndef  TCP_HASH_H 
#define  TCP_HASH_H 
#define MAX_SIZE 65536
typedef struct node
{ 
  int data_length;
  char * data_stream; //half_stream->data
  struct node * next;
}tcp;
typedef struct hashNode
{
  struct tuple4 * tup4;//四元组
  //int key;
  char *name;//该文件传输的文件名
  long int length;//文件长度
  long int count ;
 // char *key; //该文件传输对应的key 265的value，可用于取消连接删除链。
  tcp* tcp_list;//每个四元组对应的data
  tcp *rear;
  struct hashNode* next;
}list;
list * hashtable[MAX_SIZE]; //哈希表
void hashtable_print( );
void hash_init();
unsigned int hash_map(struct tuple4 *ip_and_port);
int  tuplecmp(struct tuple4 *a, struct tuple4 *b);

//void  tcp_insert(tcp *head, char *s);
void hashtable_insert(struct tuple4 *ip_and_port, char *s,char *data,long int length,int data_length);
//void hashtable_insert(int key ,char *s ,char *data);
void hashtable_print();
int Find_node(struct tuple4 *ip_and_port);
int  hashdelete(struct tuple4 *ip_and_port);
int hash_realase();
#endif

