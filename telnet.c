#include "apue.h"
#include <pcap.h>
#include <libnet.h>
#include <malloc.h>
#include <iconv.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<pthread.h>
#include <sys/wait.h>
#include "telnet_link.h"
#include "tcp_queue.h"
#include "conn.h"
#include "telnet_log.h"
#include "update.h"
#include "tcp_block.h"
#include <netinet/in.h>
#include <arpa/inet.h>
extern MY_OCI_CONTEXT_T ociCtx_p;
extern int TEL_FLAG;
extern pthread_mutex_t telbuffer_mutex; // �����ź���
extern  pthread_cond_t telbufferfull_cond ;
extern pthread_cond_t telbufferempty_cond ;
extern SqQueue *TelQ;
extern TELNET_LOG *TEL_LOG_Q;
 //ass_node *table=NULL;
extern  pthread_mutex_t tellog_mutex; // �����ź���
 extern pthread_cond_t tellogfull_cond ;
 extern pthread_cond_t tellogempty_cond ;

extern MATCHENTRY *telnet_entry;
extern MATCHENTRY *telnet_entry_1;
extern packet_telnet *telnet_head;
extern int n;
//IP
extern int IP_FLAG;

extern MATCHENTRY *ip_entry;
extern MATCHENTRY *ip_entry_1;
char ascii_string[10000];
char *char_to_ascii(char ch)
{
    char *string;
    ascii_string[0] = 0;
    string = ascii_string;
    if (isgraph(ch))
        *string++ = ch;
    else if (ch == ' ')
        *string++ = ch;
    else if (ch == '\n' || ch == '\r')
        *string++ = ch;
    else
        *string++ = '.';
    *string = 0;
    return ascii_string;
}

/*
=======================================================================================================================
�����Ƿ���TelnetЭ��Ļص�����
=======================================================================================================================
 */


void telnet_protocol_callback(struct tcp_stream *tcp_connection, void **arg)
{  
struct tuple4 ip_and_port = tcp_connection->addr;
 char *findword_1;    
  char findword[100];
    if(IP_FLAG == 1)
    {
          printf("switch IP AC\n");
           MATCHENTRY *TEMP = ip_entry;
           ip_entry = ip_entry_1;
           ip_entry_1 = TEMP;
           IP_FLAG = 0;
           RemoveEntry(50, ip_entry_1);
     }     
        char temp[30];
          strcpy(temp,inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr)))); 
              
                   
       if( (findword_1 = Match(temp,0,ip_entry,findword))!=NULL)//������δ���
      {     printf("llllhtelnet\n");
         tcp_block(tcp_connection);
         printf("find word_1 is  %s \n",findword_1);
            pthread_mutex_lock(&tellog_mutex);
            while(TELLOG_QueueLength() ==  MAX_TELNETQSIZE)
                {        
              printf("allbuffer is full,produceris waiting...\n");
              pthread_cond_wait(&tellogempty_cond,&tellog_mutex);
                 }
            struct telnet_log q ;
            q.key = (char *)malloc(strlen("ip"));
            printf("I am is produce telnet_log..\n");
            strcpy(q.key,"ip");
            q.saddr =ip_and_port.saddr;
            q.src =ip_and_port.source;
            q.daddr=ip_and_port.daddr;
            q.dst = ip_and_port.dest;
            time_t timep;  
            time(&timep);  
            q.time =(char*)malloc(30);
            strcpy(q.time,ctime(&timep)); 
            En_TELLOGQueue(q);
            pthread_mutex_unlock(&tellog_mutex);
            pthread_cond_signal(&tellogfull_cond);
            
   
         }
            
	if((tcp_connection->addr.dest == 23) ||(tcp_connection->addr.source == 23))
	{
		pthread_mutex_lock(&telbuffer_mutex);
		while(QueueLength(TelQ) == MAXQSIZE)
		{ 
			printf("allbuffer is full,produceris waiting...\n");
			pthread_cond_wait(&telbufferempty_cond,&telbuffer_mutex);
		}
			struct Qnode telnetq ;//��
			printf("I am is producer..\n");
			telnetq.tcp_connection = tcp_connection;//��
			EnQueue(telnetq,TelQ);//��
			pthread_mutex_unlock(&telbuffer_mutex);
			pthread_cond_signal(&telbufferfull_cond);											usleep(100);
	}	
    															
	    return ;
}

void telnet_process(struct tcp_stream *tcp_connection, void **arg)
{
    int i;
    char address_string[1024]; 
    char content[2048];
    char content_urgent[65535];
	char findword[50];
	char *findword_1;
	char *findword_2;
	//system_init();
	//telnet_load_keyword();
    struct tuple4 ip_and_port = tcp_connection->addr;
	//int state = Buildentry(0,telnet_entry);

    strcpy(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.saddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.source);
    strcat(address_string, " <---> ");
    strcat(address_string, inet_ntoa(*((struct in_addr*) &(ip_and_port.daddr))));
    sprintf(address_string + strlen(address_string), " : %i", ip_and_port.dest);
    strcat(address_string, "\n");
    switch (tcp_connection->nids_state)
    {
        case NIDS_JUST_EST:
            if ((tcp_connection->addr.dest == 23) ||(tcp_connection->addr.source == 23))
            {
                /* Telnet�ͻ��˺�Telnet�������˽������� */
                tcp_connection->client.collect++;
                /* Telnet�ͻ��˽������� */
                tcp_connection->server.collect++;
                /* Telnet�������������� */
                tcp_connection->server.collect_urg++;
                /* Telnet���������ս������� */
                tcp_connection->client.collect_urg++;
                /* Telnet�ͻ��˽��ս������� */
             printf("%sTELNET�ͻ�����TELNET��������������\n", address_string);
            }
            return ;
        case NIDS_CLOSE:
            /* TelnetЭ�����������ر� */
            printf("--------------------------------\n");
            printf("%sTELNET�ͻ�����TELNET���������������ر�\n", address_string);
            return ;
        case NIDS_RESET:
            /* TelnetЭ�����ӱ�RST�ر� */
            printf("--------------------------------\n");
            printf("%sTELNET�ͻ�����TELNET���������ӱ�REST�ر�\n", address_string);
            return ;
        case NIDS_DATA:
            {

				/* TelnetЭ�����µ����ݴﵽ */
                struct half_stream *hlf;
				packet_telnet *p_telnet;
                if (tcp_connection->client.count_new)
                {
                    /* Telnet�ͻ��˽��յ��µ����� */
 

				     printf("--------------------------------\n");
                  
                    /* ���Telnet�ͻ��˽��յ����µ����� */
                    //memcpy(content, hlf->data, hlf->count_new);
					memcpy( content , tcp_connection->client.data , tcp_connection->client.count_new );
                    //content[hlf->count_new] = '\0';
                    if (tcp_connection->addr.dest == 23 & (strncmp(content, "IACIAC", 6) == 0 || strncmp(content,"IAC",3 ) != 0  ))
					{
						/*for(i = 0; i < hlf->count_new; i++)
						{
                        
							printf("%s", char_to_ascii(content[i]));

						}
						*/
						p_telnet=(packet_telnet *)malloc(sizeof (packet_telnet) );
						(*p_telnet).source=tcp_connection->addr.source;
						(*p_telnet).dest=tcp_connection->addr.dest;
						(*p_telnet).saddr=tcp_connection->addr.saddr;
						(*p_telnet).daddr=tcp_connection->addr.daddr; 
						(*p_telnet).count_new=tcp_connection->server.count_new;
						memcpy( (*p_telnet).data , content , strlen(content) );
						(*p_telnet).next=NULL;
						packet_telnet_store(p_telnet);
						//printf("LaLaLa~~\n");
						if(telnet_head->next != NULL)
						{
							p_telnet=get_packet(telnet_head);
							printf("ȡ�����ǵ�%d��\n", p_telnet->p_count);
							memcpy(content,p_telnet->data,p_telnet->count_new);
							content[p_telnet->count_new] = '\0';
							 if(TEL_FLAG == 1)
                                   { printf("switch AC\n");
                                     MATCHENTRY *TEMP = telnet_entry_1;
                                     telnet_entry_1 = telnet_entry;
                                     telnet_entry = TEMP;
                                     TEL_FLAG = 0;
                                     RemoveEntry(50, telnet_entry_1);
                                              } 
							findword_2 = Match(p_telnet->data,0,telnet_entry,findword);
					
							printf("find keyword:%s\n",findword_2);
							if(NULL!=findword_2)
							{
								printf("YES!\n");
								nids_killtcp(tcp_connection);
								pthread_mutex_lock(&tellog_mutex);
             							while(TELLOG_QueueLength() ==  MAX_TELNETQSIZE)
                						{        
              								printf("allbuffer is full,produceris waiting...\n");
              								pthread_cond_wait(&tellogempty_cond,&tellog_mutex);
                						}
            							struct telnet_log q ;
                 						q.key = (char *)malloc(strlen(findword_2));
            							printf("I am is produce telnet_log..\n");
            							strcpy(q.key,findword_2);
                						q.saddr =ip_and_port.saddr;
                						q.src =ip_and_port.source;
                						q.daddr=ip_and_port.daddr;
               							q.dst = ip_and_port.dest;
                     						time_t timep;  
                                                                time(&timep);  
                                                                q.time =(char*)malloc(30);
                                                                strcpy(q.time,ctime(&timep)); 
            							En_TELLOGQueue(q);
            							pthread_mutex_unlock(&tellog_mutex);
            							pthread_cond_signal(&tellogfull_cond);
							}
						}
					}
                }
                if (tcp_connection->server.count_new)
                {
                    /* Telnet���������յ��µ����� */


					//hlf = &tcp_connection->server;
                    
                    /* ���Telnet���������յ����µ����� */
                    //memcpy(content, hlf->data, hlf->count_new);
					memcpy( content , tcp_connection->server.data , tcp_connection->server.count_new );
                    //content[hlf->count_new] = '\0';
					if( tcp_connection->addr.dest == 23 & (strncmp(content, "IACIAC", 6) == 0 || strncmp(content,"IAC",3 ) != 0  ))
					{
						/*for (i = 0; i < hlf->count_new; i++)
						{
							printf("%s", char_to_ascii(content[i]));
						}
						*/
						p_telnet=(packet_telnet *)malloc(sizeof (packet_telnet) );
						(*p_telnet).source=tcp_connection->addr.source;
						(*p_telnet).dest=tcp_connection->addr.dest;
						(*p_telnet).saddr=tcp_connection->addr.saddr;
						(*p_telnet).daddr=tcp_connection->addr.daddr;
						(*p_telnet).count_new=tcp_connection->server.count_new;
						memcpy((*p_telnet).data, tcp_connection->server.data, tcp_connection->server.count_new);
						packet_telnet_store(p_telnet);
						printf("HaHaHa~~\n");
						if(telnet_head->next != NULL)
						{
							p_telnet=get_packet(telnet_head);
							printf("ȡ�����ǵ�%d��\n",p_telnet->p_count);
							memcpy(content,p_telnet->data,p_telnet->count_new);
							content[p_telnet->count_new] = '\0';
                                                               if(TEL_FLAG == 1)
                              {     printf("switch AC\n");
                                     MATCHENTRY *TEMP = telnet_entry_1;
                                     telnet_entry_1 = telnet_entry;
                                     telnet_entry = TEMP;
                                     TEL_FLAG = 0;
                                     RemoveEntry(50, telnet_entry_1);
                               } 

							findword_1 = Match(p_telnet->data,0,telnet_entry,findword);
							printf("find keyword:%s\n",findword_1);
 
							if( NULL!=findword_1 )
							{
								 pthread_mutex_lock(&tellog_mutex);
             while(TELLOG_QueueLength() ==  MAX_TELNETQSIZE)
                {        
              printf("allbuffer is full,produceris waiting...\n");
              pthread_cond_wait(&tellogempty_cond,&tellog_mutex);
                }
            struct telnet_log q ;
                 q.key = (char *)malloc(strlen(findword_1));
            printf("I am is produce ymsg_log..\n");
            strcpy(q.key,findword_1);
                q.saddr =ip_and_port.saddr;
                q.src =ip_and_port.source;
                q.daddr=ip_and_port.daddr;
                q.dst = ip_and_port.dest;
                     q.time = (char *)malloc(strlen("20151220"));
                strcpy(q.time,"20151220");
            En_TELLOGQueue(q);
            pthread_mutex_unlock(&tellog_mutex);
            pthread_cond_signal(&tellogfull_cond);
                                                            
                                                        }
						}
					}
				}
            }
        default:
            break;
    }
    return ;
	//RemoveEntry(1, telnet_entry);
}


/*void * telnet_plug_keyword(void * arg)
{
	struct tcp_stream * tcp_connection=NULL;
	int state = Buildentry(0,telnet_entry); 
	char *findword;
	findword = (char *)malloc( 100*sizeof(char) );
	findword = Match(hlf->data,state,telnet_entry,findword);
	if
	{
		printf("find keyword:%s\n",findword);
		nids_killtcp(tcp_connection);
	}
}*/
void packet_telnet_store(packet_telnet *p_telnet)
{
	telnet_insert(telnet_head,p_telnet);
}


void* telQ_consumer(void* arg)
{
	while(1)
	{ 
		pthread_mutex_lock(&telbuffer_mutex);
		while(QueueLength(TelQ) ==0)  
		{  
			printf("TEL CONSUMERn\n");
			pthread_cond_wait(&telbufferfull_cond,&telbuffer_mutex); 
			printf("TELCONSUMER \n");
		}
		telnet_process(DeQueue(TelQ).tcp_connection,NULL);
								      
		pthread_mutex_unlock(&telbuffer_mutex);
		pthread_cond_signal(&telbufferempty_cond);
		usleep(50);
	}
	     
}
