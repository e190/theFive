#include <rtthread.h>
#include <at.h>
#include "bsp_G780.h"

#define G780_MODULE_SEND_MAX_SIZE       1460
#define G780_WAIT_CONNECT_TIME          5000
/* set real event by current socket and current state */
#define SET_EVENT(socket, event)       (((socket + 1) << 16) | (event))
/* AT socket event type */
#define G780_EVENT_CONN_OK              (1L << 0)
#define G780_EVENT_SEND_OK              (1L << 1)
#define G780_EVENT_RECV_OK              (1L << 2)
#define G780_EVNET_SET_OK             	(1L << 3)
#define G780_EVENT_CONN_FAIL            (1L << 4)
#define G780_EVENT_SEND_FAIL            (1L << 5)
static rt_event_t at_socket_event;
static rt_mutex_t at_event_lock;
typedef void (*at_evt_cb_t)(char *buff, size_t bfsz);
static at_evt_cb_t at_evt_cb_set = NULL;
extern void g780_rev_cb(char *buff, size_t bfsz);
char ttest = 0xf9;

static int g780_event_send(uint32_t event)
{
    return (int) rt_event_send(at_socket_event, event);
}

static int g780_event_recv(uint32_t event, uint32_t timeout, rt_uint8_t option)
{
    int result = 0;
    rt_uint32_t recved;

    result = rt_event_recv(at_socket_event, event, option | RT_EVENT_FLAG_CLEAR, timeout, &recved);
    if (result != RT_EOK)
    {
        return -RT_ETIMEOUT;
    }

    return recved;
}
/**
 * send data to server or client by AT commands.
 *
 * @param socket current socket
 * @param buff send buffer
 * @param bfsz send buffer size
 * @param type connect socket type(tcp, udp)
 *
 * @return >=0: the size of send success
 *          -1: send AT commands error or send data error
 *          -2: waited socket event timeout
 *          -5: no memory
 */
int g780_socket_send(const char *buff, size_t bfsz)
{
    int result = 0;
    size_t cur_pkt_size = 0, sent_size = 0;

    RT_ASSERT(buff);

    rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);

    while (sent_size < bfsz)
    {
        if (bfsz - sent_size < G780_MODULE_SEND_MAX_SIZE)
        {
            cur_pkt_size = bfsz - sent_size;
        }
        else
        {
            cur_pkt_size = G780_MODULE_SEND_MAX_SIZE;
        }

        /* send the real data to server or client */
        result = (int) at_client_send(buff + sent_size, cur_pkt_size);
        if (result == 0)
        {
            result = -RT_ERROR;
            goto __exit;
        }

        /* waiting OK or failed result */
        if (g780_event_recv(G780_EVENT_RECV_OK, rt_tick_from_millisecond(5 * 1000),
                RT_EVENT_FLAG_OR) < 0)
        {
            LOG_E("socket send failed, wait connect OK|FAIL timeout.");
            result = -RT_ETIMEOUT;
            goto __exit;
        }

        sent_size += cur_pkt_size;
    }


__exit:

    rt_mutex_release(at_event_lock);

    return result;  //返回的是已发送的数据的长度
}
/**
 * set AT socket event notice callback
 *
 * @param event notice event
 * @param cb notice callback
 */
static void g780_socket_set_event_cb(at_evt_cb_t cb)
{
       at_evt_cb_set = cb;    
}
static void urc_connect_func(const char *data, rt_size_t size)
{
    RT_ASSERT(data);
    if (strstr(data, "a"))
    {
        g780_event_send(G780_EVENT_CONN_OK);
    }
	else if (strstr(data, "+ERR:1"))
    {
        g780_event_send(G780_EVENT_CONN_FAIL);
    }
    LOG_I("URC data : %.*s", size, data);
}
static void urc_func(const char *data, rt_size_t size)
{
    RT_ASSERT(data && size);

    if (strstr(data, "+ok"))
    {
        g780_event_send(G780_EVENT_SEND_OK);
    }
    else if (strstr(data, "ERR"))
    {
        g780_event_send(G780_EVENT_SEND_FAIL);
    }
}
static void urc_recv_func(const char *data, rt_size_t size)
{
    rt_size_t bfsz = 0;
    char *recv_buf = RT_NULL;
    RT_ASSERT(data && size);

	extern at_client_t rt_at_get_client(void);
    at_client_t  client = rt_at_get_client();

	bfsz = size;
	recv_buf = rt_malloc(bfsz+1);
	if(RT_NULL == recv_buf)
		return ;
	rt_memset(recv_buf, 0, bfsz+1);
	rt_memcpy(recv_buf, client->recv_buffer, bfsz);
	g780_event_send(G780_EVENT_RECV_OK);

    /* notice the receive buffer and buffer size */
    if (at_evt_cb_set)    
        at_evt_cb_set(recv_buf, bfsz+1); 	
	
	if(recv_buf)
		rt_free(recv_buf);
}
static const struct at_urc urc_table[] = {
		{"+ok",        		"\r\n",     urc_func},
        {"a",        		"",         urc_connect_func},
		{"+ERR:1",        	"\r\n",     urc_connect_func},
		//{"SOCKA",        	&ttest,     urc_connect_func},
		{"{\"model\"",      "\r\n",     urc_recv_func},
		{"SOCKA",      		"\r\n",     urc_recv_func},
};
/* AT client port initialization */
int at_client_port_init(void)
{
    /* create current AT socket event */
    at_socket_event = rt_event_create("at_sock_event", RT_IPC_FLAG_FIFO);
    if (!at_socket_event)
    {
        LOG_E("AT client port initialize failed! at_sock_event create failed!");
        return -RT_ENOMEM;
    }

    /* create current AT socket lock */
    at_event_lock = rt_mutex_create("at_event_lock", RT_IPC_FLAG_FIFO);
    if (!at_event_lock)
    {
        LOG_E("AT client port initialize failed! at_sock_lock create failed!");
        rt_event_delete(at_socket_event);
        return -RT_ENOMEM;
    }

    /* register URC data execution function  */
    at_set_urc_table(urc_table, sizeof(urc_table) / sizeof(urc_table[0]));

    return RT_EOK;
}
#define AT_SEND_CMD(resp, resp_line, timeout, cmd)                                                              \
    do                                                                                                          \
    {                                                                                                           \
        if (at_exec_cmd(at_resp_set_info(resp, 64, resp_line, rt_tick_from_millisecond(timeout)), cmd) < 0)     \
        {                                                                                                       \
            result = -RT_ERROR;                                                                                 \
			goto __exit;																						\
        }                                                                                                       \
    } while(0);  																								\
	
/**
 * @brief  模块设置网络连接
 *  
 * @param 	_ip:  IP地址
 *		  _port:  端口号
 * @return
 */	
rt_err_t g780_set_NetConnet(const char* _ip, const char* _port)
{
    at_response_t resp = RT_NULL;
    int result = 0;
	char* temp_str = RT_NULL; 
	rt_uint8_t net_len = 0;
	
	RT_ASSERT(_ip);
	RT_ASSERT(_port);
	net_len = rt_strlen(_ip) + rt_strlen(_port) + 14;
	temp_str = rt_malloc(net_len);
	if(RT_NULL == temp_str)
		return RT_ERROR;
	rt_sprintf(temp_str, "AT+SOCKA=TCP,%s,%s", _ip, _port);
	rt_kprintf( "%s \n",temp_str);
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(500));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        return RT_ERROR;
    }
    at_exec_cmd(resp, "AT+WKMOD=NET");
   // at_exec_cmd(resp, "AT+SOCKA=TCP,120.77.182.220,9999");
	at_exec_cmd(resp, temp_str);
	at_exec_cmd(resp, "AT+SOCKAEN=ON");

	result = at_exec_cmd(resp, "AT+SOCKASL=SHORT");
    if (result != RT_EOK)
    {
        LOG_E("AT client send commands failed or return response error!");
        goto __exit;
    }
	result = at_exec_cmd(resp, "AT+SOCKALK");
    if (result != RT_EOK)
    {
        LOG_E("AT client send commands failed or return response error!");
        goto __exit;
    }
	at_exec_cmd(resp, "AT+Z"); //重启模块
__exit:
    if(resp)
    {
        at_delete_resp(resp);
    }
	if(temp_str)
		rt_free(temp_str);
	
    return result;

}
/**
 * @brief  获取本地IP地址
 *  
 * @param 
 *
 * @return
 */
rt_err_t g780_Get_LocIP(char* _ip)
{
	at_response_t resp = RT_NULL;
    int result = 0;

    resp = at_create_resp(64, 3, rt_tick_from_millisecond(500));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        return -2;
    }
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
	result = at_exec_cmd(resp, "AT+LOCIP");
	if (result != RT_EOK)
    {
        LOG_E("G780 get IP timeout!");
        goto __exit;
    }
	/* Print response line buffer */
    {
        const char *line_buffer = RT_NULL;
		rt_uint8_t len = 0;

        if((line_buffer = at_resp_get_line(resp, 2)) != RT_NULL)
		{
			len = rt_strlen(line_buffer);
			if (strstr(line_buffer, "+ERR") || len < 15)
			{
				LOG_E("G780 get IP failed or return response error!");
				result = -RT_ERROR; 
				goto __exit;
			}			
			rt_memset(_ip, 0, 15);
			memcpy(_ip, &line_buffer[7], len-8);
			
			LOG_D("line %d buffer : %s", 2, line_buffer);
		}
		else
		{
			LOG_E("Parse line buffer error!");
		} 
    }
__exit:
    if(resp)
    {
        at_delete_resp(resp);
    }
	rt_mutex_release(at_event_lock);
	
    return result;
}
rt_err_t g780_Get_BaseStation(char* lac, char* cid)
{
	at_response_t resp = RT_NULL;
    int result = 0;

    resp = at_create_resp(64, 2, rt_tick_from_millisecond(500));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        return -2;
    }
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
	result = at_exec_cmd(resp, "AT+LBS");
	if (result != RT_EOK)
    {
        LOG_E("G780 get LBS timeout!");
        goto __exit;
    }
	/* Print response line buffer */
    {
        const char *line_buffer = RT_NULL;
		rt_uint8_t len = 0;
		
		if((line_buffer = at_resp_get_line(resp, 2)) != RT_NULL)
		{
			len = rt_strlen(line_buffer);
			if (strstr(line_buffer, "+ERR") || len < 15)
			{
				LOG_E("G780 get LBS failed or return response error!");
				result = -RT_ERROR; 
				goto __exit;
			}
			memcpy(lac, &line_buffer[6], 4);
			memcpy(cid, &line_buffer[13], 8);
			LOG_D("line %d buffer : %s", 2, line_buffer);
		}
		else
		{
			LOG_E("Parse line buffer error!");
		}   
    }
__exit:
    if(resp)
    {
        at_delete_resp(resp);
    }
	rt_mutex_release(at_event_lock);
	
    return result;
}
/**
 * @brief  进入配置模式
 *  
 * @param 
 *
 * @return
 */
rt_err_t g780_enter_config(void)
{
	int result = RT_EOK, event_result = 0;;
	
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
	/* send the real data to server or client */
	result = (int) at_client_send("+++", 3);
	if (result == 0)
	{
		result = -RT_ERROR;
		goto __exit;
	}

	/* waiting result event from AT URC */
	event_result = g780_event_recv(G780_EVENT_CONN_OK | G780_EVENT_CONN_FAIL, 
										rt_tick_from_millisecond(600), RT_EVENT_FLAG_OR);
	if (G780_EVENT_CONN_FAIL == event_result || event_result < 0)
	{
		LOG_E("set failed, wait connect result timeout %d.",event_result);
		result = -RT_ETIMEOUT;
		goto __exit;
	}
	
	result = (int) at_client_send("a", 1);
	if (result == 0)
	{
		result = -RT_ERROR;
		goto __exit;
	}
	if (g780_event_recv(G780_EVENT_SEND_OK, rt_tick_from_millisecond(300), RT_EVENT_FLAG_OR) < 0)
	{
		LOG_E("set failed.");
		result = -RT_ERROR;
		goto __exit;
	}
	__exit:
    rt_mutex_release(at_event_lock);

    return result;
}
/**
 * @brief  进入通讯模式
 *  
 * @param 
 *
 * @return
 */
int g780_enter_communicate(void)
{	
	at_response_t resp = RT_NULL;
	int result = 0;
	
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
    resp = at_create_resp(16, 0, rt_tick_from_millisecond(300));
	if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -RT_ENOMEM;
    }
	result = at_exec_cmd(resp, "AT+ENTM");
	if (result != RT_EOK)
    {
        LOG_E("G780 enter communicate error!");
    }
	rt_mutex_release(at_event_lock);
	
	if (resp)
    {
        at_delete_resp(resp);
    }
	return result;
}	
/**
 * @brief  G780模块初始化
 *  
 * @param 
 *
 * @return
 */
void g780_net_init(void)
{
    at_response_t resp = RT_NULL;
	rt_err_t result = RT_EOK;
    resp = at_create_resp(64, 0, rt_tick_from_millisecond(300));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        result = -RT_ENOMEM;
        goto __exit;
    }
    LOG_D("Start initializing the G780 module");

	//进入配置模式
	if(-1 == g780_enter_config())
	{
		result = -RT_ERROR;
		goto __exit;
	}
	rt_thread_delay(rt_tick_from_millisecond(500));
	/* disable echo */
    AT_SEND_CMD(resp, 0, 300, "AT+E=OFF");
	/* wait M26 startup finish */
	if (at_client_wait_connect(5000))
	{
		result = -RT_ETIMEOUT;
		goto __exit;
	}	
	

	rt_thread_delay(rt_tick_from_millisecond(100));
    /* get module version */
    AT_SEND_CMD(resp, 2, 300, "AT+STMSG");
    /* Print response line buffer */
    {
        const char *line_buffer = RT_NULL;

        LOG_D("Response buffer");

        for(rt_size_t line_num = 1; line_num <= resp->line_counts; line_num++)
        {
            if((line_buffer = at_resp_get_line(resp, line_num)) != RT_NULL)
            {
                LOG_D("line %d buffer : %s", line_num, line_buffer);
            }
            else
            {
                LOG_E("Parse line buffer error!");
            }
        }
    }
	g780_socket_set_event_cb(g780_rev_cb);
__exit:	
	if (resp)
    {
        at_delete_resp(resp);
    }
    if (!result)
    {
        LOG_I("AT network initialize success!");
    }
    else
    {
        LOG_E("AT network initialize failed (%d)!", result);
    }
}
int g780_getIPlbs(void)
{

return 0;
}
int g780_test(void)
{
	at_response_t resp = RT_NULL;
	rt_err_t result = RT_EOK;
	
	resp = at_create_resp(64, 0, rt_tick_from_millisecond(3000));
    if (!resp)
    {
        LOG_E("No memory for response structure!");
        return -RT_ENOMEM;
    }
	
    //AT_SEND_CMD(resp, 2, 3000, "AT+STMSG");
	//AT_SEND_CMD(resp, 0, 300, "AT+SYSINFO");
	//AT_SEND_CMD(resp, 3, 300, "AT+LOCIP");
	AT_SEND_CMD(resp, 2, 300, "AT+LBS");
	//AT_SEND_CMD(resp, 0, 300, "AT+E=OFF");
    /* Print response line buffer */
    {
        const char *line_buffer = RT_NULL;

        LOG_D("Response buffer");

        for(rt_size_t line_num = 1; line_num <= resp->line_counts; line_num++)
        {
            if((line_buffer = at_resp_get_line(resp, line_num)) != RT_NULL)
            {
                LOG_D("line %d buffer : %s", line_num, line_buffer);
            }
            else
            {
                LOG_E("Parse line buffer error!");
            }
        }
    }
__exit:	
	if (resp)
    {
        at_delete_resp(resp);
    }

	return result;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(g780_net_init, at_net_init, initialize AT network);
MSH_CMD_EXPORT_ALIAS(g780_test, at_net_test, AT network test);
MSH_CMD_EXPORT_ALIAS(g780_enter_communicate, at_net_com, AT network com);
//MSH_CMD_EXPORT_ALIAS(m26_ping, at_ping, AT ping network host);
#endif
