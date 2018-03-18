#ifndef _usart_config_h
#define _usart_config_h

#include "pbdata.h"
#include "stdio.h"	
#include "sys.h" 
void USART1_config(void);
void USART2_Config(void);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
char *itoa( int value, char *string, int radix );
void USART2_printf( USART_TypeDef* USARTx, char *Data, ... );

#endif
