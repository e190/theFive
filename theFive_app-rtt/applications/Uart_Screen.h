#ifndef __UART_SCREEN_H__
#define __UART_SCREEN_H__

#include <rthw.h>
#include <rtthread.h>
#include "bsp_StepMotor.h"
#include "Heat_PID.h"

#define UartScreen_PRIORITY 11         //线程优先级
#define UartScreen_STACK_SIZE 1024     //线程栈大小
#define UartScreen_TIMESLICE 10        //时间片Tick

#define RT_SCREEN_DEVICE_NAME "uart2"

#define HEAD_1  	0xa5
#define HEAD_2  	0x5a

#define WRITE_80  	0x80
#define READ_81  	0x81
#define WRITE_82  	0x82
#define READ_83 	0x83
#define CURVE_84 	0x84

/* LCD寄存器地址 */
/* */
/*
 * 
 */
#define KEY_RETURN  		0x0010
/* LCD显示寄存器地址 */

#define SAMPLE1_PRO 		0x1120
#define SAMPLE2_PRO  		0x1130
#define SAMPLE3_PRO  		0x1140
#define SAMPLE4_PRO  		0x1150
#define SAMPLE1_INFO  		0x1160
#define SAMPLE2_INFO  		0x1180
#define SAMPLE3_INFO  		0x11A0
#define SAMPLE4_INFO  		0x11C0
/**********************************/
#define SAMPLE_CHANNEL  	0x11E0
#define SAMPLE_PRO  		0x11E5
#define SAMPLE_MIX_TIME  	0x11F0
#define SAMPLE_HEAT_TIME  	0x11F2
#define SAMPLE_READ_TIME  	0x11F4
#define SAMPLE_A1_TIME  	0x11F6
#define SAMPLE_A2_TIME  	0x11F8
/**********************************/
#define RFID_CARD  			0x4101
#define RFID_SET   			0x4105
#define RFID_WET   			0x4107

#define LIGHT_1   			0x4221
#define LIGHT_2   			0x4223
#define LIGHT_3   			0x4225
#define LIGHT_4   			0x4227
#define LIGHT_5   			0x4229
#define LIGHT_6   			0x422B
#define LIGHT_7   			0x422D
#define LIGHT_8   			0x422F

#define TEMP_1   			0x4331
#define TEMP_2   			0x4335
#define TEMP_3   			0x4339
#define TEMP_4   			0x433D
#define TEMP_CHANNEL    	0x4340
#define TEMP_SET   			0x4345
#define TEMP_TIME   		0x4349
#define TEMP_KP  			0x434B
#define TEMP_KI   			0x434F
#define TEMP_KD   			0x4353
#define TEMP_INFO   		0x4355

#define SCREEN_AUTO 		0x4410
#define SCREEN_CURRENT 		0x4412
#define SCREEN_DRAK 		0x4414

#define BLENDER_TIME1   	0x4921
#define BLENDER_TIME2   	0x4923
#define BLENDER_TIME3   	0x4925
#define BLENDER_TIME4   	0x4927
#define DOOR_RANGE   		0x4929

#define TIME_YEAR   		0x4711
#define TIME_MONTH   		0x4713
#define TIME_DAY   			0x4715
#define TIME_HOUR   		0x4717
#define TIME_MINUTE   		0x4719

#define PRINTER_FONT   		0x4811
#define PRINTER_DISTANCE   	0x4815
#define PRINTER_LANGUAGE   	0x4819

#define MOTOR_CHANNEL   	0x4511
#define MOTOR_POSITION   	0x4515
#define MOTOR_SPEED   		0x4517
#define MOTOR_ACCELTIME   	0x4519
#define MOTOR_DECELTIME   	0x451B
#define MOTOR_CURRENT   	0x4520
#define MOTOR_INFO   		0x4530
/**************测试5130******************/
#define tmc5130_POSITION   	0x4550
#define tmc5130_CURRENT   	0x4552
#define tmc5130_A1   		0x4554
#define tmc5130_AMAX   		0x4556
#define tmc5130_D1   		0x4558
#define tmc5130_DMAX   		0x455A
#define tmc5130_V1   		0x455C
#define tmc5130_VMAX   		0x455E
#define tmc5130_VSTART   	0x4560
#define tmc5130_VSTOP   	0x4562
#define tmc5130_IHOLD   	0x4564
#define tmc5130_IRUN   		0x4566
/**************测试5130******************/
#define SERVER_INFO   		0x4A11
#define SERVER_IP   		0x4A5A
#define SERVER_PORT   		0x4A78

/* LCD图标寄存器地址 */
#define SAMPLE1_SWITCH_IOC  0x1011
#define SAMPLE2_SWITCH_IOC  0x1012
#define SAMPLE3_SWITCH_IOC  0x1013
#define SAMPLE4_SWITCH_IOC  0x1014

#define SAMPLE1_PROG_IOC  	0x1015
#define SAMPLE2_PROG_IOC  	0x1016
#define SAMPLE3_PROG_IOC  	0x1017
#define SAMPLE4_PROG_IOC  	0x1018
/********************************/

#define LED1_ICO  			0x4210
#define LED2_ICO  			0x4211
#define LED3_ICO  			0x4212
#define LED4_ICO  			0x4213
#define LED5_ICO  			0x4214
#define LED6_ICO  			0x4215
#define LED7_ICO  			0x4216
#define LED8_ICO  			0x4217
#define LED1_SWITCH_ICO  	0x4218
#define LED2_SWITCH_ICO  	0x4219
#define LED3_SWITCH_ICO  	0x421A
#define LED4_SWITCH_ICO  	0x421B
#define LED5_SWITCH_ICO  	0x421C
#define LED6_SWITCH_ICO  	0x421D
#define LED7_SWITCH_ICO  	0x421E
#define LED8_SWITCH_ICO  	0x421F

#define LDOOR_ICO  			0x4911
#define RDOOR_ICO  			0x4912
#define BLENDER1_ICO  		0x4913
#define BLENDER2_ICO  		0x4914
#define BLENDER3_ICO  		0x4915
#define BLENDER4_ICO  		0x4916
#define BLENDER1_START_ICO  0x4917
#define BLENDER2_START_ICO  0x4918
#define BLENDER3_START_ICO  0x4919
#define BLENDER4_START_ICO  0x491A

#define TEMP1_ICO  			0x4311
#define TEMP2_ICO  			0x4312
#define TEMP3_ICO  			0x4313
#define TEMP4_ICO  			0x4314

#define MOTOR_POS_ICO  		0x4504
#define MOTOR_RUN_ICO  		0x4505
#define MOTOR_NEGA_ICO  	0x4506

/********键值************************/
/* 主菜单 */
#define SAMPLE  			0x1000
#define QUALITY 			0x2000
#define FIND				0x3000
#define SYSTEM				0x4000

#define BACK_MANU			0x0011

/* 样本按键 */
#define SAMPLE_SWITCH_1		0x1001
#define SAMPLE_SWITCH_2		0x1002
#define SAMPLE_SWITCH_3		0x1003
#define SAMPLE_SWITCH_4		0x1004

#define SAMPLE_SET_PARA		0x1005
#define SAMPLE_CURVE		0x1006
#define SAMPLE_PARA_OK		0x1007
#define SAMPLE_PARA_CLEAN	0x1008

/* 结果查询按键 */
#define RESULT_SLIDER		0x3001


/* 系统设置 菜单 */
#define RFID				0x4100
#define LIGHT				0x4200
#define TEMPERATURE			0x4300
#define SCREEN				0x4400
#define STEP_MOTOR			0x4500
#define CALIBRATION			0x4600
#define SET_TIME			0x4700
#define PRINTER				0x4800
#define DC_MOTOR			0x4900
#define SERVER				0x4A00

#define RFID_OK				0x4101
#define RFID_CLEAR			0x4102

#define LIGHT_SWITCH_1		0x4201
#define LIGHT_SWITCH_2		0x4202
#define LIGHT_SWITCH_3		0x4203
#define LIGHT_SWITCH_4		0x4204
#define LIGHT_SWITCH_5		0x4205
#define LIGHT_SWITCH_6		0x4206
#define LIGHT_SWITCH_7		0x4207
#define LIGHT_SWITCH_8		0x4208

#define TEMP_OK				0x4301
#define TEMP_STOP			0x4302

#define SCREEN_OK			0x4401
#define SCREEN_DEFAULT		0x4402

#define STEPMOTOR_ZERO		0x4501
#define STEPMOTOR_START		0x4502
#define STEPMOTOR_STOP		0x4503
#define tmc5130_ZERO		0x4504
#define tmc5130_START		0x4505
#define tmc5130_STOP		0x4506

#define CALI_OK				0x4601

#define SET_TIME_OK			0x4701
#define SET_TIME_CANCEL		0x4702

#define PRINTER_FORMAT_1	0x4801
#define PRINTER_FORMAT_2	0x4802
#define PRINTER_FORMAT_3	0x4803
#define PRINTER_OK			0x4804
#define PRINTER_DEFAULT		0x4805

#define DOOR_OPEN_1			0x4901
#define DOOR_CLOSE_1		0x4902
#define DOOR_OPEN_2			0x4903
#define DOOR_CLOSE_2		0x4904
#define DCMOTOR_SWITCH_1	0x4905
#define DCMOTOR_SWITCH_2	0x4906
#define DCMOTOR_SWITCH_3	0x4907
#define DCMOTOR_SWITCH_4	0x4908


#define KEY_SERVER_ACTIVATE	0x4A01
#define KEY_SERVER_UPLOAD	0x4A02
#define KEY_SERVER_SET		0x4A03
#define KEY_SERVER_TEST		0x4A04

/* 曲线LCD地址 */
#define CURVE1_DESC			0x5000
#define CURVE2_DESC			0x5020
#define CURVE3_DESC			0x5040
#define CURVE4_DESC			0x5060
#define CURVE_X				0x5080
#define CURVE_Y				0x5085
#define CURVE_CLEAR			0x5100

struct RealTime_t
{
	rt_uint8_t year;
	rt_uint8_t month;
	rt_uint8_t day;
	rt_uint8_t hour;
	rt_uint8_t minute;
};

struct MotorParaBuf_t
{
	rt_uint8_t channel;
	MotorHandle_t* h_Motor;
	rt_int16_t position;
	rt_uint16_t speed;
	rt_uint16_t acceltime;
	rt_uint16_t deceltime;
	rt_int32_t tmc5230_position;
	rt_uint32_t tmc5230_v1;
	rt_uint32_t tmc5230_vmax;
};

struct printer_t
{   
	rt_uint8_t font_size;
	rt_uint8_t gap_size;
};

struct ServerSetBuf_t
{
	char _ip[16];
	char _port[6];
};

struct heat_para_t
{
	rt_uint8_t channel;
	rt_uint8_t CycleTime;
	rt_uint16_t iSetVal;       //设定值
	float uKP_Coe;             //比例系数
	float uKI_Coe;             //积分常数
	float uKD_Coe;             //微分常数
};

struct flow_para_t
{
	rt_uint8_t channel;
	rt_uint8_t pro;
	rt_uint8_t mix_time;
    rt_uint16_t heat_time;
    rt_uint16_t read_time;
    rt_uint16_t a1_time;
    rt_uint16_t a2_time;
};

typedef struct 
{
	rt_uint8_t rxCount;          //接收buff
    rt_uint8_t txCount;          //发送buff 
    rt_uint8_t rxBuff[20];          //接收buff
    struct heat_para_t heat_para;
	struct flow_para_t flow_para;
    struct RealTime_t RealTime;
	struct MotorParaBuf_t MotorPara;
	struct ServerSetBuf_t ServerSetBuf;
	struct printer_t printer;
}UartBuff_t;

rt_err_t uart_open(const char *name);
rt_uint8_t ScreenSendData(rt_uint16_t s_addr, rt_uint8_t* str, rt_uint8_t len);
rt_uint8_t ScreenSendData_2bytes(rt_uint16_t s_addr, rt_uint16_t _data);
rt_uint8_t ScreenSendCommand(rt_uint8_t _cmd, rt_uint16_t s_addr, rt_uint8_t* str, rt_uint8_t len);
rt_uint8_t ScreenDisICON(rt_uint16_t s_addr, rt_uint8_t status);
extern void Function_UartScreen(void* parameter);
#endif
