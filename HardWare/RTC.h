/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：RTC.h
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/
#ifndef __RTC_H
#define __RTC_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

/************************* 宏定义 *************************/

/* 选择时钟源 */ 

#define RTC_CLOCK_SOURCE_LXTAL

//#define RTC_CLOCK_SOURCE_IRC32K

#define BKP_VALUE               0x32F1

/************************ 变量定义 ************************/
extern uint8_t D_Year;
extern uint8_t D_Month;
extern uint8_t D_Day;
extern uint8_t D_Hour;
extern uint8_t D_Minute;
extern uint8_t D_Second;
extern uint8_t D_Week;

/************************ 函数定义 ************************/

void rtc_pre_config(void);  // RTC 配置函数
void rtc_setup(void);  // 设置RTC时间和闹钟
uint8_t rtc_read(void);   // 读时钟
uint8_t bcd_to_byte(uint8_t Value);

	
#endif


/****************************End*****************************/

