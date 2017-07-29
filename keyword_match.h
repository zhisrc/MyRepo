﻿/******************************************************
** Copyright(c) 2011, 哈尔滨工程大学信息安全研究中心
** All rights reserved
** 文件名:keyword_match.h

** 描  述:关键匹配自动机
** 注  意:

******************************************************/
#ifndef  KEYWORD_MATCH_H 
#define  KEYOWRD_MATCH_H 
#define KN       256
#define STATE  int
#define F_TERMINATE 0x80000000
#define FIND_MATCHCASE 0x01
#define MAX_FIND_LEN 		 256				
#define DEFAULT_KEYWORD_NUM      200
#define ID_NOID              -2    
#define ID_SPECIAL1          -3
#define ID_SPECIAL2          -4
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
  
   //typdef int a[10] 相当于a是 int[10]
typedef STATE (*STATESLIST)[KN+1]; //相当于 STATESLIST 是指向二位数组的指针

typedef struct {
    unsigned char  *word;
    void * ptr;
    int    length;
    int    findnum;
    int    type;
    int    id;
}KEYWORD; 

typedef struct {
    STATESLIST states;
    KEYWORD    *wordlist;
    int       wordlistlength;
    int       wordsnum;
    STATE      wordslength;
    int       totalfind;
    STATE      usedstates;    
}MATCHENTRY;
 
typedef MATCHENTRY * pMATCHENTRY;
 MATCHENTRY * MatchEntryInit(int nSize);
 int Addkeyword(const char *word,int len, MATCHENTRY *entry, void *ptr);
 int Buildentry(int model, MATCHENTRY *entry);
 char *Match(const char *p,STATE state, MATCHENTRY *entry, char * findword);
 void RemoveEntry(int num, MATCHENTRY *entry);

#endif
