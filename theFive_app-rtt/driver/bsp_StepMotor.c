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
*         ��ʼ���������
* @param  Motor �� ������
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
*        TIM8��ʼ��
* @param 
* 
* @retval 
*/
void TIM8_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // ��ʱ��ʱ��
	TIM_OC_InitTypeDef sConfigOC;                          // ��ʱ��ͨ���Ƚ����
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;			
	
	/* ��ʱ�������������� */
	htim_8.Instance = TIM8;                          // ��ʱ�����
	htim_8.Init.Prescaler = TIM8_PRESCALER;           // ��ʱ��Ԥ��Ƶ��
	htim_8.Init.CounterMode = TIM_COUNTERMODE_UP;              // �����������ϼ���
	htim_8.Init.Period = STEPMOTOR_TIM_PERIOD;                 // ��ʱ������
	htim_8.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;          // ʱ�ӷ�Ƶ

	if (HAL_TIM_OC_Init(&htim_8) != HAL_OK)
	{
		RT_ASSERT(0);
	}

	/* ��ʱ��ʱ��Դ���� */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// ʹ���ڲ�ʱ��Դ
	HAL_TIM_ConfigClockSource(&htim_8, &sClockSourceConfig);

	/* ��ʱ���Ƚ�������� */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // �Ƚ����ģʽ����ת���
	sConfigOC.Pulse = 0;                                  // ������
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // �������
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // ����ͨ���������
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // ����ģʽ
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // ���е�ƽ
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // ����ͨ�����е�ƽ
	
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
	
	/* STEPMOTOR���GPIO��ʼ������ */
	//HAL_TIM_MspPostInit(&htim_8);
}
/**
* @brief  
*        TIM2��ʼ��
* @param 
* 
* @retval 
*/
void TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // ��ʱ��ʱ��
	TIM_OC_InitTypeDef sConfigOC;                          // ��ʱ��ͨ���Ƚ����		
	
	/* ��ʱ�������������� */
	htim_2.Instance = TIM2;                          // ��ʱ�����
	htim_2.Init.Prescaler = TIM2_PRESCALER;           // ��ʱ��Ԥ��Ƶ��
	htim_2.Init.CounterMode = TIM_COUNTERMODE_UP;              // �����������ϼ���
	htim_2.Init.Period = 0xFFFFFFFF;                 // ��ʱ������  0xFFFFFFFF
	htim_2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;          // ʱ�ӷ�Ƶ
	HAL_TIM_OC_Init(&htim_2);

	/* ��ʱ��ʱ��Դ���� */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// ʹ���ڲ�ʱ��Դ
	HAL_TIM_ConfigClockSource(&htim_2, &sClockSourceConfig);

	/* ��ʱ���Ƚ�������� */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // �Ƚ����ģʽ����ת���
	sConfigOC.Pulse = 1;                                  // ������
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // �������
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // ����ͨ���������
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // ����ģʽ
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // ���е�ƽ
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // ����ͨ�����е�ƽ
	if(HAL_TIM_OC_ConfigChannel(&htim_2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		RT_ASSERT(0);
	}

	/* STEPMOTOR���GPIO��ʼ������ */
	//HAL_TIM_MspPostInit(&htim_2);
}
/**
* @brief  
*        TIM3��ʼ��
* @param 
* 
* @retval 
*/
void TIM3_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // ��ʱ��ʱ��
	TIM_OC_InitTypeDef sConfigOC;                          // ��ʱ��ͨ���Ƚ����		
	
	/* ��ʱ�������������� */
	htim_3.Instance = TIM3;                          // ��ʱ�����
	htim_3.Init.Prescaler = TIM3_PRESCALER;           // ��ʱ��Ԥ��Ƶ��
	htim_3.Init.CounterMode = TIM_COUNTERMODE_UP;              // �����������ϼ���
	htim_3.Init.Period = STEPMOTOR_TIM_PERIOD;                 // ��ʱ������
	htim_3.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;          // ʱ�ӷ�Ƶ
	HAL_TIM_OC_Init(&htim_3);

	/* ��ʱ��ʱ��Դ���� */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// ʹ���ڲ�ʱ��Դ
	HAL_TIM_ConfigClockSource(&htim_3, &sClockSourceConfig);

	/* ��ʱ���Ƚ�������� */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // �Ƚ����ģʽ����ת���
	sConfigOC.Pulse = 0;                                  // ������
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // �������
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // ����ͨ���������
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // ����ģʽ
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // ���е�ƽ
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // ����ͨ�����е�ƽ
	
	HAL_TIM_OC_ConfigChannel(&htim_3, &sConfigOC, TIM_CHANNEL_1);
	
	/* STEPMOTOR���GPIO��ʼ������ */
	//HAL_TIM_MspPostInit(&htim_3);
}
/**
* @brief  
*        TIM4��ʼ��
* @param 
* 
* @retval 
*/
void TIM4_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;             // ��ʱ��ʱ��
	TIM_OC_InitTypeDef sConfigOC;                          // ��ʱ��ͨ���Ƚ����		
	
	/* ��ʱ�������������� */
	htim_4.Instance = TIM4;                          // ��ʱ�����
	htim_4.Init.Prescaler = TIM4_PRESCALER;           // ��ʱ��Ԥ��Ƶ��
	htim_4.Init.CounterMode = TIM_COUNTERMODE_UP;              // �����������ϼ���
	htim_4.Init.Period = STEPMOTOR_TIM_PERIOD;                 // ��ʱ������
	htim_4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;          // ʱ�ӷ�Ƶ
	HAL_TIM_OC_Init(&htim_4);

	/* ��ʱ��ʱ��Դ���� */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;       		// ʹ���ڲ�ʱ��Դ
	HAL_TIM_ConfigClockSource(&htim_4, &sClockSourceConfig);

	/* ��ʱ���Ƚ�������� */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                 // �Ƚ����ģʽ����ת���
	sConfigOC.Pulse = 0;                                  // ������
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;           // �������
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;         // ����ͨ���������
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;            // ����ģʽ
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;        // ���е�ƽ
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;      // ����ͨ�����е�ƽ
	if(HAL_TIM_OC_ConfigChannel(&htim_4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		RT_ASSERT(0);
	}
	/* STEPMOTOR���GPIO��ʼ������ */
	//HAL_TIM_MspPostInit(&htim_4);
}

/**
* @brief  TIM8_GPIOInit
*         TIM8��IO��ʼ��
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
	MOTOR1ʹ��EN PD8
	MOTOR1����CWB PD9
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

	/* ���Ƶ������ */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
}
/**
* @brief  TIM3_GPIOInit
*         TIM3��IO��ʼ��
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
	MOTOR3 ʹ��EN  PG1
	MOTOR3 ����CWB PG0
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	/* TQ2 PF15*/
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);


	/* ���Ƶ������ */
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);
}
/**
* @brief  TIM4_GPIOInit
*         TIM4��IO��ʼ��
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
	MOTOR2CWB ʹ�� PE12 
	MOTOR2CWB ���� PE10
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* ���Ƶ������ PE8*/
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
}
/**
* @brief  TIM2_GPIOInit
*         TIM2��IO��ʼ��
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
	MOTOR4 ʹ��EN  PF13
	MOTOR4 ����CWB PF12
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);		

	/* ���Ƶ������ */
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11 | GPIO_PIN_13, GPIO_PIN_SET);
}
/**
  * ��������: ������ʱ��Ӳ����ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim_base)
{	
	if(htim_base->Instance == TIM8)
	{
		/* ������ʱ������ʱ��ʹ�� */
		__HAL_RCC_TIM8_CLK_ENABLE();

		TIM8_GPIOInit();  
		
		/* ���ö�ʱ���ж����ȼ���ʹ�� */
		HAL_NVIC_SetPriority(TIM8_CC_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
	}
	if(htim_base->Instance == TIM2)
	{
		/* ������ʱ������ʱ��ʹ�� */
		__HAL_RCC_TIM2_CLK_ENABLE();

		TIM2_GPIOInit();
		
		/* ���ö�ʱ���ж����ȼ���ʹ�� */
		HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
	if(htim_base->Instance == TIM3)
	{
		/* ������ʱ������ʱ��ʹ�� */
		__HAL_RCC_TIM3_CLK_ENABLE();

		TIM3_GPIOInit();
		
		/* ���ö�ʱ���ж����ȼ���ʹ�� */
		HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
	if(htim_base->Instance == TIM4)
	{
		/* ������ʱ������ʱ��ʹ�� */
		__HAL_RCC_TIM4_CLK_ENABLE();

		TIM4_GPIOInit();
		
		/* ���ö�ʱ���ж����ȼ���ʹ�� */
		HAL_NVIC_SetPriority(TIM4_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
	}
}

/**
  * ��������: ������ʱ��Ӳ������ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef* htim_base)
{

	if(htim_base->Instance == TIM4)
	{
		/* ������ʱ������ʱ�ӽ��� */
		__HAL_RCC_TIM4_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOD,GPIO_PIN_12);
		HAL_NVIC_DisableIRQ(TIM4_IRQn);
	}
	else if(htim_base->Instance == TIM3)
	{
		/* ������ʱ������ʱ�ӽ��� */
		__HAL_RCC_TIM3_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_6);
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
	}
	else if(htim_base->Instance == TIM2)
	{
		/* ������ʱ������ʱ�ӽ��� */
		__HAL_RCC_TIM2_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_5);
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
	}
	else if(htim_base->Instance == TIM8)
	{
		/* ������ʱ������ʱ�ӽ��� */
		__HAL_RCC_TIM8_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOC,GPIO_PIN_6);
		HAL_NVIC_DisableIRQ(TIM8_CC_IRQn);
	}
}
/**
 * @brief ��ʼ�����
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
* @brief  �����ת����
*        
* @param  _bc : GPIO_PIN_SET   ��ʱ��
*  		  _bc : GPIO_PIN_RESET ˳ʱ��
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
* @brief  �����������
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
 * @brief ������
 *  
 * @param  LightSwitch = 0  ---> ����������
 *		   LightSwitch = 1  ---> �������ֵ���
 * @return false ��Ч
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
 * @brief �������
 *        ѭ�����
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
			if(0 == NowMotor->LightSwitch) //����ǻ���ֹͣ����������
				NowMotor->CurrentPosition_Pulse = 0;
			StopMotorTimer(NowMotor);
		}	
	}
#endif
}
/**
 * @brief  ��ʱ���жϺ���
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
* @brief  ��ʱ���жϻص�����
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
		/* ʹ�ܶ�ʱ���Ƚ���� */
		__HAL_TIM_DISABLE_IT(htim, htim->Channel<<1);
		HAL_TIM_OC_Stop(htim, htim->Channel);
		HAL_TIM_Base_Stop(htim);// ʹ�ܶ�ʱ��
	}
#endif
}
/**
 * @brief  ���ý����ص�����
 *  
 * @param 	Motor �� ������
 *			_end_cb ���ص�����
 * @return
 */
uint8_t motor_set_end_indicate(MotorHandle_t* Motor, void (*_end_cb)())
{
	Motor->end_cb = _end_cb;
	return 0;
}
/**
* @brief  StartMotor
*         ��ʼ��ʱ�����
* @param  Motor �� ������
* 
* @retval 
*/
void StartMotorTimer(MotorHandle_t* Motor)
{
	NowMotor = Motor;
	/* ʹ�ܱȽ����ͨ�� */
	TIM_CCxChannelCmd(Motor->TIMx, Motor->TIMx_CHANNEL, TIM_CCx_ENABLE);	
	/*�����ʱ���ж� */	
	__HAL_TIM_CLEAR_FLAG(Motor->TIMx_Handle, Motor->TIM_FLAG_CCx);
	/* ʹ�ܶ�ʱ���Ƚ���� */
	__HAL_TIM_ENABLE_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
	HAL_TIM_OC_Start(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
	HAL_TIM_Base_Start(Motor->TIMx_Handle);// ʹ�ܶ�ʱ��
}
/**
* @brief  StopMotor
*         ֹͣ��ʱ�����
* @param  Motor �� ������
* 
* @retval 
*/
void StopMotorTimer(MotorHandle_t* Motor)
{

	if(Motor->MotionStatus)
	{
		// �ر�ͨ��
		TIM_CCxChannelCmd(Motor->TIMx, Motor->TIMx_CHANNEL, TIM_CCx_DISABLE);        
		__HAL_TIM_CLEAR_FLAG(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		/* ʹ�ܶ�ʱ���Ƚ���� */
		__HAL_TIM_DISABLE_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		HAL_TIM_OC_Stop(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
		HAL_TIM_Base_Stop(Motor->TIMx_Handle);// ʹ�ܶ�ʱ��
		Motor->CurrentPosition = Motor->CurrentPosition_Pulse/8;
		Motor->HalfCurrent(1); // �رյ���������
		Motor->PulseCount = 0;  // ���㲽��������
		Motor->MotionStatus = STOP;
		NowMotor = NULL;

		/*****BEGIN*����������Ƿ���ʱ����***********/
		if(Motor->end_cb)
			Motor->end_cb();
		/*****END*����������Ƿ���ʱ����***********/
		rt_kprintf("Motor:%d,Position:%d\r\n",Motor->MotorID,Motor->CurrentPosition);
	}	
}
/**
 * @brief  �����������λ��
 *  
 * @param 
 *
 * @return 0 ����ʾ��������������ٶȿ��ƣ�
 *		   1 ����������λ�����
 *������� 2 �� ����λ�ó���
 *������� 4 �� �����ٶȳ���
 */
static uint8_t ConfigPosition(MotorHandle_t* _Motor, int32_t Position, uint32_t _TargetSpeed)
{
	if(0 == Position) /* ��ʾ���� */
	{
		_Motor->dir = CCW;  // ˳ʱ�뷽����ת			
		_Motor->CCWfun(1);				 //  ������
		if(0 == _TargetSpeed)
			_Motor->NowSpeed = 2000;
		else
		{
			if(_TargetSpeed > _Motor->MaxSpeed)/* Ŀ���ٶ����� */
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
			_Motor->dir = CCW; // ��ʱ�뷽����ת
			_Motor->CCWfun(1); // ������
			if(1 == Position)  //��λ����Ϊ1ʱ���������ٶȻ���
				_Motor->step_move = _Motor->CurrentPosition;
			else
				_Motor->step_move = _Motor->CurrentPosition - Position;
		}
		else if(Position > _Motor->CurrentPosition && Position < _Motor->MaxPosition)
		{
			_Motor->dir = CW;  // ˳ʱ�뷽����ת			
			_Motor->CCWfun(0);    // ��ǰ��
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
 * @brief  �����ٶ�
 *  
 * @param 
 *
 * @return 0 : ���ؼ���״̬
 *		   1 ����������״̬
 *������� 3 �� ���üӼ���ʱ�����
 *������� 4 �� �����ٶȳ���
 */
static uint8_t ConfigSpeed(MotorHandle_t* _Motor, uint32_t _TargetSpeed,
				    uint32_t _acceltime, uint32_t _deceltime)
{
	int32_t PulseTemp = 0;
	
	if(_TargetSpeed > _Motor->MaxSpeed)/* Ŀ���ٶ����� */
		_TargetSpeed = _Motor->MaxSpeed;
	else if(_TargetSpeed <= 0)
		return 4;

	if(0 == _acceltime && 0 == _deceltime) /* û�мӼ���ʱ����_MaxSpeed���ٶ������˶� */
	{
		_Motor->TargetSpeed = _TargetSpeed;					
		_Motor->NowSpeed = _Motor->TargetSpeed;	
		return 1;		
	}			
	else
	{
		/* ������ٶȺͼ��ٶ� */
		_Motor->step_accel = (float)(_TargetSpeed-_Motor->MinSpeed) / _acceltime;  /* ���ٶ� */
		_Motor->step_dccel = (float)(_TargetSpeed-_Motor->MinSpeed) / _deceltime;  /* ���ٶ� */
		/* 
		���ݲ�������Ԥ�ƻ��˶���������
		�Ӽ���ʱ�䵥λ��ms������ /1000
		�����ε���� / 2.
		���ϸ����Ϊ8������ /8
		*/
		_Motor->AccelCount = (_TargetSpeed+_Motor->MinSpeed) * _acceltime/2000;
		_Motor->DccelCount = (_TargetSpeed+_Motor->MinSpeed) * _deceltime/2000;
		PulseTemp = _Motor->AccelCount + _Motor->DccelCount; 
		/* �ж�����ٶ� */
		if(_Motor->step_move*8 > PulseTemp)/* ���� */
		{
			_Motor->TargetSpeed = _TargetSpeed;
		}
		else/* ������ */
		{
			/* �������ӹ��ܣ� */
			//Motor->MaxSpeed = step/(_acceltime+_deceltime) +  Motor->MinSpeed;
			return 3;
		}															
		_Motor->NowSpeed = _Motor->MinSpeed;
		return 0;
		
	}
}

/**
  * ��������: ���λ���˶����˶������Ĳ���
  * ˵    ��: �Ը����Ĳ����ƶ�����������ȼ��ٵ�����ٶȣ�Ȼ���ں���λ�ÿ�ʼ
  *           ������ֹͣ��ʹ�������˶�����Ϊָ���Ĳ���������Ӽ��ٽ׶κ̲ܶ���
  *           �ٶȺ������ǻ�û�ﵽ����ٶȾ�Ҫ��ʼ����
  *
  *			  ���û�мӼ��٣���������ٶ� �����˶�
  * �������: Motor �� ������
  *			  _position�� ��������λ�� ��λ�� �� .
  *			  _acceltime: ���ٶ�ʱ��,��λ ms
  *			  _deceltime:  ���ٶ�ʱ��,��λ ms
  *			  _TargetSpeed:  Ŀ���ٶ�,��λ Hz
  * �� �� ֵ: 
  *				1������������
  *				2���������ô���
  *				3���������ô���
  */
uint8_t StepMotor_AxisMoveRel(MotorHandle_t* Motor, int32_t _position, 
							  uint32_t _acceltime, uint32_t _deceltime, 
							  uint32_t _TargetSpeed)
{ 
	uint8_t _ret = 0;
	uint32_t compare_count = 0;

	if(Motor->MotionStatus != STOP)    //ֻ�����������ֹͣ��ʱ��ż���
		return 1;			
	/* ��ȡ���ID */
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
	
	/* �����ٶ� */	
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
*         �����ٶȲ������̶����ڵ��ã�
*					�ı��˶�״̬
* @param  Motor �� ������
* 
* @retval 
*/
void StepMotor_PeriodSet(MotorHandle_t* Motor)
{
	float F_Val;
	uint32_t Ten_Val;

	switch(Motor->MotionStatus) // �Ӽ������߽׶�
	{
	case STOP:
		// �ر�ͨ��
		//StopMotor(Motor);
		//Motor->MotionStatus = STOP;  //  ���Ϊֹͣ״̬     
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
*         ���㲽��������ص������е��ã�
*					ִ�������ٶ�
* @param  Motor �� ������
* 
* @retval 
*/
void StepMotor_ConutStep(MotorHandle_t* Motor)
{
	static uint32_t count = 0;

	if(__HAL_TIM_GET_IT_SOURCE(Motor->TIMx_Handle, Motor->TIM_IT_CCx) !=RESET)
	{
		// �����ʱ���ж�
		__HAL_TIM_CLEAR_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		/* ִ�������ٶ� */
		count = __HAL_TIM_GET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
		__HAL_TIM_SET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL, count + Motor->TimerCompareVal);
	}		

	if(Motor->Reserve++ > 0)
	{
		Motor->Reserve = 0;					
		Motor->PulseCount++;        /* ���㲽�� */  
		if(Motor->dir == CW)							  	
			Motor->CurrentPosition_Pulse++; // ����λ�ü�1						
		else							
			Motor->CurrentPosition_Pulse--; // ����λ�ü�1
		/* �ж��Ƿ��˶�������Χ���� */
		
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
			Motor->PulseCount = 0;  // ���㲽��������
			break;
		case ACCEL:      // �Ӽ������߽׶�
			//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
			break;
		case RUN:			
			/* Motor->MaxSpeed/1000 ��������ٶ��˶�1ms������  �������� */
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
			Motor->PulseCount = 0;  // ���㲽��������
			break;
		} 					 
	}			
}







