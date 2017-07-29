/********************************************************************
** Copyright(c) 2015 ,¹þ¶û±õ¹€³ÌŽóÑ§ÐÅÏ¢°²È«ÑÐŸ¿ÖÐÐÄ
** All rights reserved
**
** ÎÄŒþÃû£ºtcp_hash
** ŽŽœšÈË£ºÕÔºã
** Ãè  Êö£º¹þÏ£±í
** ×¢  Òâ£ºÓÃÓÚž÷Ä£¿étcpÖØ×é
** 
** µ±Ç°°æ±Ÿ£º1.0
** ×÷    Õß£ºÕÔºã
** Íê³ÉÈÕÆÚ£º2015-12-02
** È¡Žú°æ±Ÿ£º1.x
** ×÷    Õß£ºÊäÈëÔ­×÷Õß£š»òÐÞžÄÕß£©Ãû×Ö
** Íê³ÉÈÕÆÚ£º*********
** ********************************************************************/
#include <nids.h>
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<libnet.h>
#include<pcap.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include "tcp_hash.h"
#include"ymsg_log.h"
#include "conn.h"
#include <time.h>
extern pthread_mutex_t ymlog_mutex; // 互斥信号锁
extern pthread_cond_t ymlogfull_cond ;
extern pthread_cond_t ymlogempty_cond ;
extern int YM_FLAG;

extern YM_LOG *YM_LOG_Q;
extern MY_OCI_CONTEXT_T  ociCtx_p;
void hash_init()
{
  int i = 0;
  for(i = 0;i< MAX_SIZE; i++)
  {
    hashtable[i] = NULL;  
  }
}
/*********************************************************************************************************
 *  Function Name: hash_map
 *  Purpose      : ¹þÏ£Ëã·š                 
 *  Parameter    : ËÄÔª×é
 *********************************************************************************************************/
unsigned int  hash_map( struct tuple4 *ip_and_port)
{ 
  unsigned int saddr = ip_and_port->saddr;
  unsigned short sport = ip_and_port->source;
  unsigned int daddr = ip_and_port->daddr;
  unsigned short dport = ip_and_port->dest;
  unsigned int key;
  unsigned int  key_1;
  key_1 = (saddr ^ sport) ^ (daddr ^ dport);
  key = key_1 >> 16;
  key= key_1^key;
  key = key >> 16;
  key = key & (2 ^ 16 - 1);
  return key;
}
/*unsigned int hash_map(int key)
{
 return key%5; 
}*/
/*********************************************************************************************************
 *  Function Name: tuplecmp
 *  Purpose      : ÅÐ¶ÏËÄÔª×éÊÇ·ñÏàµÈ                 
 *  Parameter    : ËÄÔª×é
 *********************************************************************************************************/
int  tuplecmp(struct tuple4 *a, struct tuple4 *b)
{
  if(a->saddr == b->saddr && a->source == b->source && a->daddr ==b->daddr && a->dest == b->dest)
    return 1;
  else
   return 0;
}

/********************************************************************************************************
 *  Function Name: tcp_insert
 *  Purpose      : ÏòtcpÊýŸÝÁŽÖÐ²åÈëÊýŸÝ                
 *  Parameter    : head ÊýŸÝÁŽÊ×µØÖ·£»sÓû²åÈëµÄÊýŸÝ
 *********************************************************************************************************/

/*void  tcp_insert(tcp *head, char *s)
{
  tcp *p ;
  p = (tcp *) malloc(sizeof(tcp));
  p->data_stream = s;
  p->next = head;
  head = p;
  printf("has insert %s \n",head->data_stream);
  return ;
} */
/*********************************************************************************************************
 *  Function Name: hashtable_insert
 *  Purpose      : Íù¹þÏ£±íÖÐ²åÈëœÚµã                  
 *  Parameter    : s²åÔÚ³åÍ»ÁŽ£¬data²åÔÚÊýŸÝÁŽ£¬žùŸÝž÷×ÔÐèÇó×ÔÐÐÐÞžÄ
 *********************************************************************************************************/
void hashtable_insert(struct tuple4 *ip_and_port, char *s,char *data,long int length,int data_length)
//void hashtable_insert(int key ,char *s ,char *data)
{
  unsigned int pos; 
 // printf("this  hashtable_insert lengtg is %d \n",strlen(data));
  pos = hash_map(ip_and_port);
  //pos = hash_map(key);
  list *phead = hashtable[pos];
  list * front = NULL; 
  
  while(phead)
  {
    if(tuplecmp(ip_and_port,phead->tup4))
    { 
      printf("have exsists%s !!!'\n",phead->name);
     // tcp_insert(phead->tcp_list,data);
      tcp *p ,*head,*front;//²ÉÓÃÍ·²å£¬È¡ÊýŸÝÁŽÊ±ÓŠ×¢ÒâË³Ðò
      printf("sssssssssssss");
     p = (tcp *) malloc(sizeof(tcp));    
     p->data_stream = (char *)malloc(data_length);
     p->data_length =data_length;
     p->data_stream = data;
     phead->count = phead->count +data_length;
    
      

     if(phead ->rear == NULL) 
     {
        p->next = phead->rear;
        phead->tcp_list->next =p;
        phead->rear =p;
     }
     else
     {
       p->next = NULL;
       phead ->rear ->next = p;
       phead->rear =p;
     }
    
      
      if(phead->count>= phead->length)
      {    
           FILE *fp   = fopen(phead->name,"ab+");
           tcp *p= phead->tcp_list->next; printf("我要保存文件%d\n",phead->count);
        while(p)
        { 
          printf("我要保存文件%d\n",phead->length);
           
          
          fwrite(p->data_stream,sizeof(char),p->data_length,fp);
          
          p=p->next;
        }
        fclose(fp);
           
           
              pthread_mutex_lock(&ymlog_mutex);
             while(YMLOG_QueueLength() ==  MAX_YMSGQSIZE)
                {        
              printf("allbuffer is full,produceris waiting...\n");
              pthread_cond_wait(&ymlogempty_cond,&ymlog_mutex);
                }
            struct ymsg_log q ;
            q.key = (char *)malloc(strlen(phead->name));
           
            strcpy(q.key,phead->name);
               
            q.saddr = phead->tup4->saddr;
            q.src = phead->tup4->source;
            q.daddr = phead->tup4->daddr;
            q.dst = phead->tup4->dest;
            time_t timep;  
           time(&timep);  
             q.time =(char*)malloc(30);
              strcpy(q.time,ctime(&timep)); 
                  q.sql = (char *)malloc(128); 
               strcpy(q.sql,"insert into ymsg_filelog values(N_YMSG_CHATLOG_ID.nextval,:1,:2,:3,:4,:5,:6)");
                     
            En_YMLOGQueue(q);
            printf("I am is produce ymsg_log..%s\n",q.key);
            pthread_mutex_unlock(&ymlog_mutex);
            pthread_cond_signal(&ymlogfull_cond);
            hashdelete(ip_and_port);
       return ;
      } 
      
           printf("only.%dB\n",phead->count);
       return ;
    }
    front = phead;
    phead = phead->next;
  }
  /*初始化哈系节点的字段*/
  list * pnewnode = (list *)malloc(sizeof(list));
 pnewnode->tcp_list = (tcp *)malloc(sizeof(tcp));
   pnewnode->tcp_list ->next =NULL;
   pnewnode->rear = pnewnode->tcp_list ->next;
  //pnewnode -> tup4= (struct tuple4 *)malloc(sizeof(ip_and_port));//Ã»ÊÂ¹ýÕâÑùµÄÖžÕëž³Öµ
  pnewnode -> tup4 =(struct tuple4 *)malloc(sizeof(struct tuple4));
  pnewnode -> tup4->dest =  ip_and_port->dest;
   pnewnode -> tup4->daddr =  ip_and_port->daddr;
   pnewnode -> tup4->source =  ip_and_port->source;
   pnewnode -> tup4->saddr =  ip_and_port->saddr;
  pnewnode ->name = (char*) malloc(strlen(s));
  strcpy(pnewnode ->name,s);
  pnewnode ->length = length;
  pnewnode->count = 0; 
  pnewnode->count = pnewnode->count+data_length;
 
  if(pnewnode->count >= pnewnode->length && pnewnode->count!=0)//pnewnode->length)
    {  
      
       int fd  = open(pnewnode->name,O_WRONLY| O_CREAT );
       int pos= lseek(fd, 0l, SEEK_SET);
       write(fd,data,data_length);
       close(fd);
       printf("第一个包 结束\n");
       hashdelete(ip_and_port);
       return ;
    }

 // tcp *data_list =  pnewndode ->tcp_list;
  
  if(front != NULL)
  {
  pnewnode ->next = front ->next;
    front->next =pnewnode;
     list  *pHead =pnewnode;
  /*  printf("XXXXX\n");
      tcp *p ,*head;//²ÉÓÃÍ·²å£¬È¡ÊýŸÝÁŽÊ±ÓŠ×¢ÒâË³Ðò
      p = (tcp *) malloc(sizeof(tcp));
      p->data_stream = (char *)malloc(data_length);
      p->data_length =data_length;
      p->data_stream = data;
      //pHead->count = pHead->count +strlen(data);
        
        head = pHead->tcp_list;
        head->next = p;
        head = p;
        head->next =NULL;
      printf("has insert %s \n",head->data_stream);
      printf("hashtable is %s\n",hashtable[pos]->name);
  //  printf("tcp data is %s \n",hashtable[pos]->tcp_list->next->data_stream);*/

  }
  else 
  { 
    pnewnode->next = NULL;
    hashtable[pos] = pnewnode;
   // tcp_insert(pnewnode->tcp_list,data);
   printf("ZZZXXXXX\n");
    printf("hashtable is %s\n",hashtable[pos]->name);
  /*   list *pHead = hashtable[pos];
    tcp *p ,*head;//²ÉÓÃÍ·²å£¬È¡ÊýŸÝÁŽÊ±ÓŠ×¢ÒâË³Ðò

      p = (tcp *) malloc(sizeof(tcp));
      p->data_stream = (char *)malloc(data_length);
      p->data_length =data_length;
       p->data_stream = data;
      // pHead->count = pHead->count +strlen(data);
     
       
        head = pHead->tcp_list;
        head->next =p;
        head = p;
        head->next =NULL;
        printf("has insert file %s \n",head->data_stream);
       */
      printf("hashtable is %s\n",hashtable[pos]->name);
    if(!hashtable[pos]->tcp_list)
    printf("wocaoXXXXXXXXXXXXXXX");
   // printf("tcp data is %d \n",hashtable[pos]->tcp_list);
  
  }
}   
/********************************************************************************************************
 *  Function Name: hashtable_print
 *  Purpose      : ŽòÓ¡¹þÏ£±í               
 *  Parameter    : ÎÞ*********************************************************************************************************/

void hashtable_print(int n)
{
  printf("========================content of hash table=============\n");
  int i;
  for(i =n ; i< MAX_SIZE; i++)
  {
    if(hashtable[i])
    {
      list *phead = hashtable[i];
      printf("%d =>",i);
      printf(" data is");
      while(phead)
      {  
        printf(" name is %s\n",phead->name);
        printf(" length is %d\n",phead->length);
        tcp *p= phead->tcp_list->next;
        while(p)
        {
          printf("1");
         printf(" %s\n",p->data_stream);
         p=p->next;
        }
        printf("\n");
        phead = phead ->next;
      }
    }
  }return;
}
/********************************************************************************************************
 *  Function Name: Find_node
 *  Purpose      : ²éÕÒœÚµã              
 *  Parameter    : ÎÞ*********************************************************************************************************/

int Find_node(struct tuple4 *ip_and_port)
{
  list *phead = hashtable[hash_map(ip_and_port)];
  while(phead)
  {
    if(tuplecmp(ip_and_port ,phead->tup4))
    {
      printf("Find key!!!\n");
      return 1;
    }
    phead = phead->next;
  }
  printf("NO find !!!\n");
  return 0;
}
/********************************************************************************************************
 *  Function Name: hashdelete
 *  Purpose      : ÉŸ³ý¹þÏ£±íµÄœÚµãºÍtcpÊýŸÝÁŽ              
 *  Parameter    £ºkey¹þÏ£¹ØŒü×Ö *********************************************************************************************************/

int  hashdelete(struct tuple4 *ip_and_port)
{
  if(Find_node(ip_and_port))
  {
    list *phead = hashtable[hash_map(ip_and_port)];
    list *plast = NULL;
    list *pdelete =NULL;
    printf("delete %d =>",hash_map(ip_and_port));
    if(phead)
    { // hashtable[hash_map(key)] = phead->next;//
      if(tuplecmp(phead->tup4,ip_and_port))//ŒÙÈçÔÚÍ·²¿
      {
       // printf("remov head from %d \n",hash_map(ip_and_port));
        phead->tup4 =NULL ;
        phead->length = 0;
        phead->count =0;
        
        phead->name = NULL;
        tcp *p = phead->tcp_list;
        phead->tcp_list = NULL;
        hashtable[hash_map(ip_and_port)] = phead->next;
        free(phead);
        while(p)
        {
          //printf("delete %s ",p->data_stream);
          free(p);
          
          p = p->next; 
        }
      } 
      else//²»ÔÙÍ·²¿
      {
        plast = phead;
        phead = phead ->next;
        while(phead)
        {
          if(tuplecmp(phead->tup4,ip_and_port))
          {
            printf("delete %d \n",hash_map(ip_and_port));
            plast->next = phead ->next;
            tcp *p = phead->tcp_list;
             phead->tup4=NULL;
             phead->length = 0;
             phead->count =0;
           
            phead->name = NULL;
            phead->tcp_list = NULL;
            free(phead);
            while(p)
            {
               //printf(" delete the data stream %s ",p->data_stream);
              free(p);
     	      p = p->next;  
            }
            break;
          }
          else
          {
            plast = phead;
            phead = phead ->next;
          }
        }
      }
    }   
  } 
  else
  {
    printf("NO find node \n");
    return 0;
  }
}   
/********************************************************************************************************
 *  Function Name: hash_realase
 *  Purpose      : Ïú»Ù¹þÏ£±í
 *  Parameter    £ºÎÞ *********************************************************************************************************/
int hash_realase()
{
  int i;
  for(i = 0;i<MAX_SIZE;i++)
  {
    if(hashtable[i])
    {
      list * phead = hashtable[1];
      free(phead);
      tcp *p =phead ->tcp_list;
      while(p) 
      {
        free(p);
        p = p->next;
      }    
    }
  }
  return 0;
}

