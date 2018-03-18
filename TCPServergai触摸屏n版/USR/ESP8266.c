#include "ESP8266.h"
#include "led.h"
//#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "led.h"
#include "beep.h"
#include "dht11.h" 	
#include "adc.h"
#include "stmflash.h"
#include "touch.h"
#include "usart_config.h"
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);	 //在指定位置显示一个字符串 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);                    //指定位置显示字符串
void atk_8266_load_keyboard(u16 x,u16 y);                                //画矩形
void atk_8266_key_staset(u16 x,u16 y,u8 keyx,u8 sta);                    //按键状态设置
void rtp_test(void);                                                     //电阻屏
void ctp_test(void);                                                     //电容屏
u8 atk_8266_get_keynum(u16 x,u16 y);                                     //得到触摸屏键值
u8* kbd_tbl[15]={"KEY","LED","DEL","1","2","3","4","5","6","7","8","9","SURE","0","QUIT"};  //键盘字符串
u16 key_code[30]={0};
//u8* key_led[4]={"LED1","LED2","LED3","ALL"}; 
//char* wifi_send[10]={"7","8","9","a","b","c","d","e","f","g"};           //发送数据数组
void ( * pNet_Test ) ( void );
void Temp_Humi_Coal(void);
struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };
    extern u8 flag,sleep_flag;    //TIM2标志位  防休眠标志位
	  u16 adcx;                     //温湿度煤气参数
	  float temp;	 
	  u8 t=0,a,b,i,client_ID,client_IPD[6]; 
  	u8 temperature,humidity,coal=17;  
    u8 key_place=7,send_flag;               //需要设置的按键  
    int address,address_data;
extern u8 CNT3;
#define SIZE2 sizeof(key_code)	 			  	//数组长度
#define FLASH_SAVE_ADDR_START2  0X08071000//0X08008000
   //煤气浓度阈值
u8 KEY_Scan(u8); 
/*
 * 函数名：ESP8266_test
 * 描述  ：ESP8266调试程序
 * 输入  ：
 *         
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_test(void)
{
	
	char cCh;
//	LED_Init();
//	LED0=LED1=LED2=0;
//	delay_init();	    	    //延时函数初始化	
	PC_Usart("\r\n WF-ESP8266 WiFi模块测试\r\n");                            //打印测试例程提示信息
		
  PC_Usart ( "\r\n请输入模块的测试模式的编号，编号如下：\
              \r\n1.STA模式： TCP Client\
              \r\n2.AP模式： AP Server\
              \r\n3.STA+AP模式： (STA)TCP Client + (AP)TCP Server\
              \r\n输入格式为：编号+空格，点击发送\r\n" );
	STMFLASH_Read(FLASH_SAVE_ADDR_START2,(u16*)key_code,SIZE2);               //读取已匹配按键
//  scanf ( "%c",  & cCh );
  cCh='2';
  switch ( cCh )
  {
		case '1':
			pNet_Test = ESP8266_STA_TCP_Client;
		  PC_Usart("\r\n即将进行的测试是：1.STA模式： TCP Client\r\n");
		  break;
		
		case '2':
			pNet_Test = ESP8266_AP_TCP_Server;
		  PC_Usart("\r\n即将进行的测试是：2.AP模式： TCP Server\r\n");
		  break;

		case '3':
			pNet_Test = ESP8266_StaTcpClient_ApTcpServer;
		  PC_Usart("\r\n即将进行的测试是：3.STA+AP模式： (STA)TCP Client + (AP)TCP Server\r\n");
		  break;

		default:
			pNet_Test = NULL;
		  PC_Usart("\r\n输入错误，不进行测试！\r\n");
		  break;		
			
	}
//PC_Usart ( "%d",pNet_Test);
	
	if ( pNet_Test )
		  pNet_Test();

}
/*
 * 函数名：ESP8266_Choose
 * 描述  ：使能/禁用WF-ESP8266模块
 * 输入  ：enumChoose = ENABLE，使能模块
 *         enumChoose = DISABLE，禁用模块
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_Choose ( FunctionalState enumChoose )
{
	if ( enumChoose == ENABLE )
		ESP8266_CH_HIGH_LEVEL();
	
	else
		ESP8266_CH_LOW_LEVEL();
	
}


/*
 * 函数名：ESP8266_Rst
 * 描述  ：重启WF-ESP8266模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被ESP8266_AT_Test调用
 */
void ESP8266_Rst ( void )
{
	#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
	#else
	 ESP8266_RST_LOW_LEVEL();
	 delay_ms ( 500 ); 
	 ESP8266_RST_HIGH_LEVEL();
	 
	#endif

}


/*
 * 函数名：ESP8266_AT_Test
 * 描述  ：对WF-ESP8266模块进行AT测试启动
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用	
 */
void ESP8266_AT_Test ( void )
{
	ESP8266_RST_HIGH_LEVEL();
	
	delay_ms ( 1000 ); 
	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 200 ) )
		
		ESP8266_Rst (); 
	
  
}


/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包

	ESP8266_Usart ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	delay_ms ( waittime );                 //延时
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}


/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}


/*
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 7000 );
	
}


/*
 * 函数名：ESP8266_BuildAP
 * 描述  ：WF-ESP8266模块创建WiFi热点
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 *       ：enunPsdMode，WiFi加密方式代号字符串
 * 返回  : 1，创建成功
 *         0，创建失败
 * 调用  ：被外部调用
 */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 1000 );
	
}


/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d",( enumEnUnvarnishTx ? 1 : 0 ));
//	( enumEnUnvarnishTx ? 1 : 0 ) 
//	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	return ESP8266_Cmd ( "AT+CIPMUX=1", "OK", 0, 500 );
	
}


/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
	
}


/*
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
	}
	
}


/*
 * 函数名：ESP8266_UnvarnishSend
 * 描述  ：配置WF-ESP8266模块进入透传发送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_UnvarnishSend ( void )
{
	return (
	  ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) &&
	  ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", 500 ) );
	
}


/*
 * 函数名：ESP8266_SendString
 * 描述  ：WF-ESP8266模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
		
	if ( enumEnUnvarnishTx )
		ESP8266_Usart ( "%s", pStr );

	
	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength  );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength  );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}


/*
 * 函数名：ESP8266_ReceiveString
 * 描述  ：WF-ESP8266模块接收字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 * 返回  : 接收到的字符串首地址
 * 调用  ：被外部调用
 */
char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
	char * pRecStr = 0;
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	delay_ms ( 300 ) ;
	if(strEsp8266_Fram_Record .InfBit .FramFinishFlag)
	{
//	while ( ! strEsp8266_Fram_Record .InfBit .FramFinishFlag );
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	if ( enumEnUnvarnishTx )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, ">" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
	
	else 
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
}
	return pRecStr;

}


/*
 * 函数名：ESP8266_STA_TCP_Client
 * 描述  ：WF-ESP8266模块进行STA TCP Clien测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_STA_TCP_Client ( void )
{
	char cStrInput [100] = { 0 };
//	char * cStrInput="ATK-ESP8266,12345678 ";
	char * pStrDelimiter [2], * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

	ESP8266_Choose ( ENABLE );	
  
	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA );
  
	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do
	{
		PC_Usart ( "\r\n请输入要连接的WiFi名称和密钥，输入格式为：名称字符+英文逗号+密钥字符+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );
//    cStrInput[]="ATK-ESP8266,12345678 ";
		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ESP8266_JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );
	
	ESP8266_Enable_MultipleId ( ENABLE );
	
	do 
	{ 
		PC_Usart ( "\r\n请在电脑上将网络调试助手以TCP Server连接网络，并输入电脑的IP和端口号，输入格式为：电脑IP+英文逗号+端口号+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_3 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_4 ) ) );

  for ( uc = 0; uc < 5; uc ++ )
	{
		PC_Usart ( "\r\n请输入端口ID%d要发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n", uc );

		scanf ( "%s", cStrInput );

		ul = strlen ( cStrInput );
		
		ESP8266_SendString ( DISABLE, cStrInput, ul, ( ENUM_ID_NO_TypeDef ) uc );
		
	}
	
	
	PC_Usart ( "\r\n请在网络调试助手发送字符串\r\n" );
	while (1)
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s", pStr );
	}

}


/*
 * 函数名：ESP8266_AP_TCP_Server
 * 描述  ：WF-ESP8266模块进行AP TCP Server测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_AP_TCP_Server ( void )
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr,chose[1]={0}, str[10]={0};
	u8 uc = 0,quit=0,quit2=0;      //退出标志位
  u32 ul = 0;
//  KEY_Init();
  ESP8266_Choose ( ENABLE );

	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( AP );


	PC_Usart ( "\r\n请输入要创建的WiFi的名称、加密方式和密钥，加密方式的编号为：\
              \r\n0 = OPEN\
              \r\n1 = WEP\
              \r\n2 = WPA_PSK\
	            \r\n3 = WPA2_PSK\
              \r\n4 = WPA_WPA2_PSK\
							\r\n输入格式为：名称字符+英文逗号+加密方式编号+英文逗号+密钥字符+空格，点击发送\r\n" );

//	scanf ( "%s", cStrInput );
  strcpy(cStrInput, "ATK-ESP8266,4,12345678");
	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP ( pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1] );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*
		

	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	PC_Usart ( "\r\n请输入服务器要开启的端口号和超时时间（0~28800，单位：秒），输入格式为：端口号字符+英文逗号+超时时间字符+空格，点击发送\r\n" );

//	scanf ( "%s", cStrInput );
  strcpy(cStrInput, "8086,10000 ");
	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer ( ENABLE, pStrDelimiter [0], pStrDelimiter [1] );
	
	
	do
	{ 
		LED2=!LED2;
		PC_Usart ( "\r\n正查询本模块IP……\r\n" );
	  ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
	  Temp_Humi_Coal();
		PC_Usart ( "\r\n请用手机连接刚才创建的WiFi，这里只连接一个手机，作为ID0，然后用手机网络调试助手以TCP Client连接刚才开启的服务器（AP IP）……\r\n" );
		delay_ms ( 60000 ) ;
		a=KEY_Scan(0);
		if(a==KEY_RE)                //强制退出
		{quit2=1;
			break;
		  }
	}	while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:0", 0, 500 ) );
	PC_Usart ( "请输入要选择的模式-1串口至手机端  2手机端至串口 输入格式为：模式编号+空格" );
	LED2=1;
	LCD_ShowString(30,210,200,16,16,"Connection    success!");
	chose[0]='2';
	while(1)
	{
		if(chose[0]=='1')
		{
	PC_Usart ( "\r\n请输入要向端口手机（ID0）发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n" );
  
	while(1)
	{
		
//		LCD_ShowString(30,230,200,16,16,"Set up KEY1->");
//	  LCD_ShowString(30,250,200,16,16,"Set up KEY2->");
//	  LCD_ShowString(30,270,200,16,16,"Set up KEY3->");
//		LCD_ShowString(140,230,200,16,16,key_led[key_code[0]-7]);
//		LCD_ShowString(140,250,200,16,16,key_led[key_code[1]-7]);
//		LCD_ShowString(140,270,200,16,16,key_led[key_code[2]-7]);
		a=KEY_Scan(0);                   
		if(a)                            //发送
		{
			switch(a)                      
			{
				case KEY_RIGHT:              //窗帘左移
				    cStrInput[0]='5';
//			     ul = strlen ( cStrInput[0] );	
	          ESP8266_SendString ( DISABLE, cStrInput, 1, Multiple_ID_0 );
				    break;
				case  KEY_DOWN:              //窗帘右移
				    cStrInput[0]='6';
//           ul = strlen ( cStrInput );	
	          ESP8266_SendString ( DISABLE, cStrInput, 1, Multiple_ID_1 );
				    break;
				case   KEY_LEFT:                //key1		
//			      ul = strlen ( cStrInput );	
				    cStrInput[0]='7'+key_code[0];
	          ESP8266_SendString ( DISABLE, cStrInput, 1, Multiple_ID_2 );
				    break;
				case   KEY_CO:                 //key2					
//			      ul = strlen ( cStrInput );	
				    cStrInput[0]='7'+key_code[1];
	          ESP8266_SendString ( DISABLE, cStrInput, 1, Multiple_ID_3 );
				    break;
				case   KEY_LED2:                //key3
//			      ul = strlen ( cStrInput );	
				    cStrInput[0]='7'+key_code[2];
	          ESP8266_SendString ( DISABLE, cStrInput, 1, Multiple_ID_4 );
				    break;
				case   KEY_RE:               //长按更改设置   短按退出wifi模式
					 
					  if(flag)                 //进入设置
						{   
						   	LCD_Clear(WHITE);//清屏
							  if(tp_dev.touchtype!=0XFF)LCD_ShowString(60,130,200,16,16,"Press KEY0 to Adjust");//电阻屏才显示
							  delay_ms(1000);
							  LCD_Clear(WHITE);//清屏
								if(tp_dev.touchtype&0X80)ctp_test();	//电容屏测试
	              else rtp_test(); 						//电阻屏测试
/*
//							while(1)
//							{
//							LCD_ShowString(30,230,200,16,16,"Set up ");
//							LCD_ShowString(30,250,200,16,16,"Set up ");
//							LCD_ShowString(30,270,200,16,16,"Set up ");
//						 LED1=1;
//             a=KEY_Scan(0);
//						 if(a==KEY_RIGHT)
//						 {
//							 key_place++;
//							 if(key_place>9) key_place=7;
//						  }
//						 switch(key_place)
//						 {
//							 case  7:                                                     //设置KEY1
//							 LCD_ShowString(90,230,200,16,16,"KEY1->");
//							 if(a==KEY_DOWN) key_code[0]++;
//							 if(key_code[0]>10)   key_code[0]=7;
//							 switch(key_code[0])
//							 {
//								 case 7:
//								 LCD_ShowString(140,230,200,16,16,"LED1"); break;
//								 case 8:
//								 LCD_ShowString(140,230,200,16,16,"LED2"); break;
//								 case 9:
//								 LCD_ShowString(140,230,200,16,16,"LED3"); break;
//								 case 10: 
//								 LCD_ShowString(140,230,200,16,16," ALL"); break;
//							  }
//							 break;
//						   case  8:                                                     //设置KEY2
//							 LCD_ShowString(90,250,200,16,16,"KEY2->");
//							 if(a==KEY_DOWN) key_code[1]++;
//							 if(key_code[1]>10)   key_code[1]=7;
//							 switch(key_code[1])
//							 {
//								 case 7:
//								 LCD_ShowString(140,250,200,16,16,"LED1"); break;
//								 case 8:
//								 LCD_ShowString(140,250,200,16,16,"LED2"); break;
//								 case 9:
//								 LCD_ShowString(140,250,200,16,16,"LED3"); break;
//								 case 10: 
//								 LCD_ShowString(140,250,200,16,16," ALL"); break;
//							  }
//							 break;
//               case  9:                                                     //设置KEY3
//							 LCD_ShowString(90,270,200,16,16,"KEY3->");
//               if(a==KEY_DOWN) key_code[2]++;
//							 if(key_code[2]>10)   key_code[2]=7;
//               switch(key_code[2])
//							 {
//								 case 7:
//								 LCD_ShowString(140,270,200,16,16,"LED1"); break;
//								 case 8:
//								 LCD_ShowString(140,270,200,16,16,"LED2"); break;
//								 case 9:
//								 LCD_ShowString(140,270,200,16,16,"LED3"); break;
//								 case 10: 
//								 LCD_ShowString(140,270,200,16,16," ALL"); break;
//							  }							 
//               break;							 
//						 }
//					   if(a==KEY_LEFT)                                                //保存设置
//						 {
//							 STMFLASH_Write(FLASH_SAVE_ADDR_START2,(u16*)key_code,SIZE2);
//				      flag=0;
//							break;
//						 }
//						}
*/
              STMFLASH_Write(FLASH_SAVE_ADDR_START2,(u16*)key_code,SIZE2);
             	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
             	LCD_ShowString(30,170,200,16,16,"Humi:  %");
              LCD_ShowString(30,190,200,16,16,"Coal:  %");
							LCD_ShowString(20,80,200,16,16,"Access control is closed");
							LCD_ShowString(20,60,200,16,16,"wifi mode started ");
              flag=0;
							LED1=0;
						  break;
					}           
//			     ul = strlen ( cStrInput );	              //提示客户端关闭
					 cStrInput[0]='4';
	         ESP8266_SendString ( DISABLE, cStrInput, 1, Multiple_ID_0 );
						LED2=0;
				    quit=1;
			      break;
			 }
		 }
		  if(quit==1) break;                          //退出
		 	if(quit2==1) break;
 /*
//		if(a==KEY_RIGHT)
//		{
//			strcpy(cStrInput, "5");
//			ul = strlen ( cStrInput );	
//	    ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_0 );
//		 }
//		if(a==KEY_DOWN)
//    {	strcpy(cStrInput, "6");
//			ul = strlen ( cStrInput );	
//	    ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_0 );
//		}
//		if(a==KEY_LEFT)
//		{
//			strcpy(cStrInput, "5");
//			ul = strlen ( cStrInput );	
//	    ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_1 );
//		 }
//		if(a==KEY_RE) 
//		{ LED2=0;
////			break;
//		}
//	scanf ( "%s", cStrInput );
//	ul = strlen ( cStrInput );	
//	ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_0 );
*/
}
	}
	if(chose[0]=='2')
	{
	  PC_Usart ( "\r\n请在手机网络调试助手发送字符串\r\n" );

		TIM_Cmd(TIM4, ENABLE);                        //用于防休眠
		
	 while (1)
	 { 
	  Temp_Humi_Coal();                            //温湿度检测
		 if(sleep_flag==1)                           //避免wifi休眠   30s发一次
		{
		strcpy(cStrInput, "sleep");         
	  ul = strlen ( cStrInput );	
	  ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_0 );
		ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_1 );
		ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_2 );
		ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_3 );
		ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_4 );
		sleep_flag=0;
		}
	  pStr = ESP8266_ReceiveString ( DISABLE );
		 if(pStr)                                    //收到数据.发送
		  { 
         CNT3=0;				
        PC_Usart ( "%s\r\n", pStr );
				send_flag=1;
				while(send_flag)
				{
		    for(i=0;i<14;i++)
		    {
       		 *pStr++;
       		  if(i==6)                             //判断ID号
       		  {
       			 strncpy(str,pStr,1);
       			 str[1]='\0';
        		 address=atoi(str);                  //将id号转为整数
       		   PC_Usart ( "IPD=%d\r", address );
       		  }
       		  if(i==10)                             //判断接收到的数据 
       		  {
					 	address_data=atoi(pStr);              //将数据转为整数
       			PC_Usart ( "data=%d\r", address_data );
//						if(address_data>10)
//					 	{
							client_ID=address_data/10;
							PC_Usart ( "client_ID=%d\r", client_ID );
							client_IPD[client_ID]=address;
//						}
       		  }
		   }
			   
//			   if(key_code[address*5+address_data]%5==0)  itoa(5,cStrInput,10);
//				 else itoa(key_code[address*5+address_data]%5,cStrInput,10);   //将要发送的数据转为字符串
			   if(key_code[client_ID*5+address_data%10]%5==0)    itoa(5,cStrInput,10);
				 else    itoa(key_code[client_ID*5+address_data%10]%5,cStrInput,10);   //将要发送的数据转为字符串
			   address=(key_code[client_ID*5+address_data%10]-1)/5;            //发送给ID号为address的客户端
//			   cStrInput[0]='0';
			   ul = strlen ( cStrInput );
	       ESP8266_SendString ( DISABLE, cStrInput, ul,( ENUM_ID_NO_TypeDef ) client_IPD[address] );
				 pStr=0;
			   send_flag=0;
		    }
		   }
			a=KEY_Scan(0);
			 if(a)                                          //发送
		  {
			  switch(a)                      
			  {
					case   KEY_RE:                              //长按更改设置   短按退出wifi模式
					 
					  if(flag)                                   //进入设置
						{   
						   	LCD_Clear(WHITE);//清屏
							  if(tp_dev.touchtype!=0XFF)LCD_ShowString(60,130,200,16,16,"Press KEY0 to Adjust");//电阻屏才显示
							  delay_ms(1000);
							  LCD_Clear(WHITE);                     //清屏
								if(tp_dev.touchtype&0X80)ctp_test();	//电容屏测试
	              else rtp_test(); 						          //电阻屏测试
              STMFLASH_Write(FLASH_SAVE_ADDR_START2,(u16*)key_code,SIZE2);
             	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
             	LCD_ShowString(30,170,200,16,16,"Humi:  %");
              LCD_ShowString(30,190,200,16,16,"Coal:  %");
							LCD_ShowString(20,80,200,16,16,"Access control is closed");
							LCD_ShowString(20,60,200,16,16,"wifi mode started ");
              flag=0;
							LED1=0;
						  break;
					}           

//					  strcpy(cStrInput, "0");  
            cStrInput[0]='0';        					//提示客户端关闭    
	          ul = strlen ( cStrInput );
	          ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_0 );
					  ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_1 );
					  ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_2 );
					  ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_3 );
					  ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_4 );
						LED2=0;
				    quit=1;
			      break;
			  }
		  }
			 if(quit==1||quit2==1)   break;                    //退出
	 }
	
  }
	     if(quit==1||quit2==1)   break;                     //退出
}
}


/*
 * 函数名：ESP8266_StaTcpClient_ApTcpServer
 * 描述  ：WF-ESP8266模块进行STA(TCP Client)+AP(TCP Server)测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_StaTcpClient_ApTcpServer ( void )
{ 
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;
  ESP8266_Choose ( ENABLE );

	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA_AP );


	PC_Usart ( "\r\n请输入要创建的WiFi的名称、加密方式和密钥，加密方式的编号为：\
						\r\n0 = OPEN\
						\r\n1  =WEP\
						\r\n2 = WPA_PSK\
						\r\n3 = WPA2_PSK\
						\r\n4 = WPA_WPA2_PSK\
						\r\n输入格式为：名称字符+英文逗号+加密方式编号+英文逗号+密钥字符+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP ( pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1] );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*
	

	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do
	{
		PC_Usart ( "\r\n请输入要连接的WiFi名称和密钥，输入格式为：名称字符+英文逗号+密钥字符+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ESP8266_JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );

	
	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	PC_Usart ( "\r\n请输入服务器要开启的端口号和超时时间（0~28800，单位：秒），输入格式为：端口号字符+英文逗号+超时时间字符+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer ( ENABLE, pStrDelimiter [0], pStrDelimiter [1] );
	
	
	do 
	{
		PC_Usart ( "\r\n请在电脑上将网络调试助手以TCP Server连接网络，并输入电脑的IP和端口号，输入格式为：电脑IP+英文逗号+端口号+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2 ) ) );
		
	
	do
	{
		PC_Usart ( "\r\n正查询本模块IP，前一个为AP IP，后一个为STA IP……\r\n" );
	  ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
		
		PC_Usart ( "\r\n请用手机连接刚才创建的WiFi，这里只连接一个手机，作为ID3，然后用手机网络调试助手以TCP Client连接刚才开启的服务器（AP IP）……\r\n" );
		delay_ms ( 20000 ) ;
	}	while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:3", 0, 500 ) );
	

	for ( uc = 0; uc < 3; uc ++ )
	{
		PC_Usart ( "\r\n请输入端口ID%d要发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n", uc );

		scanf ( "%s", cStrInput );

		ul = strlen ( cStrInput );
		
		ESP8266_SendString ( DISABLE, cStrInput, ul, ( ENUM_ID_NO_TypeDef ) uc );
		
	}
	
	
	PC_Usart ( "\r\n请输入要向端口手机（ID3）发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	ul = strlen ( cStrInput );
	
	ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_3 );

	
	PC_Usart ( "\r\n请在电脑网络调试助手或手机网络调试助手发送字符串……\r\n" );
	while (1)
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s", pStr );
	}
}

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);	//清屏   
 	POINT_COLOR=BLUE;	//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=RED;	//设置画笔蓝色 
}
void rtp_test(void)
{
	u8 key,keyscan; 
  u8 line=0,i=0;	
	u16 key_p=0,led_code=0;
	char  code_place[6]={0};
	atk_8266_load_keyboard(0,20);			//显示键盘 
	LCD_ShowString(30,230-line,200,16,16,"Set up KEY   ---LED");
	LCD_ShowString(85,210-line,200,16,16,"___");
	while(1)
	{ 
		tp_dev.scan(0);
		keyscan=KEY_Scan(0);
		LCD_ShowNum(109,230-line,key_p,3,16);
	  LCD_ShowNum(181,230-line,led_code,3,16);
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{
		key=atk_8266_get_keynum(0,20);
		  if(key==1)                    //设置按键值
		  {   
				i=0;
        key_p=code_place[0]=code_place[1]=code_place[2]=0;				
	    }	
		  if(key==2)                    //设置led值
	  	{
				i=3;
				led_code=code_place[3]=code_place[4]=code_place[5]=0;;
		   }
			if(key==3)                     //回删
	  	{
				i--;
				code_place[i]=0; 
				if(i==255) i=0;
		   }
			if(key>3&&key<13)                //数字1-10
			{
				code_place[i]=key-3;
				i++;
				if(i>6) i=6;
			 }
			if(key==14)                        //数字0
			{
				code_place[i]=0x0a;
				i++;
				if(i>6) i=6;
			 }
				if(i<3)                           //上标线
				{
				LCD_ShowString(85,210-line,200,16,16,"___"); 
        LCD_ShowString(157,210-line,200,16,16,"   ");
				}
				if(i>2) 
				{
				LCD_ShowString(85,210-line,200,16,16,"   ");
			  LCD_ShowString(157,210-line,200,16,16,"___");
				}
				if(!code_place[0])          key_p=0;
			  if(code_place[0])          key_p=code_place[0];
				if(code_place[0]==0x0a)    key_p=code_place[0]=0;
				if(code_place[1])          key_p=code_place[0]*10+code_place[1];
				if(code_place[1]==0x0a)    key_p=code_place[0]*10;
				if(code_place[2])          
				{
					if(code_place[1]==0x0a)  key_p=code_place[0]*100+code_place[2];
					else key_p=code_place[0]*100+code_place[1]*10+code_place[2];
				
				}
				if(code_place[2]==0x0a)    
				{
					if(code_place[1]==0x0a)  key_p=code_place[0]*100;
					else key_p=code_place[0]*100+code_place[1]*10;
				}
				if(!code_place[3])         led_code=0;                           //计算值
				if(code_place[3])          led_code=code_place[3];
				if(code_place[3]==0x0a)    led_code=code_place[3];
				if(code_place[4])          led_code=code_place[3]*10+code_place[4];
				if(code_place[4]==0x0a)    led_code=code_place[3]*10;
				if(code_place[5])         
        {
				  if(code_place[4]==0x0a)  led_code=code_place[3]*100+code_place[5];
				  else  led_code=code_place[3]*100+code_place[4]*10+code_place[5];
				}
				if(code_place[5]==0x0a)         
        {
				  if(code_place[4]==0x0a)  led_code=code_place[3]*100;
				  else  led_code=code_place[3]*100+code_place[4]*10;
				}
				
			if(key==13)                                    //确认键
			{
				key_code[key_p]=led_code;
				LCD_ShowString(30,250-line,200,16,16,"KEY    has saved");
				LCD_ShowNum(54,250-line,key_p,3,16);
				code_place[0]=code_place[1]=code_place[2]=code_place[3]=code_place[4]=code_place[5]=0;
			 }
			if(key==15)                                     //退出键
			{	
				LCD_Clear(WHITE);//清屏
				break; 
			}
	 }		
		delay_ms(10);	//没有按键按下的时候
		if(keyscan==KEY_RIGHT)	//KEY0按下,则执行校准程序
		{
			LCD_Clear(WHITE);//清屏
		    TP_Adjust();  	//屏幕校准  
			Load_Drow_Dialog();
			atk_8266_load_keyboard(0,20);
		}
	}
}
//电容触摸屏测试函数
void ctp_test(void)
{
 	u8 key;
	u8 timex=0; 
  u8 line=100;	
//	char  key_code[3]={7,8,9};
	 atk_8266_load_keyboard(0,180);			//显示键盘 
	LCD_ShowString(30,230+line,200,16,16,"Set up KEY1->");
	LCD_ShowString(30,250+line,200,16,16,"Set up KEY2->");
	LCD_ShowString(30,270+line,200,16,16,"Set up KEY3->");
	while(1)
	{ 
		key=atk_8266_get_keynum(0,180);
		if(key)
		{   
			if(key>3&&key<7)
			{
				key_code[key-4]=7;
				LCD_ShowString(140,230+line+20*(key-4),200,16,16,"LED1");
			}
			if(key>6&&key<10)
			{
				key_code[key-7]=8;
				LCD_ShowString(140,230+line+20*(key-7),200,16,16,"LED2");
			 }
			if(key>9&&key<13)
			{
				key_code[key-10]=9;
				LCD_ShowString(140,230+line+20*(key-10),200,16,16,"LED3");
			 }
			if(key>12&&key<16)
			{
				key_code[key-13]=10;
				LCD_ShowString(140,230+line+20*(key-13),200,16,16," ALL");
			 } 	
		} 
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
	}
}
//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度			  
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth=0;
   	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,lcddev.width,lcddev.height,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
	}
}
//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式    	   		   
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
  u8 bHz=0;     //字符或者中文  	    				    				  	  
    while(*str!=0)//数据未结束
    { 

	        if(*str>0x80) bHz=1;//中文 
	        else              //字符
	        {      
                if(x>(x0+width-size/2))//换行
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//有效部分写入 
				str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
      				 
    }   
}
//得到触摸屏的输入
//x,y:键盘坐标
//返回值：按键键值（1~15有效；0,无效）
u8 atk_8266_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,没有任何按键按下；1~15，1~15号按键按下
	u8 key=0;
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
	{	
		for(i=0;i<5;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(tp_dev.x[0]<(x+j*80+80)&&tp_dev.x[0]>(x+j*80)&&tp_dev.y[0]<(y+i*28+28)&&tp_dev.y[0]>(y+i*28))
				{	
					key=i*3+j+1;	 
					break;	 		   
				}
			}
			if(key)
			{	   
				if(key_x==key)key=0;
				else 
				{
					atk_8266_key_staset(x,y,key_x-1,0);
					key_x=key;
					atk_8266_key_staset(x,y,key_x-1,1);
				}
				break;
			}
		}  
	}else if(key_x) 
	{
		atk_8266_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	return key; 
}
//按键状态设置
//x,y:键盘坐标
//key:键值（0~8）
//sta:状态，0，松开；1，按下；
void atk_8266_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>15)return;
	if(sta)LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,GREEN);
	else LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,WHITE); 
 Show_Str_Mid(x+j*80,y+6+28*i,(u8*)kbd_tbl[keyx],16,80);		 		 
}
void atk_8266_load_keyboard(u16 x,u16 y)
{
	u16 i;
	POINT_COLOR=RED; 
	LCD_Fill(x,y,x+240,y+140,WHITE);
	LCD_DrawRectangle(x,y,x+240,y+140);						   
	LCD_DrawRectangle(x+80,y,x+160,y+140);	 
	LCD_DrawRectangle(x,y+28,x+240,y+56);
	LCD_DrawRectangle(x,y+84,x+240,y+112);
	POINT_COLOR=BLUE;
	for(i=0;i<15;i++)
	{
		Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),(u8*)kbd_tbl[i],16,80);

	}  		 					   
}
