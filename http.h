/********************************************************************
** Copyright(c) 2015 ,哈尔滨工程大学信息安全研究中心
** All rights reserved
**
** 文件名：http.h
** 创建人：gan
** 描  述：http
** 注  意：
** 
** 当前版本：1.0
** 作    者：
** 完成日期：2015-12-12

** ********************************************************************/


#ifndef  HTTP_H 
#define  HTTP_H 
#define MAX_SIZE 65536
#include <zlib.h>
#include "nids.h"

typedef struct httphashNode
{
  struct tuple4 tup4;//四元组
  z_stream strm;
  int cont_encoding;
  int tran_encoding;
  char *cont_type; 
  int next_cklen;
  //tcp* tcp_list;   //暂定不挂数据，所以不需要这个tcp链
  struct httphashNode* next;
}httplist;

httplist * httphashtable[MAX_SIZE]; //哈希表

void hashtable_print();

void httphash_init();
unsigned int  hash_map_1( unsigned int saddr, unsigned short sport, unsigned int daddr, unsigned short dport);//哈希算法
int  tuplecmp_1(struct tuple4 a, struct tuple4 b);
int httphash_delete(struct tuple4 ip_and_port);
int hash_realase();

int get_info_from_hash(struct tuple4 ip_and_port, char *contype, int *cont_encoding, int *tran_encoding, z_stream *strm, int *next_cklen);
int insert_chunklen(struct tuple4 ip_and_port, int next_cklen);
void httphashtable_insert(struct tuple4 ip_and_port, char* contype, int *cont_encoding, int* tran_encoding, z_stream *strm, int next_cklen);
char * client_data_analysis(char content[], int number, char *contype, int *cont_encoding, int *tran_encoding,int *datalen);
int gzip_uncompress(z_stream *strm, Byte *bufin, int lenin, Byte *bufout, int lenout);
int chunk_gzip(char *data, int datalen, char ungzip_data[], z_stream *strm, struct tcp_stream *tcp_http_connection, int *is_block);
int get_url(char content[], int number, char url[]);
void http_analysis(struct tcp_stream *tcp_http_connection);


void *http_consumer(void *arg);
void http_producer(struct tcp_stream *tcp_connection, void **arg);
void http_protocol_callback(struct tcp_stream *tcp_http_connection, void **param);
#endif

