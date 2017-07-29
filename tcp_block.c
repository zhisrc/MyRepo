/*************************************************************************
	> File Name: block.c
	> Author: ganzhixiong
	> Mail: ahigzx@163.com 
	> Created Time: Fri 18 Dec 2015 10:52:16 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <sys/socket.h>
#include <libnet.h> 
#include "nids.h"
#include "tcp_block.h"

unsigned short check_sum(unsigned short *buff, int size)
{
	unsigned int sum = 0;
	while (size>1)
	{
		sum += *buff++;
		size = size - sizeof(unsigned short);
	}
	if(size)
	{
		sum += (unsigned char)*buff;  //size为1时加上最后一个字节
	}
	  //第一次高16位加到低16位时可能溢出，此时重复一次即可
	sum = (sum & 0xffff) + (sum>>16);
	sum = (sum & 0xffff) + (sum>>16);
	return (unsigned short)(~sum);
}


void forgepack(unsigned int saddr, unsigned short source, unsigned int daddr, unsigned short dest, unsigned int seq)
{
	char buf[128] = {0};
	struct iphdr *ip;
	struct tcphdr *tcp;
	struct fakheader *fake;
	int sockfd;
	struct sockaddr_in destaddr;
	const int on = 1;
	bzero(&destaddr, sizeof(destaddr));
	destaddr.sin_family = AF_INET;
	destaddr.sin_port = htons(dest);
	destaddr.sin_addr.s_addr = daddr;//*((struct in_addr*)&(ip_and_port->daddr));
	int checksize = sizeof(struct fakheader) + sizeof(struct tcphdr);

	if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP))<0)
	{
		perror("Create error");
		exit(1);
	}

	if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on))<0)
	{
		perror("IP_HDRINCL failed");
		exit(1);
	}

	int ip_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
	ip = (struct iphdr*)buf;
	ip->tot_len = htons(ip_len);
	ip->version = IPVERSION;
	ip->ihl = sizeof(struct iphdr) >> 2;
	ip->ttl = MAXTTL; 
	ip->frag_off = htons(0x4000);
	ip->daddr = daddr;//destaddr.sin_addr.s_addr;
	ip->protocol = IPPROTO_TCP;
	ip->saddr = saddr; //mark

	tcp = (struct tcphdr*)(buf+sizeof(struct iphdr));
	tcp->dest = htons(dest);//mark 
	tcp->source = htons(source);//mark
	tcp->doff = 5;
	tcp->rst = 1;
	tcp->seq = htonl(seq);  
	tcp->window = 0;//htons(300); //maybe have problem
	tcp->check = 0;
	//try below	
	fake = (struct fakheader*)(buf+sizeof(struct iphdr)+sizeof(struct tcphdr));
	fake->saddr = saddr;
	fake->daddr = daddr;
	fake->protocol = 6;  //tcp protocol
	fake->len = htons(sizeof(struct tcphdr));
	
	tcp->check = check_sum((unsigned short*)tcp,checksize);

	sendto(sockfd,buf,ip_len,0,(struct sockaddr*)&destaddr,sizeof(struct sockaddr_in));
}

void tcp_block(struct tcp_stream *strm)
{
	struct tuple4 ip_and_port;
	int client_seq;
	int server_seq;
	int client_window = strm->server.window/2;
	int server_window = strm->client.window/2;
	ip_and_port = strm->addr;
	client_seq = strm->client.first_data_seq + strm->server.count;
	server_seq = strm->server.first_data_seq + strm->client.count;
	forgepack(ip_and_port.saddr, ip_and_port.source, ip_and_port.daddr, ip_and_port.dest, client_seq);
	forgepack(ip_and_port.daddr, ip_and_port.dest, ip_and_port.saddr, ip_and_port.source, server_seq);
	forgepack(ip_and_port.saddr, ip_and_port.source, ip_and_port.daddr, ip_and_port.dest, client_seq + client_window);
	forgepack(ip_and_port.daddr, ip_and_port.dest, ip_and_port.saddr, ip_and_port.source, server_seq + server_window);
}
