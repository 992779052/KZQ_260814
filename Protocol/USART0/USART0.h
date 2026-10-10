/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：USART0.h
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/
#ifndef __USART0_H
#define __USART0_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

/************************ 变量定义 ************************/

/************************ 函数定义 ************************/

extern uint8_t SendFrame0[250]; 
extern uint8_t ReceiveFrame0[40];  
extern uint8_t SendCount0;     //发送字节数
extern uint8_t TotalSendCount0;//发送总数
extern uint8_t ReceiveCount0;  //接收字节数
extern uint8_t TotalReceiveCount0;//接收总数
extern uint8_t rcv_frame0;	//一帧数据收完
extern uint8_t rcv_time0; //接收计时


extern uint8_t SendFrame1[250]; 
extern uint8_t ReceiveFrame1[250];  
extern uint8_t SendCount1;     //发送字节数
extern uint8_t TotalSendCount1;//发送总数
extern uint8_t ReceiveCount1;  //接收字节数
extern uint8_t TotalReceiveCount1;//接收总数
extern uint8_t rcv_frame1;	//一帧数据收完
extern uint8_t rcv_time1; //接收计时

extern uint8_t SendFrame2[100]; 
extern uint8_t ReceiveFrame2[100];  
extern uint16_t SendCount2;     //发送字节数
extern uint16_t TotalSendCount2;//发送总数
extern uint16_t ReceiveCount2;  //接收字节数
extern uint16_t TotalReceiveCount2;//接收总数
extern uint8_t rcv_frame2;	//一帧数据收完
extern uint8_t rcv_time2; //接收计时
extern uint8_t SendDelayTimes2;	//发后延迟

extern uint8_t RevCRTFrm[100];
extern uint8_t ReceiveTongXinBan[100];


extern uint8_t SendFrame3[200]; 
extern uint8_t ReceiveFrame3[200];  
extern uint8_t SendCount3;     //发送字节数
extern uint8_t TotalSendCount3;//发送总数
extern uint8_t ReceiveCount3;  //接收字节数
extern uint8_t TotalReceiveCount3;//接收总数

extern uint8_t SendFrame4[200]; 
extern uint8_t ReceiveFrame4[30];  
extern uint8_t SendCount4;     //发送字节数
extern uint8_t TotalSendCount4;//发送总数
extern uint8_t ReceiveCount4;  //接收字节数
extern uint8_t TotalReceiveCount4;//接收总数

extern uint8_t SendFrame5[50]; 
extern uint8_t ReceiveFrame5[50];  
extern uint8_t SendCount5;     //发送字节数
extern uint8_t TotalSendCount5;//发送总数
extern uint8_t ReceiveCount5;  //接收字节数
extern uint8_t TotalReceiveCount5;//接收总数


void USART0_Config(void);  // 配置函数
void uart0Send(void);   // 发送函数

void USART1_Config(void);  // 配置函数
void uart1Send(void);   // 发送函数

void USART2_Config(void);  // 配置函数
void uart2Send(void);   // 发送函数

void USART3_Config(void);  // 配置函数
void uart3Send(void);   // 发送函数		 

void USART4_Config(void);  // 配置函数
void uart4Send(void);   // 发送函数

void USART5_Config(void);  // 配置函数
void uart5Send(void);   // 发送函数
		 				    
#endif


/****************************End*****************************/

