/*************************************************************************
	> File Name: telnet_link.h
	> Author: 张莹
	> Mail: ma6174@163.com 
	> Created Time: 2015年12月03日 星期四 18时28分30秒
 ************************************************************************/

#ifndef  TELNET_LINK_H 
#define  TELNET_LINK_H 
#include <nids.h>
#include "apue.h"

typedef struct 
{
		u_short source; //源端口
		u_short dest; //目的端口
		u_int saddr; //源IP
		u_int daddr; //目的IP
		int count_new; //新接受到的数据字节数
		int p_count; //是链表中第几个节点
		char data[1500]; //应用层的数据   
		struct packet_telnet *next;
}packet_telnet;

packet_telnet *create_telnet_head();
void telnet_insert ( packet_telnet *telnet_head,packet_telnet *telnet_s );

packet_telnet *get_packet ( packet_telnet *telnet_head );
void telnet_free_list ( packet_telnet *telnet_head );

#endif
