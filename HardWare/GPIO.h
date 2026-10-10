/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：LED.h
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/
#ifndef __LED_H
#define __LED_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

/************************* 宏定义 *************************/

#define CANEN_OFF()      gpio_bit_reset(GPIOA, GPIO_PIN_15) 	  // 低电平    
#define CANEN_ON()       gpio_bit_set(GPIOA, GPIO_PIN_15)      // 高电平   

#define	LOCK1_SET()     (gpio_input_bit_get(GPIOB,GPIO_PIN_14) == SET)     
#define	LOCKK_SET()     (gpio_input_bit_get(GPIOB,GPIO_PIN_15) == SET)     

#define RS485_IN()       gpio_bit_reset(GPIOD, GPIO_PIN_10)
#define RS485_OUT()      gpio_bit_set(GPIOD, GPIO_PIN_10)

#define CH453_KEY_DOWN() (gpio_input_bit_get(GPIOC,GPIO_PIN_8) == RESET)    
#define LCD_BUSY() 		(gpio_input_bit_get(GPIOC,GPIO_PIN_13) == RESET)    

 
/************************ 变量定义 ************************/

/************************ 函数定义 ************************/

void GPIO_Init(void);     // GPIO 初始化
void voice_out(uint8_t num);
void gpio_out(uint8_t num,uint8_t type);

#endif


/****************************End*****************************/

