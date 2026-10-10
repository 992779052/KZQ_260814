/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：RTC.c
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/

/************************* 头文件 *************************/

#include "RTC.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/

rtc_parameter_struct   rtc_initpara;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;

/************************ 函数定义 ************************/


/************************************************************ 
 * 函数:       rtc_pre_config(void)
 * 说明:       RTC配置
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/

void rtc_pre_config(void)
{
	rcu_periph_clock_enable(RCU_PMU);  // 使能PMU时钟
	
	pmu_backup_write_enable();  // 使能 RTC 写寄存器通道
	
	rcu_bkp_reset_disable();
	
	#if defined (RTC_CLOCK_SOURCE_IRC32K) 
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
  
          prescaler_s = 0x13F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    
          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

uint8_t byte_to_bcd(uint8_t Value) 
{ 
	uint32_t bcdhigh = 0;
 
	while(Value >= 10)
	{
		bcdhigh++;
		Value -= 10;
	}
	return  ((uint8_t)(bcdhigh << 4) | Value);
}

uint8_t bcd_to_byte(uint8_t Value)
{
	uint32_t tmp = 0;
	tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
	return (tmp + (Value & (uint8_t)0x0F));
}

/************************************************************ 
 * 函数:       rtc_setup(void)
 * 说明:       设置RTC时间和闹钟
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
void rtc_setup(void)
{
    /* setup RTC time value */

    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = byte_to_bcd(D_Year);
    //rtc_initpara.day_of_week = RTC_SATURDAY;
    rtc_initpara.month = byte_to_bcd(D_Month);
    rtc_initpara.date = byte_to_bcd(D_Day);
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.am_pm = RTC_AM;

    rtc_initpara.hour = byte_to_bcd(D_Hour);
    rtc_initpara.minute = byte_to_bcd(D_Minute);
    rtc_initpara.second = byte_to_bcd(D_Second);
	
    /* RTC current time configuration */
    rtc_init(&rtc_initpara);
    RTC_BKP0 = BKP_VALUE;
}

uint8_t rtc_read(void)
{
	uint8_t ss;
	rtc_current_time_get(&rtc_initpara);
	ss=bcd_to_byte(rtc_initpara.second);
	if(D_Second != ss )
	{
		D_Year=bcd_to_byte(rtc_initpara.year);
		D_Month=bcd_to_byte(rtc_initpara.month);
		D_Day=bcd_to_byte(rtc_initpara.date);
		
		D_Hour=bcd_to_byte(rtc_initpara.hour);
		D_Minute=bcd_to_byte(rtc_initpara.minute);
		D_Second=ss;
		return 1;
	}
	else 
		return 0;
}


/****************************End*****************************/

