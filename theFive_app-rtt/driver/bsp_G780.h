#ifndef __BSP_G780_H__
#define __BSP_G780_H__

#include <stdio.h>
#include <string.h>

int g780_socket_send(const char *buff, size_t bfsz);
rt_err_t g780_set_NetConnet(const char* _ip, const char* _port);
rt_err_t g780_Get_LocIP(char* _ip);
rt_err_t g780_Get_BaseStation(char* lac, char* cid);
int g780_test(void);
int g780_enter_communicate(void);
rt_err_t g780_enter_config(void);
#endif
