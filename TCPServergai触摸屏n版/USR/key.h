#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
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


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP
//#define KEY4 PEin(6)	//PE6
//#define KEY5 PAin(3)	//PA3

#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键2 
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 
#define KEY4  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6)//读取按键4
//#define KEY5  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)//读取按键5
#define KEY6  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)//读取按键6
#define KEY7  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)//读取按键7
#define KEY8  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//读取按键8
#define KEY9  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取按键9

#define KEY_LED3  10
#define KEY_LED2  9
#define KEY_CO    8
#define KEY_RE    7
//#define KEY_CLOSE 6
#define KEY_MC	  5
#define KEY_UP 		4
#define KEY_LEFT	3
#define KEY_DOWN	2
#define KEY_RIGHT	1

#define Line0_1  GPIO_SetBits  (GPIOA, GPIO_Pin_12 ) 		//enable port
#define Line0_0 GPIO_ResetBits  (GPIOA, GPIO_Pin_12 ) 
#define Line1_1  GPIO_SetBits  (GPIOA, GPIO_Pin_11) 		//enable port
#define Line1_0 GPIO_ResetBits  (GPIOA, GPIO_Pin_11 ) 
#define Line2_1  GPIO_SetBits  (GPIOA, GPIO_Pin_8) 		//enable port
#define Line2_0 GPIO_ResetBits  (GPIOA, GPIO_Pin_8 )
#define Line3_1  GPIO_SetBits  (GPIOA, GPIO_Pin_7) 		//enable port
#define Line3_0 GPIO_ResetBits  (GPIOA, GPIO_Pin_7 )


#define Column0  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)//读取按键2
#define Column1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//读取按键2
#define Column2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)//读取按键2 
#define Column3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//读取按键3 

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数	
u8 Keyboard_Scan(void);  	//按键扫描函数
#endif

