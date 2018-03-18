#include "pbdata.h"
#include "key.h"
#include "lcd.h"
#include "sys.h"
#include "led.h"
#include "beep.h"
#include "dht11.h" 	
#include "adc.h"
#include "timer.h"
#include "stmflash.h"
#include "oled.h"
#include "touch.h"
#include "24cxx.h" 
extern u8 CNT,CNT2,flag;                //TIM3、TIM2计时  TIM2标志位
void Temp_Humi_Coal(void);              //温湿度煤气检测
void Input_password(void);              //密码输入
void Modify_password(void);             //修改密码
void Out_door(void);                    //门外密码操作
extern  u16 adcx;                       //    
extern  float temp;	 
extern  u8 t,a,b; 
extern	u8 temperature;  	              //温度
extern  u8 humidity;                    //湿度
extern  u8 coal;                        //煤气浓度阈值
    char line,line2,code2,first_code,second_code,third_code,fourth_code=10,
			   first_code2,second_code2,third_code2,fourth_code2=10;          //门内门外密码参数
		int code;
	  char  buf_code[4]={0};               //设定密码
		u16   total=0,total2=0,all=0;        //计算密码参数
		#define SIZE sizeof(buf_code)	 			  	//数组长度
    #define FLASH_SAVE_ADDR_START  0X08070000//0X08008000 FLASH存储起始地址
		#define FLASH_SAVE_ADDR_END    0X080703F0//0X0800C000
		u8 datatemp[SIZE];
//		u8 quit_flag;
int main(void)
{
	system_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  LED_Init();		  		//初始化与LED连接的硬件接口
	LCD_Init();			   	//初始化LCD  
	OLED_Init();			  //初始化OLED 
	Adc_Init();		  		//ADC初始化
	BEEP_Init();        //蜂鸣器初始化
  TIM3_Int_Init(9999,7199);
	TIM2_Int_Init(9999,7199);
	TIM4_Int_Init(9999,7199);
	KEY_Init();         //按键初始化
	tp_dev.init();
 	POINT_COLOR=RED;		//设置字体为红色
  LED1=LED2=0;
	    	
 	while(DHT11_Init())	//DHT11初始化	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}	
	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
  LCD_ShowString(30,190,200,16,16,"Coal:  %");
	OLED_ShowString(0,20,"Input password:"); 
	OLED_Refresh_Gram();
	BEEP=0;
		
	while(1)
	{	 
		a=KEY_Scan(0);
		b=Keyboard_Scan();
			if(a==KEY_RIGHT||b==14)          //开启门禁
		{
		LED0=0;
		LED1=0;
		LCD_ShowString(20,80,220,16,16,"Access control has started");
		OLED_ShowString(10,5,"Code started"); 
		OLED_Refresh_Gram();	
		while(1)
		{
		a=KEY_Scan(0);                        //门打开
		if(a==KEY_MC)
		{	
			TIM_Cmd(TIM3, ENABLE);
			Input_password();                  //密码操作
      break;			
			}
		Temp_Humi_Coal();
	  }
  	}
		if(a==KEY_DOWN)                       //开启wifi
		{   
			  LED2=0;
		  	LCD_ShowString(20,60,200,16,16,"wifi mode started ");
				printf("系统初始化完成\r\n");
	      printf("是否进入WIFI调试配置程序\r\n");
	      printf("输入Y+空格，点击发送进入调试配置。输入N+空格，点击发送，跳过\r\n");
			
			while(1)
			{ 
				LED2=0;
				ESP8266_test();                      //wifi操作
				LCD_ShowString(20,60,200,16,16,"wifi mode closed ");
				LCD_ShowString(30,210,200,16,16,"Connection disconnect!");
				LED1=0;
				LED2=0;
				break;
			   }
		  }
		  CNT=20;
			LCD_ShowNum(200,120,CNT,2,16);
		  TIM_Cmd(TIM3, DISABLE);
			BEEP=0;
		  BEEP2=0;
	    LED0=0;
		  LCD_ShowString(20,80,200,16,16,"Access control is closed");
			OLED_ShowString(10,5,"Code  closed");
			OLED_Refresh_Gram();	
		 Temp_Humi_Coal();
	 	 delay_ms(10);
	}

}

u8 KEY_Scan(u8 mode)                   //门内按键操作
{	  	  
	if((KEY0==0||KEY1==0||KEY2==0||KEY4==1||KEY3==1||KEY6==0||KEY7==0||KEY8==0||KEY9==0))
{
		delay_ms(10);//去抖动 
		if(KEY0==0)  {while(KEY0==0);                  //E4
		              return KEY_RIGHT;}               
		else if(KEY1==0){while(KEY1==0);               //E3    
			               return KEY_DOWN;}
		else if(KEY2==0){while(KEY2==0);               //E2
			               return KEY_LEFT;}
		else if(KEY6==0){while(KEY6==0)                //E1
                      {TIM_Cmd(TIM2, ENABLE);
											 }		
                     TIM_Cmd(TIM2, DISABLE);											
			               return KEY_RE;}
//		else if(KEY5==1)return KEY_CLOSE;  
		else if(KEY7==0){while(KEY7==0)                 //E0  确认按键
			               {TIM_Cmd(TIM2, ENABLE);
											 }
										  TIM_Cmd(TIM2, DISABLE);
											 CNT2=0;
			                return KEY_CO;}
		else if(KEY8==0){while(KEY8==0);               //F0
			               return KEY_LED2;}
		else if(KEY9==0){while(KEY9==0);               //F0
			               return KEY_LED3;}
		else if(KEY4==1)  return KEY_MC;                //E6   门磁
	  else if(KEY3==1)  return KEY_UP;                //A0   
}
 	return 0;// 无按键按下

	}
void Temp_Humi_Coal(void)              //温湿度煤气传感器
{   
	  coal=17;
	  adcx=Get_Adc_Average(ADC_Channel_1,10);
		temp=(float)adcx*(3.3/4096);
		adcx=temp;
		temp-=adcx;
		temp*=20;
    if(temp>coal) 
		{ BEEP2=1;
		}
    if(temp<=coal) 
		{ BEEP2=0;
		}	
 		if(t%10==0)			//每100ms读取一次
		{									  
			DHT11_Read_Data(&temperature,&humidity);	//读取温湿度值					    
			LCD_ShowNum(30+40,150,temperature,2,16);	//显示温度	   		   
			LCD_ShowNum(30+40,170,humidity,2,16);		  //显示湿度	
      LCD_ShowNum(30+40,190,temp,2,16);	 			  //显示燃气度
		}
}
void Input_password(void)               //密码操作
{ 
	    line=140;
	    line2=40;
       if(line==148)
			LCD_ShowString(20,120,200,16,16,"Input password:");
			LCD_ShowString(line,105,200,16,16,"_");
	    STMFLASH_Read(FLASH_SAVE_ADDR_START,(u16*)buf_code,SIZE);      //读取密码
			while(1)
			{
			 
			 Modify_password();                    //修改密码
       if(line==140)
			 {
				 first_code=code;
				 LCD_ShowNum(line,120,first_code,1,16);
				 LCD_ShowString(140,105,200,16,16,"_   ");
			  }
			 {
				 second_code=code;
				 LCD_ShowNum(line,120,second_code,1,16);
				  LCD_ShowString(140,105,200,16,16," _  ");
			  }	
       if(line==156)
			 {
				 third_code=code;
				 LCD_ShowNum(line,120,third_code,1,16);
				 LCD_ShowString(140,105,200,16,16,"  _ ");
			  }
       if(line==164)
			 {
				 fourth_code=code;
				 LCD_ShowNum(line,120,fourth_code,1,16);
				 LCD_ShowString(140,105,200,16,16,"   _");
			  }
			 if(a==KEY_CO||b==15)
			{
				
			total=first_code*1000+second_code*100+third_code*10+fourth_code;
			total2=first_code2*1000+second_code2*100+third_code2*10+fourth_code2;
			all=buf_code[0]*1000+buf_code[1]*100+buf_code[2]*10+buf_code[3];
//				printf("%d\r\n",all);
//				printf("%d\r\n",total);
			   if(flag&&(total==all))
				{ 
					while(1)
					{
					 Modify_password();
					 LCD_ShowString(20,120,200,16,16,"Modif password:");
					 if(line==140)
			    {
				   buf_code[0]=code;
				   LCD_ShowNum(line,120,buf_code[0],1,16);
					 LCD_ShowString(140,105,200,16,16,"_   ");
			    }
           if(line==148)
			   {
				   buf_code[1]=code;
				   LCD_ShowNum(line,120,buf_code[1],1,16);
           LCD_ShowString(140,105,200,16,16," _  ");
			    }	
           if(line==156)
			   {
				   buf_code[2]=code;
				   LCD_ShowNum(line,120,buf_code[2],1,16);
					 LCD_ShowString(140,105,200,16,16,"  _ ");
   			    }
           if(line==164)
			   {
				   buf_code[3]=code;
				   LCD_ShowNum(line,120,buf_code[3],1,16);
					 LCD_ShowString(140,105,200,16,16,"   _");
			    }
				   flag=0;
				   if(a==KEY_CO)  
					 {
						 code=0;
						 all=buf_code[0]*1000+buf_code[1]*100+buf_code[2]*10+buf_code[3];
						 STMFLASH_Write(FLASH_SAVE_ADDR_START,(u16*)buf_code,SIZE);         //存取密码
						 LED1=0;
						 break;
					 }
					 }
					LCD_ShowString(20,120,200,16,16,"Input password:");
					LCD_ShowString(140,120,200,16,16,"0000");
				 }
			if((total==all)||(total2==all))
			{
		 	LCD_ShowString(140,120,200,16,16,"0000");
			OLED_ShowString(40,49,"****");
			OLED_Refresh_Gram();	
				code=0;
				fourth_code=fourth_code2=10;
			 break;
			}
	   	}	
OLED_Refresh_Gram();			
			}	
}
void Modify_password(void)
{
			Temp_Humi_Coal();
      Out_door();	
			a=KEY_Scan(0);
			if(a==KEY_RIGHT) 
			{ 
				LCD_Fill(line,105,line+50,105+15,WHITE);
				line+=8;
				code=0;
				if(line>164)
					line=140;
			 }
			if(a==KEY_DOWN) 
			{
				LCD_Fill(line,105,line+50,105+15,WHITE);
				line-=8;
				if(line<140)
					line=164;
			 }
			if(a==KEY_LEFT)
			{
				code++;
				if(code>9)
					code=0;
			 }
			if(a==KEY_RE)
			{
				code--;
				if(code<0)
					code=9;
			 } 
}
void Out_door(void)          //门外按键
{   
    b=Keyboard_Scan();
	 if(b==10)
		{
			line2-=7;
			if(line2<40)
			line2=40;
		  }
		if(b>0&&b<10)
		{ 
			code2=b;
			switch(line2)
			{
				case 40:
				first_code2=code2;  OLED_ShowNum(40,49,first_code2,1,16);  line2+=7; code2=0; break;
				case 47:
				second_code2=code2;  OLED_ShowNum(47,49,second_code2,1,16);  line2+=7; code2=0; break;
				case 54:
				third_code2=code2;  OLED_ShowNum(54,49,third_code2,1,16);  line2+=7; code2=0; break;
				case 61:
				fourth_code2=code2;  OLED_ShowNum(61,49,fourth_code2,1,16);  code2=0; break;
		}
	}
		if(b==16)
		{
		 code2=0;
			switch(line2)
			{
				case 40:
				first_code2=code2;  OLED_ShowNum(40,49,first_code2,1,16);  line2+=7; code2=0; break;
				case 47:
				second_code2=code2;  OLED_ShowNum(47,49,second_code2,1,16);  line2+=7; code2=0; break;
				case 54:
				third_code2=code2;  OLED_ShowNum(54,49,third_code2,1,16);  line2+=7; code2=0; break;
				case 61:
				fourth_code2=code2;  OLED_ShowNum(61 ,49,fourth_code2,1,16);  code2=0; break;
		}
		}
			 OLED_ShowString(line2,35,"_");
		   OLED_ShowString(line2-7,35," ");
		   OLED_ShowString(line2-14,35," ");
	     OLED_ShowString(line2-21,35," ");
		   OLED_ShowString(line2+7,35," ");
		   OLED_ShowString(line2+14,35," ");
	     OLED_ShowString(line2+21,35," ");
			 OLED_Refresh_Gram();
 }



