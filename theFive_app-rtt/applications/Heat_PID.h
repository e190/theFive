#ifndef __HEAT_PID_H__
#define __HEAT_PID_H__

#define Heat_PID_PRIORITY 10         //�߳����ȼ�
#define Heat_PID_STACK_SIZE 512      //�߳�ջ��С
#define Heat_PID_TIMESLICE 10        //ʱ��ƬTick

#define  TIME_FREQUENCY   10000000   // 10e9 / period = frequency
#define heat_name   	"pwm1"

#define DEFAULT_PID           \
    {                         \
        .iSetVal = 370,       \
		.CycleTime = 2,       \
        .uKP_Coe = 20,        \
        .uKI_Coe = 0.1,       \
        .uKD_Coe = 80,        \
    }

typedef enum {
	HEAT_STOP,
    HEAT_START,
} HeatSwitch;

struct PID_Value
{
	float 		iSetVal;             //�趨ֵ
	rt_uint32_t CycleTime;			 //����ʱ��
	float 		uKP_Coe;             //����ϵ��
	float 		uKI_Coe;             //���ֳ���
	float 		uKD_Coe;             //΢�ֳ���
	float		integral;
	float 		err_last;			 //��ֵ���棬�����ͷ����Ĳ�ֵ
	float 		err;
};

struct HeatSystem_t
{
	//char* deviceName;
	rt_uint8_t PWM_channel;
	rt_uint16_t duty;                //ռ�ձ� * 1000
	float 		iCurVal;             //ʵ��ֵ
	struct PID_Value PID;
};


extern struct HeatSystem_t HeatHandle_1, HeatHandle_2, HeatHandle_3, HeatHandle_4;

void set_heat_para(rt_uint8_t _ch, const struct PID_Value* handle);
int read_heat_para(rt_uint8_t _ch, struct PID_Value* handle);
rt_uint8_t get_temp_status(void);
void dis_temp_status(void);
float pid_calculate(struct HeatSystem_t* h_heat);
int set_heat_duty(struct HeatSystem_t* h_heat, rt_uint16_t percent);
rt_err_t heat_start_stop(struct HeatSystem_t* h_heat, HeatSwitch _status);
rt_err_t all_heat_start_stop(HeatSwitch _config);
void cycle_temp_heat(void);
int head_system_init(void);

#endif
