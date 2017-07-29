#ifndef  UPDATE_H 
#define  UPDATE_H 


#include<pthread.h>
#include <stdio.h>
#define LMAXQSIZE 100



   //Ac
   
void *listen_up( void *arg );

void *upd(void *arg);
#endif


