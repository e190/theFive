#include <rtthread.h>
#include <at.h>
#include "cJSON.h"
#include "cJSON_util.h"
#include "ServerData.h"
#include "bsp_G780.h"


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
server_data_t rev_server_data;
/**
* @brief  模块连接到服务器
*        
* @param 
* 
* @retval 
*/
rt_uint8_t ConnectServer(server_data_t* _send_data, const char* type)							
{
	char* tSend = NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len = 0;
	cJSON * root;

	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root,"model", _send_data->model);
	cJSON_AddStringToObject(root,"version", _send_data->version);
	cJSON_AddStringToObject(root,"device_id", _send_data->device_id);
	cJSON_AddStringToObject(root,"device_ip", _send_data->device_ip);
	cJSON_AddStringToObject(root,"source","device");
	cJSON_AddStringToObject(root,"service",type);
	tSend = cJSON_PrintUnformatted(root);	
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++] = 0x0d;
	tSend[len++] = 0x0a;
	
	rev = g780_socket_send(tSend, len);
	//at_client_send(tSend, len);
	//rt_kprintf( "%s",tSend);
	
	cJSON_Delete(root);//删除
	rt_free(tSend);//释放指针

	return rev;
}
/**
 * @brief  上传检测数据
 *  
 * @param 
 *
 * @return
 */
rt_uint8_t UploadSampleServer(server_data_t* _send_data, server_sample_t* _sample)				
{
	char * tSend = RT_NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
	cJSON * root,* value;	
	
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
	
	rev = g780_socket_send(tSend, len);
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
rt_uint8_t UploadStandardServer(void)				
{
	char * tSend = RT_NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
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
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = g780_socket_send(tSend, len);
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
rt_uint8_t UploadQCServer(void)				
{
	char * tSend = RT_NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
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
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = g780_socket_send(tSend, len);
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
rt_uint8_t GetTimeForServer(server_data_t* _send_data)				
{
	char * tSend = NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
	cJSON * root;	
	
	root = cJSON_CreateObject();//创建JSON Object 
	cJSON_AddStringToObject(root,"model", _send_data->model);//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", _send_data->version);
	cJSON_AddStringToObject(root,"device_id", _send_data->device_id);
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "time");
	
	tSend = cJSON_PrintUnformatted(root);//将cjson赋值给psend
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3);
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = g780_socket_send(tSend, len);
	//rt_kprintf( "%s \n",tSend);
	cJSON_Delete(root);
	rt_free(tSend);

	return rev;
}
/**
* @brief  设备发送位置给服务器
*        
* @param 
* 
* @retval 
*/
rt_uint8_t SendPositionToServer(server_data_t* _send_data)				
{
	char * tSend = NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
	cJSON* root;
	cJSON* value;	
	
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
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3); 
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = g780_socket_send(tSend, len);
//	rt_kprintf( "%s \n",tSend);
	cJSON_Delete(root);
	rt_free(tSend);

	return rev;
}
/**
* @brief  从服务器获取下载地址
*        
* @param 
* 
* @retval 
*/
rt_uint8_t GetULRFromServer(server_data_t* _send_data)				
{
	char * tSend = NULL;
	rt_uint8_t rev = RT_EOK;
	uint16_t len;
	cJSON* root;
	cJSON* value;	
	
	root = cJSON_CreateObject();		//创建JSON Object 
	cJSON_AddStringToObject(root,"model", _send_data->model);//加入节点，节点名称model,节点值为ibm
	cJSON_AddStringToObject(root,"version", _send_data->version);
	cJSON_AddStringToObject(root,"device_id", _send_data->device_id);
	cJSON_AddStringToObject(root, "source", "device");
	cJSON_AddStringToObject(root, "service", "update");
	cJSON_AddItemToObject(root, "value", value=cJSON_CreateObject());
	cJSON_AddStringToObject(value, "nowver", "103");
	tSend = cJSON_PrintUnformatted(root);
	len = strlen(tSend);
	tSend = rt_realloc(tSend, len+3); 
	tSend[len++]='\r';
	tSend[len++]='\n';
	
	rev = g780_socket_send(tSend, len);
//	rt_kprintf( "%s \n",tSend);
	cJSON_Delete(root);
	rt_free(tSend);

	return rev;
}
/* Parse text to JSON, then render back to text, and print! */
rt_uint8_t AnalyseJSONFromServer(char *text)
{
	cJSON *root;
	
	root = cJSON_Parse(text);
	if (!root) {
		rt_kprintf("Error before: [%s]\n",cJSON_GetErrorPtr());
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
			if(memcmp(rev_server_data.service, "time", 4) || memcmp(rev_server_data.service, "updata", 4))
				strcpy(rev_server_data.time, cJSON_item_get_string(root,"value"));
			cJSON_Delete(root);  
	}  
	return 0;  	
}
/**
 * @brief  设备激活操作
 *  
 * @param 
 *
 * @return
 */
rt_uint8_t device_activate(server_data_t* send_data, server_data_t* rev_data)
{
	int result = RT_EOK;
	
	g780_enter_config();
	result = g780_Get_LocIP(send_data->device_ip);
	if(RT_EOK != result)	
		return 1;
			
	result = g780_Get_BaseStation(send_data->LAC, send_data->CID);
	if(RT_EOK != result)	
		return 2;
	result = g780_enter_communicate();
	if(RT_EOK != result)	
		return 3;
	 
	rt_thread_delay(rt_tick_from_millisecond(100));
	at_client_send("1", 1);
	rt_thread_delay(rt_tick_from_millisecond(4000));

	result = ConnectServer(send_data, SERVER_ACTIVATE);
	if(0 == result)	
		return 4;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = SendPositionToServer(send_data);
	if(0 == result)	
		return 5;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = GetTimeForServer(send_data);
	if(0 == result)	
		return 6;
	result = 0;
	
    return result;

}
/**
 * @brief  设备测试操作
 *  
 * @param 
 *
 * @return
 */
rt_uint8_t device_test(server_data_t* send_data, server_data_t* rev_data)
{
	int result = RT_EOK;
	
	g780_enter_config();
	result = g780_Get_LocIP(send_data->device_ip);
	if(RT_EOK != result)	
		return 1;
			
	result = g780_Get_BaseStation(send_data->LAC, send_data->CID);
	if(RT_EOK != result)	
		return 2;
	result = g780_enter_communicate();
	if(RT_EOK != result)	
		return 3;
	 
	rt_thread_delay(rt_tick_from_millisecond(100));
	at_client_send("1", 1);
	rt_thread_delay(rt_tick_from_millisecond(4000));

	result = ConnectServer(send_data, SERVER_TEST);
	if(0 == result)	
		return 4;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = SendPositionToServer(send_data);
	if(0 == result)	
		return 5;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = GetTimeForServer(send_data);
	if(0 == result)	
		return 6;
	result = 0;
	
    return result;

}
/**
 * @brief  设备上传样本数据
 *  
 * @param 
 *
 * @return
 */
rt_uint8_t device_sample(server_data_t* send_data, server_sample_t* sample)
{
	int result = RT_EOK;
	
	g780_enter_config();
	result = g780_Get_LocIP(send_data->device_ip);
	if(RT_EOK != result)	
		return 1;
			
	result = g780_Get_BaseStation(send_data->LAC, send_data->CID);
	if(RT_EOK != result)	
		return 2;
	result = g780_enter_communicate();
	if(RT_EOK != result)	
		return 3;
	 
	rt_thread_delay(rt_tick_from_millisecond(100));
	at_client_send("1", 1);
	rt_thread_delay(rt_tick_from_millisecond(4000));

	result = ConnectServer(send_data, SERVER_SAMPLE);
	if(0 == result)	
		return 4;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = SendPositionToServer(send_data);
	if(0 == result)	
		return 5;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = UploadSampleServer(send_data, sample);
	if(0 == result)	
		return 6;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = UploadSampleServer(send_data, sample);
	if(0 == result)	
		return 7;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = UploadSampleServer(send_data, sample);
	if(0 == result)	
		return 7;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = GetTimeForServer(send_data);
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
rt_uint8_t device_updata(server_data_t* send_data, server_data_t* rev_data)
{
	int result = RT_EOK;
	
	g780_enter_config();
	result = g780_Get_LocIP(send_data->device_ip);
	if(RT_EOK != result)	
		return 1;
			
	result = g780_Get_BaseStation(send_data->LAC, send_data->CID);
	if(RT_EOK != result)	
		return 2;
	result = g780_enter_communicate();
	if(RT_EOK != result)	
		return 3;
	 
	rt_thread_delay(rt_tick_from_millisecond(100));
	at_client_send("1", 1);
	rt_thread_delay(rt_tick_from_millisecond(4000));

	result = ConnectServer(send_data, SERVER_UPDATA);
	if(0 == result)	
		return 4;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = SendPositionToServer(send_data);
	if(0 == result)	
		return 5;
	rt_thread_delay(rt_tick_from_millisecond(100));
	result = GetULRFromServer(send_data);
	if(0 == result)	
		return 6;
	result = 0;
	
    return result;
}

/**
 * @brief  接收服务器数据回调函数
 *  
 * @param 
 *
 * @return
 */
void g780_rev_cb(char *buff, size_t bfsz)
{
	rt_kprintf( "%s \n",buff);
	AnalyseJSONFromServer(&buff[6]);
}
