#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "pbdata.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
	//初始化KEY0-->GPIOA.13,KEY1-->GPIOA.15  上拉输入
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6;//PE2~4.6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
 //矩阵键盘
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;//PE0~3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOE2,3,4
	
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;//PE0~3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOE2,3,4
}

u8 Keyboard_Scan()
{	 	
	 Line0_0;
	 Line1_1;
	 Line2_1;
	 Line3_1;
	 if(Column0==1||Column1==1||Column2==1||Column3==1){
    delay_ms(10);//去抖动 
		if(Column0==0)return 1;
		else if(Column1==0)return 2;
		else if(Column2==0)return 3;
		else if(Column3==0)return 10;
	 }
 
		Line0_1;
	  Line1_0;
	  Line2_1;
	  Line3_1;
	  if(Column0==1||Column1==1||Column2==1||Column3==1){
    delay_ms(10);//去抖动 
		if(Column0==0)return 4;
		else if(Column1==0)return 5;
		else if(Column2==0)return 6;
		else if(Column3==0)return 11;
	 }
		Line0_1;
	  Line1_1;
	  Line2_0;
	  Line3_1;
	 if(Column0==1||Column1==1||Column2==1||Column3==1){
    delay_ms(10);//去抖动 
		if(Column0==0)return 7;
		else if(Column1==0)return 8;
		else if(Column2==0)return 9;
		else if(Column3==0)return 12;
	 }
	  Line0_1;
	  Line1_1;
	  Line2_1;
	  Line3_0;
	 if(Column0==1||Column1==1||Column2==1||Column3==1){
    delay_ms(10);//去抖动 
		if(Column0==0)return 14;
		else if(Column1==0)return 16;
		else if(Column2==0)return 15;
		else if(Column3==0)return 13;
	 }
 	return 0;// 无按键按下
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!

