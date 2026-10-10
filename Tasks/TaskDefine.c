/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：TaskDefine.c
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/

/************************* 头文件 *************************/

#include "TaskDefine.h"
#include "task.h"
#include "queue.h"

#include "usbh_core.h"
#include "malloc.h"
#include "exfuns.h"
#include "usbh_msc_core.h"
#include "drv_usb_hw.h"
#include "usbh_usr.h"
#include "drv_usb_core.h"
#include "usbh_core.h"
#include "fattester.h"
#include "semphr.h" //new_test  增加头文件
extern SemaphoreHandle_t xMutex;	//new_test


extern uint16_t Fcolor;		//前景色
extern uint16_t	Bcolor;		//背景色

extern uint8 	m_CanSendCount;		//CAN发送计时
extern uint8 	m_WCanSendCount;	//外部CAN发送计时
extern uint8 D_Year;
extern uint8 D_Month;
extern uint8 D_Day;
extern uint8 D_Hour;
extern uint8 D_Minute;
extern uint8 D_Second;
extern uint8 D_Week;
extern uint8 bSecond;	//秒标志
extern uint8 BrocastTimes;	//广播时间计数
extern uint8 WBrocastTimes;	//外部CAN广播时间计数
extern uint8 m_ZdSd;
extern uint8 bTimeSync;	//分机时钟同步
extern uint8 m_PanelAddr;
extern uint8 m_View;
extern uint8 ExternalCanFile[48];
extern volatile uint8 HaveExternalCanFile;
static uint8 ExternalCanFileNum;
static uint8 ExternalCanFileCollecting;
extern uint8 krq_SnedFlag;
extern uint8 TestKrKzqTimeDelay;
extern uint8 krq_SnedDelayFlag;

extern uint8 krndsend[5];	//实时上传的可燃气体浓度信息，依次为收到数据标志，地址，类型，浓度值高，浓度值低
#define CAN_INNER_SEND_INTERVAL      20
#define CAN_OUTER_SEND_INTERVAL      100
#define CAN_OUTER_FILE_SEND_INTERVAL 100


extern uint8 flashbuf[4096];

extern uint8 CanFile[250];  //CAN接收文件

uchar m_SendfileNum;


/************************* 宏定义 *************************/

/************************ 变量定义 ************************/
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t Main_Task_Handle = NULL;/* 主任务句柄 */
static TaskHandle_t Can_Task_Handle = NULL; /* Can通讯任务句柄 */
static uint16 Can0BusOffRecoverDelay = 0;

/* USB任务句柄 */
static TaskHandle_t USB_Task_Handle = NULL;


// 创建CAN接收队列
QueueHandle_t canrev_queue=NULL;

// 创建内部CAN发送队列
QueueHandle_t cansend_queue=NULL;

// 创建外部CAN发送队列
QueueHandle_t wcansend_queue=NULL;


can_receive_message_struct receive_message;    // 接收数据结构体
can_trasnmit_message_struct transmit_message;  // 发送数据结构体

// 创建usb发送到main的队列
QueueHandle_t sendtomain_queue;
// 创建main发送到usb的队列
QueueHandle_t sendtousb_queue;

my_usb_struct sendtomain_msg;
my_usb_struct usbmsg;

my_can_struct canrevmsg;
my_can_struct cansendmsg;
my_can_struct msg;
my_can_struct msg1;

usbh_host usb_host_msc;
usb_core_driver usbh_core;
uint8_t transmit_mailbox;
uint32_t timeout;

UINT br;

/************************ 函数定义 ************************/
static void AppTaskCreate(void);/* 用于创建任务 */

static void Main_Task(void* pvParameters);/* Main_Task */
static void Can_Task(void* pvParameters); /* Can_Task */

static void USB_Task(void* pvParameters);/* USB_Task任务实现 */

void usbh_user_demo(void)	//new_test	增加case 4
{
	uint32_t scaled = 0; 	// new_test 增加两个新的变量以实现显示百分比
	uint8_t percent = 0;	// new_test

    uint32_t total, free;
    uint8_t res = 0;
	uint16_t i = 0;
	uint16_t sector;
	uint32_t bytes_written=0; 
    i = f_mount(fs[2], "2:", 1);    /* 重新挂载U盘 */
    res = exfuns_get_free("2:", &total, &free);

	if (res == 0)
	{
		switch (usbmsg.flag)
		{	
		case 1:
			res = mf_open("2:note.dat",1);
			if(res ==0)
			{
				//taskENTER_CRITICAL();//new_test	进入临界区，关闭中断
				sector= E_NoteBegin/4096;
				for(i = 0; i<4400 ;i++)//64*11*16*200/512 = 4400
				{
					
					if((i % 8)==0)
					{
						Flash_Sector_Erase(sector,sector);
						sector++;						
					}
					f_read(fattester.file, fattester.fatbuf, 512, &br);
					Auto_Add_Write(E_NoteBegin + i * 512,fattester.fatbuf,512);
					if((i % 44)==1)
					{
						sendtomain_msg.flag=0x82;	//正在写flash
						sendtomain_msg.num=(i / 44)+1;
						xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示		
						vTaskDelay(10);						
					}
				}
				//taskEXIT_CRITICAL();//new_test 退出临界区，打开中断
				mf_close();
			}
			else
			{		
				//打开文件失败
				sendtomain_msg.flag=0x81;
				sendtomain_msg.num=0;
				xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示
			}
			break;
		case 2:
			res = mf_open("2:prog.dat",1);
			if(res ==0)
			{
				//taskENTER_CRITICAL();//new_test	进入临界区，关闭中断
				sector= E_ProgBegin/4096;
				for(i = 0; i<2048 ;i++)//512*2048
				{
					
					if((i % 8)==0)
					{
						Flash_Sector_Erase(sector,sector);
						sector++;						
					}
					f_read(fattester.file, fattester.fatbuf, 512, &br);
					Auto_Add_Write(E_ProgBegin + i * 512,fattester.fatbuf,512);
					if((i % 20)==1)
					{
						sendtomain_msg.flag=0x83;	//正在写flash
						sendtomain_msg.num=i / 20;
						xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示		
						vTaskDelay(10);						
					}
				}
				//taskEXIT_CRITICAL();//new_test 退出临界区，打开中断
				mf_close();
			}
			else
			{		
				//打开文件失败
				sendtomain_msg.flag=0x81;
				sendtomain_msg.num=0;
				xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示
			}
			break;
		case 3:
			res = mf_open("2:note1.dat",FA_WRITE | FA_CREATE_ALWAYS);
			if(res ==0)
			{
				//taskENTER_CRITICAL();//new_test	进入临界区，关闭中断
				for(i = 0; i<550 ;i++)
				{
					vTaskDelay(10);
					SPI_FLASH_HighSpeedReadCont(E_NoteBegin + i * 4096,4096,(INT8U *)flashbuf);
					f_write(fattester.file,flashbuf, 4096, &bytes_written);			
					// f_sync(fattester.file);//new_test 减少f_sync写入次数
					if((i % 5)==1)
					{
						if(i%50==0)
						{
						 	f_sync(fattester.file);		//new_test 减少f_sync写入次数				
						}
						
						scaled = (uint32_t)i * 100; // 扩大100倍避免浮点运算
						percent = (scaled + 275) / 550; // +275等效于四舍五入

						sendtomain_msg.flag=0x84;	//正在写文件
						sendtomain_msg.num=percent+1;
						xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示		
						vTaskDelay(10);						
					}				
				}
				//taskEXIT_CRITICAL();//new_test 退出临界区，打开中断
				mf_close();
			}
			else
			{
				//创建文件失败
				sendtomain_msg.flag=0x86;
				sendtomain_msg.num=0;
				xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示	
			}
			break;
		case 4:	//new_test 
			res = mf_open("2:prog1.dat",FA_WRITE | FA_CREATE_ALWAYS);
			if(res ==0)
			{
				//taskENTER_CRITICAL();//new_test	进入临界区，关闭中断
				for(i = 0; i<256 ;i++)
				{
					vTaskDelay(10);
					SPI_FLASH_HighSpeedReadCont(E_ProgBegin + i * 4096,4096,(INT8U *)flashbuf);
					f_write(fattester.file,flashbuf, 4096, &bytes_written);					
					if((i % 3)==1)
					{
						if(i%30==0)
						{
						 	f_sync(fattester.file);						
						}

						scaled = (uint32_t)i * 100; // 扩大100倍避免浮点运算
						percent = (scaled + 128) / 256; // +128等效于四舍五入



						sendtomain_msg.flag=0x85;	//正在写文件
						sendtomain_msg.num=percent+1;
					
							
						xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示		
						vTaskDelay(10);						
					}				
				}
				//taskEXIT_CRITICAL();//new_test 退出临界区，打开中断
				mf_close();
			}
			else
			{
				//创建文件失败
				sendtomain_msg.flag=0x86;
				sendtomain_msg.num=0;
				xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示
			}
			break;


		}
    }
    else
    {
        //读U盘失败
		sendtomain_msg.flag=0x80;
		sendtomain_msg.num=0;
		xQueueSend(sendtomain_queue,&sendtomain_msg,0);//发送消息让main在液晶上显示
		
    }
    

}
 
/************************************************************ 
 * 函数:       Setup_task(void)
 * 说明:       任务启动函数
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
void Setup_task(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  
  while(1);   /* 正常不会执行到这里 */       
}

/************************************************************ 
 * 函数:       AppTaskCreate(void)
 * 说明:       任务创建函数
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
  
  /* 创建CAN接收Queue */
  canrev_queue = xQueueCreate((UBaseType_t ) 256,/* 消息队列的长度 */
                            (UBaseType_t ) sizeof(my_can_struct));/* 消息的大小 */
	
  /* 创建内部CAN发送Queue */
  cansend_queue = xQueueCreate((UBaseType_t ) 100,/* 消息队列的长度 */
                            (UBaseType_t ) sizeof(my_can_struct));/* 消息的大小 */

  /* 创建外部CAN发送Queue */
  wcansend_queue = xQueueCreate((UBaseType_t ) 100,/* 消息队列的长度 */
                            (UBaseType_t ) sizeof(my_can_struct));/* 消息的大小 */
	
  sendtomain_queue = xQueueCreate((UBaseType_t ) 20,/* 消息队列的长度 */
                            (UBaseType_t ) sizeof(my_usb_struct));/* 消息的大小 */
	
  sendtousb_queue = xQueueCreate((UBaseType_t ) 20,/* 消息队列的长度 */
                            (UBaseType_t ) sizeof(my_usb_struct));/* 消息的大小 */
  
	
  /* 创建Main_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Main_Task, /* 任务入口函数 */
                        (const char*    )"Main_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Main_Task_Handle);/* 任务控制块指针 */
//  if(pdPASS == xReturn)
//    printf("创建Receive_Task任务成功!\r\n");
  
  /* 创建Can_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Can_Task, /* 任务入口函数 */
                        (const char*    )"Can_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )3,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Can_Task_Handle);/* 任务控制块指针 */				

  /* 创建USB_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )USB_Task, /* 任务入口函数 */
                        (const char*    )"USB_Task",/* 任务名字 */
                        (uint16_t       )1024,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&USB_Task_Handle);/* 任务控制块指针 */
//  if(pdPASS == xReturn)
//    printf("创建USB_Task任务成功!\r\n");
						

  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}


/************************************************************ 
 * 函数:       Main_Task(void)
 * 说明:       Main_Task
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
static void Main_Task(void* parameter)
{	
	uint8_t i;
	for(i=0;i<30;i++)
	{
		vTaskDelay(100);
		fwdgt_counter_reload(); // 喂狗
	}
  MainTaskInit();
  WirtePowerONOFF();
  while (1)
  {
	  MainTaskLoop();
  }
}

void CANSend(unsigned char LEN,unsigned char FF, unsigned int ID,unsigned char *data)
{
	
	uint8 i;
		
    transmit_message.tx_efid = ID;
    transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_EXTENDED;
    transmit_message.tx_dlen = LEN;
    
    for(i=0;i<LEN;i++) 
		transmit_message.tx_data[i] = data[i];
	
	/* transmit message */
	transmit_mailbox = can_message_transmit(CAN0, &transmit_message);
	/* waiting for transmit completed */
	timeout = 0xFF;
	while((CAN_TRANSMIT_OK != can_transmit_states(CAN0, transmit_mailbox)) && (0 != timeout)){
		timeout--;
	}
}

void m_CanSend(void)
{
	uint8_t i,bb,m_TotalSendNum;
	uint8 buf[8];
	uint16 m_ID;

	BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
	m_TotalSendNum = 45;

	//判断内部CAN发送
	if(m_CanSendCount==0)	
	{
		xReturn=xQueueReceive(cansend_queue,&msg1,0);
		if(pdTRUE == xReturn)
		{
			transmit_message.tx_efid = (msg1.type<<8) | msg1.add;
			transmit_message.tx_ft = CAN_FT_DATA;
			transmit_message.tx_ff = CAN_FF_EXTENDED;//扩展帧
			transmit_message.tx_dlen = msg1.length; 
			for(i=0;i<msg1.length;i++)
				transmit_message.tx_data[i] = msg1.rdata[i];
			can_message_transmit(CAN1,&transmit_message); 
			m_CanSendCount=CAN_INNER_SEND_INTERVAL;
		}
		else if(BrocastTimes>9)
		{
			//广播同步和自动手动信息,同时广播时间
			BrocastTimes=0;
			transmit_message.tx_efid = 0x4000;
			transmit_message.tx_ft = CAN_FT_DATA;
			transmit_message.tx_ff = CAN_FF_EXTENDED;//扩展帧
			transmit_message.tx_dlen = 8; 
			transmit_message.tx_data[0] = 0x71;
			transmit_message.tx_data[1] = m_ZdSd;
			transmit_message.tx_data[2] = D_Year;
			transmit_message.tx_data[3] = D_Month;
			transmit_message.tx_data[4] = D_Day;
			transmit_message.tx_data[5] = D_Hour;
			transmit_message.tx_data[6] = D_Minute;
			transmit_message.tx_data[7] = D_Second;
			can_message_transmit(CAN1,&transmit_message); 
			m_CanSendCount=CAN_INNER_SEND_INTERVAL;			
		}			
	}
	
	//判断外部CAN发送
	if(m_WCanSendCount==0)	
	{
		if(bTimeSync)
		{
			//主机发送分机时钟同步
			bTimeSync=0;
			transmit_message.tx_efid = 0x4000+m_PanelAddr;	//数据串
			transmit_message.tx_ft = CAN_FT_DATA;
			transmit_message.tx_ff = CAN_FF_EXTENDED;//扩展帧
			transmit_message.tx_dlen = 7; 
			transmit_message.tx_data[0]=0x35;
			transmit_message.tx_data[1]=D_Year;
			transmit_message.tx_data[2]=D_Month;
			transmit_message.tx_data[3]=D_Day;
			transmit_message.tx_data[4]=D_Hour;
			transmit_message.tx_data[5]=D_Minute;
			transmit_message.tx_data[6]=D_Second;
			can_message_transmit(CAN0,&transmit_message); 
			m_WCanSendCount=CAN_OUTER_SEND_INTERVAL;
			WBrocastTimes=0;
		} else if(krq_SnedFlag==2)
		{
			
			//有可燃气浓度要发送
			bb=	m_TotalSendNum-m_SendfileNum;
			if(bb<9)
			{
				//文件结束
				m_ID=m_PanelAddr + 0x9000;	
				for(i=0;i<bb;i++)
					buf[i]=CanFile[m_SendfileNum+i];
				CANSend(bb,1,m_ID,buf);
				m_WCanSendCount=CAN_OUTER_FILE_SEND_INTERVAL;
				WBrocastTimes=0;
				m_SendfileNum=0;
				krq_SnedFlag = 0;
				krq_SnedDelayFlag = 0;
			}
			else if(m_SendfileNum==0)
			{
				//发送文件头
				m_ID=m_PanelAddr + 0x8000;	
				for(i=0;i<8;i++)
					buf[i]=CanFile[i];
				CANSend(8,1,m_ID,buf);
				m_WCanSendCount=CAN_OUTER_FILE_SEND_INTERVAL;
				WBrocastTimes=0;
				m_SendfileNum=8;
			}
			else
			{
				if(TestKrKzqTimeDelay > 0)
				{
					//发送文件数据
					m_ID=m_PanelAddr + 0xA000;	
					for(i=0;i<8;i++)
						buf[i]=CanFile[m_SendfileNum+i];
					CANSend(8,1,m_ID,buf);
					m_WCanSendCount=CAN_OUTER_FILE_SEND_INTERVAL;
					WBrocastTimes=0;
					m_SendfileNum+=8;
					TestKrKzqTimeDelay = 0;
				}

			}


			
		}
		else 
		{
			xReturn=xQueueReceive(wcansend_queue,&msg1,0);
			if(pdTRUE == xReturn)
			{
				//有数据要发送
				transmit_message.tx_efid = (msg1.type<<8) | msg1.add;
				transmit_message.tx_ft = CAN_FT_DATA;
				transmit_message.tx_ff = CAN_FF_EXTENDED;//扩展帧
				transmit_message.tx_dlen = msg1.length; 
				for(i=0;i<msg1.length;i++)
					transmit_message.tx_data[i] = msg1.rdata[i];
				can_message_transmit(CAN0,&transmit_message); 
				m_WCanSendCount=CAN_OUTER_SEND_INTERVAL;
				WBrocastTimes=0;
			}
			else if(WBrocastTimes>14)
			{
				//广播自身信息
				WBrocastTimes=0;
				transmit_message.tx_efid=0x4000+m_PanelAddr;	//数据串
				transmit_message.tx_ft = CAN_FT_DATA;
				transmit_message.tx_ff = CAN_FF_EXTENDED;//扩展帧
				transmit_message.tx_dlen = 4; 
				transmit_message.tx_data[0]=0x70;
				transmit_message.tx_data[1]=m_PanelAddr;	
				transmit_message.tx_data[2]=0x01;	//控制器类型
				transmit_message.tx_data[3]=m_ZdSd;		
				can_message_transmit(CAN0,&transmit_message); 
				m_WCanSendCount=CAN_OUTER_SEND_INTERVAL;
			}
		}
	}
}

static void Can_Task(void* parameter)
{	 
    
  while (1)
  {
	  if(Can0BusOffRecoverDelay>0)
	  {
		  Can0BusOffRecoverDelay--;
	  }
	  else if(SET == can_flag_get(CAN0, CAN_FLAG_BOERR))
	  {
		  can_working_mode_set(CAN0, CAN_MODE_INITIALIZE);
		  can_working_mode_set(CAN0, CAN_MODE_NORMAL);
		  m_WCanSendCount=CAN_OUTER_SEND_INTERVAL;
		  WBrocastTimes=0;
		  Can0BusOffRecoverDelay=500;
	  }
	  m_CanSend();
	  vTaskDelay(2);
  }
}

/************************************************************ 
 * 函数:       CAN0_RX0_IRQHandler(void)
 * 说明:       中断接收配置
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/
/* 外部CAN接收中断。0x80表示文件首帧，0xA0表示中间帧，0x90表示末帧；收进独立的ExternalCanFile，最多48字节，避免与内部CAN/界面共用接收缓存。 */
void CAN0_RX0_IRQHandler(void)
{
    uint8_t i;
    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
	
	canrevmsg.source=1;	//外部CAN
	canrevmsg.add=receive_message.rx_efid & 0xFF;
	canrevmsg.type=(receive_message.rx_efid & 0xFF00)>>8;
	for(i=0;i<8;i++)
		canrevmsg.rdata[i]=receive_message.rx_data[i];
	
	if(canrevmsg.add<=PANEL_NET_MAX)
		m_Panel[canrevmsg.add]|=0x20;
	
	if(canrevmsg.type==0x40 )
	{
		if(canrevmsg.rdata[0] !=0x70)
			xQueueSendFromISR(canrev_queue,&canrevmsg,0);
	}
	else
	{
		/* 文件首帧：仅当上一份文件已被主循环取走时开始新收集，避免覆盖尚未处理的数据。 */
if(canrevmsg.type==0x80)
		{
			ExternalCanFileCollecting = (HaveExternalCanFile == 0);
			if(ExternalCanFileCollecting)
			{
				for(i=0;i<8;i++) ExternalCanFile[i]=canrevmsg.rdata[i];
				ExternalCanFileNum=8;
			}
		}
		/* 中间帧：仅在已收到首帧后追加8字节；长度超过48字节就丢弃本次收集，防止写出数组边界。 */
else if(ExternalCanFileCollecting && canrevmsg.type==0xA0)
		{
			if(ExternalCanFileNum + 8 <= 48)
			{
				for(i=0;i<8;i++) ExternalCanFile[ExternalCanFileNum+i]=canrevmsg.rdata[i];
				ExternalCanFileNum+=8;
			}
			else ExternalCanFileCollecting=0;
		}
		/* 末帧：追加最后8字节后结束收集，并置HaveExternalCanFile通知主循环处理整份文件。 */
else if(ExternalCanFileCollecting && canrevmsg.type==0x90)
		{
			if(ExternalCanFileNum + 8 <= 48)
			{
				for(i=0;i<8;i++) ExternalCanFile[ExternalCanFileNum+i]=canrevmsg.rdata[i];
				ExternalCanFileNum=0;
				ExternalCanFileCollecting=0;
				HaveExternalCanFile=0x80;
			}
			else ExternalCanFileCollecting=0;
		}
	}
}



void CAN1_RX0_IRQHandler(void)
{
    uint8_t i;
	/* check the receive message */
    can_message_receive(CAN1, CAN_FIFO0, &receive_message);
	
	canrevmsg.source=0;	//内部CAN
	canrevmsg.add=receive_message.rx_efid & 0xFF;
	canrevmsg.type=(receive_message.rx_efid & 0xFF00)>>8;
	canrevmsg.length=receive_message.rx_dlen;
	for(i=0;i<8;i++)
		canrevmsg.rdata[i]=receive_message.rx_data[i];
	
	if(canrevmsg.add<32)
		m_Board[canrevmsg.add] |=0x20;
			
	if(canrevmsg.type==0x40)		
	{
		if(canrevmsg.rdata[0]!=0x70)
			xQueueSendFromISR(canrev_queue,&canrevmsg,0);
		else
		{
			if(canrevmsg.add==1)
			{
				krndsend[0]=0x80;
				krndsend[1]=canrevmsg.rdata[2];
				krndsend[2]=canrevmsg.rdata[3];
				krndsend[3]=canrevmsg.rdata[4];
				krndsend[4]=canrevmsg.rdata[5];
			}
		}
	}
	else
	{
		if(canrevmsg.type==0x80) //文件开始
		{
			for(i=0;i<8;i++)
				CanFile[i]=canrevmsg.rdata[i];
			m_ReceiveCanFileNum=8;
		}
		else if(canrevmsg.type==0x90) //文件结束
		{
			for(i=0;i<8;i++)
				CanFile[m_ReceiveCanFileNum+i]=canrevmsg.rdata[i];
			m_ReceiveCanFileNum=0;
			HaveReciveCanFile=0x80; //收完文件
			if (krq_SnedFlag)
			{
				krq_SnedFlag++;//如果需要发送可燃气浓度，并且本机已经接收到可燃气浓度，则在m_CanSend里判断krq_SnedFlag标志，发送可燃气浓?
				krq_SnedDelayFlag = 1;
			}				
		}
		else if(canrevmsg.type==0xA0) //文件数据
		{
			for(i=0;i<8;i++)
				CanFile[m_ReceiveCanFileNum+i]=canrevmsg.rdata[i];
			m_ReceiveCanFileNum+=8;			
		}		
	}
}

void my_usb_init(void)
{
    
    my_mem_init(SRAMIN);                    /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);                   /* 初始化CCM内存池 */
    my_mem_init(SRAMEX);                    /* 初始化外部SRAM内存池 */
  
    exfuns_init();                          /* 为fatfs相关变量申请内存 */
	
	usb_gpio_config();                      /* USB GPIO口配置 */          
    usb_rcu_config();                       /* USB时钟配置 */
		
    /* register device class */
    usbh_class_register(&usb_host_msc, &usbh_msc);

    usbh_init(&usb_host_msc,
              &usbh_core,
              USB_CORE_ENUM_FS,
              &usr_cb);       /* 初始化USB主机 */

    usb_intr_config();        /* USB中断配置 */

}

static void USB_Task(void* pvParameters)
{
    uint8_t i = 0;
	BaseType_t xReturn;
	
	for(i=0;i<20;i++)
	{
		vTaskDelay(100);
		fwdgt_counter_reload(); // 喂狗
	}
	
	my_usb_init();
	
    while (1)
    {
        xReturn=xQueueReceive(sendtousb_queue,&usbmsg,0);
		if(pdTRUE == xReturn)
			usbh_user_demo();
		
		vTaskDelay(5);			
		usbh_core_task(&usb_host_msc); 
    }
}

/****************************End*****************************/



