/**
  ******************************************************************************
  * @file    bsp_pwm_output.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   通用定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_general_tim.h"



 /**
  * @brief  通用定时器 TIMx,x[2,3,4,5]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQn; 
	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*
 * TIM_Period / Auto Reload Register(ARR) = 1000-1   TIM_Prescaler=((SystemCoreClock/2)/1000000)-1 
 *  *	
 * 定时器时钟源TIMxCLK = 2 * PCLK1  
 *				PCLK1 = HCLK / 4 
 *				=> TIMxCLK = HCLK / 2 = SystemCoreClock /2
 * 定时器频率为 = TIMxCLK/(TIM_Prescaler+1) = (SystemCoreClock /2)/((SystemCoreClock/2)/1000000) = 1000000 = 1MHz
 * 中断周期为 = 1/(1MHz) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> 中断 且TIMxCNT重置为0重新计数 
 */

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8,15,16,17]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// 开启TIMx_CLK,x[2,3,4,5] 
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		 
  TIM_TimeBaseStructure.TIM_Period = 1000-1;       //当定时器从0计数到999，即为1000次，为一个定时周期
	
	//定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK = HCLK / 2 = SystemCoreClock /2
	// 定时器频率为 = TIMxCLK/(TIM_Prescaler+1) = (SystemCoreClock /2)/((SystemCoreClock/2)/1000000) = 1000000 = 1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/2)/1000000)-1;	    
	
	//  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
  
	
	// 初始化定时器TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	
	// 清除计数器中断标志位
	TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);
	
	// 开启计数器中断
	TIM_ITConfig(GENERAL_TIM,TIM_IT_Update,ENABLE);
	
	// 使能计数器
	TIM_Cmd(GENERAL_TIM, ENABLE);																		


}

/**
  * @brief  初始化通用定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{

	TIM_Mode_Config();

	TIMx_NVIC_Configuration();	
}

/*********************************************END OF FILE**********************/
