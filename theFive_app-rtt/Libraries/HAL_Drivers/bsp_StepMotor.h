#ifndef _BSP_STEPMOTOR_H_
#define _BSP_STEPMOTOR_H_
#include "stm32f4xx.h"
/* 
APB1 Timer clocks---84MHz
|--> TIM2 TIM3 TIM4 TIM6 TIM7 TIM12 TIM14
APB2 Timer clocks---168MHz
|--> TIM1 TIM9
*/
// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/��STEPMOTOR_TIMx_PRESCALER+1��
#define TIM8_PRESCALER                  		1
#define TIM3_PRESCALER							0
#define TIM4_PRESCALER							0
#define TIM2_PRESCALER                  		0
#define TimeFrequency                           84000000
// ���嶨ʱ�����ڣ�����Ƚ�ģʽ��������Ϊ0xFFFE
#define STEPMOTOR_TIM_PERIOD                   0xFFFF

#define CCR1_Val      42000//3500

#define Opto_1 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1)&HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)
#define Opto_2 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)&HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)
#define Opto_3 HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0)&HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)
#define Opto_4 HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1)&HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_2)

#define MOTOR_SUBDIVISION     8   //���ϸ����

#define	MOTOR1_EN	1
#define	MOTOR2_EN	1
#define	MOTOR3_EN	1
#define	MOTOR4_EN	1

#define	MOTOR_MULTI	 1     //����ͬʱ�˶�

/* ���Ͷ��� ------------------------------------------------------------------*/
typedef enum 
{
	STOP = 0,
	ACCEL,      //����
	RUN,        //��������
	DECEL,      //����
	ConSpeed    //��������
}e_RunState;

typedef enum 
{
	CW = 0,
	CCW
}e_dir;

typedef struct 
{
	uint8_t MotorID;        	    /* ���ID */
	e_dir dir;			  			//����
	uint8_t MotionStatus;	        //ת����ɱ�־
	uint8_t LightSwitch;	        //����ʹ��
	uint16_t error;		  			//�����־��Ϊ1ʱ����λ����ǿͣ��
	uint8_t Reserve;        	    /* ����λ */
	uint32_t Remainder; 			/* ���� */
	
	uint32_t step_move;				    /* ����˶����� */
	uint32_t MaxSpeed;				    /* ����ٶ� */	
	uint32_t MinSpeed;     		        /* ����ٶ� */	
	uint32_t TargetSpeed; 		        /* Ŀ���ٶ� */	
	float NowSpeed;                     /* ʵʱ�ٶ� ������Ϊ������ Ϊ�������� */
	float step_accel;			        /* ���ٶ� ������Ϊ������ Ϊ�������� */
	float step_dccel;			        /* ���ٶ� ������Ϊ������ Ϊ�������� */

	uint32_t AccelCount;  		   	    /* ���ٲ��� */
	uint32_t DccelCount;	 			/* ���ٲ��� */
	uint32_t TimerCompareVal;		    /* ��ǰ��ʱ���Ƚ��� */
	
	uint32_t PulseCount;				/* ������� */
	int32_t  CurrentPosition;			//��ǰ����λ�� ��λ ��
	int32_t  CurrentPosition_Pulse;		/*��ǰ����λ�� ��λ ����*/
	int32_t MaxPosition;				//�������λ�ã�������λ����0
	int32_t MaxPosition_Pulse;			//�������λ�� ��λ ����
	int32_t MinPosition;				//��С����λ�� ��λ ��
	int32_t MinPosition_Pulse;			//��С����λ�� ��λ ����
	void (*CCWfun)(rt_uint8_t status);
	void (*HalfCurrent)(rt_uint8_t status);
	uint8_t (*optofun)();
	void (*end_cb)();
 
	HAL_TIM_ActiveChannel activeChannel;
	TIM_HandleTypeDef* TIMx_Handle;	
	TIM_TypeDef* TIMx;
	uint32_t TIMx_CHANNEL;
	uint32_t TIM_IT_CCx;
	uint32_t TIM_FLAG_CCx;
}MotorHandle_t;

extern MotorHandle_t Motor1;
extern MotorHandle_t Motor2;
extern MotorHandle_t Motor3;
extern MotorHandle_t Motor4;

extern MotorHandle_t* NowMotor;

/****************��������**************/
void StepMotorRampDataInit(void);
void StepMotor_TIMx_Init(void);
uint8_t set_current_position(MotorHandle_t* _motor, rt_int32_t _pos);
uint8_t set_max_position(MotorHandle_t* _motor, rt_int32_t _pos);
uint8_t set_min_position(MotorHandle_t* _motor, rt_int32_t _pos);
uint8_t StepMotor_AxisMoveRel(MotorHandle_t* Motor, int32_t _position, 
							  uint32_t _acceltime, uint32_t _deceltime,
							  uint32_t _MaxSpeed);
void StepMotor_PeriodSet(MotorHandle_t* Motor);
void StartMotorTimer(MotorHandle_t* Motor);
void StopMotorTimer(MotorHandle_t* Motor);
int bsp_InitStepMotor(void);
void OptoControl(void);
uint8_t motor_set_end_indicate(MotorHandle_t* Motor, void (*_end_cb)());

#endif
