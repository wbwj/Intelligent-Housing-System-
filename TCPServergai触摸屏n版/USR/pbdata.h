#ifndef _pbdata_H
#define _pbdata_H
//定义变量、函数
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "gpio_config.h"
#include "usart_config.h"
#include "nvic_config.h"
#include "ESP8266.h"
#include <stdbool.h>

void system_init(void);
void Rcc_init(void);
void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);


#endif 
