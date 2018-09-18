#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "bsp_timer.h"
#include "bsp_StepMotor.h"

TIM_HandleTypeDef htim_2;
TIM_HandleTypeDef htim_3;
TIM_HandleTypeDef htim_4;
TIM_HandleTypeDef htim_8;

MotorHandle_t* NowMotor = NULL;
void StopMotorTimer(MotorHandle_t* Motor);

#if MOTOR1_EN
	MotorHandle_t Motor1;
	static void M1_CCW(GPIO_PinState _bc);
	static void M1_HalfCurrent(GPIO_PinState _bc);
	static uint8_t M1_optofun(void);
#endif
#if MOTOR2_EN
	MotorHandle_t Motor2;
	static void M2_CCW(GPIO_PinState _bc);
	static void M2_HalfCurrent(GPIO_PinState _bc);
	static uint8_t M2_optofun(void);
#endif
#if MOTOR3_EN
	MotorHandle_t Motor3;
	static void M3_CCW(GPIO_PinState _bc);
	static void M3_HalfCurrent(GPIO_PinState _bc);
	static uint8_t M3_optofun(void);
#endif
#if MOTOR4_EN
	MotorHandle_t Motor4;
	static void M4_CCW(GPIO_PinState _bc);
	static void M4_HalfCurrent(GPIO_PinState _bc);
	static uint8_t M4_optofun(void);
#endif

//static void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
static void StepMotor_ConutStep(MotorHandle_t* Motor);

/**
* @brief  StepMotorRampDataInit
*         初始化电机参数
* @param  Motor ： 电机句柄
* 
* @retval 
*/
void StepMotorRampDataInit(void)
{
#if MOTOR1_EN
	Motor1.MinSpeed = 1000;
	Motor1.MaxSpeed = 12000;
	Motor1.CurrentPosition = 0;
	Motor1.MaxPosition = 6000;
	Motor1.TIMx_Handle = &htim_8;
	Motor1.TIMx_CHANNEL = TIM_CHANNEL_1;
	Motor1.TIM_IT_CCx = TIM_IT_CC1;
	Motor1.TIMx = TIM8;
	Motor1.CCWfun = M1_CCW;
	Motor1.LightSwitch = 0;
	Motor1.HalfCurrent = M1_HalfCurrent;
	Motor1.optofun = M1_optofun;
	Motor1.end_cb = NULL;
#endif
#if MOTOR2_EN
	Motor2.MinSpeed = 1000;
	Motor2.MaxSpeed = 15100;
	Motor2.CurrentPosition = 0;
	Motor2.MaxPosition = 6000;
	Motor2.LightSwitch = 0;
	Motor2.TIMx_Handle = &htim_4;
	Motor2.TIMx_CHANNEL = TIM_CHANNEL_1;
	Motor2.TIM_IT_CCx = TIM_IT_CC1;
	Motor2.TIMx = TIM4;
	Motor2.CCWfun = M2_CCW;
	Motor2.HalfCurrent = M2_HalfCurrent;
	Motor2.optofun = M2_optofun;
	Motor2.end_cb = NULL;
#endif
#if MOTOR3_EN
	Motor3.MinSpeed = 1000;
	Motor3.MaxSpeed = 15000;
	Motor3.CurrentPosition = 0;
	Motor3.MaxPosition = 6000;
	Motor3.LightSwitch = 0;
	Motor3.TIMx_Handle = &htim_3;
	Motor3.TIMx_CHANNEL = TIM_CHANNEL_1;
	Motor3.TIM_IT_CCx = TIM_IT_CC1;
	Motor3.TIMx = TIM3;
	Motor3.CCWfun = M3_CCW;
	Motor3.HalfCurrent = M3_HalfCurrent;
	Motor3.optofun = M3_optofun;
	Motor3.end_cb = NULL;
#endif
#if MOTOR4_EN
	Motor4.MinSpeed = 1000;
	Motor4.MaxSpeed = 15000;
	Motor4.CurrentPosition = 0;
	Motor4.MaxPosition = 6000;
	Motor4.MinPosition = 0;
	Motor4.LightSwitch = 0;
	Motor4.TIMx_Handle = &htim_2;
	Motor4.TIMx_CHANNEL = TIM_CHANNEL_1;
	Motor4.TIM_IT_CCx = TIM_IT_CC1;
	Motor4.TIMx = TIM2;
	Motor4.CCWfun = M4_CCW;
	Motor4.HalfCurrent = M4_HalfCurrent;
	Motor4.optofun = M4_optofun;
	Motor4.end_cb = NULL;
#endif

}
/**
* @brief  
*        TIM8初始化
* @param 
* 
* @retval 
*/
void TIM8_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // 定时器时钟
	TIM_OC_InitTypeDef sConfigOC;                          // 定时器通道比较输出
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;			
	
	/* 定时器基本环境配置 */
	htim_8.Instance = TIM8;                          // 定时器编号
	htim_8.Init.Prescaler = TIM8_PRESCALER;           // 定时器预分频器
	htim_8.Init.CounterMode = TIM_COUNTERMODE_UP;              // 计数方向：向上计数
	htim_8.Init.Period = STEPMOTOR_TIM_PERIOD;                 // 定时器周期
	htim_8.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;          // 时钟分频

	if (HAL_TIM_OC_Init(&htim_8) != HAL_OK)
	{
		RT_ASSERT(0);
	}

	/* 定时器时钟源配置 */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// 使用内部时钟源
	HAL_TIM_ConfigClockSource(&htim_8, &sClockSourceConfig);

	/* 定时器比较输出配置 */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // 比较输出模式：反转输出
	sConfigOC.Pulse = 0;                                  // 脉冲数
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // 输出极性
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // 互补通道输出极性
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // 快速模式
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // 空闲电平
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // 互补通道空闲电平
	
	if(HAL_TIM_OC_ConfigChannel(&htim_8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		RT_ASSERT(0);
	}

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim_8, &sBreakDeadTimeConfig);
	
	/* STEPMOTOR相关GPIO初始化配置 */
	//HAL_TIM_MspPostInit(&htim_8);
}
/**
* @brief  
*        TIM2初始化
* @param 
* 
* @retval 
*/
void TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // 定时器时钟
	TIM_OC_InitTypeDef sConfigOC;                          // 定时器通道比较输出		
	
	/* 定时器基本环境配置 */
	htim_2.Instance = TIM2;                          // 定时器编号
	htim_2.Init.Prescaler = TIM2_PRESCALER;           // 定时器预分频器
	htim_2.Init.CounterMode = TIM_COUNTERMODE_UP;              // 计数方向：向上计数
	htim_2.Init.Period = 0xFFFFFFFF;                 // 定时器周期  0xFFFFFFFF
	htim_2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;          // 时钟分频
	HAL_TIM_OC_Init(&htim_2);

	/* 定时器时钟源配置 */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// 使用内部时钟源
	HAL_TIM_ConfigClockSource(&htim_2, &sClockSourceConfig);

	/* 定时器比较输出配置 */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // 比较输出模式：反转输出
	sConfigOC.Pulse = 1;                                  // 脉冲数
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // 输出极性
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // 互补通道输出极性
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // 快速模式
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // 空闲电平
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // 互补通道空闲电平
	if(HAL_TIM_OC_ConfigChannel(&htim_2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		RT_ASSERT(0);
	}

	/* STEPMOTOR相关GPIO初始化配置 */
	//HAL_TIM_MspPostInit(&htim_2);
}
/**
* @brief  
*        TIM3初始化
* @param 
* 
* @retval 
*/
void TIM3_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // 定时器时钟
	TIM_OC_InitTypeDef sConfigOC;                          // 定时器通道比较输出		
	
	/* 定时器基本环境配置 */
	htim_3.Instance = TIM3;                          // 定时器编号
	htim_3.Init.Prescaler = TIM3_PRESCALER;           // 定时器预分频器
	htim_3.Init.CounterMode = TIM_COUNTERMODE_UP;              // 计数方向：向上计数
	htim_3.Init.Period = STEPMOTOR_TIM_PERIOD;                 // 定时器周期
	htim_3.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;          // 时钟分频
	HAL_TIM_OC_Init(&htim_3);

	/* 定时器时钟源配置 */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// 使用内部时钟源
	HAL_TIM_ConfigClockSource(&htim_3, &sClockSourceConfig);

	/* 定时器比较输出配置 */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // 比较输出模式：反转输出
	sConfigOC.Pulse = 0;                                  // 脉冲数
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // 输出极性
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // 互补通道输出极性
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // 快速模式
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // 空闲电平
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // 互补通道空闲电平
	
	HAL_TIM_OC_ConfigChannel(&htim_3, &sConfigOC, TIM_CHANNEL_1);
	
	/* STEPMOTOR相关GPIO初始化配置 */
	//HAL_TIM_MspPostInit(&htim_3);
}
/**
* @brief  
*        TIM4初始化
* @param 
* 
* @retval 
*/
void TIM4_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // 定时器时钟
	TIM_OC_InitTypeDef sConfigOC;                          // 定时器通道比较输出		
	
	/* 定时器基本环境配置 */
	htim_4.Instance = TIM4;                          // 定时器编号
	htim_4.Init.Prescaler = TIM4_PRESCALER;           // 定时器预分频器
	htim_4.Init.CounterMode = TIM_COUNTERMODE_UP;              // 计数方向：向上计数
	htim_4.Init.Period = STEPMOTOR_TIM_PERIOD;                 // 定时器周期
	htim_4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;          // 时钟分频
	HAL_TIM_OC_Init(&htim_4);

	/* 定时器时钟源配置 */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// 使用内部时钟源
	HAL_TIM_ConfigClockSource(&htim_4, &sClockSourceConfig);

	/* 定时器比较输出配置 */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // 比较输出模式：反转输出
	sConfigOC.Pulse = 0;                                  // 脉冲数
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // 输出极性
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // 互补通道输出极性
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // 快速模式
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // 空闲电平
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // 互补通道空闲电平
	if(HAL_TIM_OC_ConfigChannel(&htim_4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		RT_ASSERT(0);
	}
	/* STEPMOTOR相关GPIO初始化配置 */
	//HAL_TIM_MspPostInit(&htim_4);
}

/**
* @brief  TIM8_GPIOInit
*         TIM8的IO初始化
* @param 
* 
* @retval 
*/
void TIM8_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/**TIM8 GPIO Configuration  
	PC6     ------> TIM8_CH1  
	*/
	GPIO_InitStruct.Pin =  GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);

	/* 
	MOTOR1使能EN PD8
	MOTOR1方向CWB PD9
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;		
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	
	/* MOTOR1 REF */
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	

	/* 控制电机电流 */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
}
/**
* @brief  TIM3_GPIOInit
*         TIM3的IO初始化
* @param 
* 
* @retval 
*/
void TIM3_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/**TIM3 GPIO Configuration    
	PA6     ------> TIM3_CH1 
	*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);
	/* 
	MOTOR3 使能EN  PG1
	MOTOR3 方向CWB PG0
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	/* TQ2 PF15*/
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);


	/* 控制电机电流 */
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);
}
/**
* @brief  TIM4_GPIOInit
*         TIM4的IO初始化
* @param 
* 
* @retval 
*/
void TIM4_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    /**TIM4 GPIO Configuration    
		PD12     ------> TIM4_CH1 
    */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
		
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_12, GPIO_PIN_RESET);
	/* 
	MOTOR2CWB 使能 PE12 
	MOTOR2CWB 方向 PE10
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* 控制电机电流 PE8*/
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
}
/**
* @brief  TIM2_GPIOInit
*         TIM2的IO初始化
* @param 
* 
* @retval 
*/
void TIM2_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    /**TIM2 GPIO Configuration    
		PA5     ------> TIM2_CH1 
    */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_RESET);
	/* 
	MOTOR4 使能EN  PF13
	MOTOR4 方向CWB PF12
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);		

	/* 控制电机电流 */
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11 | GPIO_PIN_13, GPIO_PIN_SET);
}
/**
  * 函数功能: 基本定时器硬件初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim_base)
{	
	if(htim_base->Instance == TIM8)
	{
		/* 基本定时器外设时钟使能 */
		__HAL_RCC_TIM8_CLK_ENABLE();

		TIM8_GPIOInit();  
		
		/* 配置定时器中断优先级并使能 */
		HAL_NVIC_SetPriority(TIM8_CC_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
	}
	if(htim_base->Instance == TIM2)
	{
		/* 基本定时器外设时钟使能 */
		__HAL_RCC_TIM2_CLK_ENABLE();

		TIM2_GPIOInit();
		
		/* 配置定时器中断优先级并使能 */
		HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
	if(htim_base->Instance == TIM3)
	{
		/* 基本定时器外设时钟使能 */
		__HAL_RCC_TIM3_CLK_ENABLE();

		TIM3_GPIOInit();
		
		/* 配置定时器中断优先级并使能 */
		HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
	if(htim_base->Instance == TIM4)
	{
		/* 基本定时器外设时钟使能 */
		__HAL_RCC_TIM4_CLK_ENABLE();

		TIM4_GPIOInit();
		
		/* 配置定时器中断优先级并使能 */
		HAL_NVIC_SetPriority(TIM4_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
	}
}

/**
  * 函数功能: 基本定时器硬件反初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef* htim_base)
{

	if(htim_base->Instance == TIM4)
	{
		/* 基本定时器外设时钟禁用 */
		__HAL_RCC_TIM4_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOD,GPIO_PIN_12);
		HAL_NVIC_DisableIRQ(TIM4_IRQn);
	}
	else if(htim_base->Instance == TIM3)
	{
		/* 基本定时器外设时钟禁用 */
		__HAL_RCC_TIM3_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_6);
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
	}
	else if(htim_base->Instance == TIM2)
	{
		/* 基本定时器外设时钟禁用 */
		__HAL_RCC_TIM2_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_5);
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
	}
	else if(htim_base->Instance == TIM8)
	{
		/* 基本定时器外设时钟禁用 */
		__HAL_RCC_TIM8_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOC,GPIO_PIN_6);
		HAL_NVIC_DisableIRQ(TIM8_CC_IRQn);
	}
}
/**
 * @brief 初始化电机
 *  
 * @param 
 *
 * @return
 */
int bsp_InitStepMotor(void)
{
	StepMotorRampDataInit();
	TIM8_Init();
	TIM2_Init();
	TIM3_Init();
	TIM4_Init();
	TIM6_Init(1000-1, 84-1);
	return 0;
}
INIT_DEVICE_EXPORT(bsp_InitStepMotor);
/**
* @brief  电机旋转方向
*        
* @param  _bc : GPIO_PIN_SET   反时针
*  		  _bc : GPIO_PIN_RESET 顺时针
* @retval 
*/
void M1_CCW(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, _bc);
}
void M2_CCW(rt_uint8_t _bc)
{
//	if(0 == _bc)
//		_bc = GPIO_PIN_SET;
//	else
//		_bc = GPIO_PIN_RESET;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, _bc);
}
void M3_CCW(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, _bc);
}
void M4_CCW(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, _bc);
}
/**
* @brief  电机半流控制
*        
* @param _bc : GPIO_PIN_SET   ---> 20%
* 		 _bc : GPIO_PIN_RESET ---> 100%
* @retval 
*/
void M1_HalfCurrent(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, _bc);
}
void M2_HalfCurrent(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, _bc);
}
void M3_HalfCurrent(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, _bc);
}
void M4_HalfCurrent(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, _bc);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, _bc);
}
/**
 * @brief 光耦检测
 *  
 * @param  LightSwitch = 0  ---> 开启回零检测
 *		   LightSwitch = 1  ---> 开启最大值检测
 * @return false 有效
 */
uint8_t M1_optofun(void)
{
	if(Motor1.LightSwitch)
	{
return 0;
	}		
	else
		return HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1);
}
uint8_t M2_optofun(void)
{
	if(Motor2.LightSwitch)
		return HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4);
	else
		return HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3);
}
uint8_t M3_optofun(void)
{
	if(0 == Motor3.LightSwitch)
		return HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6);
	else
		return 1;
}
uint8_t M4_optofun(void)
{
	if(0 == Motor4.LightSwitch)
		return HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_1);
	if(1 == Motor4.LightSwitch)
		return HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_2);
	else	
		return 1;
	
}

/**
 * @brief 光耦控制
 *        循环检测
 * @param 
 *
 * @return
 */
void OptoControl(void)
{
#if (MOTOR_MULTI)
	
#else
	if(NowMotor)
	{		
		if(0 == NowMotor->optofun())
		{
			if(0 == NowMotor->LightSwitch) //如果是回零停止，坐标置零
				NowMotor->CurrentPosition_Pulse = 0;
			StopMotorTimer(NowMotor);
		}	
	}
#endif
}
/**
 * @brief  定时器中断函数
 *  
 * @param 
 *
 * @return
 */
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim_4);
}
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim_3);
}
void TIM8_CC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim_8);
}
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim_2);
}
/**
* @brief  定时器中断回调函数
*        
* @param 
* 
* @retval 
*/
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{	
#if (MOTOR_MULTI)
	MotorHandle_t* motor[4] = {&Motor1, &Motor2, &Motor3, &Motor4};
	for(rt_uint8_t i=0;i<4;i++)
	{
		if(htim == motor[i]->TIMx_Handle)	
		{
			StepMotor_ConutStep(motor[i]);
		}
	
	}
#else	
	if(NowMotor)	
		StepMotor_ConutStep(NowMotor);
	else
	{
		TIM_CCxChannelCmd(htim->Instance, htim->Channel, TIM_CCx_DISABLE);        
		__HAL_TIM_CLEAR_FLAG(htim, htim->Channel<<1);
		/* 使能定时器比较输出 */
		__HAL_TIM_DISABLE_IT(htim, htim->Channel<<1);
		HAL_TIM_OC_Stop(htim, htim->Channel);
		HAL_TIM_Base_Stop(htim);// 使能定时器
	}
#endif
}
/**
 * @brief  设置结束回调函数
 *  
 * @param 	Motor ： 电机句柄
 *			_end_cb ：回调函数
 * @return
 */
uint8_t motor_set_end_indicate(MotorHandle_t* Motor, void (*_end_cb)())
{
	Motor->end_cb = _end_cb;
	return 0;
}
/**
* @brief  StartMotor
*         开始定时器输出
* @param  Motor ： 电机句柄
* 
* @retval 
*/
void StartMotorTimer(MotorHandle_t* Motor)
{
	NowMotor = Motor;
	/* 使能比较输出通道 */
	TIM_CCxChannelCmd(Motor->TIMx, Motor->TIMx_CHANNEL, TIM_CCx_ENABLE);	
	/*清除定时器中断 */	
	__HAL_TIM_CLEAR_FLAG(Motor->TIMx_Handle, Motor->TIM_FLAG_CCx);
	/* 使能定时器比较输出 */
	__HAL_TIM_ENABLE_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
	HAL_TIM_OC_Start(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
	HAL_TIM_Base_Start(Motor->TIMx_Handle);// 使能定时器
}
/**
* @brief  StopMotor
*         停止定时器输出
* @param  Motor ： 电机句柄
* 
* @retval 
*/
void StopMotorTimer(MotorHandle_t* Motor)
{

	if(Motor->MotionStatus)
	{
		// 关闭通道
		TIM_CCxChannelCmd(Motor->TIMx, Motor->TIMx_CHANNEL, TIM_CCx_DISABLE);        
		__HAL_TIM_CLEAR_FLAG(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		/* 使能定时器比较输出 */
		__HAL_TIM_DISABLE_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		HAL_TIM_OC_Stop(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
		HAL_TIM_Base_Stop(Motor->TIMx_Handle);// 使能定时器
		Motor->CurrentPosition = Motor->CurrentPosition_Pulse/8;
		Motor->HalfCurrent(1); // 关闭电机半流输出
		Motor->PulseCount = 0;  // 清零步数计数器
		Motor->MotionStatus = STOP;
		NowMotor = NULL;

		/*****BEGIN*电机结束后是否延时操作***********/
		if(Motor->end_cb)
			Motor->end_cb();
		/*****END*电机结束后是否延时操作***********/
		rt_kprintf("Motor:%d,Position:%d\r\n",Motor->MotorID,Motor->CurrentPosition);
	}	
}
/**
 * @brief  处理方向和坐标位置
 *  
 * @param 
 *
 * @return 0 ：表示回零操作（包含速度控制）
 *		   1 ：处理坐标位置完成
 *错误代码 2 ： 坐标位置出错
 *错误代码 4 ： 设置速度出错
 */
static uint8_t ConfigPosition(MotorHandle_t* _Motor, int32_t Position, uint32_t _TargetSpeed)
{
	if(0 == Position) /* 表示回零 */
	{
		_Motor->dir = CCW;  // 顺时针方向旋转			
		_Motor->CCWfun(1);				 //  往后走
		if(0 == _TargetSpeed)
			_Motor->NowSpeed = 2000;
		else
		{
			if(_TargetSpeed > _Motor->MaxSpeed)/* 目标速度限制 */
				_TargetSpeed = _Motor->MaxSpeed;
			else if(_TargetSpeed <= 0)
				return 4;
			_Motor->NowSpeed = _TargetSpeed;
		}
		_Motor->TimerCompareVal = (TimeFrequency/2)/_Motor->NowSpeed;	
		_Motor->CurrentPosition_Pulse = _Motor->MaxPosition*8;
		_Motor->step_move = _Motor->MaxPosition;
		_Motor->LightSwitch = 0;
		NowMotor = _Motor;
		__HAL_TIM_SET_COMPARE(_Motor->TIMx_Handle, _Motor->TIMx_CHANNEL, _Motor->TimerCompareVal);
		_Motor->HalfCurrent(0);
		rt_thread_delay(50);	
		_Motor->error = 0;
		StartMotorTimer(_Motor);
		_Motor->MotionStatus = ConSpeed;
		return 0;
	}
	else
	{
		if(Position < _Motor->CurrentPosition && Position > _Motor->MinPosition)
		{
			_Motor->dir = CCW; // 逆时针方向旋转
			_Motor->CCWfun(1); // 往后走
			if(1 == Position)  //当位置设为1时，按设置速度回零
				_Motor->step_move = _Motor->CurrentPosition;
			else
				_Motor->step_move = _Motor->CurrentPosition - Position;
		}
		else if(Position > _Motor->CurrentPosition && Position < _Motor->MaxPosition)
		{
			_Motor->dir = CW;  // 顺时针方向旋转			
			_Motor->CCWfun(0);    // 往前走
			_Motor->step_move =  Position - _Motor->CurrentPosition;
		}
		else
			return 2;
		if(Position > 0)
			_Motor->LightSwitch = 1;
		else
			_Motor->LightSwitch = 2;

		return 1;
	}
}
/**
 * @brief  处理速度
 *  
 * @param 
 *
 * @return 0 : 返回加速状态
 *		   1 ：返回匀速状态
 *错误代码 3 ： 设置加减速时间出错
 *错误代码 4 ： 设置速度出错
 */
static uint8_t ConfigSpeed(MotorHandle_t* _Motor, uint32_t _TargetSpeed,
				    uint32_t _acceltime, uint32_t _deceltime)
{
	int32_t PulseTemp = 0;
	
	if(_TargetSpeed > _Motor->MaxSpeed)/* 目标速度限制 */
		_TargetSpeed = _Motor->MaxSpeed;
	else if(_TargetSpeed <= 0)
		return 4;

	if(0 == _acceltime && 0 == _deceltime) /* 没有加减速时，以_MaxSpeed的速度匀速运动 */
	{
		_Motor->TargetSpeed = _TargetSpeed;					
		_Motor->NowSpeed = _Motor->TargetSpeed;	
		return 1;		
	}			
	else
	{
		/* 计算加速度和减速度 */
		_Motor->step_accel = (float)(_TargetSpeed-_Motor->MinSpeed) / _acceltime;  /* 加速度 */
		_Motor->step_dccel = (float)(_TargetSpeed-_Motor->MinSpeed) / _deceltime;  /* 减速度 */
		/* 
		根据参数计算预计会运动的脉冲数
		加减速时间单位是ms，所以 /1000
		三角形的面积 / 2.
		电机细分数为8，所以 /8
		*/
		_Motor->AccelCount = (_TargetSpeed+_Motor->MinSpeed) * _acceltime/2000;
		_Motor->DccelCount = (_TargetSpeed+_Motor->MinSpeed) * _deceltime/2000;
		PulseTemp = _Motor->AccelCount + _Motor->DccelCount; 
		/* 判断最大速度 */
		if(_Motor->step_move*8 > PulseTemp)/* 梯形 */
		{
			_Motor->TargetSpeed = _TargetSpeed;
		}
		else/* 三角形 */
		{
			/* （可增加功能） */
			//Motor->MaxSpeed = step/(_acceltime+_deceltime) +  Motor->MinSpeed;
			return 3;
		}															
		_Motor->NowSpeed = _Motor->MinSpeed;
		return 0;
		
	}
}

/**
  * 函数功能: 相对位置运动：运动给定的步数
  * 说    明: 以给定的步数移动步进电机，先加速到最大速度，然后在合适位置开始
  *           减速至停止，使得整个运动距离为指定的步数。如果加减速阶段很短并且
  *           速度很慢，那还没达到最大速度就要开始减速
  *
  *			  如果没有加减速，就以最大速度 匀速运动
  * 输入参数: Motor ： 电机句柄
  *			  _position： 绝对坐标位置 单位： 步 .
  *			  _acceltime: 加速度时间,单位 ms
  *			  _deceltime:  减速度时间,单位 ms
  *			  _TargetSpeed:  目标速度,单位 Hz
  * 返 回 值: 
  *				1：正在运行中
  *				2：坐标设置错误
  *				3：参数设置错误
  */
uint8_t StepMotor_AxisMoveRel(MotorHandle_t* Motor, int32_t _position, 
							  uint32_t _acceltime, uint32_t _deceltime, 
							  uint32_t _TargetSpeed)
{ 
	uint8_t _ret = 0;
	uint32_t compare_count = 0;

	if(Motor->MotionStatus != STOP)    //只允许步进电机在停止的时候才继续
		return 1;			
	/* 获取电机ID */
	if(Motor == &Motor1)	
		Motor->MotorID = 1;
	else if(Motor == &Motor2)	
		Motor->MotorID = 2;
	else if(Motor == &Motor3)	
		Motor->MotorID = 3;
	else if(Motor == &Motor4)	
		Motor->MotorID = 4;
	else
		return 3;
	_ret = ConfigPosition(Motor,_position,_TargetSpeed);
	if(0 == _ret)
		return 0;
	else if(1 != _ret)
		return _ret;
	
	_ret = ConfigSpeed(Motor, _TargetSpeed, _acceltime, _deceltime);
	if(_ret > 1)
		return _ret;	
	
	/* 设置速度 */	
	Motor->TimerCompareVal = (TimeFrequency/2)/Motor->NowSpeed;	
	compare_count = __HAL_TIM_GET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
	__HAL_TIM_SET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL, compare_count + Motor->TimerCompareVal);	
	Motor->HalfCurrent(0);
	//rt_thread_delay(50);
	Motor->Remainder = 0;
	Motor->Reserve = 0;	
	StartMotorTimer(Motor);	
	
	if(_ret)
		Motor->MotionStatus = ConSpeed;
	else
		Motor->MotionStatus = ACCEL;
	Motor->error = 0;	

	return 0;
}

/**
* @brief  StepMotor_PeriodSet
*         设置速度参数（固定周期调用）
*					改变运动状态
* @param  Motor ： 电机句柄
* 
* @retval 
*/
void StepMotor_PeriodSet(MotorHandle_t* Motor)
{
	float F_Val;
	uint32_t Ten_Val;

	switch(Motor->MotionStatus) // 加减速曲线阶段
	{
	case STOP:
		// 关闭通道
		//StopMotor(Motor);
		//Motor->MotionStatus = STOP;  //  电机为停止状态     
		break;
	case ACCEL:				
		F_Val =  ((float)TimeFrequency/2)/Motor->NowSpeed;
		Ten_Val = F_Val * 100 + Motor->Remainder;
		Motor->Remainder = (uint32_t)Ten_Val%100;
		Motor->TimerCompareVal = Ten_Val/100;		
		Motor->NowSpeed += Motor->step_accel;					
		if(Motor->NowSpeed >= (Motor->TargetSpeed-5))
		{
			Motor->TimerCompareVal = (TimeFrequency/2)/Motor->NowSpeed;
			Motor->MotionStatus = RUN;												
		}					
		break;
	case RUN:			
		//Motor->error++;
		break;
	case DECEL:		
		//Motor->error++;		
		Motor->NowSpeed -= Motor->step_dccel;					
		if(Motor->NowSpeed <= Motor->MinSpeed)
			Motor->NowSpeed = Motor->MinSpeed;								
		F_Val =  ((float)TimeFrequency/2)/Motor->NowSpeed;
		Ten_Val = F_Val * 100 + Motor->Remainder;
		Motor->Remainder = (uint32_t)Ten_Val%100;
		Motor->TimerCompareVal = Ten_Val/100;	
		break;
	case ConSpeed:			
		break;
	default:							
		break;
	} 
}
/**
* @brief  StepMotor_ConutStep
*         计算步数（脉冲回调函数中调用）
*					执行设置速度
* @param  Motor ： 电机句柄
* 
* @retval 
*/
void StepMotor_ConutStep(MotorHandle_t* Motor)
{
	static uint32_t count = 0;

	if(__HAL_TIM_GET_IT_SOURCE(Motor->TIMx_Handle, Motor->TIM_IT_CCx) !=RESET)
	{
		// 清楚定时器中断
		__HAL_TIM_CLEAR_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		/* 执行设置速度 */
		count = __HAL_TIM_GET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
		__HAL_TIM_SET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL, count + Motor->TimerCompareVal);
	}		

	if(Motor->Reserve++ > 0)
	{
		Motor->Reserve = 0;					
		Motor->PulseCount++;        /* 计算步数 */  
		if(Motor->dir == CW)							  	
			Motor->CurrentPosition_Pulse++; // 绝对位置加1						
		else							
			Motor->CurrentPosition_Pulse--; // 绝对位置减1
		/* 判断是否运动超出范围坐标 */
		
		if(Motor->CurrentPosition_Pulse < 0)
		{
			if(Motor->CurrentPosition_Pulse < Motor->MinPosition*8)
				StopMotorTimer(Motor);
		}
		else if( Motor->CurrentPosition_Pulse >= Motor->MaxPosition*8)
		{
			StopMotorTimer(Motor);
		}
		switch(Motor->MotionStatus) 
		{
		case STOP:
			Motor->PulseCount = 0;  // 清零步数计数器
			break;
		case ACCEL:      // 加减速曲线阶段
			//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
			break;
		case RUN:			
			/* Motor->MaxSpeed/1000 这是最大速度运动1ms的脉冲  用来补偿 */
			if(Motor->PulseCount >= (Motor->step_move*8-Motor->DccelCount-Motor->TargetSpeed/2000))	
			{
				Motor->MotionStatus = DECEL;
			}
			break;
		case DECEL:									
			if(Motor->PulseCount >= Motor->step_move*8)
			{
				StopMotorTimer(Motor);								
			}		
			break;
		case ConSpeed:	
			if(Motor->PulseCount >= Motor->step_move*8)
			{
				StopMotorTimer(Motor);
			}								
			break;
		default:
			Motor->PulseCount = 0;  // 清零步数计数器
			break;
		} 					 
	}			
}







