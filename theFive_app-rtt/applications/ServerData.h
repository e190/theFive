#ifndef __SERVERDATA_H__
#define __SERVERDATA_H__

#include <stdio.h>
#include <string.h>

#define SERVER_THREAD_STACK_SIZE          1024
#define SERVER_THREAD_PRIORITY            (RT_THREAD_PRIORITY_MAX/2)

typedef enum {
	sample = 1,
    activate,
	qc,
	standard,
	updata,
	test,
	real_time,
	location
}UploadType;

typedef struct 
{
	char model[10];
	char version[10];
	char device_id[10];
	char device_ip[15];
	char service[10];
	char error_code[5];
	char LAC[5];
	char CID[10];
	char time[10];
}server_data_t;

typedef struct 
{
	char user_name[10];
	char age[3];
	char sex[10];
	char ID[20];
	char phone_1[12];
	char phone_2[12];
	char pro_name[10];
	char pro_ID[11];
	char concentration[8];
	char time[13];
	
}server_sample_t;


int server_start(rt_uint8_t _task);

#endif
