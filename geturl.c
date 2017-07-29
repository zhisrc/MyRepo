/*************************************************************************
	> File Name: geturl.c
	> Author: ganzhixiong
	> Mail: ahigzx@163.com 
	> Created Time: Wed 02 Dec 2015 11:17:07 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "http.h"
int get_url(char content[], int number, char url[])
{
	char line[2048];
	char uri[2048];
	int i = 0;
	int j = 0;
	int k = 0;
	int already_get_flag = 0;//判断是否已经获得完uri 以免每行的重复判断
	for (i = 0; i < number; i++)
	{
		if (content[i] != '\n')
		{
			k++;
			continue;
		}
		for (j = 0; j < k; j++)
		{
			line[j] = content[j + i -k]; //按行分析，还可以用指针方法做
		}
		line[j] = '\0';	
		if(!already_get_flag)
		{		
			if ((strstr(line, "GET"))||(strstr(line, "POST")))
			{
				sscanf(line, "%*s %s",uri);
				//printf("uri:%s\n",uri);
				already_get_flag = 1;
			}
		}
		if(strstr(line, "Host"))
		{
			sscanf(line, "%*s %s",url);
			//printf("Host:%s\n",url);
			strcat(url,uri);
			//printf("url:%s\n",url);
			return 0;
		}
		k = 0;
	}
	return 1;	
}
