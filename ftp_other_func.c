#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "nids.h"
#include <oci.h>
#include <time.h>
#include "data_hash.h"
#include "control_hash.h"
#include "tcp_queue.h"

#include "write_orcl_circular_queue.h"
#include "conn.h"
#include "ftp_other_func.h"
extern MY_OCI_CONTEXT_T  ociCtx_p;
extern FTP_FLAG;
extern SqQueue *FTPQ;
extern MATCHENTRY *ftp_entry;
extern MATCHENTRY *ftp_entry_1;

extern pthread_mutex_t ftpmutex;
extern pthread_cond_t ftpfull_cond;
extern pthread_cond_t ftpempty_cond;
extern pthread_mutex_t orclmutex;
extern pthread_cond_t orcl_full_cond;
extern pthread_cond_t orcl_empty_cond;
//#define MAXQSIZE 150
#define MAXORCLQSIZE 100
//获取到被动模式下服务器端的数据端口
int getPasvServerDataPort(char *content)
{
	int count = 0;
	int server_data_port;
	char high_part_of_port[5];
	char low_part_of_port[5];
	memset(high_part_of_port,0,sizeof(high_part_of_port));
	memset(low_part_of_port,0,sizeof(low_part_of_port));
	char *p = content;
	int j = 0;
	for (count = 0; count < 4; p++){
		if(*p == ',')
		  count++;
	}
	for (j = 0; *p != ','; p++, j++){
		high_part_of_port[j]  = *p;
	}
	high_part_of_port[j++] = '\0';
	for (p++, j = 0; *p != ')'; p++, j++){
		low_part_of_port[j]  = *p;
	}
	low_part_of_port[j+1]  = '\0';
	//printf("high=%s, %p,low=%s\n",high_part_of_port, high_part_of_port, low_part_of_port);
	unsigned int high = (unsigned int)atoi(high_part_of_port);
	unsigned int low= (unsigned int)atoi(low_part_of_port);
	server_data_port = (high<<8)|(low&0xff);
	//printf("server_data_port=%d\n", server_data_port);
	return server_data_port;
}
//获取到文件名和文件后缀然后去做相应的操作
void determineFileSuffixToDo(struct tuple4 tul4,char *content)
{
	char *filename = content + strlen("RETR ");
	char *temp = strstr(content,".");
	char *c = strstr(temp,"\r");
	*c = '\0';
	strstr(content,".");
	strcmp(strstr(content,"."),".txt");
	if(strcmp(strstr(content,"."),".txt") == 0)
	{
		printf("为文本文件\n");
		//control_hash_print();
		int serverdataport;
		serverdataport = control_find_node(tul4);
		//printf("serverdataport = %d\n",serverdataport);
		//先查找是否在关联hash表里，如果在证明是被动传输，需要获取服务器端数据端口

		if(serverdataport != -1)//证明在关联表里
		{
			//printf("证明在关联表里，则接下来写入data hash\n");
			//再写入data hash里
			data_hash_insert_one(tul4,filename,serverdataport);
			//	data_hash_print();
		}
		else//没在关联表里证明是主动传输，端口为20
		{
			//先将其插入关联hash表里
			control_hash_insert(tul4,20);
			//再写入data hash里
			data_hash_insert_one(tul4,filename,20);
			//	data_hash_print();
		}
	}
	else if(strcmp(strstr(content,"."),".doc")==0||strcmp(strstr(content,"."),".docx")==0)
	{
		//	control_hash_print();
		int serverdataport;
		serverdataport = control_find_node(tul4);
		//先查找是否在关联hash表里，如果在证明是被动传输，需要获取服务器端数据端口

		if(serverdataport != -1)//证明在关联表里
		{
			//再写入data hash里
			data_hash_insert_one(tul4,filename,serverdataport);
			//	data_hash_print();
		}
		else//没在关联表里证明是主动传输，端口为20
		{
			//先将其插入关联hash表里
			control_hash_insert(tul4,20);
			//再写入data hash里
			data_hash_insert_one(tul4,filename,20);
			//	data_hash_print();
		}
	}
	else if(strcmp(strstr(content,"."),".xls")==0||strcmp(strstr(content,"."),".xlsx")==0)
	{
		//	control_hash_print();
		int serverdataport;
		serverdataport = control_find_node(tul4);
		//先查找是否在关联hash表里，如果在证明是被动传输，需要获取服务器端数据端口

		if(serverdataport != -1)//证明在关联表里
		{
			//再写入data hash里
			data_hash_insert_one(tul4,filename,serverdataport);
			//	data_hash_print();
		}
		else//没在关联表里证明是主动传输，端口为20
		{
			//先将其插入关联hash表里
			control_hash_insert(tul4,20);
			//再写入data hash里
			data_hash_insert_one(tul4,filename,20);
			//	data_hash_print();
		}
	}
	else if(strcmp(strstr(content,"."),".ppt")==0||strcmp(strstr(content,"."),".pptx")==0)
	{
		//	control_hash_print();
		int serverdataport;
		serverdataport = control_find_node(tul4);
		//先查找是否在关联hash表里，如果在证明是被动传输，需要获取服务器端数据端口
		if(serverdataport != -1)//证明在关联表里
		{
			//再写入data hash里
			data_hash_insert_one(tul4,filename,serverdataport);
			//	data_hash_print();
		}
		else//没在关联表里证明是主动传输，端口为20
		{
			//先将其插入关联hash表里
			control_hash_insert(tul4,20);
			//再写入data hash里
			data_hash_insert_one(tul4,filename,20);
			//	data_hash_print();
		}
	}
	else 
	{
		control_hash_print();
		int serverdataport;
		serverdataport = control_find_node(tul4);
		//先查找是否在关联hash表里
		if(serverdataport != -1)//证明在关联表里,否则，关联表里没有，则无需任何操作
		{
			//将在关联表里的此项删除
			control_hash_delete(tul4);
		}
		data_hash_delete(tul4,serverdataport);
	}
}
//当数据链接结束去data hash取数据进行处理，文本文件去匹配
//office文件进行保存，并写相应的日志

void fetch_data_to_manage(struct tuple4 ip_and_port,int port)
{
	unsigned int pos;
	pos = data_hash_map(ip_and_port.daddr,port,ip_and_port.saddr);
	datalist *phead = datahashtable[pos];
	datalist * front = NULL;
	//用于自动机
	char findword[50];
	char *findword_1;
	char totalfindword[100];
	memset(totalfindword,0,100);
    //用于存储sql
	struct orclQnode q;
	while(phead)
	{
		if(data_tuple_cmp(ip_and_port.daddr,port,ip_and_port.saddr,phead->tul3))
		{
			//printf("have exsists%s !!!'\n",phead->filename);
			if(strcmp(strstr(phead->filename,"."),".txt") == 0)
			{
				datatcp *p;
     				p = phead->tcp_list;
                                    if(FTP_FLAG == 1 && ftp_entry_1 != NULL)
                              {     printf("switch FTP AC\n");
                                     MATCHENTRY *TEMP = ftp_entry_1;
                                     ftp_entry_1 = ftp_entry;
                                     ftp_entry = TEMP;
                                     FTP_FLAG = 0;
                                     RemoveEntry(50, ftp_entry_1); 
                               } 
        



				while(p)
				{
                                        if(ftp_entry != NULL)
                                        {
					  findword_1 = Match(p->data_stream,0,ftp_entry,findword);
					  //printf("Match find keyword:%s\n",findword_1);
					   strcat(totalfindword,findword_1);
                                         }
					p= p->next;  
				}

				//加入到orcl缓冲队列里
				pthread_mutex_lock(&orclmutex);
				while(orclQueueLength() == MAXORCLQSIZE)
				{
					printf("orcl all bufeer is full,producef is waiting\n");
					pthread_cond_wait(&orcl_empty_cond,&orclmutex);
				}
				//printf("I am a producer..\n");
	//strcpy(mysql, "insert into employees values('109', 'x12er', 'na', '1888-06-08')");
	           // q->sql = (char *)malloc(sizeof(char)*200);
				strcpy(q.sql,"insert into ftp_log values(N_FTP_LOG_ID.nextval,:1,:2,:3,:4,:5,:6)");
				time_t rawtime;
				struct tm * timeinfo;
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				char time[100];
				strcpy(time,asctime(timeinfo));
	            char *c = strstr(time,"\n");
	            *c = '\0';
			//	strcat(q.sql,asctime(timeinfo));
                                
				strcpy(q.log.saddr,inet_ntoa(*((struct in_addr *)&(ip_and_port.saddr))));
                               
				q.log.src = phead->source; 
				strcpy(q.log.daddr,inet_ntoa(*((struct in_addr *)&(ip_and_port.daddr))));
				q.log.dst = port;
				strcpy(q.log.text,totalfindword); 
				strcpy(q.log.time,time);
  
				orclEnqueue(q);
				pthread_mutex_unlock(&orclmutex);
				pthread_cond_signal(&orcl_full_cond);
                                printf("switch FTP8 AqqC\n");
				usleep(100);
				return;
			}
			else
			{
				FILE *fd;
				fd = fopen(phead->filename,"wb+");
				if(fd == NULL)
				{
					//printf("fopen fail to open\n");
					exit(1);
				}
				// tcp_insert(phead->tcp_list,data);
				datatcp *p ;
				p = phead->tcp_list;
				// printf("data is \n");
				while(p)
				{
					// printf("%s\n",p->data_stream);
					//printf("the length of this data_stream is:%d\n",p->eachdatalength);
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
				//加入到orcl缓冲队列里
				pthread_mutex_lock(&orclmutex);
				while(orclQueueLength() == MAXORCLQSIZE)
				{
					printf("orcl all bufeer is full,producef is waiting\n");
					pthread_cond_wait(&orcl_empty_cond,&orclmutex);
				}
				printf("I am a producer..\n");
	//strcpy(mysql, "insert into employees values('109', 'x12er', 'na', '1888-06-08')");
	           // q->sql = (char *)malloc(sizeof(char)*200);
				strcpy(q.sql,"insert into ftp_filelog values(N_FTP_FILELOG_ID.nextval,:1,:2,:3,:4,:5,:6)");
				time_t rawtime;
				struct tm * timeinfo;
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				char time[100];
				strcpy(time,asctime(timeinfo));
	            char *c = strstr(time,"\n");
	            *c = '\0';
			//	strcat(q.sql,asctime(timeinfo));
				printf("q.sql = %s\n",q.sql);

				strcpy(q.log.saddr,inet_ntoa(*((struct in_addr *)&(ip_and_port.saddr))));
				q.log.src = phead->source;
				strcpy(q.log.daddr,inet_ntoa(*((struct in_addr *)&(ip_and_port.daddr))));
				q.log.dst = port;
				strcpy(q.log.text,phead->filename);
				strcpy(q.log.time,time);

				orclEnqueue(q);
				pthread_mutex_unlock(&orclmutex);
				pthread_cond_signal(&orcl_full_cond);
				usleep(100);
				return ;
			}
		}
		front = phead;
		phead = phead->next;
	}
}
//ftp处理函数
void ftp_protocol_manage(struct tcp_stream *ftp_connection)
{
	//如果是21端口，则为ftp数据链接的包
	
	//	printf("addr.source =%d\n",ftp_connection->addr.source);
	//	printf("addr.daddr =%d\n",ftp_connection->addr.daddr);
	//	printf("addr.saddr =%d\n",ftp_connection->addr.saddr);
	if((ftp_connection->addr.dest == 21) || (ftp_connection->addr.source == 21))
	{
            printf("manage addr.dest =%d\n",ftp_connection->addr.dest);
		if(ftp_connection->nids_state == NIDS_JUST_EST)
		{
			ftp_connection->client.collect++;
			ftp_connection->server.collect++;
			ftp_connection->client.collect_urg++;
			ftp_connection->server.collect_urg++;
		}
		if(ftp_connection->nids_state == NIDS_DATA)
		{
			struct half_stream* hlf;
			if (ftp_connection->client.count_new)
			{
				/*FTP客户端收到新的数据*/
				hlf = &ftp_connection->client;
				char content[65535];
				memcpy(content,hlf->data,hlf->count_new);
				content[hlf->count_new] = '\0';
				if(strncmp(content,"221",3) == 0)
				{
					//关闭链接，则删除hash表中的此项
					//printf("控制链接关闭\n");
					control_hash_delete(ftp_connection->addr);
				}
				if(strncmp(content,"227",3) == 0)
				{
					//获取到被动模式下服务器端的数据端口
					int serverdataport;
					serverdataport = getPasvServerDataPort(content);
					//将端口写入关联表即controlhashtable
					control_hash_insert(ftp_connection->addr,serverdataport);
					//printf("将端口写入关联表，查看control hash\n");
					//	data_hash_insert_one(ftp_connection->addr," ",serverdataport);
					//	control_hash_print();
				}
				if(strncmp(content,"226",3) == 0)
				{
					int serverdataport;
					//数据链接已经关闭，则去处理该数据;处理完成则删除此项在hash
					//printf("数据链接已经正常关闭\n");
					//查询关联表获取到数据链接的四元组
					serverdataport = control_find_node(ftp_connection->addr);
					if(serverdataport != -1)
					{
						//然后查询datahashtable,取数据然后进行数据处理，保存或检测关键字
						fetch_data_to_manage(ftp_connection->addr,serverdataport);
						//处理结束删除在data hash的该节点
						data_hash_delete(ftp_connection->addr,serverdataport);
					}
				}
				if(strncmp(content,"550",3) == 0)
				{
					int serverdataport;
					//数据链接已经关闭，则去处理该数据;处理完成则删除此项在hash
					//printf("虽然表示未执行的请求操作但是居然能够下载文件\n");
					//查询关联表获取到数据链接的四元组
					serverdataport = control_find_node(ftp_connection->addr);
					if(serverdataport != -1)
					{
						//然后查询datahashtable,取数据然后进行数据处理，保存或检测关键字
						fetch_data_to_manage(ftp_connection->addr,serverdataport);
						//处理结束删除在data hash的该节点
						data_hash_delete(ftp_connection->addr,serverdataport);
					}
				}
			}
			if(ftp_connection->server.count_new)
			{
				/*FTP服务器接收到新的数据*/
				hlf = &ftp_connection->server;
				char content[65535];
				memcpy(content,hlf->data,hlf->count_new);
				content[hlf->count_new] = '\0';
				if(strstr(content,"RETR"))
				{
					printf("保存文件为（RETR）:%s\n", content + strlen("RETR"));
					determineFileSuffixToDo(ftp_connection->addr,content);
				}
				if(strstr(content,"STOR"))
				{
					printf("保存文件为（STOR）:%s\n", content + strlen("STOR"));
					determineFileSuffixToDo(ftp_connection->addr,content);
				}
			}
		}
	}
	else//如果不是21端口的看看是不是所需要的数据包
	{
		//先判断在不在数据链接hash表,在则将data写入，不在则不是所要的数据包,不错任何处理
		//printf("判断这个是不是数据包\n");
		if(ftp_connection->nids_state == NIDS_JUST_EST)
		{
			ftp_connection->client.collect++;
			ftp_connection->server.collect++;
			ftp_connection->client.collect_urg++;
			ftp_connection->server.collect_urg++;
		}
		if(ftp_connection->nids_state == NIDS_DATA)
		{
			struct half_stream* hlf;
			if (ftp_connection->client.count_new)
			{
				hlf = &ftp_connection->client;
				char content[65535];
				//	printf("hlf->count_new=%d\n",hlf->count_new);
				memcpy(content,hlf->data,hlf->count_new);
				content[hlf->count_new] = '\0';
				//检测是不是所要的数据包，是则插入，不是则不做任何操作
				//printf("client!用2方式插入data hash\n");
				data_hash_insert_two(ftp_connection->addr,content,hlf->count_new,2);
				//printf("插入data hash 数据看看是否插入进去\n");
				//	data_hash_print();
			}
			if(ftp_connection->server.count_new)
			{
				hlf = &ftp_connection->server;
				//	printf("hlf->count_new=%d\n",hlf->count_new);
				char content[65535];
				memcpy(content,hlf->data,hlf->count_new);
				content[hlf->count_new] = '\0';
				//检测是不是所要的数据包，是则插入，不是则不做任何操作
				//printf("server !用方式插入data hash\n");
				data_hash_insert_two(ftp_connection->addr,content,hlf->count_new,2);
				//printf("插入data hash 数据看看是否插入进去\n");
				//	data_hash_print();
			}
		}
	}
	return ;
}
//往数据库里写日志
void write_log_to_orcl(struct orclQnode q)
{
  insert_ftpdata(&ociCtx_p,q.sql,q.log);
}
//去取orcl缓冲队列
void *fetch_orcl_queue(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&orclmutex);
		while(orclQueueLength() == 0)
		{
			printf("orcl buffer is empty\n");
			pthread_cond_wait(&orcl_full_cond,&orclmutex);
		}
		write_log_to_orcl(orclDequeue());
		pthread_mutex_unlock(&orclmutex);
		pthread_cond_signal(&orcl_empty_cond);
		//	usleep(50);
	}
}
//去取ftp缓冲队列
void *fetch_ftp_queue(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&ftpmutex);
		while(QueueLength(FTPQ) == 0)
		{
			printf("ftp buffer is empty\n");
			pthread_cond_wait(&ftpfull_cond,&ftpmutex);
		}
		ftp_protocol_manage(DeQueue(FTPQ).tcp_connection);
		pthread_mutex_unlock(&ftpmutex);
		pthread_cond_signal(&ftpempty_cond);
		//	usleep(50);
	}
}
//nids的回调函数
void ftp_protocol_callback(struct tcp_stream* ftp_connection,void **arg)
{
	//printf("callback ddr.dest =%d\n",ftp_connection->addr.dest);
	//	printf("addr.source =%d\n",ftp_connection->addr.source);
	//printf("callback addr.daddr =%d\n",ftp_connection->addr.daddr);
	//	printf("addr.saddr =%d\n",ftp_connection->addr.saddr);
	//如果是21端口，则为ftp数据链接的包
	//	if((determin_if_in_datahash(ftp_connection->addr)) || (ftp_connection->addr.dest == 21) || (ftp_connection->addr.source == 21))
	if((ftp_connection->addr.dest > 1023) || (ftp_connection->addr.dest == 21) || (ftp_connection->addr.source == 21))
	{
		//加入到ftp缓冲队列里
		pthread_mutex_lock(&ftpmutex);
		while(QueueLength(FTPQ) == MAXQSIZE)
		{
			printf("allbufeer is full,producef is waiting\n");
			pthread_cond_wait(&ftpempty_cond,&ftpmutex);
		}
		struct Qnode q;
		printf("I am a FTP producer..\n");
		q.tcp_connection = ftp_connection;
		EnQueue(q,FTPQ);
		pthread_mutex_unlock(&ftpmutex);
		pthread_cond_signal(&ftpfull_cond);
		usleep(50);
	}
	//	if(determin_if_in_datahash)
	return ;
}
