#ifndef __HEAT_PID_H__
#define __HEAT_PID_H__

#define Heat_PID_PRIORITY 10         //线程优先级
#define Heat_PID_STACK_SIZE 512      //线程栈大小
#define Heat_PID_TIMESLICE 10        //时间片Tick

#define  TIME_FREQUENCY   10000000   // 10e9 / period = frequency
#define heat_name   	"pwm1"

typedef enum {
	HEAT_STOP,
    HEAT_START,
} HeatSwitch;

typedef struct 
{
	float 		liEkVal[3];          //差值保存，给定和反馈的差值
	float 		uKP_Coe;             //比例系数
	float 		uKI_Coe;             //积分常数
	float 		uKD_Coe;             //微分常数
	float		integral;
	float 		err_last;
	float 		err;
}PID_Value;

struct HeatSystem_t
{
	char* deviceName;
	rt_uint8_t PWM_channel;
	rt_uint32_t CycleTime;			 //周期时间
	rt_uint16_t duty;                //占空比 * 1000
	float 		iSetVal;             //设定值
	float 		iCurVal;             //实际值
	PID_Value PID;
};


extern struct HeatSystem_t HeatHandle_1, HeatHandle_2, HeatHandle_3, HeatHandle_4;

rt_uint8_t get_temp_status(void);
void dis_temp_status(void);
float pid_calculate(struct HeatSystem_t* h_heat);
int set_heat_duty(struct HeatSystem_t* h_heat, rt_uint16_t percent);
rt_err_t heat_start_stop(struct HeatSystem_t* h_heat, HeatSwitch _status);
rt_err_t all_heat_start_stop(HeatSwitch _config);
void cycle_temp_heat(void);

extern void Function_Heat_PID(void* parameter);
#endif
