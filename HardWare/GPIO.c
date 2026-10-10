/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：LED.c
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/

/************************* 头文件 *************************/

#include "GPIO.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/
extern uint8_t qtvoicecount;

/************************ 函数定义 ************************/

void voice_out(uint8_t num)
{
	m_Voice=num;
	switch(num)
	{
	case 0:	//消音
		gpio_bit_reset(GPIOE,GPIO_PIN_2);
		gpio_bit_reset(GPIOE,GPIO_PIN_3);
		gpio_bit_reset(GPIOE,GPIO_PIN_4);
		gpio_bit_reset(GPIOD,GPIO_PIN_13);
		break;
	case 6:	//喷洒声
		gpio_bit_reset(GPIOE,GPIO_PIN_2);
		gpio_bit_reset(GPIOE,GPIO_PIN_3);
		gpio_bit_reset(GPIOE,GPIO_PIN_4);
		gpio_bit_set(GPIOD,GPIO_PIN_13);
		break;
	case 5:	//气体高警
		gpio_bit_reset(GPIOE,GPIO_PIN_2);
		gpio_bit_reset(GPIOE,GPIO_PIN_3);
		gpio_bit_reset(GPIOE,GPIO_PIN_4);
		gpio_bit_set(GPIOD,GPIO_PIN_13);
		qtvoicecount=0;
		break;		
	case 4:	//气体低警
		gpio_bit_reset(GPIOE,GPIO_PIN_2);
		gpio_bit_reset(GPIOE,GPIO_PIN_3);
		gpio_bit_reset(GPIOE,GPIO_PIN_4);
		gpio_bit_set(GPIOD,GPIO_PIN_13);
		qtvoicecount=0;
		break;				
	case 3:	//火警声
		gpio_bit_reset(GPIOE,GPIO_PIN_2);
		gpio_bit_reset(GPIOE,GPIO_PIN_3);
		gpio_bit_set(GPIOE,GPIO_PIN_4);
		gpio_bit_reset(GPIOD,GPIO_PIN_13);
		break;
	case 2:	//联动声
		gpio_bit_set(GPIOE,GPIO_PIN_2);
		gpio_bit_reset(GPIOE,GPIO_PIN_3);
		gpio_bit_set(GPIOE,GPIO_PIN_4);
		gpio_bit_reset(GPIOD,GPIO_PIN_13);
		break;
	case 1:	//故障声
		gpio_bit_set(GPIOE,GPIO_PIN_2);
		gpio_bit_set(GPIOE,GPIO_PIN_3);
		gpio_bit_set(GPIOE,GPIO_PIN_4);
		gpio_bit_reset(GPIOD,GPIO_PIN_13);
		break;
	
	}
}

//直控IO输出
void gpio_out(uint8_t num,uint8_t type)
{
	if(type)
	{
		switch(num)
		{
		case 0:	//系统故障灯
			gpio_bit_set(GPIOE,GPIO_PIN_12);
			break;
		case 1:	//蜂鸣器
			gpio_bit_set(GPIOD,GPIO_PIN_13);
			break;
		case 2:	//火警继电器
			gpio_bit_set(GPIOD,GPIO_PIN_12);
			break;
		case 3:	//故障继电器
			gpio_bit_set(GPIOD,GPIO_PIN_11);
			break;
		case 4:	//可燃气体继电器1
			gpio_bit_set(GPIOE,GPIO_PIN_13);
			break;
		case 5:	//可燃气体继电器2
			gpio_bit_set(GPIOE,GPIO_PIN_14);
			break;		
		}
	}
	else
	{
		switch(num)
		{
		case 0:	//系统故障灯
			gpio_bit_reset(GPIOE,GPIO_PIN_12);
			break;
		case 1:	//蜂鸣器
			gpio_bit_reset(GPIOD,GPIO_PIN_13);
			break;
		case 2:	//火警继电器
			gpio_bit_reset(GPIOD,GPIO_PIN_12);
			break;
		case 3:	//故障继电器
			gpio_bit_reset(GPIOD,GPIO_PIN_11);
			break;
		case 4:	//可燃气体继电器1
			gpio_bit_reset(GPIOE,GPIO_PIN_13);
			break;
		case 5:	//可燃气体继电器2
			gpio_bit_reset(GPIOE,GPIO_PIN_14);
			break;		
		}
	}
}
	
/************************************************************ 
 * 函数:       LED_Init(void)
 * 说明:       LED初始化
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/

void GPIO_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);    // 初始化时钟
	rcu_periph_clock_enable(RCU_GPIOB);    // 初始化时钟
	rcu_periph_clock_enable(RCU_GPIOC);    // 初始化时钟
	rcu_periph_clock_enable(RCU_GPIOD);    // 初始化时钟
	rcu_periph_clock_enable(RCU_GPIOE);    // 初始化时钟
	
	//CAN_EN
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);   // GPIO 模式设置
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);  // 输出参数设置
	
	CANEN_ON();
	
	//手自动转换锁
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_14|GPIO_PIN_15);        // 引脚配置
    gpio_output_options_set(GPIOB, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, GPIO_PIN_14|GPIO_PIN_15);   // 参数设置
		
	//音响
	gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);   // GPIO 模式设置
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);  // 输出参数设置
	
	voice_out(0);
	
	//短路块
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_6);        // 引脚配置
    gpio_output_options_set(GPIOE, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, GPIO_PIN_6);   // 参数设置
	
	//RS485方向控制
	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_10);   // GPIO 模式设置
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // 输出参数设置
	
	RS485_IN();
	
	//继电器，蜂鸣器
	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13);   // GPIO 模式设置
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13);  // 输出参数设置
	
	//液晶忙判断
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_13);        // 引脚配置
    gpio_output_options_set(GPIOC, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, GPIO_PIN_13);   // 参数设置
	
	//系统故障灯，可燃气体继电器
	gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14);   // GPIO 模式设置
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14);  // 输出参数设置
	
	//CH453按键判断
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);        // 引脚配置
    gpio_output_options_set(GPIOC, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, GPIO_PIN_8);   // 参数设置
	
	
	gpio_out(0,1);
	gpio_out(1,0);
	gpio_out(2,0);
	gpio_out(3,0);
	gpio_out(4,0);
	gpio_out(5,0);
		
}



/****************************End*****************************/

