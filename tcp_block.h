/*************************************************************************
	> File Name: tcp_block.h
	> Author: ganzhixiong
	> Mail: ahigzx@163.com 
	> Created Time: Fri 18 Dec 2015 03:14:37 PM CST
 ************************************************************************/

#ifndef  TCP_BLOCK_H
#define  TCP_BLOCK_H

struct fakheader
{
	unsigned int saddr;
	unsigned int daddr;
	unsigned char zero;
	unsigned char protocol;
	unsigned short len;
};

unsigned short check_sum(unsigned short *buff, int size);

void forgepack(unsigned int saddr, unsigned short source, unsigned int daddr, unsigned short dest, unsigned int seq);

void tcp_block(struct tcp_stream *strm);

#endif
