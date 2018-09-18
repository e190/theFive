#include "bsp_timer.h"
#include "bsp_StepMotor.h"

TIM_HandleTypeDef TIM6_Handler;         //定时器句柄 

  
 /**
 * @brief  TIM6_Init
 *         基本定时器6中断初始化
 *		   定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
 *		   Ft=定时器工作频率,单位:Mhz
 *		   这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
 * @param 
 *			arr：自动重装值。
 *			psc：时钟预分频数
 * @retval 
 */

void TIM6_Init(uint16_t arr,uint16_t psc)
{  
	  TIM_MasterConfigTypeDef sMasterConfig;
	
    TIM6_Handler.Instance = TIM6;                         	 //基本定时器6
    TIM6_Handler.Init.Prescaler = psc;                    	 //分频系数
    TIM6_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;   	 //向上计数器
    TIM6_Handler.Init.Period = arr;                          //自动装载值
    TIM6_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM6_Handler);
		
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&TIM6_Handler, &sMasterConfig);
    
    HAL_TIM_Base_Start_IT(&TIM6_Handler); //使能定时器6和定时器6更新中断：TIM_IT_UPDATE  
	HAL_TIM_Base_Start(&TIM6_Handler);		// 使能定时器	
}
/**
* @brief  
*         定时器底册驱动，开启时钟，设置中断优先级
*					此函数会被HAL_TIM_Base_Init()函数调用
* @param 
* 
* @retval 
*/

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();           		  //使能TIM3时钟		
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn,0,0);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);          //开启ITM6中断   
	}
}
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance == TIM6)
	{
		__HAL_RCC_TIM6_CLK_DISABLE();           //使能TIM3时钟	
		HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);          //开启ITM6中断   
	}
}
/**
* @brief  
*         定时器6中断服务函数
* @param 
* 
* @retval 
*/

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM6_Handler);
}
/**
* @brief  
*        回调函数，定时器中断服务函数调用
* @param 
* 
* @retval 
*/


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#if (MOTOR_MULTI)
	MotorHandle_t* motor[4] = {&Motor1, &Motor2, &Motor3, &Motor4};
	for(rt_uint8_t i=0;i<4;i++)
	{
		if(motor[i]->MotionStatus != 0)	
		{
			StepMotor_PeriodSet(motor[i]);
		}
	
	}
#else	
    if(htim == (&TIM6_Handler))
    {
		if(NowMotor)
			StepMotor_PeriodSet(NowMotor);
    }
#endif
}




