/********************************************************************
** Copyright(c) 2015 ,¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
** All rights reserved
**
** ¿¿¿¿control_hash
** ¿¿¿¿¿¿
** ¿  ¿¿¿¿¿
** ¿  ¿¿¿¿tcp¿¿
** 
** ¿¿¿¿¿1.0
** ¿    ¿¿¿¿
** ¿¿¿¿¿2015-12-04
** ¿¿¿¿¿1.x
** ¿    ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
** ¿¿¿¿¿*********

** ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nids.h>
#include "control_hash.h"

void control_hash_init()
{
  int i = 0;
  for(i = 0;i< MAX_SIZE; i++)
  {
    controlhashtable[i] = NULL;  
  }
}
/*********************************************************************************************************
 *  Function Name: control_hash_map
 *  Purpose      :                  
 *  Parameter    : 
 *********************************************************************************************************/
unsigned int  control_hash_map(struct tuple4 ip_and_port)
{
 unsigned int  key;
  key = (ip_and_port.saddr ^ ip_and_port.source) ^ (ip_and_port.daddr ^ ip_and_port.dest);
  key ^= key >> 16;
  key = key >> 16;
  key = key & (2 ^ 16 - 1);
  return key;
}
/*********************************************************************************************************
 *  Function Name: control_tuple_cmp
 *  Purpose      :                
 *  Parameter    : 
 *********************************************************************************************************/
int  control_tuple_cmp(struct tuple4 a, struct tuple4 b)
{
		if(a.saddr == b.saddr && a.source == b.source && a.daddr ==b.daddr && a.dest == b.dest)
		  return 1;
    	if(a.saddr == b.daddr && a.source == b.dest&& a.daddr ==b.saddr && a.dest == b.source)
		  return 1;
	return 0;
}

/*********************************************************************************************************
 *  Function Name: control_hash_insert
 *  Purpose      :                
 *  Parameter    : 
 *********************************************************************************************************/
void control_hash_insert(struct tuple4 ip_and_port,int port)
{
  unsigned int pos;
  //unsigned int saddr = ip_and_port.saddr;
  //unsigned short sport = ip_and_port.source;
  //unsigned int daddr = ip_and_port.daddr;
  //unsigned short dport = ip_and_port.dest;
  //pos = hash_map(saddr,sport,daddr,dport);
  pos = control_hash_map(ip_and_port);
  controllist *phead = controlhashtable[pos];
  controllist * front = NULL;
  while(phead)
  {
    if(control_tuple_cmp(ip_and_port,phead->tup4))
    //if(key == phead->key)
	{
		printf("have exsists%d !!!'\n",phead->port);
		phead->port = port;
		printf("new port%d\n",phead->port);
		return ;
	}
	front = phead;
	phead = phead->next;
  }
  controllist * pnewnode = (controllist *)malloc(sizeof(controllist));
  pnewnode -> tup4= ip_and_port;//
  pnewnode -> port = port;
  if(front != NULL)
  {
	  pnewnode ->next = front ->next;
	  front->next = pnewnode;
	 // controllist  *pHead = pnewnode;
	  printf("tail hashtable is %d\n",controlhashtable[pos]->port);
  }
  else 
  {
	  pnewnode->next = NULL;
	  controlhashtable[pos] = pnewnode;
	  printf("head hashtable is %d\n",controlhashtable[pos]->port);
  }
}   
/********************************************************************************************************
 *  Function Name: control_hash_print
 *  Purpose      :               
 *  Parameter    : 
 *  *********************************************************************************************************/
void control_hash_print()
{
  printf("========================content of control hash table=============\n");
  unsigned int i;
  for(i = 0 ; i< MAX_SIZE; i++)
  {
    if(controlhashtable[i])
    {
      controllist *phead = controlhashtable[i];
      printf("%d =>",i);
      while(phead)
      {
        printf("port is %d\n",phead->port);
        printf("\n");
        phead = phead ->next;
      }
    }
  }
}
/********************************************************************************************************
 *  Function Name: control_find_node
 *  Purpose      :              
 *  Parameter    : 
 *  *********************************************************************************************************/
int control_find_node(struct tuple4 ip_and_port)
{
  controllist *phead = controlhashtable[control_hash_map(ip_and_port)];
  while(phead)
  {
    // if(key == phead->key)
    //if(control_tuple_cmp(ip_and_port,*(phead->tup4),flag))
    if(control_tuple_cmp(ip_and_port,phead->tup4))
    {
      printf("Find key!!!\n");
      return phead->port;
    }
    phead = phead->next;
  }
  printf("NO find !!!\n");
  return -1;
}
/********************************************************************************************************
 *  Function Name: control_hash_delete
 *  Purpose      : 
 *  Parameter    :
 *  *********************************************************************************************************/
int  control_hash_delete(struct tuple4 ip_and_port)
{
	if(control_find_node(ip_and_port) != -1)
	{
		controllist *phead = controlhashtable[control_hash_map(ip_and_port)];
		controllist *front= NULL;
		while(phead)
		{
			if(control_tuple_cmp(ip_and_port,phead->tup4))
			{
		        if(phead == controlhashtable[control_hash_map(ip_and_port)])
				{
				    controlhashtable[control_hash_map(ip_and_port)] = phead->next;
					printf("head delete port is %d\n",phead->port);
					free(phead);
					return;
				}
                front->next = phead->next;
				printf("body delete port is %d\n",phead->port);
				free(phead);
		    } 
			front = phead;
			phead = phead->next;
		}
	} 
	else
	{
		printf("NO find node ,fail delete\n");
		return 0;
	}
}   
/********************************************************************************************************
 *  Function Name: control_hash_realase
 *  Purpose      : 
 *  Parameter
 *  *********************************************************************************************************/
int control_hash_release()
{
	int i;
	for(i = 0;i<MAX_SIZE;i++)
	{
		if(controlhashtable[i])
		{
			printf("now is releaing hash\n");
			controllist * phead = controlhashtable[i];
			free(phead);
		}
	}
	return 0;
}
