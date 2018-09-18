#ifndef __SERVERDATA_H__
#define __SERVERDATA_H__

#include <stdio.h>
#include <string.h>

#define SERVER_SAMPLE  			"sample"
#define SERVER_ACTIVATE  		"activate"
#define SERVER_TEST  			"test"
#define SERVER_TIME   			"time"
#define SERVER_QC   			"qc"
#define SERVER_STANDARD   		"standard"
#define SERVER_LOCATION         "location"
#define SERVER_UPDATA        	"update"

typedef struct 
{
	char model[10];
	char version[10];
	char device_id[10];
	char device_ip[15];
	char service[10];
	char error_code[4];
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

extern server_data_t send_server_data;
extern server_data_t rev_server_data;
extern server_sample_t server_sample;

rt_uint8_t ConnectServer(server_data_t* _send_data, const char* type);
rt_uint8_t UploadSampleServer(server_data_t* _send_data, server_sample_t* _sample);	
rt_uint8_t GetTimeForServer(server_data_t* _send_data);
rt_uint8_t SendPositionToServer(server_data_t* _send_data);
rt_uint8_t AnalyseJSONFromServer(char *text);
rt_uint8_t device_activate(server_data_t* send_data, server_data_t* rev_data);
rt_uint8_t device_test(server_data_t* send_data, server_data_t* rev_data);
rt_uint8_t device_sample(server_data_t* send_data, server_sample_t* sample);
rt_uint8_t device_updata(server_data_t* send_data, server_data_t* rev_data);
#endif
