/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：USART0.c
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/

/************************* 头文件 *************************/

#include "TaskDefine.h"
#include "task.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/

uint8_t SendFrame0[250]; 
uint8_t ReceiveFrame0[40];  
uint8_t SendCount0;     //发送字节数
uint8_t TotalSendCount0;//发送总数
uint8_t ReceiveCount0;  //接收字节数
uint8_t TotalReceiveCount0;//接收总数
uint8_t rcv_frame0;	//一帧数据收完
uint8_t rcv_time0; //接收计时


uint8_t SendFrame1[250]; 
uint8_t ReceiveFrame1[250];  
uint8_t SendCount1;     //发送字节数
uint8_t TotalSendCount1;//发送总数
uint8_t ReceiveCount1;  //接收字节数
uint8_t TotalReceiveCount1;//接收总数
uint8_t rcv_frame1;	//一帧数据收完
uint8_t rcv_time1; //接收计时

uint8_t SendFrame2[100]; 
uint8_t ReceiveFrame2[100];  
uint16_t SendCount2;     //发送字节数
uint16_t TotalSendCount2;//发送总数
uint16_t ReceiveCount2;  //接收字节数
uint16_t TotalReceiveCount2;//接收总数
uint8_t rcv_frame2;	//一帧数据收完
uint8_t rcv_time2; //接收计时
uint8_t SendDelayTimes2;	//发后延迟

uint8_t RevCRTFrm[100];
uint8_t	ReceiveTongXinBan[100];

uint8_t SendFrame3[200]; 
uint8_t ReceiveFrame3[200];  
uint8_t SendCount3;     //发送字节数
uint8_t TotalSendCount3;//发送总数
uint8_t ReceiveCount3;  //接收字节数
uint8_t TotalReceiveCount3;//接收总数

uint8_t SendFrame4[200]; 
uint8_t ReceiveFrame4[30];  
uint8_t SendCount4;     //发送字节数
uint8_t TotalSendCount4;//发送总数
uint8_t ReceiveCount4;  //接收字节数
uint8_t TotalReceiveCount4;//接收总数

uint8_t SendFrame5[50]; 
uint8_t ReceiveFrame5[50];  
uint8_t SendCount5;     //发送字节数
uint8_t TotalSendCount5;//发送总数
uint8_t ReceiveCount5;  //接收字节数
uint8_t TotalReceiveCount5;//接收总数

/************************ 函数定义 ************************/

int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
/************************************************************ 
 * 函数:       USART0_config(void)
 * 说明:       串口配置
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
void USART0_Config(void)
{
    //与黑匣子通讯
	rcu_periph_clock_enable(RCU_GPIOA);    // 使能GPIO时钟

    rcu_periph_clock_enable(RCU_USART0);   // 使能串口时钟
	
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
	
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);   // GPIO 模式设置
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  // 输出参数设置

	gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_10);   // GPIO 模式设置
	gpio_output_options_set(GPIOA, GPIO_MODE_ANALOG, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // 输出参数设置


    usart_deinit(USART0);    // 串口复位
    usart_word_length_set(USART0, USART_WL_8BIT);  // 字长
    usart_stop_bit_set(USART0, USART_STB_1BIT);    // 停止位
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 19200U);     // 波特率
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);     // 接收使能
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);   // 发送使能
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_enable(USART0);           // 串口使能
	
	nvic_irq_enable(USART0_IRQn, 2, 1);//使能USART0中断
	usart_interrupt_enable(USART0, USART_INT_RBNE);//接收中断打开
	
	ReceiveCount0=0;
	TotalReceiveCount0=0;
	rcv_frame0=0;
	rcv_time0=0;
}

void uart0Send(void)
 {
     uint8_t i;
     for (i = 0; i < TotalSendCount0; i++)   
     {           
         while(usart_flag_get(USART0, USART_FLAG_TC) == RESET);  
         usart_data_transmit(USART0,SendFrame0[i]);
     }     
     while(usart_flag_get(USART0, USART_FLAG_TC) == RESET);          
}
 
//黑匣子接收数据
void uart0Rev(uint8_t rev)
{
	rcv_time0=0;//0716 rcv_time1->rcv_time0
	switch(ReceiveCount0)
	{
	case 0:
		if(rev==0x40)
		{
			ReceiveFrame0[0] = rev;
			ReceiveCount0=1;
		}
		break;
	case 1:
		if(rev==0x7E)
		{
			ReceiveFrame0[1] = rev;
			ReceiveCount0=2;
		}
		else
			ReceiveCount0=0;
		break;
	case 2:
		if(rev<20)
		{
			ReceiveFrame0[2] = rev;
			ReceiveCount0=3;
			TotalReceiveCount0=rev+5;
		}
		else
			ReceiveCount0=0;
		break;
	default:
		ReceiveFrame0[ReceiveCount0] = rev;
		ReceiveCount0++;
		if(ReceiveCount0>=TotalReceiveCount0)
		{
			rcv_frame0=1;
			ReceiveCount0=0;
		}
		break;
	}
}

void USART0_IRQHandler(void)
{
    uint8_t rev;
	if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
	{			
		rev = usart_data_receive(USART0);    //  接收串口数据
		uart0Rev(rev);		
    }    
}


void USART1_Config(void)
{
    //与通讯板通讯
	rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_USART1);   // 使能串口时钟
	
	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_5 | GPIO_PIN_6);
	
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5);   // GPIO 模式设置
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);  // 输出参数设置

	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);   // GPIO 模式设置      设为GPIO_MODE_INPUT，RX不好用
	// gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  // 输出参数设置


    usart_deinit(USART1);    // 串口复位
    usart_word_length_set(USART1, USART_WL_8BIT);  // 字长
    usart_stop_bit_set(USART1, USART_STB_1BIT);    // 停止位
    usart_parity_config(USART1, USART_PM_NONE);
    usart_baudrate_set(USART1, 9600U);     // 波特率
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);     // 接收使能
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);   // 发送使能
	usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
    usart_enable(USART1);           // 串口使能
	
	nvic_irq_enable(USART1_IRQn, 2, 2);//使能USART0中断
	usart_interrupt_enable(USART1, USART_INT_RBNE);//接收中断打开
	usart_interrupt_enable(USART1, USART_INT_ERR);//新增错误中断
	
	ReceiveCount1=0;
	TotalReceiveCount1=7;
	rcv_frame1=0;
	rcv_time1=0;

}

void uart1Send(void)
 {
	uint8_t i;
	SendCount1=0;

	usart_interrupt_enable(USART1, USART_INT_TBE);	//使能发送中断
	//  uint8_t i;
    //  for (i = 0; i < TotalSendCount1; i++)   
    //  {           
    //      while(usart_flag_get(USART1, USART_FLAG_TC) == RESET);  
    //      usart_data_transmit(USART1,SendFrame1[i]);
    //  }     
	//  while(usart_flag_get(USART1, USART_FLAG_TC) == RESET);      
}


//modbus：设备地址+功能码+寄存器起始地址+寄存器数量+校验
void uart1Rev(uint8_t rev)
{
	uint8_t i;
	if(ReceiveCount1==0)
	{
		if((rev > 0x00) && (rev < 0x40))//0716  0x00<rev<0x21 -> (rev > 0x00) && (rev < 0x40)
		{
			ReceiveFrame1[0]=rev;
			ReceiveCount1=1;
		}
	}
	else if(ReceiveCount1<99)
	{
		ReceiveFrame1[ReceiveCount1] = rev; 
		ReceiveCount1++;			
		if(ReceiveCount1>TotalReceiveCount1)
		{
			for(i=0;i<=TotalReceiveCount1;i++)
				ReceiveTongXinBan[i]= ReceiveFrame1[i];
			rcv_frame1=1;			
			ReceiveCount1=0;
		}
	}
	
}

void USART1_IRQHandler(void)
{
    uint8_t rev;
	//uint32_t error_flags_yichu,error_flags_zhen;0716注释

	//接收中断处理
	if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
	{			
		rev = usart_data_receive(USART1);    //  接收串口数据
		uart1Rev(rev);		
    }    

	//发送中断处理
	if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_TBE))
	{
		usart_data_transmit(USART1, SendFrame1[SendCount1]);
		SendCount1++;
		if(SendCount1 >= TotalSendCount1)
		{
			while(RESET == usart_flag_get(USART1,USART_FLAG_TC))	{   }
			usart_interrupt_disable(USART1, USART_INT_TBE);
		}
    }

	/*0716
    if((RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_ERR_FERR)) || (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_ERR_ORERR)))
    {
        error_flags_yichu = usart_flag_get(USART1,USART_FLAG_ORERR);
		error_flags_zhen = usart_flag_get(USART1,USART_FLAG_FERR);
        if(error_flags_yichu & USART_FLAG_ORERR)
        {
            uint8_t dummy = usart_data_receive(USART1);
            (void)dummy;
        }
        if(error_flags_zhen & USART_FLAG_FERR)
        {
            uint8_t dummy = usart_data_receive(USART1);
            (void)dummy;
        }      
        usart_flag_clear(USART1, USART_FLAG_ORERR | USART_FLAG_FERR | USART_FLAG_PERR);
    }
	*/
	//注意: RBNE处理中usart_data_receive会同时清除错误标志0717
	//仅当RBNE未触发但错误标志独立置位时才需要处理
	FlagStatus orerr = usart_flag_get(USART1, USART_FLAG_ORERR);
	FlagStatus ferr  = usart_flag_get(USART1, USART_FLAG_FERR);
	FlagStatus perr  = usart_flag_get(USART1, USART_FLAG_PERR);
	if(orerr == SET || ferr == SET || perr == SET) //0717
    {
        uint8_t dummy = usart_data_receive(USART1);//0717 读DR清除错误标志
        (void)dummy;//0717 RBNE已处理过有效数据，这里仅丢弃
        usart_flag_clear(USART1, USART_FLAG_ORERR | USART_FLAG_FERR | USART_FLAG_PERR);//0717
    }
}

void USART2_Config(void)
{
    //与FECbus协议图显通讯
    rcu_periph_clock_enable(RCU_USART2);   // 使能串口时钟
	
	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_8 | GPIO_PIN_9);
	
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);   // GPIO 模式设置
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);  // 输出参数设置

	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);   // GPIO 模式设置
	gpio_output_options_set(GPIOD, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  // 输出参数设置


    usart_deinit(USART2);    // 串口复位
    usart_word_length_set(USART2, USART_WL_8BIT);  // 字长
    usart_stop_bit_set(USART2, USART_STB_1BIT);    // 停止位
    usart_parity_config(USART2, USART_PM_NONE);
    usart_baudrate_set(USART2, 9600U);     // 波特率
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);     // 接收使能
	usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);   // 发送使能
	usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_enable(USART2);           // 串口使能
	
	nvic_irq_enable(USART2_IRQn, 2, 3);//使能USART2中断
	usart_interrupt_enable(USART2, USART_INT_RBNE);//接收中断打开
	
	ReceiveCount2=0;
	TotalReceiveCount2=0;
	rcv_frame2=0;
	rcv_time2=0;
}

void uart2Send(void)
 {
	 RS485_OUT();
	 SendCount2=0;
	 usart_interrupt_enable(USART2, USART_INT_TBE);	//使能发送中断
	 
//     for (i = 0; i < TotalSendCount2; i++)   
//     {           
//         while(usart_flag_get(USART2, USART_FLAG_TC) == RESET);  
//         usart_data_transmit(USART2,SendFrame2[i]);
//     }     
//     while(usart_flag_get(USART2, USART_FLAG_TC) == RESET);          
}
 
void uart2Rev(uint8_t rev)
{
	//与新图显通讯
	uint8_t i;
	if(ReceiveCount2==0)
	{
		if(rev==0x7E)
		{
			ReceiveFrame2[0]=0x7E;
			ReceiveCount2=1;
		}
	}
	else if(ReceiveCount2==7)
	{
		ReceiveFrame2[7]=rev;
		TotalReceiveCount2=rev+11;
		ReceiveCount2=8;
	}
	else
	{
		if(ReceiveCount2<99)
		{
			ReceiveFrame2[ReceiveCount2] = rev; 
			ReceiveCount2++;			
			if(ReceiveCount2>=TotalReceiveCount2 && rev==0x7E)
			{
				//保存到缓存区
				for(i=0;i<TotalReceiveCount2;i++)
					RevCRTFrm[i]= ReceiveFrame2[i];
				rcv_frame2=1;
				ReceiveCount2=0;
			}
		}
	}
}

void USART2_IRQHandler(void)
{
    uint8_t rev;
	if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE))
	{			
		rev = usart_data_receive(USART2);    //  接收串口数据
		uart2Rev(rev);		
    }

	if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_TBE))
	{
		/* transmit data */
		usart_data_transmit(USART2, SendFrame2[SendCount2]);
		SendCount2++;
		if(SendCount2 >= TotalSendCount2)
		{
			while(RESET == usart_flag_get(USART2,USART_FLAG_TC))	{   }
			usart_interrupt_disable(USART2, USART_INT_TBE);
			RS485_IN();
		}
    }      
    
}

void USART3_Config(void)
{
    //与液晶通讯
    rcu_periph_clock_enable(RCU_UART3);   // 使能串口时钟
	
	gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_10 | GPIO_PIN_11);
	
	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);   // GPIO 模式设置
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // 输出参数设置

	gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_11);   // GPIO 模式设置
	gpio_output_options_set(GPIOC, GPIO_MODE_ANALOG, GPIO_OSPEED_50MHZ, GPIO_PIN_11);  // 输出参数设置


    usart_deinit(UART3);    // 串口复位
    usart_word_length_set(UART3, USART_WL_8BIT);  // 字长
    usart_stop_bit_set(UART3, USART_STB_1BIT);    // 停止位
    usart_parity_config(UART3, USART_PM_NONE);
    usart_baudrate_set(UART3, 115200U);     // 波特率
    usart_receive_config(UART3, USART_RECEIVE_ENABLE);    // 接收禁止
	usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);   // 发送使能
	usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE);
    usart_enable(UART3);           // 串口使能

}

void uart3Send(void)
 {
     uint8_t i,delay;
	 
	 //判断液晶忙
	 delay=0;
	 while(LCD_BUSY())
	 {
		 vTaskDelay(2);
		 delay++;
		 if(delay>10)
			 break;
	 }
	 
     for (i = 0; i < TotalSendCount3; i++)   
     {           
         while(usart_flag_get(UART3, USART_FLAG_TC) == RESET);  
         usart_data_transmit(UART3,SendFrame3[i]);
     }     
     while(usart_flag_get(UART3, USART_FLAG_TC) == RESET);          
}


void USART4_Config(void)
{
    //与打印机通讯
    rcu_periph_clock_enable(RCU_UART4);   // 使能串口时钟
	
	gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_12);
	gpio_af_set(GPIOD, GPIO_AF_8, GPIO_PIN_2);
	
	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);   // GPIO 模式设置
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);  // 输出参数设置
	
	gpio_mode_set(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_2);   // GPIO 模式设置
	gpio_output_options_set(GPIOD, GPIO_MODE_ANALOG, GPIO_OSPEED_50MHZ, GPIO_PIN_2);  // 输出参数设置

    usart_deinit(UART4);    // 串口复位
    usart_word_length_set(UART4, USART_WL_8BIT);  // 字长
    usart_stop_bit_set(UART4, USART_STB_1BIT);    // 停止位
    usart_parity_config(UART4, USART_PM_NONE);
    usart_baudrate_set(UART4, 9600U);     // 波特率
    usart_receive_config(UART4, USART_RTS_DISABLE);    // 接收使能
	usart_transmit_config(UART4, USART_TRANSMIT_ENABLE);   // 发送使能
	usart_hardware_flow_rts_config(UART4, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART4, USART_CTS_DISABLE);
    usart_enable(UART4);           // 串口使能

}

void uart4Send(void)
 {
     uint8_t i;
     for (i = 0; i < TotalSendCount4; i++)   
     {           
         while(usart_flag_get(UART4, USART_FLAG_TC) == RESET);  
         usart_data_transmit(UART4,SendFrame4[i]);
     }     
     while(usart_flag_get(UART4, USART_FLAG_TC) == RESET);          
}
 

void USART5_Config(void)
{
    //与电源通讯
    rcu_periph_clock_enable(RCU_USART5);   // 使能串口时钟
	
	gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_6 | GPIO_PIN_7);
	
	gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);   // GPIO 模式设置
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  // 输出参数设置

	gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_7);   // GPIO 模式设置
	gpio_output_options_set(GPIOC, GPIO_MODE_ANALOG, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  // 输出参数设置


    usart_deinit(USART5);    // 串口复位
    usart_word_length_set(USART5, USART_WL_8BIT);  // 字长
    usart_stop_bit_set(USART5, USART_STB_1BIT);    // 停止位
    usart_parity_config(USART5, USART_PM_NONE);
    usart_baudrate_set(USART5, 4800U);     // 波特率
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);    // 接收使能
	usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);   // 发送使能
	usart_hardware_flow_rts_config(USART5, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART5, USART_CTS_DISABLE);
    usart_enable(USART5);           // 串口使能
	
	nvic_irq_enable(USART5_IRQn, 2, 4);//使能USART5中断
	usart_interrupt_enable(USART5, USART_INT_RBNE);//接收中断打开

	ReceiveCount5=0;
	TotalReceiveCount5=0;
}

void uart5Send(void)
 {
     uint8_t i;
     for (i = 0; i < TotalSendCount5; i++)   
     {           
         while(usart_flag_get(USART5, USART_FLAG_TC) == RESET);  
         usart_data_transmit(USART5,SendFrame5[i]);
     }     
     while(usart_flag_get(USART5, USART_FLAG_TC) == RESET);          
}
 

void USART5_IRQHandler(void)
{
    uint8_t rev;
	if(RESET != usart_interrupt_flag_get(USART5, USART_INT_FLAG_RBNE))
	{			
		rev = usart_data_receive(USART5);    //  接收串口数据
		if(ReceiveCount5<49)
		{
			ReceiveFrame5[ReceiveCount5] = rev; 
			ReceiveCount5++;			
		}	
    }    
}


/****************************End*****************************/

/************************************************************ 
 * 函数:       Printf
 * 说明:       Printf函数使用说明
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
/*
    charc,s[20],*p;
    inta=1234,*i;
    floatf=3.141592653589;
    doublex=0.12345678987654321;
    p="Howdoyoudo";
    strcpy(s,"Hello,Comrade");
    *i=12;
    c='\x41';
		
    printf("a=%d\n",a);         // 结果输出十进制整数 a = 1234
    printf("a=%6d\n",a);        // 结果输出6位十进制整数 a = 1234
    printf("a=%06d\n",a);       // 结果输出十进制整数 a = 001234
    printf("a=%2d\n",a);        // a 超过2位，按照实际值输出 a = 1234
    printf("*i=%4d\n",*i);      // 输出4位十进制整数 i = 12
    printf("*i=%-4d\n",*i);     // 输出左对齐4位十进制整数 i = 12
    printf("i=%p\n",i);         // 结果输出i 地址 i = 06e4
    printf("f=%f\n",f);         // 结果输出浮点数 a = 3.141593
    printf("f=%6.4f\n",f);      // 结果输出六位其中小数点后4位的浮点数

    printf("x=%lf\n",x);        // 输出浮点数 x = 0.12345  
    printf("x=%18.16lf\n",x);   // 输出18位其中小数点后16位的长浮点数 x = 0.12345678987654321 
    printf("c=%c\n",c);         // 输出字符c=A		
    printf("c=%x\n",c);         // 输出字符的ASCII码值 c = 41
    printf("s[]=%s\n",s);       // 输出数组字符串s[] = Hello,Comrade
    printf("s[]=%6.9s\n",s);    // 输出最多9个字符的字符串 s[]= Hello,Co
    printf("s=%p\n",s);         // 输出数组字符串首字符地址 s = FFBE
    printf("*p=%s\n",p);        // 输出指针字符串 p="Howdoyoudo"
    printf("p=%p\n",p);         // 输出指针的值 p = 0194 

*/
