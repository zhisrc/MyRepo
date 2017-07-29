#include <nids.h>
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<libnet.h>
#include<pcap.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include "tcp_hash.h"
#include <time.h>
#include "associate.h"
#include"conn.h"
#include "ymsg_log.h"

extern pthread_mutex_t ymlog_mutex; // 互斥信号锁
extern pthread_cond_t ymlogfull_cond ;
extern pthread_cond_t ymlogempty_cond ;
extern int YM_FLAG;

extern YM_LOG *YM_LOG_Q ;
extern MATCHENTRY *entry;
extern MATCHENTRY *entry_ym1;
extern MY_OCI_CONTEXT_T  ociCtx_p;
void ass_insert(ass_node *a_node)
{
  ass_node *p;
  p = (ass_node*)malloc(sizeof(ass_node));
  p->c_tup4 = a_node->c_tup4;
  p->filename = a_node->filename;
  p->length =a_node->length;
  p->uri = (char *)malloc(10);
  p->uri = NULL;
  p->d_tup4 = a_node->d_tup4;
  p->ip = a_node->ip;
  p->next = table;
  table =  p;
  
}
void ass_inserturi( char *filename,char *uri)
{
  ass_node *p;
  p = table;
  //printf("this is insert uriname %s\n",filename);
  while(p)
  {  //printf("插入URI %s\n",p->filename);
    if(!strcmp(filename,p->filename))
      { p->uri = (char *)malloc(strlen(uri));
        strcpy(p->uri,uri);
        //printf("插入URI %s name %s\n",p->uri,p->filename);
       return ;
      }
    p= p->next;
  }
  
  return ;
}
void ass_insertfile(unsigned int ip,char *filename)
{
  ass_node *p;
  p = table;
  while(p)
  {
    if(!strcmp(p->filename,filename))
    { 
      //printf("have found sssss filename %s \n",filename);
      p->ip = ip;
     // printf("ip is  %d \n",p->ip);
      return;
    }
    else
    {
      p =p->next;
    }
  }
  return ;
}

int Find_anode(unsigned int ip)
{
 ass_node *head;
 head = table;
 while(head)
 {
   
   if(head->ip == ip)
   {
     //printf("Find_anode ip %d \n",head->ip);
     
     return 1;
   }
   head = head->next;
}
 return 0;
}
char *Find_name(char *uri)
{
 printf("\n itis me %s\n",uri);
 ass_node *head;
 head = table;
 while(head)
 {  //printf("ccccchead->uri%s \n",head->uri);
    
   if(!strcmp(head->uri,uri))
   {
     
     //printf("Find_anode name %s \n",head->filename);
     
     return head->filename;
   }
   head = head->next;
}
 return NULL;
}

char *Find_filename(unsigned int ip)
{
  ass_node *head;
 head = table;
 while(head)
 {
   
   if(head->ip == ip)
   {
     //printf("Find_anode name %s \n",head->filename);
     
     return head->filename;
   }
   head = head->next;
}
 return NULL;
}
long int  Find_anode_length(char *uri)
{
 ass_node *head;
 head = table;
 while(head)
 {
   
   if(!strcmp(head->uri,uri))
   { 
    // printf("Find_anode ip %d \n",head->ip);
     //printf("%s lentgh %d",table->filename,head->length);
     return head->length;
   }
   head = head->next;
}
 return -1;

}
void analy_uri(char *content,int count_new)
{   
   int i,k;
   char *file_name;
   char *URI;
   if(content[10]== 0 && content[11]== -34)//文件信息
   {    
    
      //该关联节点
      printf("servcie status File information accept!!\n");
     
      i=20;
      while(i< count_new)
      {  
         char buf_filename[100];
         if(content[i]=='2'&& content[i+1]=='7')
         { 
            printf("key is 27\n");
            i=i+4;
            //printf("文件名为：\n");
            k =0;
            while(content[i]!=-64 )
            { 
	      //printf("%c",content[i]);
             //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。                                      
              buf_filename[k++] = content[i];
              i++;
             }
             buf_filename[k] = '\0';
             file_name = (char *)malloc(k+3);
             strcpy(file_name,buf_filename);
                          // printf("this is buf_filename\n",buf_filename);	//while截取文件名		
              i=i+2; printf("file infor filename %s", buf_filename);
          }//if
                     
          if(content[i]=='2'&& content[i+1]=='5'&&content[i+2]=='1')
          {  
            char uri[10];k=0;
            printf("key is 251\n");
            i=i+5;
            printf("文件服务器URI：\n");
            while(content[i]!=-64 &&k<9)
            { 
               printf("%c",content[i]);
               uri[k++] = content[i];
                             //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。
	       i++;
             }
             uri[k] ='\0';
             printf("\n");
             URI =(char*)malloc(10);
             strcpy(URI,uri);
             ass_inserturi(file_name,URI);
             //printf("uri :%s\n",URI);
            // printf("\n");//找251	//while 截取服务器IP	
             memset(content,0,count_new);
             return ;
         }//if251
          i++;
       }//便利content
        return ;
    }//文件传输information
}
 void analy_chat(char *content,int count_new,struct tuple4 ip_and_port)
 {  int i,k;  		      
   printf("servcie status chat !!");
   printf("\n");
   i=20;
     void * ptr;
   char  key[10];
   char  findword[100];
   char *findword_1;
    int size_1; 
   while(i< count_new)
   {  
     char chat[1024];
     if(content[i]=='1'&& content[i+1]=='4')
     { 
       struct tuple4 ip_and_port_1 = ip_and_port;
       printf("key is 14\n");
       i=i+4;
       printf("聊天信息为：\n");
       k=0;
       FILE *fp=fopen("chat.txt","ab+");
       while(content[i]!=-64 )
       {
          //printf("%c",content[i]);
          //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。        v
          chat[k++] = content[i]; 
          fseek(fp, 0, SEEK_END);
          fputc(content[i],fp); 
          i++;
        }
        fputc('\n',fp); fclose(fp);
         	//while		
        chat[k]='\0';
         printf("%s\n",chat);
      if(YM_FLAG == 1)
        {     printf("switch AC\n");
           MATCHENTRY *TEMP = entry;
           entry = entry_ym1;
           entry_ym1 = TEMP;
           YM_FLAG = 0;
           RemoveEntry(50, entry_ym1);
        }
        
        findword_1 = Match(chat,0,entry,findword);//有问题段错误
        printf("find word_1 is  %s \n",findword_1);
          if(findword_1)//写日志
            {         
             pthread_mutex_lock(&ymlog_mutex);
             while(YMLOG_QueueLength() ==  MAX_YMSGQSIZE)
                {        
              printf("allbuffer is full,produceris waiting...\n");
              pthread_cond_wait(&ymlogempty_cond,&ymlog_mutex);
                }
            struct ymsg_log q ;
                 q.key = (char *)malloc(strlen(chat));
            printf("I am is produce ymsg_log..\n");
            strcpy(q.key,chat);
                q.saddr =ip_and_port_1.saddr;
                q.src =ip_and_port_1.source;
                q.daddr=ip_and_port_1.daddr;
                q.dst = ip_and_port_1.dest;
                time_t timep;  
                time(&timep);  
                      q.time =(char*)malloc(30);
                strcpy(q.time,ctime(&timep)); 
                     q.sql = (char *)malloc(128); 
               strcpy(q.sql,"insert into ymsg_chatlog values(N_YMSG_CHATLOG_ID.nextval,:1,:2,:3,:4,:5,:6)");
            En_YMLOGQueue(q);
            pthread_mutex_unlock(&ymlog_mutex);
            pthread_cond_signal(&ymlogfull_cond);
    
         
            //insert_data( &ociCtx_p,findword_1);
           // printf("need insert %s\n",findword_1);
            
             
          }
       //char *chat_content = (char *)malloc(strlen(chat)+3);
       //strcpy(chat_content,chat);
       //hashtable_insert(&ip_and_port_1,"chat",chat_content,1000000,0);
       //printf("chat content is %s\n",chat);
       break;                           
     }i++;//if
   }//wjlie
}//service


void analy_fileinfo(char *content,int count_new)
{ 
  int i,k;
  char buf_filename[100];
  char buf_length[20];  
  ass_node * a_node = (ass_node*)malloc(sizeof(ass_node));//该关联节点
  printf("servcie status File !!\n");
   i=20;
  while(i< count_new)
  {   
    if(content[i]=='2'&& content[i+1]=='7')
    { 
      printf("key is 27\n");
      i=i+4;
     // printf("文件名为：\n");
      k = 0;//将文件名赋值到buf
      while(content[i]!=-64 )
      { 
         buf_filename[k++] = content[i];
	// printf("%c",content[i]);
         //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。
	 i++;
      }buf_filename[k]='\0';
       //printf("this is buf_filename %s",buf_filename);
       a_node->filename = (char *)malloc(strlen(buf_filename)+3);
       strcpy(a_node->filename,buf_filename);
      // printf("\n");	//while截取文件名		
        i=i+2; 
     }//if
   //  printf("%c",content[i]);//27之前的都会被printf出来
     if(content[i]=='2'&& content[i+1]=='8')
     { 
       printf("key is 28\n");
       i=i+4;
       //printf("文件长度为：\n");
       k = 0;
       while(content[i]!=-64 )
       { 
	 //printf("%c",content[i]);
         //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。  
	 buf_length[k++] = content[i];
	 i++;
       }
       buf_length[k] = '\0';
       char *p_end;
       a_node->length = strtol(buf_length,&p_end,10);
       printf("B\n");
       //printf("a_node->length %ld B\n",a_node->length);
       //printf("a_node->filename%s B\n",a_node->filename);
       ass_insert(a_node);//第一次建立关联表
       break;	//while 截取文件大小		
     }i++;//if
   }//便利content
}
void analy_fileinfo_2(char *content,int count_new)
{   
   int i,k,ip;
   char buf_filename[100];
   char buf_ip[20];
   printf("servcie status File information !!\n");
   //printf("\n");
   i=20;
   while(i< count_new)
   {   
     if(content[i]=='2'&& content[i+1]=='7')
     { 
       printf("key is 27\n");
       i=i+4;
       printf("文件名为：\n");
       k =0;
       while(content[i]!=-64 )
       { 
	 //printf("%c",content[i]);
        //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。        
         buf_filename[k++] = content[i];
	 i++;
       }
       buf_filename[k] = '\0';
       // printf("this is buf_filename\n",buf_filename);	//while截取文件名		
       i=i+2;  
       printf("file infor filename %s", buf_filename);
     }//if
                        //printf("%c",content[i]);//27之hou的都会被printf出来
     if(content[i]=='2' && content[i+1]=='5'&&content[i+2]=='0')
     { 
       printf("key is 250\n");
       i=i+5;
       printf("文件服务器IP：\n");
       k = 0;
       while(content[i]!=-64 )
       { 
	 //printf("%c",content[i]);
         buf_ip[k++] = content[i];
          //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。
	 i++;
       }buf_ip[k] = '\0';//m模拟关联表
       printf("buf _ip %s\n",buf_ip);
                           // a_node->ip = inet_addr(buf_ip);
       ip =inet_addr(buf_ip);
       printf("this a_node ip %d",ip);
       printf("\n");i=i+2;//找251	//while 截取服务器IP
       ass_insertfile(ip,buf_filename);	
     }//if250
    // if(content[i]=='2'&& content[i+1]=='5'&&content[i+2]=='1')
     //{ 
     //   printf("key is 251\n");
     //   i=i+5;
      //  printf("文件服务器URI：\n");
     //   while(content[i]!=-64 )
      // { 
	   //printf("%c",content[i]);
           //不用转ASSCii码，因为utf-8可以识别ascci字符，只是一个字节一个字节读汉字汇出错。//
	//   i++;
        //}
      //  printf("\n");//找251	//while 截取服务器IP	
    // }//if251                     
     i++;
   }//便利content
}//文件传输information
void analy_filecontent(char *content,int count_new,struct tuple4 ip_and_port)
{ 
  int i;
  struct tuple4 ip_and_port_1 =ip_and_port;
  int k =0;
  char *file_content = (char*)malloc(count_new);
  char uri[10];
  for(i = 0;i<count_new;i++)
  {   
    if(content[i] == 'P' &&content[i+1] == 'O' && content[i+2] == 'S' && content[i+3] == 'T')      
    {  
      for(i=18;i<=26;i++)
        uri[k++] = content[i];
      uri[k]='\0';
      //printf("this post uri%s\n",uri);
      char *a_filename = Find_name(uri);
     // printf("\nsssssssssssscccccccccccccccccc\n");
      long int length = Find_anode_length(uri);
      printf("  文件传输 %s  \n",a_filename);
      hashtable_insert(&ip_and_port,a_filename,NULL,length,0);
      return ;
                              
    }
    else
    {   
      memcpy(file_content,content,count_new);
      hashtable_insert(&ip_and_port,NULL,file_content,0,count_new);
      return ;
    }                        
  }     
} 
void   analy_stocfilecontent(char *content,int count_new,struct tuple4 ip_and_port)
{ //printf("this is stoc\n");
  int i;
  struct tuple4 ip_and_port_1 =ip_and_port;
  int k =0;
  char *file_content = (char*)malloc(count_new);
  char uri[10];
  for(i = 0;i<count_new;i++)
  {   
    if(content[i] == 'G' &&content[i+1] == 'E' && content[i+2] == 'T')      
    {  
      for(i=17;i<=25;i++)
        uri[k++] = content[i];
      uri[k]='\0';
     // printf("this get uri%s\n",uri);
      char *a_filename = Find_name(uri);
     // printf("\nsssssssssssscccccccccccccccccc\n");
      long int length = Find_anode_length(uri);
      //printf("  文件传输 %s  \n",a_filename);
      hashtable_insert(&ip_and_port,a_filename,NULL,length,0);
      return ;                              
    }
    else
    { //printf("\ngetssssssssssssssssssssssssssssssssss\n"); 
      memcpy(file_content,content,count_new);
      hashtable_insert(&ip_and_port,NULL,file_content,0,count_new);
      return ;
    }                        
  }     
}


