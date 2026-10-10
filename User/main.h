/****************************************Copyright (c) ***************************************************
**                         Guangzhou ZHIYUAN electronics Co.,LTD.                               
**                                     
**                               http://www.embedtools.com
**
**--File Info---------------------------------------------------------------------------------------------
**  File name:              main.h
**  Last modified Date:
**  Last Version:           V1.0
**  Descriptions: 
**
**--------------------------------------------------------------------------------------------------------
** Created by:
** Created date: 
** Version: 
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/

#ifndef __MAIN_H 
#define __MAIN_H

#define PANEL_NET_MAX 64
#define PANEL_SLAVE_MAX 63

#ifdef __cplusplus
extern "C" {
#endif                                                                  /*  __cplusplus                 */


/*********************************************************************************************************
  定义与编译器无关的数据类型
*********************************************************************************************************/
typedef unsigned char  BOOLEAN;                                         /*  布尔变量                    */
typedef unsigned char  INT8U;                                           /*  无符号8位整型变量           */
typedef signed   char  INT8S;                                           /*  有符号8位整型变量           */
typedef unsigned short INT16U;                                          /*  无符号16位整型变量          */
typedef signed   short INT16S;                                          /*  有符号16位整型变量          */
typedef unsigned long  INT32U;                                          /*  无符号32位整型变量          */
typedef signed   long  INT32S;                                          /*  有符号32位整型变量          */
typedef float          FP32;                                            /*  单精度浮点数（32位长度）    */
typedef double         FP64;                                            /*  双精度浮点数（64位长度）    */

typedef unsigned char  uchar; 
typedef unsigned int   uint;  
typedef unsigned long  ulong; 

typedef unsigned char  uint8; 
typedef unsigned short uint16;  
typedef unsigned long  uint32; 

typedef unsigned char  u8; 
typedef unsigned short u16;  
typedef unsigned long  u32; 



// 设备类型枚举
typedef enum {
    DEVICE_KongZhiQi = 0,   // 控制器
    DEVICE_YanGan,           // 烟感
    DEVICE_WenGan,     // 温感
    DEVICE_ShouBao,    // 手报
    DEVICE_ShengGuang,      // 声光
    DEVICE_XiaoBao,      // 消报
    DEVICE_MoKuai,          // 模块
    DEVICE_QiTing,      // 启停
    DEVICE_KeRanQi,             // 可燃气
    DEVICE_QiMieFenQu,     // 气灭分区
    DEVICE_DuoXian,      // 多线

    DEVICE_TYPE_COUNT       // 设备类型总数
} TongXinBanDeviceType;


typedef struct	//modbus帧结构
{
    uint8_t addr;
	uint8_t function_code;  
	uint16_t register_addr;      
	uint16_t register_num;   
	uint8_t byte_num;
	uint16_t data[DEVICE_TYPE_COUNT][10];
	uint16_t send_data;
} Modbus_Data;

typedef struct {  //CAN结构
	uchar sign;  //标志
	uchar loop;	 
	uchar addr;
	uchar type;
	uchar type1;
}CCAN;

typedef struct
{
    uint8_t source;
	uint8_t add;
	uint8_t type;
	uint8_t length;
    uint8_t rdata[8];             
} my_can_struct;

typedef struct
{
    uint8_t flag;
	uint16_t num;            
} my_usb_struct;




typedef struct CFECbusStruct{  //FECBus帧结构
	uint8 type;   //帧类型
	uint8 target;	//目标地址
	uint8 pa;		//优先级
	uint8 source;	//源地址
	uint8 mnum;		//报文编号
	uint8 tnum;		//分组编号
	uint8 dlc;		//数据长度
	uint8 da[8];	//数据
}CFecbusStruct;


typedef struct my_RECORD{
    uint8 sign;
    uint8 panel;
    uint8 loop;
    uint8 addr;
    uint8 type;
    uint8 type1;
    uint8 type2;
    uint8 year;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 minute;
	uint8 second;
	uint8 confirm;
}my_Record;

extern my_Record m_Record;  //存储的记录

extern uint8 HistoryBuf[256];

extern uint8 	m_Board[33]; //各板状态
extern uint8	m_Panel[PANEL_NET_MAX+1]; //机器状态

extern uint8 HaveReceiveZlData;	//收到纵览数据
extern uint8 m_Zlbuf[8];

extern uint8 CanFile[250];  //CAN接收文件
extern uint8 m_ReceiveCanFileNum; //接收CAN字节数
extern uint8 HaveReciveCanFile; //接收CAN文件标志


extern uint16 	Fcolor;		//前景色
extern uint16	Bcolor;		//背景色
extern uint8 	m_View;
extern uchar D_Year;
extern uchar D_Month;
extern uchar D_Day;
extern uchar D_Hour;
extern uchar D_Minute;
extern uchar D_Second;
extern uchar D_Week;

extern uint16 Total_Isolate;
extern uint8 m_deng[4];	//指示灯
extern uint8 m_Voice;		//当前音响


extern uint8 m_View;
extern uint16  CrtSendDelay;	//CRT发送延迟
extern uint8   nettrstimes;	//组网传输灯时间

extern INT8U  FjState[8];	//查询分机状态用  依次为标志 byte0=0x80,需要查询，0xC0查到，byte1开始为机器号，类型，手自动，气体分区手自动

#ifdef __cplusplus
    }
#endif                                                                  /*  __cplusplus                 */
#endif                                                                  /*  __MAIN_H                    */
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
