/************************************************ 
* WKS GD32F427ZGT6核心板
* usbd_hw 驱动代码  
* 版本：V1.0								  
************************************************/	

#include "drv_usb_hw.h"


/**
 * @brief       配置USB时钟
 * @param       无
 * @retval      无
 */
void usb_rcu_config(void)
{
//    /* 这里选择PLLQ时钟做为CK48M的时钟源，系统时钟设为168M进行分频 */
//    rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);   /* PLLQ被选择为PLL48M时钟的时钟源 */
//  
//    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);   /* PLL48M被选择为CK48M时钟的时钟源 */

//    rcu_periph_clock_enable(RCU_USBFS);            /* 使能USBFS时钟 */
	
    /* 这里选择PLLQ时钟做为CK48M的时钟源，系统时钟设为168M进行分频 */
    //rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);   /* PLLQ被选择为PLL48M时钟的时钟源 */
  
    //rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);   /* PLL48M被选择为CK48M时钟的时钟源 */
	
	//使用PLLAI_Q作为PLL_SEL的输入
    rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLSAIP);
    //使用PLL_SEL作为usb主频
    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);

    /* configure PLLSAI          
     　PLLAI_N=192;PLL_P=192/4=48MHZ,PLL_Q=192/2=96MHZ,PLL_R=192/3=64MHZ
       注意，此处的PLL_P用于后面的USB主时钟，要确保为48MHZ
    */
    if(ERROR == rcu_pllsai_config(192, 4, 2))            
    {
        while(1);
    }
    
    rcu_osci_on(RCU_PLLSAI_CK);
    
    if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK))
    {
        while(1);
    }

    rcu_periph_clock_enable(RCU_USBFS);            /* 使能USBFS时钟 */	
}

/**
 * @brief       配置USB数据传输线的IO口
 * @param       无
 * @retval      无
 */
void usb_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);   /* 使能GPIOA时钟 */

    /* USBFS_DM引脚(PA11)，USBFS_DP引脚(PA12)模式配置 复用推挽输出*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12 );
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL3, GPIO_PIN_11 | GPIO_PIN_12 );

    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_11 | GPIO_PIN_12);
}

/**
 * @brief       配置USB中断
 * @param       无
 * @retval      无
 */
void usb_intr_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  
    nvic_irq_enable((uint8_t)USBFS_IRQn, 3U, 3U);     /* USBFS中断配置，抢占优先级0，子优先级3 */
}

/*!
    \brief      drives the VBUS signal through gpio(本例程未用到)
    \param[in]  state: VBUS states
    \param[out] none
    \retval     none
*/
void usb_vbus_drive (uint8_t state)
{
//    if(state == 1){
//        gpio_bit_set(RCU_GPIOA,GPIO_PIN_8);
//    } else {

//    }
    
}

/**
 * @brief       USBD 延时函数(以us为单位)
 * @param       usec   : 延时的us数
 * @retval      无
 */
void usb_udelay (const uint32_t usec)
{
	volatile uint16_t x,y;   
	for(x = usec;x>0;x--)
    {
      for(y = 3;y>0;y--);    
    }
}

/**
 * @brief       USBD 延时函数(以ms为单位)
 * @param       msec   : 延时的ms数
 * @retval      无
 */
void usb_mdelay (const uint32_t msec)
{
	volatile uint16_t x,y;   
	for(x = msec;x>0;x--)
    {
      for(y = 3000;y>0;y--);    
    }
}

