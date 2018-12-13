#include <rtthread.h>
#include <sys/socket.h>
#include <at.h>
#include <at_socket.h>
#include "bsp_G780.h"

#define LOG_TAG  "at.G780"
#include <at_log.h>

struct g780_Info_t g780_Info;
static int cur_socket;
static rt_event_t at_socket_event;
static rt_mutex_t at_event_lock;

static at_evt_cb_t at_evt_cb_set[] = {
        [AT_SOCKET_EVT_RECV] = NULL,
        [AT_SOCKET_EVT_CLOSED] = NULL,
};

//har ttest = 0xf9;

static int at_socket_event_send(uint32_t event)
{
    return (int) rt_event_send(at_socket_event, event);
}

static int at_socket_event_recv(uint32_t event, uint32_t timeout, rt_uint8_t option)
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
static modeStatus get_g780_mode(void)
{
	return g780_Info.mode;
}
/**
 * @brief  进入配置模式
 *
 * @param
 *
 * @return
 */
static rt_err_t g780_enter_config(void)
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
	event_result = at_socket_event_recv(G780_EVENT_SW_OK, rt_tick_from_millisecond(1000), RT_EVENT_FLAG_OR);
	if (G780_EVENT_SET_ERROR == event_result || event_result < 0)
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
	if (at_socket_event_recv(G780_EVENT_SW_OK, rt_tick_from_millisecond(1000), RT_EVENT_FLAG_OR) < 0)
	{
		LOG_E("set failed.");
		result = -RT_ERROR;
		goto __exit;
	}
__exit:
    rt_mutex_release(at_event_lock);
    g780_Info.mode = config;

    return result;
}
/**
 * @brief  进入通讯模式
 *
 * @param
 *
 * @return
 */
static int g780_enter_communicate(void)
{
	int result = 0, event_result = 0;;
	at_response_t resp = RT_NULL;

	resp = at_create_resp(8, 2, rt_tick_from_millisecond(2 * 1000));
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);

	result = at_exec_cmd(resp, "AT+ENTM");
	if (result != RT_EOK)
    {
        LOG_E("G780 enter communicate error!");
        goto __exit;
    }
	g780_Info.mode = communicate;
__exit:
	rt_mutex_release(at_event_lock);

	return result;
}

/**
 * close socket by AT commands.
 *
 * @param current socket
 *
 * @return  0: close socket success
 *         -1: send AT commands error
 *         -2: wait socket event timeout
 *         -5: no memory
 */
static int g780_socket_close(int socket)
{
#define LK_RETRY   5
   int result = 0, event_result, i;
	at_response_t resp = RT_NULL;

	resp = at_create_resp(64, 2, rt_tick_from_millisecond(2 * 1000));
	cur_socket = socket;

	if(get_g780_mode() != config)
		g780_enter_config();
	result = at_exec_cmd(resp, "AT+SOCK%cEN=OFF", socket+65);
	if(result == -RT_ERROR)
	{
		goto __exit;
	}

    for (i = 0; i < LK_RETRY; i++)
    {
    	result = at_exec_cmd(resp, "AT+SOCK%cLK",socket + 65);
    	if(result == -RT_ERROR)
    	{
    		goto __exit;
    	}
	    if (at_socket_event_recv(SET_EVENT(socket, 0), rt_tick_from_millisecond(1000), RT_EVENT_FLAG_OR) < 0)
		{
			LOG_E("socket (%d) close failed, wait connect result timeout.", socket);
			result = -RT_ETIMEOUT;
			goto __exit;
		}
    	event_result = at_socket_event_recv(G780_EVENT_CONN_OK | G780_EVENT_CONN_CLOSE,
    										rt_tick_from_millisecond(300*3), RT_EVENT_FLAG_OR);

    	if(event_result == G780_EVENT_CONN_CLOSE)
    	{
    		break;
    	}
        rt_thread_delay(rt_tick_from_millisecond(1000));
    }
    if (i == LK_RETRY)
    {
        LOG_E("socket close failed");
        result = -RT_ERROR;
        goto __exit;
    }

__exit:
	if (resp)
	{
		at_delete_resp(resp);
	}

	return result;
}
/**
 * create TCP/UDP client or server connect by AT commands.
 *
 * @param socket current socket
 * @param ip server or client IP address
 * @param port server or client port
 * @param type connect socket type(tcp, udp)
 * @param is_client connection is client
 *
 * @return   0: connect success
 *          -1: connect failed, send commands error or type error
 *          -2: wait socket event timeout
 *          -5: no memory
 */
static int g780_socket_connect(int socket, char *ip, int32_t port, enum at_socket_type type, rt_bool_t is_client)
{
#define LK_RETRY   5
    int result = 0, event_result = 0, i;
	rt_bool_t retryed = RT_FALSE;

	RT_ASSERT(ip);
	RT_ASSERT(port >= 0);
	at_response_t resp = RT_NULL;

	resp = at_create_resp(64, 2, rt_tick_from_millisecond(2 * 1000));
	/* lock AT socket connect */
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
	if(get_g780_mode() != config)
		g780_enter_config();
__retry:
	if (is_client)
	{
		switch (type)
		{
		case AT_SOCKET_TCP:
			result = at_exec_cmd(resp, "AT+SOCK%c=TCP,%s,%d", socket + 65, ip, port);
			break;

		case AT_SOCKET_UDP:
			result = at_exec_cmd(resp, "AT+SOCK%c=UDP,%s,%d", socket + 65, ip, port);
			break;

		default:
			LOG_E("Not supported connect type : %d.", type);
			return -RT_ERROR;
		}
	}
	if(result == -RT_ERROR)
	{
		if (!retryed)
		{
			LOG_E("socket (%d) connect failed, maybe the socket was not be closed at the last time and now will retry.", socket);
			if (g780_socket_close(socket) < 0)
			{
				goto __exit;
			}
			retryed = RT_TRUE;
			goto __retry;
		}
		LOG_E("socket (%d) connect failed, failed to establish a connection.", socket);
		result = -RT_ERROR;
		goto __exit;
	}

	result = at_exec_cmd(resp, "AT+SOCK%cSL=LONG",socket + 65);
	if (result != RT_EOK)
	{
		LOG_E("AT client send commands failed or return response error!");
		goto __exit;
	}
	result = at_exec_cmd(resp, "AT+SOCK%cEN=ON",socket + 65);
	if (result != RT_EOK)
	{
		LOG_E("AT client send commands failed or return response error!");
		goto __exit;
	}
	rt_thread_delay(rt_tick_from_millisecond(500));
    for (i = 0; i < LK_RETRY; i++)
    {
    	result = at_exec_cmd(resp, "AT+SOCK%cLK",socket + 65);
    	if(result == -RT_ERROR)
    	{
    		goto __exit;
    	}
	    if (at_socket_event_recv(SET_EVENT(socket, 0), rt_tick_from_millisecond(2 * 1000), RT_EVENT_FLAG_OR) < 0)
		{
			LOG_E("socket (%d) connect failed, wait connect result timeout.", socket);
			result = -RT_ETIMEOUT;
			goto __exit;
		}
    	event_result = at_socket_event_recv(G780_EVENT_CONN_OK | G780_EVENT_CONN_CLOSE,
    										rt_tick_from_millisecond(300*3), RT_EVENT_FLAG_OR);
    	if(event_result == G780_EVENT_CONN_OK)
    	{
    		break;
    	}
        rt_thread_delay(rt_tick_from_millisecond(1000));
    }
    if (i == LK_RETRY)
    {
        LOG_E("socket connect failed");
        result = -RT_ERROR;
        goto __exit;
    }

	g780_enter_communicate();
	//at_exec_cmd(RT_NULL, "AT+Z"); //重启模块
__exit:
	if (resp)
	{
		at_delete_resp(resp);
	}
	/* unlock AT socket connect */
	rt_mutex_release(at_event_lock);

	return result;
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
static int g780_socket_send(int socket, const char *buff, size_t bfsz, enum at_socket_type type)
{
    int result = 0;
    size_t cur_pkt_size = 0, sent_size = 0;

    RT_ASSERT(buff);

	if(get_g780_mode() != communicate)
		g780_enter_communicate();
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
        if (at_socket_event_recv(G780_EVENT_RECV_OK, rt_tick_from_millisecond(5 * 1000),
                RT_EVENT_FLAG_OR) < 0)
        {
            LOG_E("socket send failed, wait receive timeout.");
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
static void g780_socket_set_event_cb(at_socket_evt_t event, at_evt_cb_t cb)
{
    if (event < sizeof(at_evt_cb_set) / sizeof(at_evt_cb_set[1]))
    {
        at_evt_cb_set[event] = cb;
    }
}
/**
 * domain resolve by AT commands.
 *
 * @param name domain name
 * @param ip parsed IP address, it's length must be 16
 *
 * @return  0: domain resolve success
 *         -1: send AT commands error or response error
 *         -2: wait socket event timeout
 *         -5: no memory
 */
static int g780_domain_resolve(const char *name, char ip[16])
{
	rt_strncpy(ip, name, 15);
	ip[15] = '\0';

    return RT_EOK;
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

    resp = at_create_resp(64, 2, rt_tick_from_millisecond(500));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        return -2;
    }
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
	result = at_exec_cmd(resp, "AT+LOCIP");
	if (result  == -RT_ERROR)
    {
        LOG_E("G780 get IP Error!");
        goto __exit;
    }
	/* Print response line buffer */
    {
        const char *line_buffer = RT_NULL;
		rt_uint8_t len = 0;

        if((line_buffer = at_resp_get_line(resp, 2)) != RT_NULL)
		{
			len = rt_strlen(line_buffer);
			rt_memset(_ip, 0, 15);
			//rt_memcpy(_ip, &line_buffer[7], len-8);

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

    resp = at_create_resp(128, 2, rt_tick_from_millisecond(500));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        return -2;
    }
	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
	result = at_exec_cmd(resp, "AT+LBS");
	if (result == -RT_ERROR)
    {
        LOG_E("G780 get LBS Error!");
        goto __exit;
    }
	/* Print response line buffer */
    {
        const char *line_buffer = RT_NULL;
		rt_uint8_t len = 0;

		if((line_buffer = at_resp_get_line(resp, 2)) != RT_NULL)
		{
			len = rt_strlen(line_buffer);
			rt_memcpy(lac, &line_buffer[6], 4);
			rt_memcpy(cid, &line_buffer[13], 8);
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
static void urc_switchover_func(const char *data, rt_size_t size)
{
    RT_ASSERT(data);

    if (rt_strstr(data, "a") || rt_strstr(data, "+ok"))
    {
    	at_socket_event_send(G780_EVENT_SW_OK);
    }

    LOG_I("URC data : %.*s", size, data);
}
static void urc_connect_func(const char *data, rt_size_t size)
{
    int socket = 0;
    RT_ASSERT(data);

    if (rt_strstr(data, "OFF"))
    {
    	at_socket_event_send(SET_EVENT(socket, G780_EVENT_CONN_CLOSE));
	    /* notice the socket is disconnect by remote */
	    if (at_evt_cb_set[AT_SOCKET_EVT_CLOSED])
	    {
	        at_evt_cb_set[AT_SOCKET_EVT_CLOSED](socket, AT_SOCKET_EVT_CLOSED, NULL, 0);
	    }
    }
	else if (rt_strstr(data, "ON"))
    {
		at_socket_event_send(SET_EVENT(socket, G780_EVENT_CONN_OK));
    }
    LOG_I("URC data : %.*s", size, data);
}

static void urc_func(const char *data, rt_size_t size)
{
    RT_ASSERT(data && size);

    RT_ASSERT(data);

    LOG_I("URC data : %.*s", size, data);

}
static void urc_recv_func(const char *data, rt_size_t size)
{
    int socket = 0;
    rt_size_t bfsz = 0;
    char *recv_buf = RT_NULL;

    RT_ASSERT(data && size);

    if (socket < 0)
        return;
    bfsz = size;
    LOG_I("receive size(%d) data!", bfsz);
    recv_buf = rt_calloc(1, bfsz + 4);
    if (!recv_buf)
    {
        LOG_E("no memory for URC receive buffer (%d)!", bfsz);
        return;
    }
    rt_memcpy(recv_buf, &bfsz, 4);
    rt_memcpy(recv_buf + 4, data, bfsz);
    at_socket_event_send(SET_EVENT(socket, G780_EVENT_RECV_OK));
    /* notice the receive buffer and buffer size */
    if (at_evt_cb_set[AT_SOCKET_EVT_RECV])
    {
        at_evt_cb_set[AT_SOCKET_EVT_RECV](socket, AT_SOCKET_EVT_RECV, recv_buf, bfsz + 4);
    }
}
static const struct at_urc urc_table[] = {
        {"a",        		"",         urc_switchover_func},
		{"+ok",        		"\r\n",     urc_switchover_func},
		{"+SOCKALK",        "\r\n",     urc_connect_func},
		//{"SOCKA",        	&ttest,     urc_connect_func},
		{"{\"model\"",      "\r\n",     urc_recv_func},
		{"SOCK",      		"\r\n",     urc_recv_func},
		//{"+SYSINFO",        		"\r\n",     urc_func},
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
	
/* init for G780 */
static void g780_init_thread_entry(void *parameter)
{
#define CSQ_RETRY   10
	int i;
	char parsed_data[10];
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
	g780_enter_config();

	rt_thread_delay(rt_tick_from_millisecond(100));

	rt_thread_delay(rt_tick_from_millisecond(100));
	/* wait G780 startup finish */
	if (at_client_wait_connect(G780_WAIT_CONNECT_TIME))
	{
		result = -RT_ETIMEOUT;
		goto __exit;
	}
	/* disable echo */
	AT_SEND_CMD(resp, 0, 300, "AT+E=OFF");
	/* get module version */
	AT_SEND_CMD(resp, 2, 300, "AT+STMSG");
	/* Print response line buffer */
    for (i = 1; i < (int) resp->line_counts; i++)
    {
        LOG_D("%s", at_resp_get_line(resp, i+1));
    }
    AT_SEND_CMD(resp, 0, 300, "AT+WKMOD=NET");
    /* check signal strength */
    AT_SEND_CMD(resp, 0, 300, "AT+CSQ");
    for (i = 0; i < CSQ_RETRY; i++)
    {
        AT_SEND_CMD(resp, 0, 300, "AT+CSQ");
        at_resp_parse_line_args_by_kw(resp, "+CSQ:", "+CSQ: %s", &parsed_data);
        if (rt_strncmp(parsed_data, "99,99,99", sizeof(parsed_data)))
        {
            LOG_D("Signal strength: %s", parsed_data);
            break;
        }
        rt_thread_delay(rt_tick_from_millisecond(1000));
    }
    if (i == CSQ_RETRY)
    {
        LOG_E("Signal strength check failed (%s)", parsed_data);
        result = -RT_ERROR;
        goto __exit;
    }
    /* get system info */
    AT_SEND_CMD(resp, 2, 300, "AT+SYSINFO");
    for (i = 1; i < (int) resp->line_counts; i++)
    {
        LOG_D("%s", at_resp_get_line(resp, i+1));
    }

    AT_SEND_CMD(resp, 2, 300, "AT+LOCIP");
    {
        const char *line_buffer = RT_NULL;

        for(int line_num = 1; line_num < resp->line_counts; line_num++)
        {
            if((line_buffer = at_resp_get_line(resp, line_num + 1)) != RT_NULL)
            {
            	rt_size_t len = rt_strlen(line_buffer);
    			if (rt_strstr(line_buffer, "+ERR") || len < 15)
    			{
    				LOG_E("G780 get IP failed or return response error!");
    				result = -RT_ERROR;
    				goto __exit;
    			}
    			rt_memset(g780_Info.device_ip, 0, 15);
    			rt_memcpy(g780_Info.device_ip, &line_buffer[7], len-8);
                LOG_D("line %d buffer : %s", line_num, line_buffer);
            }
            else
            {
                LOG_E("Parse line buffer error!");
            }
        }
    }

    AT_SEND_CMD(resp, 2, 300, "AT+LBS");
    {
        const char *line_buffer = RT_NULL;

        for(int line_num = 1; line_num < resp->line_counts; line_num++)
        {
            if((line_buffer = at_resp_get_line(resp, line_num + 1)) != RT_NULL)
            {
            	rt_size_t len = rt_strlen(line_buffer);
    			if (rt_strstr(line_buffer, "+ERR") || len < 15)
    			{
    				LOG_E("G780 get LBS failed or return response error!");
    				result = -RT_ERROR;
    				goto __exit;
    			}
    			rt_memcpy(g780_Info.LAC, &line_buffer[6], 4);
    			rt_memcpy(g780_Info.CID, &line_buffer[13], 8);
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
	if (!result)
	{
		LOG_I("AT network initialize success!");
	}
	else
	{
		LOG_E("AT network initialize failed (%d)!", result);
	}
}
/**
 * @brief  G780模块初始化
 *  
 * @param 
 *
 * @return
 */
int g780_net_init(void)
{
#ifdef AT_INIT_BY_THREAD
    rt_thread_t tid;

    tid = rt_thread_create("g780_net_init", g780_init_thread_entry, RT_NULL, G780_THREAD_STACK_SIZE, G780_THREAD_PRIORITY, 20);
    if (tid)
    {
        rt_thread_startup(tid);
    }
    else
    {
        LOG_E("Create AT initialization thread fail!");
    }
#else
    g780_init_thread_entry(RT_NULL);
#endif

    return RT_EOK;
}

static const struct at_device_ops g780_socket_ops = {
	g780_socket_connect,
	g780_socket_close,
	g780_socket_send,
	g780_domain_resolve,
	g780_socket_set_event_cb,
};

static int at_socket_device_init(void)
{
    /* create current AT socket event */
    at_socket_event = rt_event_create("at_se", RT_IPC_FLAG_FIFO);
    if (at_socket_event == RT_NULL)
    {
        LOG_E("AT client port initialize failed! at_sock_event create failed!");
        return -RT_ENOMEM;
    }

    /* create current AT socket event lock */
    at_event_lock = rt_mutex_create("at_se", RT_IPC_FLAG_FIFO);
    if (at_event_lock == RT_NULL)
    {
        LOG_E("AT client port initialize failed! at_sock_lock create failed!");
        rt_event_delete(at_socket_event);
        return -RT_ENOMEM;
    }

    /* initialize AT client */
    at_client_init(AT_DEVICE_NAME, AT_DEVICE_RECV_BUFF_LEN);

    /* register URC data execution function  */
    at_set_urc_table(urc_table, sizeof(urc_table) / sizeof(urc_table[0]));

    /* initialize m26 network */
    g780_net_init();

    /* set m26 AT Socket options */
    at_socket_device_register(&g780_socket_ops);

    return RT_EOK;
}
//INIT_APP_EXPORT(at_socket_device_init);

int g780_test(void)
{
   int result = 0;

	rt_mutex_take(at_event_lock, RT_WAITING_FOREVER);
	LOG_E("  %c", 65);
	result = at_exec_cmd(RT_NULL, "AT+SOCK%CEN=OFF", 65);

	rt_mutex_release(at_event_lock);

	return result;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(g780_test, at_net_test, AT network test);
MSH_CMD_EXPORT_ALIAS(g780_enter_communicate, at_net_com, AT network com);

#endif
