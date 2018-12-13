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
	/* STEPMOTOR相关GPIO初始化配置 */
	//HAL_TIM_MspPostInit(&htim_3);
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
	MOTOR1 使能EN  --> PD8
	MOTOR1 方向CWB --> PD9
	MOTOR1 半流HOLD --> PE15

	MOTOR2 使能EN  --> PE12
	MOTOR2 方向CWB --> PE10
	MOTOR2 半流HOLD --> PE8

	MOTOR3 使能EN  --> PG1
	MOTOR3 方向CWB --> PG0
	MOTOR3 半流HOLD --> PF15

	MOTOR4 使能EN  --> PF13
	MOTOR4 方向CWB --> PF12
	MOTOR4 半流HOLD --> PF11
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
	/* 控制电机电流 */
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);

	/*光耦*/
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * 函数功能: 基本定时器硬件初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim_base)
{	
	if(htim_base->Instance == TIM3)
	{
		/* 基本定时器外设时钟使能 */
		__HAL_RCC_TIM3_CLK_ENABLE();

		TIM3_GPIOInit();
		
		/* 配置定时器中断优先级并使能 */
		HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
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

	if(htim_base->Instance == TIM3)
	{
		/* 基本定时器外设时钟禁用 */
		__HAL_RCC_TIM3_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_6);
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
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
	TIM3_Init();
	TIM6_Init(1000-1, 84-1);   // 1ms 溢出中断
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
* @brief  电机半流控制
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
 * @brief 光耦检测
 *  
 * @param  LightSwitch = 0  ---> 开启回零检测
 *		   LightSwitch = 1  ---> 开启最大值检测
 * @return false 有效
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
 * @brief 光耦控制
 *        循环检测
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
			if(0 == _motor[i]->LightSwitch) //如果是回零停止，坐标置零
				_motor[i]->CurrentPosition_Pulse = 0;
			StopMotorTimer(_motor[i]);
		}
	}
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
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim_3);
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
/*
 * 设置电机当前位置
 * */
uint8_t set_current_position(MotorHandle_t* _motor, rt_int32_t _pos)
{
	_motor->CurrentPosition = _pos;
	_motor->CurrentPosition_Pulse = _pos * MOTOR_SUBDIVISION;

	return 0;
}
/*
 * 设置电机最大位置
 * */
uint8_t set_max_position(MotorHandle_t* _motor, rt_int32_t _pos)
{
	_motor->MaxPosition = _pos;
	_motor->MaxPosition_Pulse = _pos * MOTOR_SUBDIVISION;

	return 0;
}
/*
 * 设置电机最小位置
 * */
uint8_t set_min_position(MotorHandle_t* _motor, rt_int32_t _pos)
{
	_motor->MinPosition = _pos;
	_motor->MinPosition_Pulse = _pos * MOTOR_SUBDIVISION;

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
		__HAL_TIM_CLEAR_FLAG(Motor->TIMx_Handle, Motor->TIM_FLAG_CCx);
		/* 使能定时器比较输出 */
		__HAL_TIM_DISABLE_IT(Motor->TIMx_Handle, Motor->TIM_IT_CCx);
		HAL_TIM_OC_Stop(Motor->TIMx_Handle, Motor->TIMx_CHANNEL);
		HAL_TIM_Base_Stop(Motor->TIMx_Handle);// 使能定时器
		Motor->CurrentPosition = Motor->CurrentPosition_Pulse/8;
		Motor->HalfCurrent(1);  // 关闭电机半流输出
		Motor->PulseCount = 0;  // 清零步数计数器
		Motor->MotionStatus = STOP;
		NowMotor = NULL;

		/*****BEGIN*电机结束后是否延时操作***********/
		if(Motor->end_cb)
			Motor->end_cb();
		/*****END*电机结束后是否延时操作***********/
		//rt_kprintf("Motor:%d,Position:%d-%d\r\n",Motor->MotorID, Motor->CurrentPosition, Motor->CurrentPosition_Pulse);
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
static uint8_t ConfigPosition(MotorHandle_t* _Motor, int32_t Position)
{
	if(0 == Position) /* 表示回零 */
	{
		_Motor->dir = CCW;  // 顺时针方向旋转
		_Motor->CCWfun(0);				 //  往下走
		_Motor->CurrentPosition_Pulse = _Motor->MaxPosition * MOTOR_SUBDIVISION;
		_Motor->step_move = _Motor->MaxPosition;
		_Motor->LightSwitch = 1;
	}
	else
	{
		if(Position < _Motor->CurrentPosition && Position > _Motor->MinPosition)
		{
			_Motor->dir = CCW; // 逆时针方向旋转
			_Motor->CCWfun(1); // 往上走
			if(1 == Position)  //当位置设为1时，按设置速度回零
				_Motor->step_move = _Motor->CurrentPosition;
			else
				_Motor->step_move = _Motor->CurrentPosition - Position;
		}
		else if(Position > _Motor->CurrentPosition && Position < _Motor->MaxPosition)
		{
			_Motor->dir = CW;  // 顺时针方向旋转			
			_Motor->CCWfun(0);    // 往下走
			_Motor->step_move =  Position - _Motor->CurrentPosition;
		}
		else
			return 2;
	}
	return 0;
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
	
	if(_TargetSpeed > _Motor->MaxSpeed)		/* 目标速度限制 */
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
	RT_ASSERT(Motor);

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
	_ret = ConfigPosition(Motor,_position);
	if(0 != _ret)
		return _ret;
	
	_ret = ConfigSpeed(Motor, _TargetSpeed, _acceltime, _deceltime);
	if(_ret > 1)
		return _ret;	
	
	/* 设置速度 */	
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
*         计算步数（脉冲回调函数中调用）
*					执行设置速度
* @param  Motor ： 电机句柄
* 
* @retval 
*/
void StepMotor_ConutStep(MotorHandle_t* Motor)
{
	uint32_t count = 0;

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
			Motor->PulseCount = 0;  // 清零步数计数器
			break;
		case ACCEL:      // 加减速曲线阶段
			//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
			break;
		case RUN:			
			/* Motor->MaxSpeed/1000 这是最大速度运动1ms的脉冲  用来补偿 */
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
			Motor->PulseCount = 0;  // 清零步数计数器
			break;
		} 					 
	}			
}
