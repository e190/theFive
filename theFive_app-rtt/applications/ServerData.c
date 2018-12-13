#include <rtthread.h>
#include "ServerData.h"
#include <sys/socket.h>
#include "cJSON.h"
#include "cJSON_util.h"

#define LOG_TAG  "at.Server"
#define DBG_ENABLE
#define DBG_SECTION_NAME               "SERVER"
#define DBG_LEVEL                      DBG_INFO
#define DBG_COLOR
#include <rtdbg.h>

static rt_uint8_t thread_para = 0;

server_data_t send_server_data = {
								"vit-700",
								"0100",
								"201808001",
								"183.56.158.54",
								"sample"
								"0000"
								};

server_sample_t server_sample = {
								"小红",
								"25",
								"女",
								"511122199901011234",
								"13012345678",
								"0",
								"Hcy",
								"0011808080",
								"68.157",
								"201808081545"
};
static server_data_t rev_server_data;

static server_data_t* get_server_data(void)
{
	return &send_server_data;
}
static server_sample_t* get_server_sample(void)
{
	return &server_sample;
}
static server_data_t* get_server_rev_data(void)
{
	return &rev_server_data;
}

/* Parse text to JSON, then render back to text, and print! */
static rt_uint8_t AnalyseJSONFromServer(char *text)
{
	cJSON *root;

	root = cJSON_Parse(text);
	if (!root) {
		LOG_E("Error before: [%s]\n",cJSON_GetErrorPtr());
		return 1;
	}
	else
	{
		strcpy(rev_server_data.model, cJSON_item_get_string(root,"model"));
		strcpy(rev_server_data.version, cJSON_item_get_string(root,"version"));
		strcpy(rev_server_data.device_id, cJSON_item_get_string(root,"device_id"));
		strcpy(rev_server_data.error_code, cJSON_item_get_string(root,"error_code"));
		//strcpy(rev_server_data.source, cJSON_item_get_string(root,"source"));
		strcpy(rev_server_data.service, cJSON_item_get_string(root,"service"));
		if(rt_memcmp(rev_server_data.service, "time", 4) || memcmp(rev_server_data.service, "updata", 4))
			strcpy(rev_server_data.time, cJSON_item_get_string(root,"value"));
		cJSON_Delete(root);
	}
	return 0;
}
/**
 * @brief  处理接收服务器数据
 *
 * @param
 *
 * @return
 */
rt_uint8_t dump_rev_data(void)
{
	rt_uint8_t rev = RT_EOK;
	rt_size_t _bfsz = 0;
	char *recv_buf = RT_NULL;

	recv(0, &_bfsz, 4, 0);
	recv_buf = rt_calloc(1, _bfsz);
	recv(0, recv_buf, _bfsz, 0);
	LOG_I( "%s \n", recv_buf);
	rev = AnalyseJSONFromServer(&recv_buf[6]);


	rt_free(recv_buf);
	return rev;
}
/**
* @brief  模块连接到服务器
*        
* @param 
* 
* @retval 
*/
static rt_uint8_t ConnectServer(const UploadType type)
{
	char* tSend = NULL;
	int rev = RT_EOK;
	rt_uint16_t len = 0;
	cJSON * root;
	server_data_t* _send_data = get_server_data();
	char* type_str;

	switch (type)
	{
	case sample:
		type_str = "sample";
		break;
	case activate:
		type_str = "activate";
		break;
	case real_time:
		type_str = "time";
		break;
	case qc:
		type_str = "qc";
		break;
	case standard:
		type_str = "standard";
		break;
	case location:
		type_str = "location";
		break;
	case updata:
		type_str = "update";
		break;
	case test:
		type_str = "test";
		break;
	default:
		break;
	}

	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "model", _send_data->model);
	cJSON_AddStringToObject(root, "version", _send_data->version);
	cJSON_AddStringToObject(root, "device_id", _send_data->device_id);
	cJSON_AddStringToObject(root, "device_ip", _send_data->device_ip);
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", type_str);
	tSend = cJSON_PrintUnformatted(root);	
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++] = 0x0d;
	tSend[len++] = 0x0a;
	
	//LOG_I( "%s",tSend);
	rev = send(0, tSend, len, 0);

	cJSON_Delete(root);//删除
	rt_free(tSend);

	if(rev > 0)
	{
		rev = dump_rev_data();
		if(0 == rev)
		{
			LOG_I("%s", rev_server_data.error_code);
		}
	}

	return rev;
}
/**
 * @brief  上传检测数据
 *  
 * @param 
 *
 * @return
 */
static rt_uint8_t UploadSampleServer(void)
{
	char * tSend = RT_NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
	cJSON * root,* value;	
	server_data_t* _send_data = get_server_data();
	server_sample_t* _sample = get_server_sample();
	
	root = cJSON_CreateObject();//创建JSON Object 
	cJSON_AddStringToObject(root,"model", _send_data->model);//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", _send_data->version);
	cJSON_AddStringToObject(root,"device_id", _send_data->device_id);	
	cJSON_AddStringToObject(root,"device_ip", _send_data->device_ip);	
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "sample");
	cJSON_AddItemToObject(root, "value", value=cJSON_CreateObject());
	cJSON_AddStringToObject(value, "name", _sample->user_name);
	cJSON_AddStringToObject(value, "age", _sample->age);
	cJSON_AddStringToObject(value, "sex", _sample->sex);
	cJSON_AddStringToObject(value, "id", _sample->ID);
	cJSON_AddStringToObject(value, "phone_1", _sample->phone_1);//"1301345678"
	cJSON_AddStringToObject(value, "phone_2", _sample->phone_2);
	cJSON_AddStringToObject(value, "pro_name", _sample->pro_name);
	cJSON_AddStringToObject(value, "pro_id", _sample->pro_ID);//"123456987"
	cJSON_AddStringToObject(value, "concentration", _sample->concentration);
	cJSON_AddStringToObject(value, "time", _sample->time);
	tSend = cJSON_PrintUnformatted(root);
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = send(0, tSend, len, 0);
	//rt_kprintf( "%s \n",tSend);
		
	cJSON_Delete(root);
	rt_free(tSend);

	return rev;
} 
/**
 * @brief  上传定标数据
 *  
 * @param 
 *
 * @return
 */
static rt_uint8_t UploadStandardServer(void)
{
	char * tSend = RT_NULL;
	rt_uint8_t rev = RT_EOK;
	rt_uint16_t len;
	cJSON * root,* value;	
	
	root = cJSON_CreateObject();//创建JSON Object 
	cJSON_AddStringToObject(root,"model", "vit-700");//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", "0100");
	cJSON_AddStringToObject(root,"device_id", "123456789");	
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "standard");
	cJSON_AddItemToObject(root, "value", value=cJSON_CreateObject());
	cJSON_AddStringToObject(value, "pro_name", "Hcy");
	cJSON_AddStringToObject(value, "pro_id", "0011808080");
	cJSON_AddStringToObject(value, "cal_1", "5000,10.9");
	cJSON_AddStringToObject(value, "cal_2", "5000,10.9");
	cJSON_AddStringToObject(value, "cal_3", "5000,10.9");
	cJSON_AddStringToObject(value, "cal_4", "5000,10.9");
	cJSON_AddStringToObject(value, "time", "201808081545");
	tSend = cJSON_PrintUnformatted(root);
	len = rt_strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++] = '\r';
	tSend[len++] = '\n';
	
	rev = send(0, tSend, len, 0);
	//rt_kprintf( "%s \n",tSend);
		
	cJSON_Delete(root);
	rt_free(tSend);

	return rev;
} 
/**
 * @brief  上传QC数据
 *  
 * @param 
 *
 * @return
 */
static rt_uint8_t UploadQCServer(void)
{
	char * tSend = RT_NULL;
	rt_uint8_t rev = RT_EOK;
	rt_uint16_t len;
	cJSON * root,* value;	
	
	root = cJSON_CreateObject();//创建JSON Object 
	cJSON_AddStringToObject(root,"model", "vit-700");//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", "0100");
	cJSON_AddStringToObject(root,"device_id", "123456789");	
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "qc");
	cJSON_AddItemToObject(root, "value", value=cJSON_CreateObject());
	cJSON_AddStringToObject(value, "pro_name", "Hcy");
	cJSON_AddStringToObject(value, "pro_id", "0011808080");
	cJSON_AddStringToObject(value, "qc_1", "5000,10.9");
	cJSON_AddStringToObject(value, "qc_2", "5000,10.9");
	cJSON_AddStringToObject(value, "qc_3", "5000,10.9");
	cJSON_AddStringToObject(value, "qc_4", "5000,10.9");
	cJSON_AddStringToObject(value, "qc_5", "5000,10.9");
	cJSON_AddStringToObject(value, "qc_6", "5000,10.9");
	cJSON_AddStringToObject(value, "time", "201808081545");
	tSend = cJSON_PrintUnformatted(root);
	len = rt_strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = send(0, tSend, len, 0);
	//rt_kprintf( "%s \n",tSend);
		
	cJSON_Delete(root);
	rt_free(tSend);

	return rev;
}
/**
* @brief  模块从服务器端请求时间
*        
* @param 
* 
* @retval 
*/
static rt_uint8_t GetTimeForServer(void)
{
	char * tSend = NULL;
	rt_uint8_t rev = RT_EOK;
	rt_uint16_t len;
	cJSON * root;	
	server_data_t* _send_data = get_server_data();
	
	root = cJSON_CreateObject();//创建JSON Object 
	cJSON_AddStringToObject(root,"model", _send_data->model);//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", _send_data->version);
	cJSON_AddStringToObject(root,"device_id", _send_data->device_id);
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "time");
	
	tSend = cJSON_PrintUnformatted(root);//将cjson赋值给psend
	len = rt_strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++] = '\r';
	tSend[len++] = '\n';
	
	//LOG_I( "%s \n",tSend);
	rev = send(0, tSend, len, 0);

	cJSON_Delete(root);
	rt_free(tSend);
	if(rev > 0)
	{
		rev = dump_rev_data();
		if(0 == rev)
		{
			LOG_I("%s", rev_server_data.error_code);
			LOG_I("%s", rev_server_data.time);
		}
	}
	return rev;
}
/**
* @brief  设备发送位置给服务器
*        
* @param 
* 
* @retval 
*/
static rt_uint8_t SendPositionToServer(void)
{
	char * tSend = NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
	cJSON* root;
	cJSON* value;	
	server_data_t* _send_data = get_server_data();
	
	root = cJSON_CreateObject();		//创建JSON Object 
	cJSON_AddStringToObject(root,"model", _send_data->model);//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", _send_data->version);
	cJSON_AddStringToObject(root,"device_id", _send_data->device_id);
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "location");
	cJSON_AddItemToObject(root, "value", value=cJSON_CreateObject());
	cJSON_AddStringToObject(value, "net", "ctcc");
	cJSON_AddStringToObject(value, "lac", _send_data->LAC);
	cJSON_AddStringToObject(value, "cid", _send_data->CID);
	tSend = cJSON_PrintUnformatted(root);
	len = rt_strlen(tSend);
	tSend = rt_realloc(tSend, len+3); 
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = send(0, tSend, len, 0);
//	rt_kprintf( "%s \n",tSend);
	cJSON_Delete(root);
	rt_free(tSend);
	if(rev > 0)
	{
		rev = dump_rev_data();
	}
	return rev;
}
/**
* @brief  从服务器获取下载地址
*        
* @param 
* 
* @retval 
*/
static rt_uint8_t GetULRFromServer(void)
{
	char * tSend = NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
	cJSON* root;
	cJSON* value;	
	server_data_t* _send_data = get_server_data();
	
	root = cJSON_CreateObject();		//创建JSON Object 
	cJSON_AddStringToObject(root,"model", _send_data->model);//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", _send_data->version);
	cJSON_AddStringToObject(root,"device_id", _send_data->device_id);
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "update");
	cJSON_AddItemToObject(root, "value", value=cJSON_CreateObject());
	cJSON_AddStringToObject(value, "nowver", "103");
	tSend = cJSON_PrintUnformatted(root);
	len = rt_strlen(tSend);
	tSend = rt_realloc(tSend, len+3); 
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = send(0, tSend, len, 0);
//	rt_kprintf( "%s \n",tSend);
	cJSON_Delete(root);
	rt_free(tSend);

	return rev;
}
/**
 * @brief  设备上传样本数据
 *  
 * @param 
 *
 * @return
 */
static rt_uint8_t device_sample(void)
{
	int result = RT_EOK;
	 
//	rt_thread_delay(rt_tick_from_millisecond(100));
//	at_client_send("1", 1);
//	rt_thread_delay(rt_tick_from_millisecond(4000));

	result = ConnectServer(sample);
	if(0 == result)	
		return 4;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = SendPositionToServer();
	if(0 == result)	
		return 5;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = UploadSampleServer();
	if(0 == result)	
		return 6;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = UploadSampleServer();
	if(0 == result)	
		return 7;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = UploadSampleServer();
	if(0 == result)	
		return 7;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = GetTimeForServer();
	if(0 == result)	
		return 8;
	result = 0;
	
    return result;
}
/**
 * @brief  设备获取下载地址
 *  
 * @param 
 *
 * @return
 */
static rt_uint8_t device_updata(server_data_t* rev_data)
{
	int result = RT_EOK;
	
//	rt_thread_delay(rt_tick_from_millisecond(100));
//	at_client_send("1", 1);
//	rt_thread_delay(rt_tick_from_millisecond(4000));

	result = ConnectServer(updata);
	if(0 == result)	
		return 4;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = SendPositionToServer();
	if(0 == result)	
		return 5;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = GetULRFromServer();
	if(0 == result)	
		return 6;
	result = 0;
	
    return result;
}

static int ServerConnect(void)
{
#define IP_PORT     9999
	int result = RT_EOK;
	struct sockaddr socketAddr = {
			16,
			0,
			{IP_PORT>>8, IP_PORT & 0x00ff, 119, 23, 71, 241, 0}
	};
	socket(AF_AT, SOCK_STREAM, 0);
	result = connect(0, &socketAddr, sizeof(socketAddr));
	if(result != RT_EOK)
	{
		LOG_E("connect Server failed :%d.", result);
	}

	return result;
}
static int ServerClose(void)
{
	int result = RT_EOK;

	result = closesocket(0);
	if(result != RT_EOK)
	{
		LOG_E("Close Server failed :%d.", result);
	}

	return result;
}

static void server_thread_entry(void *parameter)
{
	int result = 0;
	UploadType task_num = *(UploadType*)parameter;

	result = ServerConnect();
	if (result != RT_EOK)
	{
		goto __exit;
	}

	result = ConnectServer(task_num);
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = SendPositionToServer();
	switch (task_num)
	{
	case sample:
		device_sample();
		break;
	case activate:

		break;
	case qc:
		UploadQCServer();
		break;
	case standard:
		UploadStandardServer();
		break;
	case updata:
		device_updata(&rev_server_data);
		break;
	case test:

		break;
	default:
		break;
	}
	result = GetTimeForServer();
//	rt_memset(string, 0, 30);
//	ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,30);
//	if(RT_EOK == _ret)
//	{
//
//		rt_sprintf(string, "服务器激活成功！");
//		ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
//	}
//	else
//	{
//		rt_sprintf(string, "服务器激活失败，错误代码：%d.",_ret);
//		ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
//	}
__exit:
	ServerClose();
}
/**
 * @brief  开始服务器任务
 *
 * @param _task：任务号
 * 			1：
 * 			2：
 *
 * @return
 */
int server_start(rt_uint8_t _task)
{
    rt_thread_t tid;

    thread_para = _task;
    tid = rt_thread_create("server",
							server_thread_entry,
							&thread_para,
							SERVER_THREAD_STACK_SIZE,
							SERVER_THREAD_PRIORITY,
							20);
    if (tid)
    {
    	LOG_I("addr: %x\n",tid);
        rt_thread_startup(tid);
    }
    else
    {
        LOG_E("Create server thread fail!");
    }

    return RT_EOK;
}


#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(ServerConnect, ServerConnect, AT network test);
MSH_CMD_EXPORT_ALIAS(ServerClose, ServerClose, AT network test);

#endif
