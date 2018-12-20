#include "bsp_timer.h"
#include "bsp_StepMotor.h"

TIM_HandleTypeDef TIM6_Handler;         //��ʱ����� 

  
 /**
 * @brief  TIM6_Init
 *         ������ʱ��6�жϳ�ʼ��
 *		        ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
 *		   Ft=��ʱ������Ƶ��,��λ:Mhz
 *		       ����ʹ�õ��Ƕ�ʱ��3!(��ʱ��3����APB1�ϣ�ʱ��ΪHCLK/2)
 * @param 
 *			arr���Զ���װֵ��
 *			psc��ʱ��Ԥ��Ƶ��
 * @retval 
 */

void TIM6_Init(uint16_t arr,uint16_t psc)
{  
	TIM_MasterConfigTypeDef sMasterConfig;
	
    TIM6_Handler.Instance = TIM6;                         	 //������ʱ��6
    TIM6_Handler.Init.Prescaler = psc;                    	 //��Ƶϵ��
    TIM6_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;   	 //���ϼ�����
    TIM6_Handler.Init.Period = arr;                          //�Զ�װ��ֵ
    TIM6_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM6_Handler);
		
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&TIM6_Handler, &sMasterConfig);
    
    HAL_TIM_Base_Start_IT(&TIM6_Handler); //ʹ�ܶ�ʱ��6�Ͷ�ʱ��6�����жϣ�TIM_IT_UPDATE  
	HAL_TIM_Base_Start(&TIM6_Handler);		// ʹ�ܶ�ʱ��	
}

/**
* @brief  
*         ��ʱ��6�жϷ�����
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
*        �ص���������ʱ���жϷ���������
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
			StepMotor_PeriodSet(motor[i]);
		if(motor[i]->LightSwitch) //������
		{
			if(motor[i]->optofun())
			{
				motor[i]->CurrentPosition_Pulse = 0;
				StopMotorTimer(motor[i]);
				motor[i]->LightSwitch = 0;
			}
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



