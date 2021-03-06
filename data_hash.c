/********************************************************************
** Copyright(c) 2015 ,哈尔滨工程大学信息安全研究中心
** All rights reserved
**
** 文件名：tcp_hash
** 创建人：赵恒
** 描  述：哈希表
** 注  意：用于各模块tcp重组
** 
** 当前版本：1.0
** 作    者：赵恒
** 完成日期：2015-12-02
** 取代版本：1.x
** 作    者：输入原作者（或修改者）名字
** 完成日期：*********
** ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nids.h>
#include <fcntl.h>
#include "data_hash.h"
//#include "keyword_match.h"
void data_hash_init()
{
  int i = 0;
  for(i = 0;i< MAX_SIZE; i++)
  {
   datahashtable[i] = NULL;  
  }
}
/*********************************************************************************************************
 *  Function Name: data_hash_map
 *  Purpose      :                 
 *  Parameter    : 
 *********************************************************************************************************/
unsigned int  data_hash_map( unsigned int serverip, unsigned short serverport, unsigned int clientip)
{
  unsigned int  key;
  key = (serverip ^ serverport) ^ (clientip);
  key ^= key >> 16;
  key = key >> 16;
  key = key & (2 ^ 16 - 1);
  return key;
}
/*********************************************************************************************************
 *  Function Name: data_tuple_cmp
 *  Purpose      :              
 *  Parameter    : 
 *********************************************************************************************************/
int data_tuple_cmp(unsigned int sip,short int sport,unsigned int cip, struct tuple3 b)
{
  if(sip== b.serverip && sport == b.serverport && cip ==b.clientip)
    return 1;
  else
   return 0;
}
/*********************************************************************************************************
 *  Function Name: data_hash_insert_one
 *  Purpose      :                  
 *  Parameter    : 
 *********************************************************************************************************/
void data_hash_insert_one(struct tuple4 ip_and_port, char *filename,int port)
//void hashtable_insert(int key ,char *s ,char *data)
{
	//printf("have come in the function data_hash_insert_one\n");
  unsigned int pos;
  //unsigned int saddr = ip_and_port.saddr;
  //unsigned short sport = ip_and_port.source;
  //unsigned int daddr = ip_and_port.daddr;
  //unsigned short dport = ip_and_port.dest;
  //pos = hash_map(saddr,sport,daddr,dport);
  pos = data_hash_map(ip_and_port.daddr,port,ip_and_port.saddr);
  datalist *phead = datahashtable[pos];
  datalist * front = NULL;
  while(phead)
  {
	  if(data_tuple_cmp(ip_and_port.daddr,port,ip_and_port.saddr,phead->tul3))
	  {
		//  printf("change the filename in data_hash:%s\n",filename);
	      strcpy(phead->filename,filename);
		  return;
	  }
    front = phead;
    phead = phead->next;
  }
  datalist * pnewnode = (datalist *)malloc(sizeof(datalist));
  pnewnode ->tul3.serverip = ip_and_port.daddr;
  pnewnode->tul3.serverport = port;
  pnewnode->tul3.clientip = ip_and_port.saddr;
  strcpy(pnewnode->filename,filename);
  pnewnode->source = 0;
  pnewnode ->tcp_list = NULL;
  if(front != NULL)
  {
      pnewnode ->next = front ->next;
      front->next = pnewnode;
    //  printf("data_hash head is %s\n",datahashtable[pos]->filename);
  }
  else 
  {
      pnewnode->next = NULL;
      datahashtable[pos] = pnewnode;
     // printf("ZZZXXXXX\n");
    //  printf("data_hash head is %s\n",datahashtable[pos]->filename);
  }
}   
/*********************************************************************************************************
 *  Function Name: data_hash_insert_two
 *  Purpose      :                   
 *  Parameter    : 
 *********************************************************************************************************/
void data_hash_insert_two(struct tuple4 ip_and_port,char *data,int length,int flag)
//void hashtable_insert(int key ,char *s ,char *data)
{
	unsigned int pos;
	datalist *phead ;
	//unsigned int saddr = ip_and_port.saddr;
	//unsigned short sport = ip_and_port.source;
	//unsigned int daddr = ip_and_port.daddr;
	//unsigned short dport = ip_and_port.dest;
	//pos = hash_map(saddr,sport,daddr,dport);
	if(flag == 1)
	{
		//printf("use the theod 1\n");
		pos = data_hash_map(ip_and_port.saddr,ip_and_port.source,ip_and_port.daddr);
		////printf("pos in data_hash_isert_two:%d\n",pos);
		phead = datahashtable[pos];
		while(phead)
		{
			//printf("data_tuple_cmp:%d\n",data_tuple_cmp(ip_and_port.saddr,ip_and_port.source,ip_and_port.daddr,phead->tul3));
			if(data_tuple_cmp(ip_and_port.saddr,ip_and_port.source,ip_and_port.daddr,phead->tul3))
			{
				//printf("1 have exsists %s !!!\n",phead->filename);
				// tcp_insert(phead->tcp_list,data);
				phead->source = ip_and_port.source;
				datatcp *p ;//
				datatcp *q;
				p = (datatcp *) malloc(sizeof(datatcp));
				p->data_stream = (char *)malloc(sizeof(char)*length);
				memcpy(p->data_stream,data,length);
			//	p->data_stream = data;
			    p->eachdatalength = length;
				if(phead->tcp_list == NULL)
				{
					p->next = phead->tcp_list;
					phead->tcp_list = p;
				//	printf("data hash has insert data\n");
					return ;
				}
				else
				{
					q = phead->tcp_list;
					while(q->next)
					{
						q = q->next;
					}
					q->next = p;
					p->next = NULL;
					//printf("data hash has insert data\n");
					return;
				}
			}
			phead = phead->next;
		}
	}
	if(flag == 2)
	{
		//printf("use the theod 2\n");
		pos = data_hash_map(ip_and_port.daddr,ip_and_port.dest,ip_and_port.saddr);
		//printf("pos in data_hash_isert_two:%d\n",pos);
		phead = datahashtable[pos];
		while(phead)
		{
			if(data_tuple_cmp(ip_and_port.daddr,ip_and_port.dest,ip_and_port.saddr,phead->tul3))
			{
			//	printf("data_hash_insert_two have exsists %s !!!\n");
				// tcp_insert(phead->tcp_list,data);
				phead->source = ip_and_port.source;
				datatcp *p;//
				datatcp *q;
				p = (datatcp *) malloc(sizeof(datatcp));
				p = (datatcp *) malloc(sizeof(datatcp));
				p->data_stream = (char *)malloc(sizeof(char)*length);
				memcpy(p->data_stream,data,length);
				//p->data_stream = data;
			    p->eachdatalength = length;
				if(phead->tcp_list == NULL)
				{
					p->next = phead->tcp_list;
					phead->tcp_list = p;
					//printf("data hash has insert data\n");
					return ;
				}
				else
				{
					q = phead->tcp_list;
					while(q->next)
					{
						q = q->next;
					}
					q->next = p;
					p->next = NULL;
					//printf("data hash has insert data\n");
				}
			}
			phead = phead->next;
		}
	}
}   
/*********************************************************************************************************
 *  Function Name: fetch_data_to_manage
 *  Purpose      :                 
 *  Parameter    : 
 *********************************************************************************************************/
/*void fetch_data_to_manage(struct tuple4 ip_and_port,int port,MATCHENTRY* entry)
//void fetch_data_to_manage(struct tuple4 ip_and_port,int port)
//void hashtable_insert(int key ,char *s ,char *data)
{
	unsigned int pos;
	pos = data_hash_map(ip_and_port.daddr,port,ip_and_port.saddr);
	datalist *phead = datahashtable[pos];
	datalist * front = NULL;
    char findword[50];
	char *findword_1;
	while(phead)
	{
		if(data_tuple_cmp(ip_and_port.daddr,port,ip_and_port.saddr,phead->tul3))
		{
			printf("have exsists%s !!!'\n",phead->filename);
			if(strcmp(strstr(phead->filename,"."),".txt") == 0)
			{
			   datatcp *p;
			   p = phead->tcp_list;
			   while(p)
			   {
			      findword_1 = Match(p->data_stream,0,entry,findword);
				  prntf("find keyword:%s\n",findword_1);
				  p= p->next;
			   }
			   return;
			}
			else
			{
				FILE *fd;
				fd = fopen(phead->filename,"wb+");
				if(fd == NULL)
				{
					printf("fopen fail to open\n");
					exit(1);
				}
				// tcp_insert(phead->tcp_list,data);
				datatcp *p ;
				p = phead->tcp_list;
				// printf("data is \n");
				while(p)
				{
					// printf("%s\n",p->data_stream);
					printf("the length of this data_stream is:%d\n",p->eachdatalength);
					//	printf("the return value of write = %d\n",write(fd,p->data_stream,p->eachdatalength));
					if(fwrite(p->data_stream,sizeof(char),p->eachdatalength,fd) != p->eachdatalength)
					{
						printf("fail to write\n");
						exit(1);
					}
					fseek(fd,0,SEEK_END);
					p = p->next;
				}
				// printf("data end\n");
				fclose(fd);
				return ;
			}
		}
		front = phead;
		phead = phead->next;
	}
}*/  
/********************************************************************************************************
 *  Function Name: data_hash_print
 *  Purpose      :               
 *  Parameter    : 
 *  *********************************************************************************************************/
void data_hash_print()
{
  printf("========================content of data hash table=============\n");
  int i;
  for(i = 0 ; i< MAX_SIZE; i++)
  {
    if(datahashtable[i])
    {
      datalist *phead = datahashtable[i];
      printf("%d =>",i);
     // printf(" data is");
      while(phead)
      {
      //  printf(" data_hash_print filename is %s\n",phead->filename);
        datatcp *p= phead->tcp_list;
        while(p)
        {
       //  printf("data_hash_print data \n");
		// printf("the length of this data_stream is:%d\n",p->eachdatalength);
         p =p->next;
        }
        printf("\n");
        phead = phead->next;
      }
    }
  }
}
/********************************************************************************************************
 *  Function Name: data_find_node
 *  Purpose      :               
 *  Parameter    :
 *  *********************************************************************************************************/

/*int data_find_node(struct tuple4 ip_and_port)
{
  datalist *phead = datahashtable[data_hash_map(ip_and_port)];
  while(phead)
  {
    //if(key == phead->key)
    if(data_tuple_cmp(ip_and_port,phead->tul3))
    {
      printf("Find key!!!\n");
      return 1;
    }
    phead = phead->next;
  }
  printf("NO find !!!\n");
  return 0;
}*/
/********************************************************************************************************
 *  Function Name: data_hash_delete
 *  Purpose      :              
 *  Parameter    : 
 *  *********************************************************************************************************/
int data_hash_delete(struct tuple4 ip_and_port,int port)
{
    unsigned int pos;
	pos = data_hash_map(ip_and_port.daddr,port,ip_and_port.saddr);
	datalist *phead = datahashtable[pos];
	datalist *front;
	//printf("just delete %d =>",pos);
	while(phead)
	{ 
		if(data_tuple_cmp(ip_and_port.daddr,port,ip_and_port.saddr,phead->tul3))
		{
			if(phead == datahashtable[pos])//
			{
				//printf("remov head one from pos =%d \n",pos);
				datatcp *p = phead->tcp_list;
				phead->tcp_list = NULL;
                datahashtable[pos] = phead->next;
				free(phead);
				while(p)
				{
					//printf("delete \n");
					free(p);
					p = p->next; 
				}
				return;
			}
			//printf("remov head two from pos =%d \n",pos);
			datatcp *p = phead->tcp_list;
			phead->tcp_list = NULL;
			front->next = phead->next;
			free(phead);
			while(p)
			{
				//printf("delete \n");
				free(p);
				p = p->next; 
			}
			return;
		} 
		front = phead;
        phead = phead->next;
	}
	return 0;
}   
/********************************************************************************************************
 *  Function Name: data_hash_realase
 *  Purpose      : 
 *  Parameter    :
 *  *********************************************************************************************************/
/*int hash_realase()
{
  int i;
  for(i = 0;i<MAX_SIZE;i++)
  {
    if(datahashtable[i])
    {
      datalist * phead = datahashtable[i];
      datatcp *p =phead ->tcp_list;
	 // datatcp *q = p->next;
      free(phead);
      while(p) 
      {
        free(p);
        p = p->next;
      }    
    }
  }
  return 0;
}*/
