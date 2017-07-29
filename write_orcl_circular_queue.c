/********************************************************************
** Copyright(c) 2015 ,哈尔滨工程大学信息安全研究中心
** All rights reserved
**
** 文件名：write_orcl_circular_queue
** 创建人：赵旭
** 描  述：写数据库的循环缓冲队列
** 注  意：
** 
** 当前版本：1.0
** 作    者：赵旭
** 完成日期：2015-12-117
** 取代版本：1.x
** 作    者：输入原作者（或修改者）名字
** 完成日期：*********
** ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <nids.h>
#include "write_orcl_circular_queue.h"
#define MAXORCLQSIZE 100
orclQueue *orclQ = NULL;

/*********************************************************************************************************
 *  Function Name: 
 *  Purpose      :                 
 *  Parameter    : 
 *********************************************************************************************************/
int orcl_create_queue()
{
	orclQ = (orclQueue *)malloc(sizeof(orclQueue));
	orclQ->base = (struct orclQnode *)malloc(MAXORCLQSIZE*sizeof(struct orclQnode));
	if(!(orclQ->base))
	{
		printf("orcl circular queue Memory allocation failure\n");  
		return 0;        //退出程序 
	}
	orclQ->front = 0;         //初始化参数  
	orclQ->rear = 0; 
	return 1;
}
/*********************************************************************************************************
 *  Function Name: 
 *  Purpose      :                 
 *  Parameter    : 
 *********************************************************************************************************/
/*bool orclFullQueue(PQUEUE Q)  
{  
	if(Q->front == (Q->rear+1)%Q->maxsize)//判断循环链表是否满，留一个预留空间不用  
	  return true;  
	else  
	  return false;  
}  */
/*********************************************************************************************************
 *  Function Name: 
 *  Purpose      :                
 *  Parameter    : 
 *********************************************************************************************************/
/*bool ftpEmptyQueue(PQUEUE Q)  
{  
	if(Q->front == Q->rear)//判断是否为空  
	  return true;  
	else  
	  return false;  
} */
/*********************************************************************************************************
 *  Function Name: 
 *  Purpose      :                  
 *  Parameter    : 
 *********************************************************************************************************/
int orclEnqueue(struct orclQnode q)  
{ 
	if((orclQ->rear +1)%MAXORCLQSIZE == orclQ->front)
      return 0;  
	orclQ->base[orclQ->rear] = q;  
	orclQ->rear = (orclQ->rear+1)%MAXORCLQSIZE;  
	return 1;   
} 
/*********************************************************************************************************
 *  Function Name: 
 *  Purpose      :                  
 *  Parameter    :
 *********************************************************************************************************/
struct orclQnode orclDequeue()  
{  
	struct orclQnode orclqnode;
	if(orclQ->front == orclQ->rear)
	{
	  return;
	}
	orclqnode = orclQ->base[orclQ->front];
	orclQ->front = (orclQ->front+1)%MAXORCLQSIZE;
	return orclqnode; 
}

int orclQueueLength()
{
  return (orclQ->rear - orclQ->front + MAXORCLQSIZE)%MAXORCLQSIZE;
}
