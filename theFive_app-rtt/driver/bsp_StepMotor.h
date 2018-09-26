#ifndef _BSP_STEPMOTOR_H_
#define _BSP_STEPMOTOR_H_
#include "stm32f4xx.h"
/* 
APB1 Timer clocks---84MHz
|--> TIM2 TIM3 TIM4 TIM6 TIM7 TIM12 TIM14
APB2 Timer clocks---168MHz
|--> TIM1 TIM9
*/
// 定义定时器预分频，定时器实际时钟频率为：84MHz/（STEPMOTOR_TIMx_PRESCALER+1）
#define TIM8_PRESCALER                  		1
#define TIM3_PRESCALER							0
#define TIM4_PRESCALER							0
#define TIM2_PRESCALER                  		0
#define TimeFrequency                           84000000
// 定义定时器周期，输出比较模式周期设置为0xFFFE
#define STEPMOTOR_TIM_PERIOD                   0xFFFF

#define CCR1_Val      42000//3500

#define Opto_1 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1)&HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)
#define Opto_2 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)&HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)
#define Opto_3 HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0)&HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)
#define Opto_4 HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1)&HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_2)

#define MOTOR_SUBDIVISION     8   //电机细分数

#define	MOTOR1_EN	1
#define	MOTOR2_EN	1
#define	MOTOR3_EN	1
#define	MOTOR4_EN	1

#define	MOTOR_MULTI	 1     //多电机同时运动

/* 类型定义 ------------------------------------------------------------------*/
typedef enum 
{
	STOP = 0,
	ACCEL,      //加速
	RUN,        //高速匀速
	DECEL,      //减速
	ConSpeed    //低速匀速
}e_RunState;

typedef enum 
{
	CW = 0,
	CCW
}e_dir;

typedef struct 
{
	uint8_t MotorID;        	    /* 电机ID */
	e_dir dir;			  			//方向
	uint8_t MotionStatus;	        //转动完成标志
	uint8_t LightSwitch;	        //光耦使能
	uint16_t error;		  			//错误标志，为1时，限位开关强停。
	uint8_t Reserve;        	    /* 保留位 */
	uint32_t Remainder; 			/* 余数 */
	
	uint32_t step_move;				    /* 相对运动步数 */
	uint32_t MaxSpeed;				    /* 最高速度 */	
	uint32_t MinSpeed;     		        /* 最低速度 */	
	uint32_t TargetSpeed; 		        /* 目标速度 */	
	float NowSpeed;                     /* 实时速度 （这里为浮点型 为了消除误差） */
	float step_accel;			        /* 加速度 （这里为浮点型 为了消除误差） */
	float step_dccel;			        /* 减速度 （这里为浮点型 为了消除误差） */

	uint32_t AccelCount;  		   	    /* 加速步长 */
	uint32_t DccelCount;	 			/* 减速步长 */
	uint32_t TimerCompareVal;		    /* 当前定时器比较数 */
	
	uint32_t PulseCount;				/* 脉冲计数 */
	int32_t  CurrentPosition;			//当前坐标位置 单位 步
	int32_t  CurrentPosition_Pulse;		/*当前坐标位置 单位 脉冲*/
	int32_t MaxPosition;				//最大坐标位置，超过该位置置0
	int32_t MaxPosition_Pulse;			//最大坐标位置 单位 脉冲
	int32_t MinPosition;				//最小坐标位置 单位 步
	int32_t MinPosition_Pulse;			//最小坐标位置 单位 脉冲
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

/****************函数声明**************/
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
