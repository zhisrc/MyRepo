/*************************************************************************
	> File Name: http_analysis.c
	> Author: ganzhixiong
	> Mail: ahigzx@163.com 
	> Created Time: Thu 03 Dec 2015 10:52:14 AM CST
 ************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <stdio.h>
#include "http.h"
#include "keyword_match.h"
#include "logqueue.h"
//暂时先写一个能放程序中实验分析的东西。

extern MATCHENTRY *urlmatch;
extern MATCHENTRY *http_blockmatch;
extern MATCHENTRY *http_match;
extern MATCHENTRY *urlmatch_1;
extern MATCHENTRY *http_blockmatch_1;
extern MATCHENTRY *http_match_1;
extern int URL_FLAG;
extern int HTTP_FLAG;
extern int BLOCK_FLAG;

void http_analysis(struct tcp_stream *tcp_http_connection)
{
    char address_content[1024]; //临时加的
	char content[4096];
	char ungzip_data[10000] = {0};
	struct tuple4 ip_and_port = tcp_http_connection->addr; 
	char url[2048];  //url最大长度暂时这样
	char *entity_data;
	int ret;
	struct half_stream *hlf;
	z_stream *strm = NULL;   //创建z_stream型指针来指向zlib中需要维持的流(gzip)
	printf("\nhttp.c\n");
	if (tcp_http_connection->client.count_new)  //客户端(浏览器)接受数据
	{     printf("\nhttp.c1111\n");
		char *contype;
		char *findword = (char*)malloc(sizeof(char)*200); //大小待改
		int *is_block   = (int*)malloc(sizeof(int));
		int *next_cklen = (int*)malloc(sizeof(int));   //记录后来的数据包的chunk所剩下的长度
		int *cont_encoding = (int*)malloc(sizeof(int)); //指针指向的内容默认值0表示无,1表示内容编码为gzip,deflate可不用考虑,已过时基本不用,若需要支持则为2
		int *tran_encoding = (int*)malloc(sizeof(int)); //默认值0表示无,1表示传输编码为chunked

		hlf = &tcp_http_connection->client;		//hlf表示客户端(浏览器)接受的数据_
		*is_block = 0;

		//strcpy(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
		//sprintf(address_content + strlen(address_content), ":%i", ip_and_port.source);
		//strcat(address_content, " <---- ");
		//strcat(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
		//sprintf(address_content + strlen(address_content), ":%i", ip_and_port.dest);
		//strcat(address_content, "\n");
		//printf("\n");
		//printf("%s", address_content);
                printf("content\n");
		memcpy(content, hlf->data, hlf->count_new);
		content[hlf->count_new] = '\0';
		//如果是第一个包来的话就需要插入哈希表或者更新哈希表对应的结点信息，如果不是第一个包(不包含报文头的各种信息)只需要从哈希表中查找结点,看响应的哈希结点的值,查出值后对信息做相应的处理,主要看gzip,chunk,type。
               printf("\nhttp.c1\n");
		if (content[0] != 'H' && content[1] != 'T' && content[2] != 'T' && content[3] != 'P')
		{
			printf("This tcp packet is data packet\n");
		//mark 这里传进去的strm指针指向哈希表中结点对应的值，如果不是gzip赋值null,contype也是所以不分配空间	
			contype = (char*)malloc(sizeof(char)*6);
			strm = (z_stream*)malloc(sizeof(z_stream));
			if ((get_info_from_hash(ip_and_port, contype, cont_encoding, tran_encoding, strm ,next_cklen)) == 0) 
			{
				printf("Data cant find node in hashtable\n");
				free(is_block);
				free(findword);
				free(next_cklen);
				free(cont_encoding);
				free(tran_encoding);printf("\nhttp.c2\n");
				return;
			}
			if (!(strcmp(contype, "text"))) //待改 图片什么都不插入，则走上面应该可以不用这个判断
			{
				if(BLOCK_FLAG == 1)
				{       printf("\nhttp.c3\n");
					printf("switch AC\n");
					MATCHENTRY *TEMP = http_blockmatch_1;
					http_blockmatch_1 = http_blockmatch;
					http_blockmatch = TEMP;
					BLOCK_FLAG = 0;
					RemoveEntry(50, http_blockmatch_1);
				} 
				if(HTTP_FLAG == 1)
				{     printf("\nhttp.c4\n");
					printf("switch AC\n");
					MATCHENTRY *TEMP = http_match_1;
					http_match_1 = http_match;
					http_match = TEMP;
					HTTP_FLAG = 0;
					RemoveEntry(50, http_match_1);
				} 

				if (*cont_encoding)
				{      printf("\nhttp.c5\n");
					if (*tran_encoding)
					{
						printf("data gzip and chunked\n");
						if (*next_cklen > (hlf->count_new))
						{
							gzip_uncompress(strm, content, hlf->count_new, ungzip_data, 10000);
							         
							if(Match(ungzip_data, 0, http_blockmatch, findword))
							{
								//nids_killtcp(tcp_http_connection);
								tcp_block(tcp_http_connection);
								*is_block = 1;
								httplog_producer(findword, ip_and_port, *is_block, NULL);
							}
							else if(Match(ungzip_data, 0, http_match, findword))
							{
								httplog_producer(findword, ip_and_port, *is_block, NULL);
								//日志
							}
							//printf("entity ungzip data:%s\n",ungzip_data);
							*next_cklen = *next_cklen - hlf->count_new;
						}
						else
						{ //判断是否第一个包的}
							if(*next_cklen)
							{
								gzip_uncompress(strm, content, *next_cklen, ungzip_data, 10000);
								if(Match(ungzip_data, 0, http_blockmatch, findword))
								{
									//nids_killtcp(tcp_http_connection);
									tcp_block(tcp_http_connection);
									*is_block = 1;
									httplog_producer(findword, ip_and_port, *is_block, NULL);
								}
								else //封堵上了就不往下分析了
								{
									
									if(Match(ungzip_data, 0, http_match, findword))
									{
										//日志
										httplog_producer(findword, ip_and_port, *is_block, NULL);
									}
									//printf("new ungzip data:%s\n", ungzip_data);
									char *gzipdata = content + *next_cklen + 2; //到下一个长度的起始处
									int redata_len = hlf->count_new - *next_cklen -2; //剩下的长度
									if(redata_len)
									{
										*next_cklen = chunk_gzip(gzipdata, redata_len, ungzip_data, strm, tcp_http_connection, is_block);
									}
									else
									{
										*next_cklen = 0;
									}
								}
							}
							else //*next_cklen等于0的时候为上一个包刚好到chunk结尾
							{
								*next_cklen = chunk_gzip(content, hlf->count_new, ungzip_data, strm, tcp_http_connection, is_block);
							}
						}
						if(*is_block) //封堵了删除哈希结点
						{
							httphash_delete(ip_and_port);
						}
						else
						{
							insert_chunklen(ip_and_port, *next_cklen);//之前已经进过一次哈希表获取数据，这里插入时再查一次哈希，效率可能不太好，可以在前面的时候返回相应哈希的位置
						}
						
					}
					else //gzip里的chunk判断
					{      printf("\nhttp.c6\n");
						printf("data only gzip\n");
						gzip_uncompress(strm, content, hlf->count_new, ungzip_data, 10000);

						if(Match(ungzip_data, 0, http_blockmatch, findword))
						{
							//nids_killtcp(tcp_http_connection);
							tcp_block(tcp_http_connection);
							*is_block = 1;
							httphash_delete(ip_and_port);
							httplog_producer(findword, ip_and_port, *is_block, NULL);
						}
						else if(Match(ungzip_data, 0, http_match, findword))
						{
							httplog_producer(findword, ip_and_port, *is_block, NULL);
							//日志
						}
						
						//printf("ungzip data:%s\n",ungzip_data);
						//将ungzip_data匹配
						
					}
					free(strm); //用完此z_strem即释放
				}
				else //gzip else
				{
					printf("not gzip,match directly\n");
				//	printf("data:\n%s",content);
					if(BLOCK_FLAG == 1)
					{    
						printf("switch AC\n");
						MATCHENTRY *TEMP = http_blockmatch_1;
						http_blockmatch_1 = http_blockmatch;
						http_blockmatch = TEMP;
						BLOCK_FLAG = 0;
						RemoveEntry(50, http_blockmatch_1);
					} 
					if(HTTP_FLAG == 1)
					{    
						printf("switch AC\n");
						MATCHENTRY *TEMP = http_match_1;
						http_match_1 = http_match;
						http_match = TEMP;
						HTTP_FLAG = 0;
						RemoveEntry(50, http_match_1);
					} 

					if(Match(content, 0, http_blockmatch, findword))
					{
						//nids_killtcp(tcp_http_connection);
						tcp_block(tcp_http_connection);
						*is_block = 1;
						httphash_delete(ip_and_port);
						httplog_producer(findword, ip_and_port, *is_block, NULL);
					}
					else if(Match(content, 0, http_match, findword))
					{
						httplog_producer(findword, ip_and_port, *is_block, NULL);
						//日志
					}
					//不是gzip直接匹配
				}
			}
			else //文本判断
			{
				free(is_block);
				free(findword);
				free(next_cklen);
				free(cont_encoding);
				free(tran_encoding);
				printf("is not text\n");
				return; //不是文本类型直接不处理
			}
		}
		else //第一个包包含报文头等信息
		{
			int *datalen = (int*)malloc(sizeof(int));
			contype = (char*)malloc(6*sizeof(char));  //需要给contype指向的内容赋值
			entity_data = client_data_analysis(content, hlf->count_new, contype, cont_encoding, tran_encoding, datalen);    //分析出的实体数据              
             printf("if \n");
			if (!(strcmp(contype, "text")))
			{
                             printf("if esle \n");
			   

				if(BLOCK_FLAG == 1)
				{    
					printf("switch AC\n");
					MATCHENTRY *TEMP = http_blockmatch_1;
					http_blockmatch_1 = http_blockmatch;
					http_blockmatch = TEMP;
					BLOCK_FLAG = 0;
					RemoveEntry(50, http_blockmatch_1);
				} 
				if(HTTP_FLAG == 1)
				{    
					printf("switch AC\n");
					MATCHENTRY *TEMP = http_match_1;
					http_match_1 = http_match;
					http_match = TEMP;
					HTTP_FLAG = 0;
					RemoveEntry(50, http_match_1);
				} 

				if (*cont_encoding)
				{      printf("if shipin esle \n");
			   
					strm = (z_stream*)malloc(sizeof(z_stream)); //初始化z_stream
					strm ->zalloc = NULL;
					strm ->zfree  = NULL;
					strm ->opaque = NULL;
					strm ->avail_in = 0;
					strm ->next_in  = NULL;
					int result = inflateInit2(strm, 47);
					if (entity_data)
					{
								
						if(*tran_encoding)
						{
							*next_cklen = chunk_gzip(entity_data, *datalen, ungzip_data, strm,tcp_http_connection, is_block);
							printf("gzip and chunked\n");
						}
						else
						{
							gzip_uncompress(strm, entity_data, *datalen, ungzip_data,10000);
							if(Match(ungzip_data, 0, http_blockmatch, findword))
							{
								//nids_killtcp(tcp_http_connection);
								tcp_block(tcp_http_connection);
								*is_block = 1;
								httplog_producer(findword, ip_and_port, *is_block, NULL);
							}
							else if(Match(ungzip_data, 0, http_match, findword))
							{
								httplog_producer(findword, ip_and_port, *is_block, NULL);
								//日志
							}
							printf("only gzip\n");
					//		printf("ungzip data:%s\n",ungzip_data);
							//这边用ungzip_data匹配
						}
					}
					else
					{
						*next_cklen = 0;
					}
				}
				else//不是gzip
				{
					//直接匹配
					
					if(entity_data)
					{
						if(Match(entity_data, 0, http_blockmatch, findword))
						{      
							//nids_killtcp(tcp_http_connection);
							tcp_block(tcp_http_connection);
							*is_block = 1;
							httplog_producer(findword, ip_and_port, *is_block, NULL);
						}
						else if(Match(entity_data, 0, http_match, findword))
						{         printf("data:,entity_data\n");
							//日志
							httplog_producer(findword, ip_and_port, *is_block, NULL);
						}
					}
					printf("not gzip,match directly\n");
				}
		//		}//entity_data 为空  实体内容为空照样需要插入 将下面 注释掉。。
			//	else
			//	{
			//		free(datalen);//释放内存空间
			//		free(contype);
			//		free(next_cklen);
			//		free(cont_encoding);
			//		free(tran_encoding);
			//		return;
			//	}
			} //不是text
			else
			{
				printf("first packet is not text\n"); //改：第一个包类型不是text不插入,后面来的数据包发现找不到结点就直接放弃。
				httphash_delete(ip_and_port); //如果第一个包不是text则查找哈希表，如果当前位置存在结点则将结点删掉
				free(is_block);
				free(findword);
				free(datalen);
				free(contype);
				free(next_cklen);
				free(cont_encoding);
				free(tran_encoding);
				return;
			}
			if (!*is_block) //封堵过了就不插哈希表，下个数据来了就会找不到结点直接丢弃了
			{
				httphashtable_insert(ip_and_port, contype, cont_encoding, tran_encoding, strm, *next_cklen);//相关信息结点插入哈希表
			}
			free(datalen);//释放内存空间
			free(contype);
		} //判断是否第一个包的}
		free(is_block);
		free(findword);
		free(next_cklen);
		free(cont_encoding);
		free(tran_encoding);

	}
	else
	{ 


	printf("wtf...\n");
		if(URL_FLAG == 1)
		{    
			printf("switch AC\n");
			MATCHENTRY *TEMP = urlmatch_1;
			urlmatch_1 = urlmatch;
			urlmatch = TEMP;
			URL_FLAG = 0;
			RemoveEntry(50, urlmatch_1);
		} 

		//服务器接受数据
		char *findurl  = (char*)malloc(sizeof(char)*100); //待改
        hlf = &tcp_http_connection->server;

		//strcpy(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
		//sprintf(address_content + strlen(address_content), " : %i", ip_and_port.source);
		//strcat(address_content, " ----> ");
		//strcat(address_content, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
		//sprintf(address_content + strlen(address_content), ":%i", ip_and_port.dest);
		//strcat(address_content, "\n");
		//printf("\n");
		//printf("%s", address_content);

		memcpy(content, hlf->data, hlf->count_new);
		content[hlf->count_new] = '\0';
		ret = get_url(content, hlf->count_new, url);
		if (ret)
		{
			printf("get_url error\n");
			return;
		}
		if(Match(url, 0, urlmatch, findurl))
		{
			//nids_killtcp(tcp_http_connection);
			tcp_block(tcp_http_connection);
			httpurl_producer(findurl, ip_and_port, NULL);
		}
		free(findurl);
		printf("geturl:%d\n",ret);
		//之后就是Url放自动机匹配
	}






printf("http.cccccc\n");




}
