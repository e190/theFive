#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "bsp_timer.h"
#include "bsp_StepMotor.h"

TIM_HandleTypeDef htim_3;

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
	Motor1.MaxPosition = 5500;
	Motor1.MinPosition = -5500;
	Motor1.activeChannel = HAL_TIM_ACTIVE_CHANNEL_4;
	Motor1.TIMx_Handle = &htim_3;
	Motor1.TIMx_CHANNEL = TIM_CHANNEL_4;
	Motor1.TIM_IT_CCx = TIM_IT_CC4;
	Motor1.TIMx = TIM3;

	Motor1.CCWfun = M1_CCW;
	Motor1.LightSwitch = 0;
	Motor1.HalfCurrent = M1_HalfCurrent;
	Motor1.optofun = M1_optofun;
	Motor1.end_cb = NULL;
#endif
#if MOTOR2_EN
	Motor2.MinSpeed = 1000;
	Motor2.MaxSpeed = 12000;
	Motor2.CurrentPosition = 0;
	Motor2.MaxPosition = 5500;
	Motor2.MinPosition = -5500;
	Motor2.LightSwitch = 0;
	Motor2.activeChannel = HAL_TIM_ACTIVE_CHANNEL_3;
	Motor2.TIMx_Handle = &htim_3;
	Motor2.TIMx_CHANNEL = TIM_CHANNEL_3;
	Motor2.TIM_IT_CCx = TIM_IT_CC3;
	Motor2.TIMx = TIM3;

	Motor2.CCWfun = M2_CCW;
	Motor2.HalfCurrent = M2_HalfCurrent;
	Motor2.optofun = M2_optofun;
	Motor2.end_cb = NULL;
#endif
#if MOTOR3_EN
	Motor3.MinSpeed = 1000;
	Motor3.MaxSpeed = 12000;
	Motor3.CurrentPosition = 0;
	Motor3.MaxPosition = 5500;
	Motor3.MinPosition = -5500;
	Motor3.LightSwitch = 0;
	Motor3.activeChannel = HAL_TIM_ACTIVE_CHANNEL_2;
	Motor3.TIMx_Handle = &htim_3;
	Motor3.TIMx_CHANNEL = TIM_CHANNEL_2;
	Motor3.TIM_IT_CCx = TIM_IT_CC2;
	Motor3.TIMx = TIM3;
	Motor3.CCWfun = M3_CCW;
	Motor3.HalfCurrent = M3_HalfCurrent;
	Motor3.optofun = M3_optofun;
	Motor3.end_cb = NULL;
#endif
#if MOTOR4_EN
	Motor4.MinSpeed = 1000;
	Motor4.MaxSpeed = 12000;
	Motor4.CurrentPosition = 0;
	Motor4.MaxPosition = 5500;
	Motor4.MinPosition = -5500;
	Motor4.LightSwitch = 0;
	Motor4.activeChannel = HAL_TIM_ACTIVE_CHANNEL_1;
	Motor4.TIMx_Handle = &htim_3;
	Motor4.TIMx_CHANNEL = TIM_CHANNEL_1;
	Motor4.TIM_IT_CCx = TIM_IT_CC1;
	Motor4.TIMx = TIM3;
	Motor4.CCWfun = M4_CCW;
	Motor4.HalfCurrent = M4_HalfCurrent;
	Motor4.optofun = M4_optofun;
	Motor4.end_cb = NULL;
#endif

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
	
    if (HAL_TIM_OC_ConfigChannel(&htim_3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        RT_ASSERT(0);
    }
    if (HAL_TIM_OC_ConfigChannel(&htim_3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        RT_ASSERT(0);
    }
    if (HAL_TIM_OC_ConfigChannel(&htim_3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        RT_ASSERT(0);
    }
    if (HAL_TIM_OC_ConfigChannel(&htim_3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        RT_ASSERT(0);
    }
	/* STEPMOTOR���GPIO��ʼ������ */
	//HAL_TIM_MspPostInit(&htim_3);
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
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	/**TIM3 GPIO Configuration    
	PA6     ------> TIM3_CH1 
	PA7     ------> TIM3_CH2
	PB0     ------> TIM3_CH3
	PB1     ------> TIM3_CH4
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);
	/* 
	MOTOR1 ʹ��EN  --> PD8
	MOTOR1 ����CWB --> PD9
	MOTOR1 ����HOLD --> PE15

	MOTOR2 ʹ��EN  --> PE12
	MOTOR2 ����CWB --> PE10
	MOTOR2 ����HOLD --> PE8

	MOTOR3 ʹ��EN  --> PG1
	MOTOR3 ����CWB --> PG0
	MOTOR3 ����HOLD --> PF15

	MOTOR4 ʹ��EN  --> PF13
	MOTOR4 ����CWB --> PF12
	MOTOR4 ����HOLD --> PF11
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	/* ���Ƶ������ */
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);

	/*����*/
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * ��������: ������ʱ��Ӳ����ʼ������
  * �������: htim_base��������ʱ���������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim_base)
{	
	if(htim_base->Instance == TIM3)
	{
		/* ������ʱ������ʱ��ʹ�� */
		__HAL_RCC_TIM3_CLK_ENABLE();

		TIM3_GPIOInit();
		
		/* ���ö�ʱ���ж����ȼ���ʹ�� */
		HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
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

	if(htim_base->Instance == TIM3)
	{
		/* ������ʱ������ʱ�ӽ��� */
		__HAL_RCC_TIM3_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_6);
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
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
	TIM3_Init();
	TIM6_Init(1000-1, 84-1);   // 1ms ����ж�
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
static void M1_CCW(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, _bc);
}
static void M2_CCW(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, _bc);
}
static void M3_CCW(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, _bc);
}
static void M4_CCW(rt_uint8_t _bc)
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
static void M1_HalfCurrent(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, _bc);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, _bc);
}
static void M2_HalfCurrent(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_12, _bc);
}
static void M3_HalfCurrent(rt_uint8_t _bc)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, _bc);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, _bc);
}
static void M4_HalfCurrent(rt_uint8_t _bc)
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
static rt_uint8_t M1_optofun(void)
{
	return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0);
}
static rt_uint8_t M2_optofun(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
}
static rt_uint8_t M3_optofun(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);
}
static rt_uint8_t M4_optofun(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
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
	MotorHandle_t* _motor[4] = {&Motor1, &Motor2, &Motor3, &Motor4,};
	for(rt_uint8_t i = 0;i<4;i++)
	{
		if(0 == _motor[i]->optofun())
		{
			if(0 == _motor[i]->LightSwitch) //����ǻ���ֹͣ����������
				_motor[i]->CurrentPosition_Pulse = 0;
			StopMotorTimer(_motor[i]);
		}
	}
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
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim_3);
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
		if(htim == motor[i]->TIMx_Handle && htim->Channel == motor[i]->activeChannel)
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
/*
 * ���õ����ǰλ��
 * */
uint8_t set_current_position(MotorHandle_t* _motor, rt_int32_t _pos)
{
	_motor->CurrentPosition = _pos;
	_motor->CurrentPosition_Pulse = _pos * MOTOR_SUBDIVISION;

	return 0;
}
/*
 * ���õ�����λ��
 * */
uint8_t set_max_position(MotorHandle_t* _motor, rt_int32_t _pos)
{
	_motor->MaxPosition = _pos;
	_motor->MaxPosition_Pulse = _pos * MOTOR_SUBDIVISION;

	return 0;
}
/*
 * ���õ����Сλ��
 * */
uint8_t set_min_position(MotorHandle_t* _motor, rt_int32_t _pos)
{
	_motor->MinPosition = _pos;
	_motor->MinPosition_Pulse = _pos * MOTOR_SUBDIVISION;

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
		__HAL_TIM_CLEAR_FLAG(Motor->TIMx_Handle, Motor->TIM_FLAG_CCx);
		/* ʹ�ܶ�ʱ���Ƚ���� */
		__HAL_TIM_DISABLE_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		HAL_TIM_OC_Stop(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
		HAL_TIM_Base_Stop(Motor->TIMx_Handle);// ʹ�ܶ�ʱ��
		Motor->CurrentPosition = Motor->CurrentPosition_Pulse/8;
		Motor->HalfCurrent(1);  // �رյ���������
		Motor->PulseCount = 0;  // ���㲽��������
		Motor->MotionStatus = STOP;
		NowMotor = NULL;

		/*****BEGIN*����������Ƿ���ʱ����***********/
		if(Motor->end_cb)
			Motor->end_cb();
		/*****END*����������Ƿ���ʱ����***********/
		//rt_kprintf("Motor:%d,Position:%d-%d\r\n",Motor->MotorID, Motor->CurrentPosition, Motor->CurrentPosition_Pulse);
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
static uint8_t ConfigPosition(MotorHandle_t* _Motor, int32_t Position)
{
	if(0 == Position) /* ��ʾ���� */
	{
		_Motor->dir = CCW;  // ˳ʱ�뷽����ת
		_Motor->CCWfun(0);				 //  ������
		_Motor->CurrentPosition_Pulse = _Motor->MaxPosition * MOTOR_SUBDIVISION;
		_Motor->step_move = _Motor->MaxPosition;
		_Motor->LightSwitch = 1;
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
			_Motor->CCWfun(0);    // ������
			_Motor->step_move =  Position - _Motor->CurrentPosition;
		}
		else
			return 2;
	}
	return 0;
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
	
	if(_TargetSpeed > _Motor->MaxSpeed)		/* Ŀ���ٶ����� */
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
	RT_ASSERT(Motor);

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
	_ret = ConfigPosition(Motor,_position);
	if(0 != _ret)
		return _ret;
	
	_ret = ConfigSpeed(Motor, _TargetSpeed, _acceltime, _deceltime);
	if(_ret > 1)
		return _ret;	
	
	/* �����ٶ� */	
	Motor->TimerCompareVal = (TimeFrequency/2)/Motor->NowSpeed;	
	compare_count = __HAL_TIM_GET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
	__HAL_TIM_SET_COMPARE(Motor->TIMx_Handle, Motor->TIMx_CHANNEL, compare_count + Motor->TimerCompareVal);	
	Motor->HalfCurrent(0);
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
	case DECEL:
		Motor->NowSpeed -= Motor->step_dccel;					
		if(Motor->NowSpeed <= Motor->MinSpeed)
			Motor->NowSpeed = Motor->MinSpeed;								
		F_Val =  ((float)TimeFrequency/2)/Motor->NowSpeed;
		Ten_Val = F_Val * 100 + Motor->Remainder;
		Motor->Remainder = (uint32_t)Ten_Val%100;
		Motor->TimerCompareVal = Ten_Val/100;	
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
	uint32_t count = 0;

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
			if(Motor->CurrentPosition_Pulse < Motor->MinPosition * MOTOR_SUBDIVISION)
				StopMotorTimer(Motor);
		}
		else if( Motor->CurrentPosition_Pulse >= Motor->MaxPosition * MOTOR_SUBDIVISION)
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
			if(Motor->PulseCount >= (Motor->step_move * MOTOR_SUBDIVISION \
								- Motor->DccelCount - Motor->TargetSpeed/2000))
			{
				Motor->MotionStatus = DECEL;
			}
			break;
		case DECEL:									
			if(Motor->PulseCount >= Motor->step_move * MOTOR_SUBDIVISION)
			{
				StopMotorTimer(Motor);								
			}		
			break;
		case ConSpeed:	
			if(Motor->PulseCount >= Motor->step_move * MOTOR_SUBDIVISION)
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
