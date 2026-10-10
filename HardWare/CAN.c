/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：CAN.c
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/

/************************* 头文件 *************************/

#include "CAN.h"

/************************* 宏定义 *************************/

/************************ 变量定义 ************************/
/************************ 函数定义 ************************/


/************************************************************ 
 * 函数:       CAN0_Init(void)
 * 说明:       CAN初始化
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
void CAN0_Init(void)
{
    //外部CAN通讯
	can_parameter_struct can_parameter;
    can_filter_parameter_struct can_filter0;

    rcu_periph_clock_enable(RCU_CAN0);

	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);  // 输出参数设置  CAN TX  发送引脚
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);   // GPIO 模式设置
	gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_1);
	
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  // 输出参数设置  CAN RX  接收引脚
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);   // GPIO 模式设置
	gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_0);
	
//	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);  // 输出参数设置  CAN TX  发送引脚
//	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);   // GPIO 模式设置
//	gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_12);
//	
//	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);  // 输出参数设置  CAN RX  接收引脚
//	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);   // GPIO 模式设置
//	gpio_af_set(GPIOA, GPIO_AF_9, GPIO_PIN_11);
	
	nvic_irq_enable(CAN0_RX0_IRQn,1,2);
	
    /* initialize CAN register */
    can_deinit(CAN0);
    
    /* initialize CAN */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.no_auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
//    /* baudrate 20kbps */
    can_parameter.prescaler = 250;
    /* baudrate 50bps */
//    can_parameter.prescaler = 100;
    can_init(CAN0, &can_parameter);

    /* initialize filter */
    /* CAN0 filter number */
    can_filter0.filter_number = 0;

    /* initialize filter */    
    can_filter0.filter_mode = CAN_FILTERMODE_MASK;
    can_filter0.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter0.filter_list_high = 0x0000;
    can_filter0.filter_list_low = 0x0000;
    can_filter0.filter_mask_high = 0x0000;
    can_filter0.filter_mask_low = 0x0000;  
    can_filter0.filter_fifo_number = CAN_FIFO0;
    can_filter0.filter_enable = ENABLE;
    can_filter_init(&can_filter0);	
	
	can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE0);  // 使能接收中断

}


void CAN1_Init(void)
{
    //内部CAN通讯
	can_parameter_struct can_parameter;
    can_filter_parameter_struct can_filter1;

    rcu_periph_clock_enable(RCU_CAN1);
	
	/* configure CAN1 GPIO, CAN1_TX(PB6) and CAN1_RX(PB5) */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_5);
    
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_6);	
	
	nvic_irq_enable(CAN1_RX0_IRQn,0,2);
	
	/* initialize CAN register */
    can_deinit(CAN1);
    
    /* initialize CAN */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.no_auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
    /* baudrate 50bps */
    can_parameter.prescaler = 100;
    can_init(CAN1, &can_parameter);

    /* initialize filter */
    /* CAN1 filter number */
    can_filter1.filter_number = 15;

    /* initialize filter */    
    can_filter1.filter_mode = CAN_FILTERMODE_MASK;
    can_filter1.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter1.filter_list_high = 0x0000;
    can_filter1.filter_list_low = 0x0000;
    can_filter1.filter_mask_high = 0x0000;
    can_filter1.filter_mask_low = 0x0000;  
    can_filter1.filter_fifo_number = CAN_FIFO0;
    can_filter1.filter_enable = ENABLE;
    can_filter_init(&can_filter1); 
	
	can_interrupt_enable(CAN1, CAN_INTEN_RFNEIE0);  // 使能接收中断
	
	can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &transmit_message);     // 初始化发送信息  
	can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &receive_message);  // 使能接收


}



/****************************End*****************************/

