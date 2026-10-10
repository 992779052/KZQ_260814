
//SPIOption.h

#ifndef  SPIOPTION_H
#define  SPIOPTION_H

#include "HeaderFiles.h"


//SPI flash地址分配
//使用芯片W25Q128,16M字节，共4096个扇区   集中机需要注释


//sector 0~31   	通用数据存储	

//sector 1  系统配置存储
/*
0x1001 	m_LoopBoardNum;
0x1002 	m_MiehuoNum;
0x1003	m_DirectNum;
0x1004 	
0x1005 	m_PrintSet;
0x1006  m_CommSet;
0x1008	m_PanelAddr;
0x1009	m_PanelType;
0x100A	m_PanelNum;
0x100B	m_Code[0];
0x100C	m_Code[1];
0x100D	m_Code[2];
0x100E	m_Code[3];
0x100F	m_Code[4];
0x1010	m_Code[5];
0x1011	m_ZdSd;
0x1012  bIsolateVoice;	//声光屏蔽标志
0x1013  bOption;		//操作允许设置
0x1014  bInturn;		//声光广播交替启动标识
0x1015  VoiceRunTime;	//声光启动时间
0x1016  BroadcastRunTime;	//广播启动时间
0x1017  m_Code1[0];
0x1018  m_Code1[1];
0x1019  m_Code1[2];
0x101A  BengAnsPanel
0x101B  BengAnsLoop
0x101C  BengAnsAddr
0x101D  FecBusAddr

*/
//sector 2  从机登记存储区
//sector 3  检查功能需要的配置存储区

//secotr 30 屏蔽存储区
//secotr 31 历史记录指针存储区

//sector 32~47		火警历史记录  每条火警16字节，共4096条		 16块
//sector 48~63      联动历史记录  每条记录16字节，共4096条		 16块
//sector 64~127     其它历史记录  每条记录16字节，共16384条		 64块
//sector 128~143    可燃气体报警历史记录，每条记录16字节，共4096条		 16块

//sector 512~1023  联动编程存储  每条512字节*2048条，共1M字节	256块   预留512块
#define E_ProgBegin		0x200000	//4096*512

//sector 1024~1523   汉字注释信息  每条64字节*256地址*16回路*9机器，共576块
#define E_NoteBegin		0x400000	//4096*576

//sector 30 屏蔽存储区
#define E_DisableAddr		0x1E000

//secotr 31 历史记录指针存储区
#define E_HistoryInitAddr	0x1F000
#define E_FHistoryAddr		0x1F004	   
#define E_RHistoryAddr		0x1F008	
#define E_OHistoryAddr		0x1F00C
#define E_KFHistoryAddr		0x1F010


#define E_FireBegin			0x20000
#define E_FireEnd			0x2FFF0

#define E_RunBegin			0x30000
#define E_RunEnd			0x3FFF0

#define E_OtherBegin		0x40000
#define E_OtherEnd			0x7FFF0

#define E_KFireBegin		0x80000
#define E_KFireEnd			0x8FFF0


extern uint32_t pFireHistory; 	//火警历史记录指针
extern uint32_t	pRunHistory;	//联动历史记录指针
extern uint32_t	pOtherHistory;	//其它历史记录指针
extern uint32_t pKFireHistory; 	//燃气报警历史记录指针

//历史记录初始化
extern void InitHistory(void);

//写入一条历史记录	  0:其它 1：火警 2：联动
extern void WriteHisRecord(uint8_t type);

//屏蔽初始化
extern void InitDisable(void);



#endif



