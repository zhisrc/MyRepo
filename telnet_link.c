/*************************************************************************
	> File Name: telnet_link.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年12月03日 星期四 18时32分13秒
 ************************************************************************/

#include "apue.h"
#include "telnet_link.h"

packet_telnet *telnet_head;

packet_telnet *create_telnet_head()
{
		telnet_head = (packet_telnet *)malloc(sizeof (packet_telnet) );
		if(telnet_head==NULL)							
		{
			printf("申请头结点失败!\n");
			return NULL;
		}
			telnet_head->next=NULL;
			return telnet_head; //全局变量
}//创建一个只有头结点的空链表

void telnet_insert ( packet_telnet *telnet_head,packet_telnet *telnet_s )  
{
		if(telnet_head->next == NULL)
				{
					telnet_s->p_count = telnet_head->p_count + 1;
					telnet_head->next=telnet_s;    //连接结点
					telnet_s->next=NULL;   //p->next就等于NULL
					printf("s_port:%d\n",telnet_s->source);
					printf("d_port:%d\n",telnet_s->dest);
					printf("data:%s\n",telnet_s->data);
					printf("p_count:%d\n",telnet_s->p_count);										}
				else	
				{
					packet_telnet *telnet_p;
					telnet_p = telnet_head;
					while(telnet_p->next != NULL)
					{
						telnet_p=telnet_p->next;
					}
					telnet_s->p_count = telnet_p->p_count + 1;
					telnet_p->next=telnet_s;
					telnet_s->next=NULL;
					printf("else中插入\n");
					printf("s_port:%d\n",telnet_s->source);
					printf("d_port:%d\n",telnet_s->dest);
					printf("data:%s\n",telnet_s->data);
					printf("p_count:%d\n",telnet_s->p_count);
				}	
}//将s指向的结点插入链表

packet_telnet *get_packet ( packet_telnet *telnet_head )
{
	packet_telnet *telnet_p;
	if( telnet_head->next != NULL )
	{		
		telnet_p=telnet_head->next;
		telnet_head->next = telnet_p->next;
											}
		printf("get_packet:%s\n",telnet_p->data);
		return telnet_p;
		}//顺序取出链表头指针后的第一个节点
				
void telnet_free_list ( packet_telnet *telnet_head )
{
	packet_telnet *telnet_p;
	telnet_p=telnet_head;
	printf("释放链表:\n");
	while(telnet_p!=NULL)
	{
		telnet_head=telnet_head->next;
		free(telnet_p);
		telnet_p=telnet_head;
	}
	printf("释放链表成功!\n");
}//释放链表

