#ifndef __BSP_G780_H__
#define __BSP_G780_H__

#define G780_MODULE_SEND_MAX_SIZE       1460
#define G780_WAIT_CONNECT_TIME          5000
#define G780_THREAD_STACK_SIZE          1024
#define G780_THREAD_PRIORITY            (RT_THREAD_PRIORITY_MAX/2)

/* set real event by current socket and current state */
#define SET_EVENT(socket, event)       (((socket + 1) << 16) | (event))

/* AT socket event type */

#define G780_EVENT_CONN_OK              (1L << 0)
#define G780_EVENT_SW_OK            	(1L << 1)
#define G780_EVENT_RECV_OK              (1L << 2)
#define G780_EVENT_SET_ERROR            (1L << 3)
#define G780_EVENT_CONN_CLOSE           (1L << 4)

typedef enum {
	config,
    communicate
}modeStatus;

struct g780_Info_t
{
	modeStatus mode;
    char message[10];
	char device_id[10];
	char device_ip[15];
	char LAC[5];
	char CID[10];
};

int g780_test(void);

#endif
