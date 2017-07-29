/********************************************************************
** Copyright(c) 2015 ,���������̴�ѧ��Ϣ��ȫ�о�����
** All rights reserved
**
** �ļ�����http_hash
** �����ˣ�gan
** ��  ����http��ϣ��
** 
** ��ǰ�汾��1.0
** ��    �ߣ��Ժ�
** ������ڣ�2015-12-02
** ȡ���汾��1.x
** ������ڣ�*********
** ********************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "http.h"


void httphash_init()
{
  int i = 0;
  for(i = 0;i< MAX_SIZE; i++)
  {
    httphashtable[i] = NULL; 
  }
}
/*********************************************************************************************************
 *  Function Name: hash_map_1
 *  Purpose      : ��ϣ�㷨                 
 *  Parameter    : ��Ԫ��
 *********************************************************************************************************/
unsigned int  hash_map_1( unsigned int saddr, unsigned short sport, unsigned int daddr, unsigned short dport)
{
  unsigned int  key;
  key = (saddr ^ sport) ^ (daddr ^ dport);
  key ^= key >> 16;
  key = key >> 16;
  key = key & (2 ^ 16 - 1);
  return key;
}
/*unsigned int hash_map(int key)
{
 return key%5;90<F9>```````````````````` 
}
*/
/*********************************************************************************************************
 *  Function Name: tuplecmp_1
 *  Purpose      : �ж���Ԫ���Ƿ����                 
 *  Parameter    : ��Ԫ��
 *********************************************************************************************************/
int  tuplecmp_1(struct tuple4 a, struct tuple4 b)
{
  if(a.saddr == b.saddr && a.source == b.source && a.daddr ==b.daddr && a.dest == b.dest)
    return 1;
  else
   return 0;
}

/*********************************************************************************************************
 *  Function Name: httphashtable_insert
 *  Purpose      : ��http��ϣ���в���ڵ�                  
 *  Parameter    : 
 *********************************************************************************************************/
//void hashtable_insert(struct tuple4 ip_and_port, char *s,char *data)
void httphashtable_insert(struct tuple4 ip_and_port, char* contype, int *cont_encoding, int* tran_encoding, z_stream *strm, int next_cklen)
{
  unsigned int pos;
  unsigned int saddr = ip_and_port.saddr;
  unsigned short sport = ip_and_port.source;
  unsigned int daddr = ip_and_port.daddr;
  unsigned short dport = ip_and_port.dest;
  pos = hash_map_1(saddr,sport,daddr,dport);
  httplist *phead = httphashtable[pos];
  httplist * front = NULL;
  while(phead)
  {
    if(tuplecmp_1(ip_and_port,phead->tup4))
    {
	  //phead ->cont_type = (char*)malloc(6*sizeof(char));  
	  strcpy(phead->cont_type, contype); //�Ѿ����˾Ͳ��÷���ռ�ֱ�ӽ��и���
	  phead ->tran_encoding = *tran_encoding;
	  phead ->cont_encoding = *cont_encoding;
	  phead ->next_cklen = next_cklen;
	 // inflateEnd(&(phead ->strm));// ���°��ͽ���zstream�ͷ�
	  if (strm)
	  {
		phead ->strm = *strm; //����������ָ�� mark
		free(strm);
	  } //���ָ��Ϊ�գ����Դ˳�Ա���и�ֵ��ָ��Ϊ��ʱ���������ݰ�����ʱ��ȡ��ֵ����֪�Ƿ��ܱ������
      return;
    }
    front = phead;
    phead = phead->next;
  }
  httplist * pnewnode = (httplist *)malloc(sizeof(httplist));

  pnewnode ->cont_type = (char*)malloc(6*sizeof(char));  
  strcpy(pnewnode->cont_type, contype);
  pnewnode ->tran_encoding = *tran_encoding;
  pnewnode ->cont_encoding = *cont_encoding;
  pnewnode ->next_cklen = next_cklen;
  pnewnode ->tup4 = ip_and_port;
  if (strm)
  {
	pnewnode ->strm = *strm; //����������ָ�� mark
	free(strm);  //��ֵ�꼴�ͷ�ָ��ָ��Ŀռ�
  }
  if(front != NULL)
  {
    pnewnode ->next = front ->next;
    front->next =pnewnode;
	printf("inserted\n");
  }
  else 
  {
    pnewnode->next = NULL;
    httphashtable[pos] = pnewnode;
	printf("new position inserted\n");
  }
}   
/********************************************************************************************************
 *  Function Name: hashtable_print
 *  Purpose      : ��ӡ��ϣ��               
 *  Parameter    : ��*********************************************************************************************************/

//void hashtable_print(int n)
//{
//  printf("========================content of hash table=============\n");
//  int i;
//  for(i =n ; i< MAX_SIZE; i++)
//  {
//    if(hashtable[i])
//    {
//      list *phead = hashtable[i];
//      printf("%d =>",i);
//      printf(" data is");
//      while(phead)
//      {
//        printf("name is %s\n",phead->name);
//        tcp *p= phead->tcp_list;
//        while(p)
//        {
//         
//         printf(" %s",p->data_stream);
//         p =p->next;
//        }
//        printf("\n");
//        phead = phead ->next;
//      }
//    }
//  }
//}
/********************************************************************************************************
 *  Function Name: get_info_from_hash
 *  Purpose      : ���ҽڵ�              
 *  Parameter    : *********************************************************************************************************/

int get_info_from_hash(struct tuple4 ip_and_port, char *contype, int *cont_encoding, int *tran_encoding, z_stream *strm, int *next_cklen)
{

  unsigned int pos;
  unsigned int saddr = ip_and_port.saddr;
  unsigned short sport = ip_and_port.source;
  unsigned int daddr = ip_and_port.daddr;
  unsigned short dport = ip_and_port.dest;
  pos = hash_map_1(saddr,sport,daddr,dport);
  httplist *phead = httphashtable[pos];
  while(phead)
  {
    if(tuplecmp_1(ip_and_port,phead->tup4))
    {
      printf("Find node!!!\n");
	  //contype = phead->cont_type;  
	  strcpy(contype,phead->cont_type);
	  *cont_encoding = phead->cont_encoding;
	  *tran_encoding = phead->tran_encoding;
	  *next_cklen    = phead->next_cklen;
	  if (*cont_encoding == 0)  //�ӽṹ����ȡֵʱ��� cont_encodingΪ0(��gzip)����Ҫȡstrm����ֶΣ���ֵΪnull
	  {
		free(strm);
		strm = NULL;
	  }
	  else
	  {
		*strm = phead->strm; //��ָ��ֱ��ָ����߹�ϣ���е�strm��Ա��ַ
	  }
      return 1;
    }
    phead = phead->next;
  }
  printf("Can't find node!!!\n");
  return 0;
}
//��ʣ���chunk���Ȳ���

int insert_chunklen(struct tuple4 ip_and_port, int next_cklen)
{
  unsigned int pos;
  unsigned int saddr = ip_and_port.saddr;
  unsigned short sport = ip_and_port.source;
  unsigned int daddr = ip_and_port.daddr;
  unsigned short dport = ip_and_port.dest;
  pos = hash_map_1(saddr,sport,daddr,dport);
  httplist *phead = httphashtable[pos];
  while(phead)
  {
    if(tuplecmp_1(ip_and_port,phead->tup4))
    {
      printf("Find node!!!\n");
	  phead ->next_cklen = next_cklen;
      return 1;
    }
    phead = phead->next;
  }
  printf("Can't find node!!!\n");
  return 0;
}
/********************************************************************************************************
 *  Function Name: httphash_delete
 *  Purpose      : ɾ��http��ϣ���Ľڵ�           
 *  Parameter    ��key��ϣ�ؼ��� *********************************************************************************************************/

int httphash_delete(struct tuple4 ip_and_port)
{
  unsigned int pos; 
  unsigned int saddr = ip_and_port.saddr;
  unsigned short sport = ip_and_port.source;
  unsigned int daddr = ip_and_port.daddr;
  unsigned short dport = ip_and_port.dest;
  pos = hash_map_1(saddr,sport,daddr,dport);
  httplist *phead = httphashtable[pos];
  httplist * front = NULL;
  while(phead)
  {
    if(tuplecmp_1(ip_and_port,phead->tup4))
    {
		printf("delete node");
		if(front)
		{
			front ->next = phead->next;
			free(phead);
			return 1;
		}
		else
		{
			httphashtable[pos] = NULL;
			free(phead);
			return 1;
		}
    }
    front = phead;
    phead = phead->next;
  }
  printf("can't find node");
  return 0;
}   
/********************************************************************************************************
 *  Function Name: hash_realase
 *  Purpose      : ���ٹ�ϣ��
 *  Parameter    ���� *********************************************************************************************************/
//int hash_realase()
//{
//  int i;
//  for(i = 0;i<MAX_SIZE;i++)
//  {
//    if(hashtable[i])
//    {
//      list * phead = hashtable[i];
//      free(phead);
//	}    
//  }
//  return 0;
//}
