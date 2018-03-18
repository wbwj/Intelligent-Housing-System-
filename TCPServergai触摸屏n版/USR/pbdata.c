#include "pbdata.h"
void system_init(void)
{
//	Rcc_init();
	GPIO_Config();
	USART1_config();
	USART2_Config();
	NVIC_Configuration();
}
//系统时钟初始化
void Rcc_init(void)
{
	//RCC_DeInit();//复位RCC
	RCC_HSEConfig(RCC_HSE_ON);//使能外部时钟
	RCC_HSICmd(DISABLE);//关闭内部时钟
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);//锁相环9倍频，选择外部时钟,72M
	RCC_PLLCmd(ENABLE);//使能锁相环
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//外部时钟作为系统时钟源,8M
	RCC_HCLKConfig(RCC_SYSCLK_Div1);//AHB时钟等于系统时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);		
}


/*以下两个函数调用系统SYSTICK时钟定时，精确不用改动 */
void delay_ms(uint16_t nms)
{
	uint32_t temp;
	SysTick->LOAD=9000*nms;
	SysTick->VAL=0x00;
	SysTick->CTRL=0x01;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&(!(temp&(1<<16))));
	SysTick->CTRL=0x00;
	SysTick->VAL=0x00;
}

void delay_us(uint32_t nus)
{
	uint32_t temp;
	SysTick->LOAD=9*nus;
	SysTick->VAL=0x00;
	SysTick->CTRL=0x01;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&(!(temp&(1<<16))));
	SysTick->CTRL=0x00;
	SysTick->VAL=0x00;
}

