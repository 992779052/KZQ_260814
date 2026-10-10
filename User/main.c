

/************************* 头文件 *************************/

#include "HeaderFiles.h"
#include "task.h"
#include "queue.h"
#include "semphr.h" //new_test  增加头文件

/************************* 宏定义 *************************/

#define SgAddr        99    //总线声光地址

#define FecbusSoure   1		//集中控制器发送源
#define FecbusTarget  3     //图显或用传为目标       2



#define Key_Xing		0x5B	//*
#define Key_Jing		0x59	//#
#define Key_Menu		0x43		
#define Key_Reset1    	0x61		

#define Key_Up      	0x62
#define Key_Down   		0x79
#define Key_Before  	0x72	//方向键左
#define Key_Next		0x69	//方向键右

#define Key_Enter    	0x6A
#define Key_Esc			0x7A
#define Key_BackSpace 	0x71

#define Key_VoiceOut    0x44	//报警器
#define Key_RunAct   	0x4C    //联动启动
#define Key_Confirm     0x54	//信息确认
#define Key_SelfTest   	0x5C	//自检
#define Key_Silence   	0x6B	//消音
#define Key_Reset	 	0x73	//复位
#define Key_JC			0x7B	//检查

#define PowerLowValue   235		//备电欠压设定值23.5

/************************ 变量定义 ************************/
extern SemaphoreHandle_t xMutex;	//new_test

// 创建CAN接收队列
extern QueueHandle_t canrev_queue;
// 创建内部CAN发送队列
extern QueueHandle_t cansend_queue;
// 创建外部CAN发送队列
extern QueueHandle_t wcansend_queue;

extern my_can_struct canrevmsg;
extern my_can_struct cansendmsg;
extern my_can_struct msg;
extern my_can_struct msg1;

// 创建usb发送到main的队列
extern QueueHandle_t sendtomain_queue;
// 创建main发送到usb的队列
extern QueueHandle_t sendtousb_queue;

my_usb_struct sendtousb_msg;
my_usb_struct usbmsg1;

extern  uint8_t byte_to_bcd(uint8_t Value); 


uint32_t TimCounter;  // 计数器
uint8_t  LedFlag;     // LED 翻转标志

//	BIT 7	      BIT 6	       BIT 5	   BIT 4	   BIT 3	   BIT 2	   BIT 1	   BIT 0
//0	反馈        启动延时	   启动        消音  	    故障	   火警        备电        主电
//1	备电故障      检查         屏蔽	       监管 	    自动	   手动	       声故        声启
//2 主电故障    可燃延时     可燃故障    信息接收     LED20         运行      信息发送    可燃报警   
uint8 m_deng[4];	//指示灯 
uint8 key;		//键值
uint8 keyvalue;
uint8 specialkey;	//特殊按键
uint8 m_KeyVoiceDelay;	//键盘音响延时
uint8 m_SelfTestTimes;	//自检计时
uint8 m_Dengloop;	//指示灯刷新循环

uint8 m_Voice;		//当前音响
uint8 m_View;		//当前页面
uint8 m_View1;      
uint8 m_FirstView;	//首次显示页面
uint8   m_LCDback;  //液晶背光计数
uint8	m_LcdLightStatus;	//液晶背光标志

uint8  bTiaoshi;

uint8 fengjiflag;	//1：启动气灭分区30s倒计时时，二级火警，不允许根据可燃气浓度启动风机；0：根据可燃气浓度自动启停风机
					//默认为六路多线102回路2地址

uint16 Total_Fire;
uint16 Total_Error;
uint16 Total_Act;
uint16 Total_Answer;
uint16 Total_Isolate;
uint16 Total_Watch;
uint16 Total_Gas;
uint16 Total_Fire1;
uint16 Total_Fire2;
uint16 Total_KError;


uint8 	m_LoopBoardNum;
uint8 	m_DirectNum;
uint8	m_MiehuoNum;
uint8	m_PrintSet; //打印设置
uint8	m_CommSet;	//通讯选择
uint8	m_PanelAddr;	//本机地址
uint8	m_PanelAddr1;	//跨机启动主从临时标记的本机地址
uint8	m_PanelAddr2;	//查询可燃气浓度的机器地址
uint8	krq_SnedFlag;	//可燃气浓度查询标志，外can收到查询指令后置1，代表需要发送浓度值
uint8 krq_SnedDelayFlag;//主机查询从机可燃气浓度，延时发送数据标志

uint8	m_PanelType;	//本机类型主机从机
uint8	m_PanelNum;		//带从机数
uint8	m_Code[6];	//设置二级密码
uint8	m_Code1[3];	//设置一级密码
uint8 	m_Input[100];  //输入

uint8   m_SoundStatus; //声光状态

uint8 	NoteBuf[65];


CCAN m_CanCommand;
CCAN m_CanReturn; //命令反馈
uint8 m_Return; //有命令反馈

uint8 m_Cursor;
uint8 m_Cursor1;
uint8   isRegister;
uint8	RegisterTimes;

uint8	sggznum;
uint8	sgqdnum;

uint8   m_SetTestRefrshLcd;		//自检时刷屏标志
uint8	m_RunTimes;		//启动后需要常亮的时间


uint8   m_Board[33]; //各板状态
uint8   m_Panel[PANEL_NET_MAX+1]; //机器状态

uint8   m_OhterReset;	//其它机器复位标志


my_Record m_Record;  //存储的记录
my_Record FireList[256];
my_Record ErrorList[500]; 	//综合故障记录
my_Record RunList[256]; 	//联动记录
my_Record KFireList[64];    //可燃气体报警记录
uint16 pError;  //写入故障指针
uint16 pRun;    //写入联动指针
uint16 pFire;	//写入火警指针
uint8  pKFire;  //写入可燃气体报警指针

my_Record MhRunList[64];    //气体灭火控制器联动显示区
uint16 pmhRun;
my_Record HjErrList[256];	//火灾报警控制器的故障显示区
uint16 phjError;  //写入火警控制器故障指针
my_Record LdErrList[256];	//消防联动控制器的故障显示区
uint16 pldError;  //写入火警控制器故障指针
my_Record KrErrList[64];	//可燃控制器的故障显示区
uint16 pkrError;  //写入火警控制器故障指针
my_Record MhErrList[128];	//气灭控制器的故障显示区
uint16 pmhError;  //写入火警控制器故障指针

uint8 hjzonetype;
uint8 krzonetype;
uint8 ldzonetype;
uint8 mhzonetype;
uint8 optzonenum;	//火警页面下可操作的分区号

my_Record  DisableList[100];	//要向CRT发送的屏蔽及解除
uint8	m_pCRTDis;   	//发送屏蔽及解除指针
uint8	m_pWriteDis; 	//写入屏蔽及解除指针
uint16	m_SendDisNum;	//初始发送屏蔽数
uint8	bHaveSendDis;	//初始已发送屏蔽标志

uint8   m_NumofEnd;		//历史记录中连续出现的0xFF


uint16  m_ActNoAnswer; //启动未反馈总数
uint16  m_ActAndAnswer; //启动并反馈总数
uint8 	m_RunLamp;
uint8	bRefreshRunDisplay;	//刷新联动显示区
uint8	RefreshRunTimes;	//刷新联动显示区间隔
uint8	bRefreshErrorDisplay;	//刷新故障显示区
uint8	RefreshErrorTimes;	//刷新故障显示区间隔
uint8   RunDelayNum;	//启动延时的总数，用于灯状态
uint8   bFlash;

uint16 DisFireNum;
uint16 DisRunNum;
uint16 DisWatchNum;
uint8 m_FireAuto;
uint8 m_RunAuto;
uint8 m_mhRunAuto;

uint8  m_KrFireAuto;
uint8  DisKrFireNum;
uint8  HaveDisplayKrFire;

uint16 Display_Number1;
uint16 Display_Number;
uint16 HaveDisplay;
uint16 HaveDisplayRun;
uint8 m_ErrorAuto;
uint8 m_DisableAuto;
uint8 m_WatchAuto;
uint8 bConfirm;	//信息确认
uint16 CurDisplay;		//当前需要确认的行
uint16 CurDisNum;		//需要确认的火警显示地址
uint8	bDisplaying;	//故障、火警等一条一条正在显示
uint8	bDisplaying1;	//联动一条一条正在显示

uint8 DisMhNum;
uint8 HaveDisplayMh;

uint8_t HistoryBuf[256];

uint8_t m_ZdSd;	//自动手动状态		  D7=1 自动允许  D6=1 手动允许
uint8_t zdsdtimes1;
uint8_t zdsdtimes2;
uint8_t bldqq;		//有联动请求标志


uint8_t D_Year;
uint8_t D_Month;
uint8_t D_Day;
uint8_t D_Hour;
uint8_t D_Minute;
uint8_t D_Second;
uint8_t D_Week;
uint8_t bSecond;	//秒标志
uint8_t BrocastTimes;	//广播时间计数
uint8_t WBrocastTimes;	//外部CAN广播时间计数
uint8 	m_CanSendCount;		//CAN发送计时
uint8 	m_WCanSendCount;	//外部CAN发送计时

uint16  m_AutoEscTime; //自动退出时间

uint8  bInturn;		//声光广播交替启动标识
uint8  VoiceRunTime;	//声光启动时间
uint8  BroadcastRunTime;	//广播启动时间


uint32 His_Addr;	//历史记录显示指针
uint8  bHistoryEnd;	//到历史记录末尾
uint32 PrintHisAddr;
uint32 PrintHisAddr1;
uint8  PrintHisNum;	//需打印历史记录的总数
uint8  PrintHisType;	//需打印历史记录的类型

uint8 m_PrintInit; //打印机初始化标志
uint32 	pPrintFire; 	//火警打印指针
uint32	pPrintRun;		//联动打印指针
uint32	pPrintOther;	//其它打印指针
uint32  pPrintKFire;	//燃气报警打印指针

uint32 	pCRTFire; 	//火警CRT指针
uint32	pCRTRun;	//联动CRT指针
uint32	pCRTOther;	//其它CRT指针
uint32	pCRTKFire;	//燃气报警CRT指针


uint16_t 	Fcolor;		//前景色
uint16_t	Bcolor;		//背景色

uint16 sberr[16];	//设备故障数

uint8 flashbuf[4096];

// 各设备类型的寄存器数量
const uint8_t TongXinBan_REGISTER_COUNT[DEVICE_TYPE_COUNT] = {
    1,   // 控制器
    10,  // 烟感
    10,  // 温感
    2,   // 手报
    2,   // 声光
    2,   // 消报
    6,   // 模块
    2,   // 启停
    6,   // 可燃气
    2,   // 气灭分区
    6    // 多线
};

// 各设备类型的起始寄存器地址
const uint16_t TongXinBan_START_ADDR[DEVICE_TYPE_COUNT] = {
    0x0000,  // 控制器
    0x0001,  // 烟感
    0x000B,  // 温感
    0x0015,  // 手报
    0x0017,  // 声光
    0x0019,  // 消报
    0x001B,  // 模块
    0x0021,  // 启停
    0x0023,  // 可燃气
    0x0029,  // 气灭分区
    0x002B   // 多线
};

// 状态位定义
#define STATUS_REGISTER   (1 << 0)  // bit0: 登记状态 (0:未登记;1:登记)
#define STATUS_ALARM      (1 << 1)  // bit1: 报警状态 (0:未报警;1:报警)
#define STATUS_FAULT      (1 << 2)  // bit2: 故障状态 (0:未故障;1:故障)
#define STATUS_START      (1 << 3)  // bit3: 启动状态 (0:未启动;1:启动)
#define STATUS_FEEDBACK   (1 << 4)  // bit4: 反馈状态 (0:未反馈;1:反馈)
#define STATUS_SHIELD     (1 << 5)  // bit5: 屏蔽状态 (0:未屏蔽;1:屏蔽)
#define STATUS_POLLUTION  (1 << 6)  // bit6: 污染状态 (0:未污染;1:污染)

// 可燃气状态位定义
#define KERANQI_UPLOAD_REGISTER    STATUS_REGISTER
#define KERANQI_UPLOAD_ALARM       STATUS_ALARM
#define KERANQI_UPLOAD_FAULT       STATUS_FAULT
#define KERANQI_UPLOAD_SPRAY       STATUS_START
#define KERANQI_UPLOAD_FEEDBACK    STATUS_FEEDBACK
#define KERANQI_UPLOAD_LOW_ALARM   0x0080
#define KERANQI_UPLOAD_HIGH_ALARM  0x0100
#define KERANQI_UPLOAD_FIRE2       0x0200
#define STATUS_UPLOAD_PENSA      	 0x0400
#define QIMIE_UPLOAD_STATE_MASK   (STATUS_UPLOAD_PENSA | KERANQI_UPLOAD_FIRE2 | STATUS_FEEDBACK)
#define KERANQI_UPLOAD_CONC_START  0x0031
#define KERANQI_UPLOAD_CONC_COUNT  6
#define TONGXINBAN_TOTAL_REGISTER_COUNT  (KERANQI_UPLOAD_CONC_START + KERANQI_UPLOAD_CONC_COUNT)

// 通信板值数组: TongXinBan_value[设备类型][索引]
// 每个元素16位，位定义如上

// 存储每个设备在TongXinBan_value中的索引
// device_index_map[回路][地址] = {类型, 索引}
typedef struct {
    uint8_t type;
    uint8_t index;
	uint8_t recorded;  // 0:未记录, 1:已记录
} DeviceIndex;

DeviceIndex device_index_map[7][151]; // 回路1-6, 地址1-150

// 每种类型设备的当前登记数量
uint8_t type_device_count[31] = {0};

// 全局标志，表示是否需要重新查询1-6回路登记
/* 自动登记扫描状态：管理上电后逐回路查询、超时重试和失败回路延后重扫。 */
uint8_t need_query_loops = 0;                    // 1=还有回路需要扫描；0=当前扫描已结束
uint8_t boot_query_register_loop = 1;             // 当前正在查询的回路号，正常范围为1到6
uint8_t boot_query_register_wait = 0;             // 1=该回路查询命令已发出，正在等待CAN文件回复
uint16_t boot_query_register_delay = 0;            // 扫描间隔/回复超时计数，由主循环累加
uint8_t boot_query_register_retry = 0;             // 当前回路已重试次数；达到上限后记录失败并换下一回路
uint8_t boot_query_register_failed_mask = 0;       // 失败回路位图：bit0=回路1，bit1=回路2……bit5=回路6；1表示失败
uint16_t boot_query_register_rescan_delay = 0;     // 失败回路延后重扫计数，避免立即反复占用CAN总线
uint8_t need_query_kr_register = 0;                // 1=需要发起可燃气数据查询；完成或重试结束后清零
uint8_t boot_query_kr_wait = 0;                    // 1=可燃气查询命令已发出，正在等待CAN回复
uint16_t boot_query_kr_delay = 0;                   // 气体查询发送延时/回复超时计数，由主循环累加
uint8_t boot_query_kr_retry = 0;                    // 当前气体查询已重试次数
uint8_t kr_query_priority = 0;                      // 1=Modbus在读气体寄存器，气体查询优先；0=按普通顺序调度



WINDOWS win;
uint8 bDispWin;

uint8 bTimeSync;	//分机时钟同步

uint8 HxzList[256][12];
uint16 phxz1;	//写黑匣子发送缓冲区
uint16 phxz2;	//处理黑匣子发送缓冲区
uint16 hxznum;	//写黑匣子的数量
uint8  hxzstate; //黑匣子状态

uint8 CanFile[250];  //CAN接收文件
uint8 m_ReceiveCanFileNum; //接收CAN字节数
uint8 HaveReciveCanFile;
uint8 ExternalCanFile[48];
volatile uint8 HaveExternalCanFile; //接收CAN文件标志

uint8	m_PowerCommand;	//向电源发送的命令
uint8	m_LastPowerCommand;
uint8	m_PowerComStatus;	//电源通讯状态
uint16	m_PowerStatus;  	//电源状态
uint16	m_PowerRev;
uint8	PowerData[30];
uint8 	bRevPowerData;
uint8	bPowerLow;

uint16 crc;
CFecbusStruct FecbusSendFrame[10];
uint8 fwater;
uint8 bNeedSendFecbus;
uint8 FecbusSendNum;
uint8 FecbusSend1;
uint8 bNeedAns;
uint8 bRevAns;
uint8 RepeatTimes;
uint8 AnsTimesDelay;
uint8 NormalTimesDelay;

uint8 SendLedStatus;
uint8 RevLedStatus;
uint8 bSendCRTCommDelay;
uint8 bRevCRTCommDelay;
uint8 bRevCRTReset;
uint8 bBuildlianjie;	//建立连接标志

uint8 ModbusDeviceType;
uint8 ModbusDeviceIndex;
Modbus_Data Rev_Modbus_Data;
Modbus_Data Send_Modbus_Data;
uint16	TongXinBan_value[DEVICE_TYPE_COUNT][10];
uint16	KeRanQiNongDu_value[KERANQI_UPLOAD_CONC_COUNT];
uint8 	TongXinBan_type_index;//根据设备类型对应通讯板寄存器索引
uint8 	TongXinBan_register_count;

// // 设备信息结构体
// typedef struct {
//     uint16_t loop;
//     uint16_t addr;
//     uint8_t type;
//     uint8_t registered;  // 是否已登记
//     uint8_t fault;       // 是否故障
// } DeviceInfo;

// // 为每种设备类型维护一个设备列表
// #define MAX_DEVICES_PER_TYPE 10  // 根据实际情况调整

// DeviceInfo TongXinBan_device_list[DEVICE_TYPE_COUNT][MAX_DEVICES_PER_TYPE];
uint16_t TongXinBan_device_count[DEVICE_TYPE_COUNT];  // 每种类型实际登记的设备数量

CFecbusStruct FecbusRevFrame[20];	//电气控制装置485接收多帧事件
uint8_t EleFrameNum;	//多帧事件的数量


uint8 TimeCount3;
uint8 TestKrKzqTimeDelay;

uint8	TimeToProg;
uint16	m_TotalLogic;	//编程总条数
uint16  m_LogicCycle; 	//编程循环
uint8   m_Logi[512]; 	//符合逻辑关系标志区
uint8	m_ProgBuf[512];	//读联动关系缓存
uint16  pQdZone;	//需启动停止的总数
uint8   QdZone[512][6];

uint16 sberr[16];	//设备故障数
uint16 totalsb[16];	//设计设备数，也就是设备登记数
uint8	m_CheckTotal;	//检查回路板总数
uint8	m_HaveCheck;	//已检查数

uint8 m_suo;	//手自动锁状态
uint8 qtvoicecount;

uint8 krndsend[5];	//实时上传的可燃气体浓度信息，依次为收到数据标志，地址，类型，浓度值高，浓度值低

uint8 bTestQx;
uint8 TestQxTimes;
uint16	m_LastX1;	//画曲线时上次的X坐标
uint16	m_LastY1;	//画曲线时上次的Y坐标
uint8	m_TotalDot1;	//已画曲线的点数

//得到键值
uint8 GetKey(uint8 k)
{
    switch(k)
    {
        case 0x63:    return 0x31;
        case 0x42:    return 0x32;
        case 0x41:    return 0x33;
        case 0x4B:    return 0x34;
        case 0x4A:    return 0x35;
        case 0x49:    return 0x36;
        case 0x53:    return 0x37;
        case 0x52:    return 0x38;
        case 0x51:    return 0x39;
        case 0x5A:    return 0x30;
        default:    return k;
    }
}


/************************ 函数定义 ************************/

//写入Can命令
void WriteCanCommand() 
{
	cansendmsg.source=0;
	cansendmsg.add=0;
	cansendmsg.type=0x40;
	cansendmsg.length=5;
	cansendmsg.rdata[0]=m_CanCommand.sign;
	cansendmsg.rdata[1]=m_CanCommand.loop;
	cansendmsg.rdata[2]=m_CanCommand.addr;
	cansendmsg.rdata[3]=m_CanCommand.type;
	cansendmsg.rdata[4]=m_CanCommand.type1;
	xQueueSend(cansend_queue,&cansendmsg,0);	
	
}

void QiDongFengJi(void)//启动风机，默认地址为回路102地址2
{
	if (fengjiflag!=1)
	{
		m_CanCommand.sign=0x02;
		m_CanCommand.loop=0x66;//102回路
		m_CanCommand.addr=0x02;//2号地址
		m_CanCommand.type=0;
		WriteCanCommand();
	}
}

void QiDongShengGuang(void)//启动声光，默认地址为回路4地址99
{
	m_CanCommand.sign=0x02;
	m_CanCommand.loop=0x04;//4回路
	m_CanCommand.addr=0x63;//99号地址
	m_CanCommand.type=0;
	WriteCanCommand();
}

void m_Treat453(void)
{
	uint8_t k;
	//判断按键
	if(CH453_KEY_DOWN())
	{
		k=CH453_ReadByte();	//读键值
		if(k>0x40 && k<0x80)
		{
			//有效按键
			keyvalue=GetKey(k);
		    m_AutoEscTime=0; //自动退出时间
			if(m_Voice==0)
			{
				m_KeyVoiceDelay=2; //键盘音响计数
				gpio_out(1,1);
			}
		}
	}
	//刷新八个指示灯
	if(m_SelfTestTimes==0)
	{	
		CH453_Write(0x60+m_Dengloop*2,m_deng[m_Dengloop]);	
	}
	else
	{	
		CH453_Write(0x60+m_Dengloop*2,0xFF);		//自检
	}
	m_Dengloop++;
	if(m_Dengloop>3)	m_Dengloop=0;	//4
	if(m_KeyVoiceDelay>0)
	{	
		m_KeyVoiceDelay--;
		if(m_KeyVoiceDelay==0 && m_Voice==0)
			gpio_out(1,0);
	}

}

/************************************************************ 
 * 函数:       TIMER_Config(void)
 * 说明:       定时器配置
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       系统主频200MHZ,
               APB1为4分频
               timer_initpara.prescaler为199，
			   timer_initpara.period为999，频率就为1KHZ
以上参数实测为定时2ms。
************************************************************/
void TIMER_Config(void)
{
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);
    /* 定时器参数配置 */
    timer_initpara.prescaler         = 199;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	nvic_irq_enable(TIMER1_IRQn, 0, 1);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    /* 自动加载定时器参数 */
    timer_auto_reload_shadow_enable(TIMER1);
    /* 使能定时器 */
    timer_enable(TIMER1);
}

/************************************************************ 
 * 函数:       TIMER1_IRQHandler(void)
 * 说明:       定时器1中断
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/

void TIMER1_IRQHandler(void)
{
	timer_flag_clear(TIMER1,TIMER_FLAG_UP);    // 清除中断标志
	
	TimCounter ++;        // 计数累加
	if((TimCounter % 10)==1)
		m_Treat453();	//刷新CH453
	else if((TimCounter % 10)==3)
		TimeToProg=1;
	else if((TimCounter % 10)==5)
	{
		//判断可燃气体气体报警声
		if(m_Voice==4)
		{
			if(qtvoicecount<15)
				gpio_bit_set(GPIOD,GPIO_PIN_13);
			else
				gpio_bit_reset(GPIOD,GPIO_PIN_13);
			qtvoicecount++;
			if(qtvoicecount>29)
			   qtvoicecount=0;
		}
		else if(m_Voice==5)
		{
			if(qtvoicecount<5)
				gpio_bit_set(GPIOD,GPIO_PIN_13);
			else
				gpio_bit_reset(GPIOD,GPIO_PIN_13);
			qtvoicecount++;
			if(qtvoicecount>9)
			   qtvoicecount=0;			
		}
	}
	
	if(bRefreshRunDisplay)
	    RefreshRunTimes++;	//刷新联动显示区间隔

	if(bRefreshErrorDisplay)
	    RefreshErrorTimes++; //刷新故障显示区间隔
	
	if(m_CanSendCount>0) 	m_CanSendCount--;
	if(m_WCanSendCount>0) 	m_WCanSendCount--;
	
	TimeCount3++;


}


/************************************************************ 
 * 函数:       FWDG_Config(void)
 * 说明:       FWDG配置
 * 输入:       无
 * 输出:       无
 * 返回值:     无
 * 作者        凌智君
 * 其他:       无
************************************************************/

void FWDG_Config(void)
{
	/* 使能 40k 时钟 */
	rcu_osci_on(RCU_IRC32K);  // IRC40K时钟打开
	/* 看门狗计数时钟配置: 40KHz(IRC40K) / 64 = 0.625 KHz */
	fwdgt_config(2*500, FWDGT_PSC_DIV64);  // 看门狗时间设置 
	fwdgt_enable();  // 看门狗使能
}

// 初始化设备索引映射表
void InitDeviceIndexMap()
{
    uint8_t loop, addr;
    
    for(loop = 0; loop < 7; loop++)
    {
        for(addr = 0; addr < 151; addr++)
        {
            device_index_map[loop][addr].type = 0;
            device_index_map[loop][addr].index = 0;
			device_index_map[loop][addr].recorded = 0;
        }
    }
}

// 初始化设备记录系统
void InitDeviceRecordSystem(void)
{
    uint8 i, j;
    
    // 清零数组
    for(i=0; i<DEVICE_TYPE_COUNT; i++)
    {
        type_device_count[i] = 0;
        for(j=0; j<10; j++)
        {
            TongXinBan_value[i][j] = 0;
        }
    }
    
    // 初始化设备索引映射表
    InitDeviceIndexMap();
    
    need_query_loops = 1;
    boot_query_register_loop = 1;
    boot_query_register_wait = 0;
    boot_query_register_delay = 0;
    boot_query_register_retry = 0;
    boot_query_register_failed_mask = 0;
    boot_query_register_rescan_delay = 0;
    need_query_kr_register = 1;
    boot_query_kr_wait = 0;
    boot_query_kr_delay = 0;
    boot_query_kr_retry = 0;
    kr_query_priority = 0;
}

// 系统初始化
void System_Init()
{
	//uint32_t flash_id = 0;
	

	GPIO_Init();
	CAN0_Init();
	CAN1_Init();
	
	SPI_Flash_Init();    // SPI 初始化
	
	// 初始化CH453,SLEEP=0,2/4占空比,键盘中断使能，键盘使能，显示使能  01001011
	CH453_Write(0x48,0x4B);  	

	
	//flash_id = spi_flash_read_id();   // 获取Flash 的ID
	
	phxz1=0;	//写黑匣子发送缓冲区
	phxz2=0;	//处理黑匣子发送缓冲区
	hxznum=0;	//写黑匣子的数量
	fengjiflag=0;
	
	rtc_pre_config();  // RTC 配置函数
	if(BKP_VALUE != RTC_BKP0)
	{
		D_Year=24;
		D_Month=9;
		D_Day=25;
		D_Hour=16;
		D_Minute=59;
		D_Second=0;

		rtc_setup(); 
	}
	
	
	USART0_Config();	//黑匣子
	USART1_Config();    //通讯板
	USART2_Config();    //CRT
	USART3_Config();    //液晶
	USART4_Config();    //打印机
	USART5_Config();    //电源
	
	InitDeviceRecordSystem();
	
	InitHistory();
	
	TIMER_Config();
	
	FWDG_Config();// 狗
		
	m_suo=0;	//手自动锁状态

}	

int main(void)
{
	
	xMutex = xSemaphoreCreateMutex();
  
	if (xMutex != NULL) {
		xSemaphoreGive(xMutex); /* Give after create binary sem success. */
	}

	System_Init();   // 系统初始化
	
	m_OhterReset=0x00;	//其它机器复位标志
	
	Setup_task();
	
}

// 获取设备在TongXinBan_value中的状态
uint16_t GetDeviceStatus(uint8_t type, uint8_t index)
{
    if(type < DEVICE_TYPE_COUNT && index < 10)
        return TongXinBan_value[type][index];
    return 0;
}

uint16_t GetTongXinBan_type_index(uint16_t type)
{
	uint8 i,j;
	switch (type)
	{
		case 01:	type = DEVICE_YanGan;	break;		// 烟感
		case 02:	type = DEVICE_WenGan;	break;		// 温感
		case 04:	type = DEVICE_ShouBao;	break;		// 手报

		case 10:	type = DEVICE_XiaoBao;	break;		// 消报		
		case 11:
		case 12:
		case 13:
					type = DEVICE_MoKuai;	break;		// 模块

		case 15:	type = DEVICE_ShengGuang;	break;	// 声光

		case 21:	type = DEVICE_QiTing;	break;		// 启停

		case 22:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29: 	type = DEVICE_KeRanQi;	break;		// 可燃气
		case 23:	type = DEVICE_QiMieFenQu;	break;	// 气灭分区
		case 24:	type = DEVICE_DuoXian;	break;		// 多线

	default:
	break;
	}
	return type;
}

// 设置设备状态位
void SetDeviceStatusBit(uint16_t type1, uint8_t index, uint16_t status_bit, uint8_t value)
{
	uint16_t type = GetTongXinBan_type_index(type1);
    if(type >= DEVICE_TYPE_COUNT || index == 0 || index > 10)
        return;

    if(value)
        TongXinBan_value[type][index-1] |= status_bit;
    else
        TongXinBan_value[type][index-1] &= ~status_bit;
}

void SetKeRanQiUploadBit(uint8_t addr, uint16_t status_bit, uint8_t value)
{
    if(addr == 0 || addr > KERANQI_UPLOAD_CONC_COUNT)
        return;

    if(value)
        TongXinBan_value[DEVICE_KeRanQi][addr-1] |= status_bit;
    else
        TongXinBan_value[DEVICE_KeRanQi][addr-1] &= ~status_bit;
}

void SetKeRanQiUploadAlarm(uint8_t addr, uint8_t low_alarm, uint8_t high_alarm)
{
    SetKeRanQiUploadBit(addr, KERANQI_UPLOAD_ALARM, (low_alarm || high_alarm));
    SetKeRanQiUploadBit(addr, KERANQI_UPLOAD_LOW_ALARM, low_alarm);
    SetKeRanQiUploadBit(addr, KERANQI_UPLOAD_HIGH_ALARM, high_alarm);
}

void ResetKeRanQiUploadStatus(void)
{
	uint8_t i;
	for(i=1; i<=KERANQI_UPLOAD_CONC_COUNT; i++)
	{
		TongXinBan_value[DEVICE_KeRanQi][i-1] &= KERANQI_UPLOAD_REGISTER;
	}
}
void ResetTongXinBanRunStatus(void)
{
	uint8_t i;

	for(i=0; i<TongXinBan_REGISTER_COUNT[DEVICE_QiMieFenQu]; i++)
	{
		TongXinBan_value[DEVICE_QiMieFenQu][i] &= ~QIMIE_UPLOAD_STATE_MASK;
	}

	for(i=0; i<TongXinBan_REGISTER_COUNT[DEVICE_DuoXian]; i++)
	{
		TongXinBan_value[DEVICE_DuoXian][i] &= ~(STATUS_START | STATUS_FEEDBACK);
	}

	TongXinBan_value[0][0] &= ~(STATUS_START | STATUS_FEEDBACK | STATUS_UPLOAD_PENSA | KERANQI_UPLOAD_FIRE2);
}


void SetKeRanQiUploadConcentration(uint8_t addr, uint16_t concentration)
{
    if(addr == 0 || addr > KERANQI_UPLOAD_CONC_COUNT)
        return;

    KeRanQiNongDu_value[addr-1] = concentration & 0x3FFF;
}

// 处理设备状态
void HandleDeviceState(uint8 loop, uint8 address, uint8 state, uint8_t value)
{
    DeviceIndex dev_index;

	if(loop >= 7 || address >= 151)
		return;

	dev_index = device_index_map[loop][address];
    
    if(dev_index.type > 0 && dev_index.index > 0 && dev_index.index <= 10)
    {
        SetDeviceStatusBit(dev_index.type, dev_index.index, state, value);
    }
}

void SyncDuoXianUploadState(uint8 loop, uint8 address, uint16_t state, uint8_t value)
{
    DeviceIndex dev_index;
    uint16_t type;
    uint8_t index = 0;

    if(loop < 7 && address < 151)
    {
        dev_index = device_index_map[loop][address];
        type = GetTongXinBan_type_index(dev_index.type);
        if(type == DEVICE_DuoXian && dev_index.index > 0 &&
           dev_index.index <= TongXinBan_REGISTER_COUNT[DEVICE_DuoXian])
        {
            index = dev_index.index;
        }
    }

    if(index == 0 && address > 0 && address <= TongXinBan_REGISTER_COUNT[DEVICE_DuoXian])
    {
        index = address;
    }

    if(index == 0)
        return;

    if(value)
        TongXinBan_value[DEVICE_DuoXian][index-1] |= state;
    else
        TongXinBan_value[DEVICE_DuoXian][index-1] &= ~state;
}
void SyncShengGuangUploadState(uint8 loop, uint8 address, uint16_t state, uint8_t value)
{
    DeviceIndex dev_index;
    uint16_t type;
    uint8_t index = 0;

    if(loop < 7 && address < 151)
    {
        dev_index = device_index_map[loop][address];
        type = GetTongXinBan_type_index(dev_index.type & 0x1F);
        if(type == DEVICE_ShengGuang && dev_index.index > 0 &&
           dev_index.index <= TongXinBan_REGISTER_COUNT[DEVICE_ShengGuang])
        {
            index = dev_index.index;
        }
    }

    if(index == 0 && address > 0 && address <= TongXinBan_REGISTER_COUNT[DEVICE_ShengGuang])
    {
        index = address;
    }

    if(index == 0 && loop == 4 && (address == SgAddr || address + 1 == SgAddr) && type_device_count[DEVICE_ShengGuang] > 0)
    {
        index = 1;
    }

    if(index == 0)
        return;

    if(value)
        TongXinBan_value[DEVICE_ShengGuang][index-1] |= state;
    else
        TongXinBan_value[DEVICE_ShengGuang][index-1] &= ~state;
}
// 获取设备状态位
uint8_t GetDeviceStatusBit(uint8_t type, uint8_t index, uint16_t status_bit)
{
    if(type < DEVICE_TYPE_COUNT && index < 10)
        return (TongXinBan_value[type][index] & status_bit) ? 1 : 0;
    return 0;
}

void TingZhiFengJi(void)//停止风机，默认地址为回路102地址2
{
	m_CanCommand.sign=0x03;
	m_CanCommand.loop=0x66;//102回路
	m_CanCommand.addr=0x02;//2号地址
	m_CanCommand.type=0;
	WriteCanCommand();	
}

//刷新总数
void DisplayTotal(uint8 n)
{
	//0火警 1启动 2反馈 3喷洒 4故障 5屏蔽 6高警 7低警
	uint16 num,ll;
	uint8 r;
	switch(n)
	{
	case 0:		num=Total_Fire;		break;
	case 1:		num=Total_Act;		break;
	case 2:		num=Total_Answer;	break;
	case 3:		num=Total_Gas;		break;
	case 4:		num=Total_Error;	break;
	case 5:		num=Total_Isolate;	break;
	case 6:		num=Total_Fire1;	break;
	case 7:		num=Total_Fire2;	break;
	}
	ll=n*135+60;
	r=0;
	if(Fcolor !=WHITE || Bcolor !=BLUE)
	{
		SetColor(WHITE,BLUE);
		r=1;
	}
	if(n==6 || n==7)
	{
		if(num>99)	num=99;
		if(n==6)
			OutTwoChar1(60,456,num,0);
		else
			OutTwoChar1(160,456,num,0);	
	}
	else
	{
		HistoryBuf[0]=num/100+0x30;
		HistoryBuf[1]=(num%100)/10+0x30;
		HistoryBuf[2]=num%10+0x30;
		HistoryBuf[3]=0;
		TextOut(ll,0,HistoryBuf,0);	
	}
	if(r) 	SetColor(Fcolor,Bcolor); 
	
	if (Total_Fire | Total_Fire1 | Total_Fire2)
	{
		TongXinBan_value[0][0] |= 0x02;//通信板：控制器火警状态
	} else {
		TongXinBan_value[0][0] &= ~0x02;
	}
	
	if (Total_Error)
	{
		TongXinBan_value[0][0] |= 0x04;//通信板：控制器故障状态
	} else {
		TongXinBan_value[0][0] &= ~0x04;
	}

	if (Total_Act)
	{
		TongXinBan_value[0][0] |= 0x08;//通信板：控制器启动状态
	} else {
		TongXinBan_value[0][0] &= ~0x08;
	}
	
	if (Total_Answer)
	{
		TongXinBan_value[0][0] |= 0x10;//通信板：控制器反馈状态
	} else {
		TongXinBan_value[0][0] &= ~0x10;
	}

	if (Total_Isolate)
	{
		TongXinBan_value[0][0] |= 0x20;//通信板：控制器屏蔽状态
	} else {
		TongXinBan_value[0][0] &= ~0x20;
	}
	
	
//	if (Total_Fire1 | Total_Fire2)
//	{
//		QiDongFengJi();//高警|低警，启动风机
//		QiDongShengGuang();

//	} else {
//		TingZhiFengJi();
//	}
	
	if (Total_Gas)
	{
		TongXinBan_value[0][0] |= 0x0400;//通信板：喷洒状态
	} else {
		TongXinBan_value[0][0] &= ~0x0400;
	}
	
	if (Total_Fire1 )
	{
		TongXinBan_value[0][0] |= 0x80;//通信板：控制器低警状态
	} else {
		TongXinBan_value[0][0] &= ~0x80;
	}
	
	if (Total_Fire2)
	{
		TongXinBan_value[0][0] |= 0x0100;//通信板：控制器高警状态
	} else {
		TongXinBan_value[0][0] &= ~0x0100;
	}
	


}	

//首页显示
void FirstPageView()
{
	m_View=0;
	m_FirstView=0;
	m_View1=0;

	//液晶初始化
	LcdCtrl(1,0);  //开背光
	m_LcdLightStatus=1;	//液晶背光标志
	
	DrawPicture(0);	//显示首页

	Fcolor=WHITE;
	Bcolor=BLUE;
	SetColor(Fcolor,Bcolor);
	
	//Lcdclr();
	
	DrawRect(0,0,800,25,2);
	DrawRect(0,455,800,479,2);
	
	TextOut(0,0,"火警:",0);
	TextOut(135*1,0,"启动:",0);
	TextOut(135*2,0,"反馈:",0);
	TextOut(135*3,0,"喷洒:",0);
	TextOut(135*4,0,"故障:",0);
	TextOut(135*5,0,"屏蔽:",0);
	
	TextOut(0,456,"低警:",0);
	TextOut(100,456,"高警:",0);
	
	
	DisplayTotal(0);
	DisplayTotal(1);
	DisplayTotal(2);
	DisplayTotal(3);
	DisplayTotal(4);
	DisplayTotal(5);
	DisplayTotal(6);
	DisplayTotal(7);

//	Fcolor=WHITE;
//	Bcolor=NAVY;
//	SetColor(Fcolor,Bcolor);     

}

void FirePageView()
{
	//火警首页显示
	
	u16 y;
	Fcolor=WHITE;
	Bcolor=BLACK;
	SetColor(Fcolor,Bcolor);
	DrawRect(0,25,800,455,2);	//整体显示区域
	TextOut(50,26,"火灾报警控制器信息",0);
	TextOut(450,26,"消防联动控制器信息",0);
	TextOut(50,280,"可燃气体控制器信息",0);
	TextOut(450,280,"气体灭火控制器信息",0);


	Bcolor=BLACK;
	Fcolor=LGRAY;
	SetColor(Fcolor,Bcolor);
	DrawRect(388,25,391,455,3);	//画分隔线
	
	y=75*0+52;
	DrawLine(0,y,800,y);
	y=75*3+52;
	DrawLine(0,y,800,y);
	y=75*0+305;
	DrawLine(0,y,800,y);
	
	
	Fcolor=WHITE;
	Bcolor=BLACK;
	SetColor(Fcolor,Bcolor);
	
}

void MainTaskInit(void)
{
	uint8 i;
	uint16 j;
	m_deng[0]=0x01;
	m_deng[1]=0x00;
	m_deng[2]=0x00;
	
	gpio_out(0,1);
	gpio_out(1,0);
	gpio_out(2,0);
	gpio_out(3,0);
	gpio_out(4,0);
	gpio_out(5,0);

	
	Total_Fire=0;
	Total_Error=0;
	Total_Act=0;
	Total_Answer=0;
	Total_Isolate=0;
	Total_Watch=0;
	Total_Gas=0;
	Total_Fire1=0;
	Total_Fire2=0;
	Total_KError=0;
	
	m_ActNoAnswer=0; //启动未反馈总数
	m_ActAndAnswer=0; //启动并反馈总数
	m_RunLamp=0;
	
	key=0;
	keyvalue=0;

	
	bTiaoshi=0;
	pError=0;
	pRun=0;
	pFire=0;
	pKFire=0;
	
	pmhRun=0;
	phjError=0;
	pldError=0;
	pkrError=0;
	pmhError=0;
	
	m_pCRTDis=0;   	//发送屏蔽及解除指针
	m_pWriteDis=0; 	//写入屏蔽及解除指针
	m_SendDisNum=0;	//初始发送屏蔽数
	bHaveSendDis=0;	//初始已发送屏蔽标志
	
	m_SoundStatus=0;
	voice_out(0);
	
	sggznum=0;
	sgqdnum=0;
	m_RunTimes=0;	//启动后需要常亮的时间
	RunDelayNum=0;	//启动延时的总数，用于灯状态
	
	SendLedStatus=0;
	RevLedStatus=0;
	
	krndsend[0]=0x00;
	krndsend[1]=0;
	krndsend[2]=0;
	krndsend[3]=0;
	krndsend[4]=0;

	TongXinBan_value[0][0] = 0x01;//通信板：控制器初始登记状态为1

	for(i=0;i<16;i++)
		sberr[i]=0;	//设备故障数
	
	//读各种配置信息
	SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);
	if(HistoryBuf[1]<9)	
		m_LoopBoardNum=HistoryBuf[1];	//回路板
	else
		m_LoopBoardNum=2;//合并pt：默认2块回路板
	if(HistoryBuf[2]<9)	
		m_MiehuoNum=HistoryBuf[2];		//扩展灭火盘
	else
		m_MiehuoNum=0;
	if(HistoryBuf[3]<9)	
		m_DirectNum=HistoryBuf[3];		//扩展多线盘
	else
		m_DirectNum=0;
	
	//初始化各板
	for(i=0;i<32;i++)
		m_Board[i]=0;
	for(i=0;i<m_LoopBoardNum;i++)
		m_Board[i+1]=0x80;
	for(i=0;i<m_MiehuoNum;i++)
		m_Board[i+9]=0x80;
	for(i=0;i<m_DirectNum;i++)
		m_Board[i+17]=0x80;
	
	m_PrintSet=HistoryBuf[5]; //打印设置
	//m_CommSet=HistoryBuf[6]; //通讯选择
	m_PanelAddr=HistoryBuf[8];	//本机地址
	
	if(m_PanelAddr>PANEL_NET_MAX)	m_PanelAddr=0;
	m_PanelType=HistoryBuf[9];	//本机类型主机从机
	m_PanelNum=HistoryBuf[10];	//带从机数
	if(m_PanelNum>PANEL_SLAVE_MAX)	m_PanelNum=0;
	for(i=0;i<PANEL_NET_MAX;i++)
	{
		m_Panel[i]=0x00;
	}
	if(m_PanelType)
	{
		//主机
		for(i=1;i<m_PanelNum+1;i++)
			m_Panel[i]=0x80;	
	}
	for(i=0;i<6;i++)
	{
		m_Code[i]=HistoryBuf[i+11];	//二级密码
		if(m_Code[i]<0x30 || m_Code[i]>0x39)	m_Code[i]=0x30;	
	}
	
	//自动手动设置读取：保留掉电记忆，开机不产生新的钥匙切换。
	m_ZdSd=HistoryBuf[17] & 0xC0;
	if(m_ZdSd != 0x40 && m_ZdSd != 0xC0)
		m_ZdSd=0x40;
	if(LOCK1_SET())
		m_suo=0;
	else
		m_suo=0x80;
	if(m_ZdSd & 0x80)
	{
		m_deng[1] |=0x08;
		m_deng[1] &=0xFB;
	}
	else
	{
		m_deng[1] |=0x04;
		m_deng[1] &=0xF7;
	}

	
	//声光广播交替启动设置
	bInturn=HistoryBuf[20];
	VoiceRunTime=HistoryBuf[21];
	if(VoiceRunTime==0 || VoiceRunTime>99)
		VoiceRunTime=15;
	BroadcastRunTime=HistoryBuf[22];
	if(BroadcastRunTime==0 || BroadcastRunTime>99)
		BroadcastRunTime=15;	
	
	for(i=0;i<3;i++)
	{
		m_Code1[i]=HistoryBuf[i+23];	//一级密码
		if(m_Code1[i]<0x30 || m_Code1[i]>0x39)	m_Code1[i]=0x30;	
	}
	
	hxzstate=0; //黑匣子状态
	
	bTimeSync=0;	//分机时钟同步
	BrocastTimes=7;	//广播时间计数
	WBrocastTimes=0;	//外部广播时间计数	
	m_CanSendCount=20;	//CAN发送计时
	m_WCanSendCount=100;	//外部CAN发送计时


	m_PrintInit=0; //打印机初始化标志
	pPrintFire=pFireHistory; 	//火警打印指针
	pPrintRun=pRunHistory;		//联动打印指针
	pPrintOther=pOtherHistory;	//其它打印指针
	pPrintKFire=pKFireHistory;
	
	PrintHisNum=0;	//需打印历史记录的总数
	
	pCRTFire=pFireHistory; 	//火警CRT指针
	pCRTRun=pRunHistory;		//联动CRT指针
	pCRTOther=pOtherHistory;	//其它CRT指针
	pCRTKFire=pKFireHistory;
	
	InitDisable();	 //屏蔽初始化

	
	m_PowerCommand=0x33;	//向电源发送的命令
	m_LastPowerCommand=0;
	m_PowerStatus=0;		//电源状态
	m_PowerRev=0;  
	m_PowerComStatus=0;		//电源通讯状态
	bRevPowerData=0;
	bPowerLow=0;
	
	//新标准图显通讯用
	FecbusSendNum=0;
	FecbusSend1=0;
	bNeedSendFecbus=0;
	bNeedAns=0;
	bRevAns=0;
	RepeatTimes=0;
	
	SendLedStatus=0;
	RevLedStatus=0;
	bRevCRTReset=0;
	bRevCRTCommDelay=0;
	bSendCRTCommDelay=0;
	fengjiflag=0;
	
	TimeToProg=0;
	m_LogicCycle=0; 	//编程循环
	//清符合逻辑关系标志区
	for(j=0;j<512;j++)
		m_Logi[j]=0;

	pQdZone=0;	//需启动停止的总数		

	for(i=0;i<16;i++)
		sberr[i]=0;	//设备故障数

	FirstPageView();	
}

//存储开关机记录
void WirtePowerONOFF()
{
	uint8 t,t1;
	
	fwater=1;
	
	t1=GET_DATE_YR(RTC_BKP2);
	t=bcd_to_byte(t1);

	m_Record.sign=0x04; //关机
	t1=GET_DATE_YR(RTC_BKP2);
	t=bcd_to_byte(t1);
    m_Record.year=t;
	t1=GET_DATE_MON(RTC_BKP2);
	t=bcd_to_byte(t1);
    m_Record.month=t;
	t1=GET_DATE_DAY(RTC_BKP2);
	t=bcd_to_byte(t1);
    m_Record.day=t;
 	t1=GET_TIME_HR(RTC_BKP3);
	t=bcd_to_byte(t1);
	m_Record.hour=t;
	t1=GET_TIME_MN(RTC_BKP3);
	t=bcd_to_byte(t1);
    m_Record.minute=t;
	t1=GET_TIME_SC(RTC_BKP3);
	t=bcd_to_byte(t1);
    m_Record.second=t;

	WriteHisRecord(0); //写入关机记录
	
	rtc_read();

	m_Record.sign=0x01; //开机
	m_Record.year=D_Year;
	m_Record.month=D_Month;
	m_Record.day=D_Day;
	m_Record.hour=D_Hour;
	m_Record.minute=D_Minute;
	m_Record.second=D_Second;
	WriteHisRecord(0); //写入开机记录
}


void DealLed()
{
	if(TimCounter >= 250)
	{
		TimCounter = 0;
		//0.5s
		if(bFlash)
			bFlash=0;
		else
			bFlash=1;
		if(bFlash==0)
			m_deng[2] &=0xFB;
		else
			m_deng[2] |=0x04;
		
		//判断启动灯状态
        if(m_RunLamp>0)
        {
            if(m_ActAndAnswer>=m_ActNoAnswer || m_RunTimes>0)
                m_RunLamp=1;	//常亮
            else 
                m_RunLamp=2;	//闪亮
        }
		if(m_RunTimes>0)  m_RunTimes--;		 //启动后需要常亮的时间
		
		//判断发送灯状态
		if(bSendCRTCommDelay>1)
		{
			bSendCRTCommDelay--;
			SendLedStatus=2;	//闪亮
		}
		else if(bSendCRTCommDelay==1)
			SendLedStatus=1;	//常亮
		else
			SendLedStatus=0;	//灭
		

		//判断接收灯状态
		if(bRevCRTCommDelay>1)
		{
			bRevCRTCommDelay--;
			RevLedStatus=2;	//闪亮
		}
		else if(bRevCRTCommDelay==1)
			RevLedStatus=1;	//常亮
		else
			RevLedStatus=0;	//灭

		//刷新启动灯
		if(m_RunLamp==1)	m_deng[0] |=0x20;	//常亮
		else if(m_RunLamp==2)
		{
			//闪亮
			if(bFlash==0)
				m_deng[0] &=0xDF;
			else
				m_deng[0] |=0x20;	
		}
		else
			m_deng[0] &=0xDF;
		//接收指示灯
		if(RevLedStatus==1)	m_deng[2] |=0x10;	//常亮
		else if(RevLedStatus==2)
		{
			//闪亮
			if(bFlash==0)
				m_deng[2] &=0xEF;
			else
				m_deng[2] |=0x10;			
		}
		else
			m_deng[2] &=0xEF;
		//发送指示灯
		if(SendLedStatus==1)	m_deng[2] |=0x02;	//常亮
		else if(SendLedStatus==2)
		{
			//闪亮
			if(bFlash==0)
				m_deng[2] &=0xFD;
			else
				m_deng[2] |=0x02;			
		}
		else
			m_deng[2] &=0xFD;
		
		
		//判断延时灯
		if(RunDelayNum>0)
		{
			//闪亮
			if(bFlash==0)
				m_deng[0] &=0xBF;
			else
				m_deng[0] |=0x40;
		}
		else
			m_deng[0] &=0xBF;
		
		//声光故障灯
		if(sggznum>0)
			m_deng[1] |=0x02;
		else
			m_deng[1] &=0xFD;
		
	}
	
}




void HJSG()
{
	m_deng[0] &=0xEF;   //消音灯
	m_deng[0] |=0x04;	//火警灯
	voice_out(3);
	gpio_out(2,1);	//火警继电器
	if(Total_Fire<999)
		Total_Fire++;
	DisplayTotal(0);	//显示总数
	if(bTiaoshi)
	{
		 if(m_View==0 || m_View==91 || m_View==92 || m_View==93)
		 {
			 m_View=90;   //火警页面
			 m_FirstView=0;  //首次显示页面
		 }
	}
	else
	{
		 if(m_View !=90 && m_View !=61 && m_View !=46)
		 {
			m_View=90;   //火警页面
			m_FirstView=0;  //首次显示页面
			m_deng[1] &=0xBF;	//清检查灯
		 }
	}
	if(m_SoundStatus==0)
	{
		m_SoundStatus=3;	//声光启动
		m_deng[1] |=0x01;
		m_CanCommand.sign=0x01;  //启动声光命令
		m_CanCommand.loop=0x09;
		m_CanCommand.addr=0;
		m_CanCommand.type=0;
		WriteCanCommand();
	}
}

void KBJSG()
{
	m_deng[0] &=0xEF;   //消音灯
	m_deng[2] |=0x01;	//可燃气体报警灯
	if(m_Record.sign==0x7A)
	{
		gpio_out(4,1);	//继电器
		Total_Fire1++;
		DisplayTotal(6);	//显示总数
		voice_out(4);
	}
	else if(m_Record.sign==0x7B)
	{
		gpio_out(5,1);	//继电器
		Total_Fire2++;
		DisplayTotal(7);	//显示总数
		voice_out(5);
	}
	if(bTiaoshi)
	{
		 if(m_View==0 || m_View==91 || m_View==92 || m_View==93)
		 {
			 m_View=90;   //火警页面
			 m_FirstView=0;  //首次显示页面
		 }
	}
	else
	{
		 if(m_View !=90 && m_View !=61)
		 {
			m_View=90;   //火警页面
			m_FirstView=0;  //首次显示页面
			m_deng[1] &=0xBF;	//清检查灯
		 }
	}
}


//启动声光
void QDSG()
{
     if(m_Record.sign==0x8F && m_Record.type==0x01)
	 {
		 //喷洒启动
		 Total_Gas++;
		 DisplayTotal(3);
	 }
	 else
	 {
		 m_deng[0] |=0x20;
		 m_RunLamp=1;  //启动灯标志
		 if(Total_Act<999)
			Total_Act++;
		 DisplayTotal(1);	//刷新动作总数
	 }
     if(m_Voice<2 || m_Voice==4 || m_Voice==5)
     {
        voice_out(2);
	    m_deng[0] &=0xEF;   //消音灯
     }
     if(bTiaoshi)
	 {
		 if(m_View==0 || m_View==91 || m_View==92 || m_View==93)
		 {
			 m_View=90;   //火警页面
			 m_FirstView=0;  //首次显示页面
		 }
	 }
	 else
	 {
		 if(m_View !=90 && m_View !=61)
		 {
			m_View=90;   //火警页面
			m_FirstView=0;  //首次显示页面       
			m_deng[1] &=0xBF;	//清检查灯
		 }
	 }
}

//反馈声光
void FKSG()
{
     m_deng[0] |=0x40;	//置反馈灯
     if(m_Voice<2 || m_Voice==4 || m_Voice==5)
     {
        voice_out(2);
	    m_deng[0] &=0xEF;   //消音灯
     }
	 if(Total_Answer<999)
     	Total_Answer++;
     DisplayTotal(2);	//刷新反馈总数
     if(bTiaoshi)
	 {
		 if(m_View==0 || m_View==91 || m_View==92 || m_View==93)
		 {
			 m_View=90;   //火警页面
			 m_FirstView=0;  //首次显示页面
		 }
	 }
	 else
	 {
		 if(m_View !=90 && m_View !=61)
		 {
			m_View=90;   //火警页面
			m_FirstView=0;  //首次显示页面 
			m_deng[1] &=0xBF;	//清检查灯
			 
		 }
	 }
}

//故障声光
void GZSG()
{ 
    //故障声
    if(m_Voice==0)
    {
        voice_out(1);
	    m_deng[0] &=0xEF;   //消音灯
    }
    //故障灯
    m_deng[0] |=0x08;	
    //故障总数
	if(Total_Error<999)
    	Total_Error++;
	DisplayTotal(4);	//刷新总数
	gpio_out(3,1);		//故障继电器
	
    if(bTiaoshi)
	{
		 if(m_View==0 || m_View==92)
		 {
			 m_View=91;   //故障页面
			 m_FirstView=0;  //首次显示页面
		 }
	}
	else
	{
		if(m_View !=90 && m_View !=91 && m_View !=61  && m_View !=46)
		{
			m_View=91;   //故障页面
			m_FirstView=0;  //首次显示页面
			m_deng[1] &=0xBF;	//清检查灯

		}
	}
	
	//可燃气体故障数
	if(m_Record.sign==0x81 && m_Record.loop==3)
	{
		Total_KError++;
		m_deng[2] |=0x20;	
	}
}

//清除火警页面的显示区
void Qxsq(u8 type)
{
	u8 i;
	u16 y;
	
	if(type==0)
	{
		if(hjzonetype==1)
		{
			Fcolor=WHITE;
			Bcolor=RED;				
		}
		else if(hjzonetype==2)
		{
			Fcolor=BLUE;
			Bcolor=YELLOW;							
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;										
		}
		SetColor(Fcolor,Bcolor);
		for(i=0;i<3;i++)
		{
			y=i*75+53;
			DrawRect(0,y,387,y+73,2);	//清火警区域
		}
	}
	else if(type==1)
	{
		if(ldzonetype==1)
		{
			Fcolor=WHITE;
			Bcolor=RED;				
		}
		else
		{
			Fcolor=BLUE;
			Bcolor=YELLOW;							
		}
		SetColor(Fcolor,Bcolor);
		for(i=0;i<3;i++)
		{
			y=i*75+53;
			DrawRect(400,y,799,y+73,2);//清联动区域
		}		
	}
	else if(type==2)
	{
		if(krzonetype==1)
		{
			Fcolor=WHITE;
			Bcolor=RED;				
		}
		else
		{
			Fcolor=BLUE;
			Bcolor=YELLOW;							
		}
		SetColor(Fcolor,Bcolor);
		for(i=0;i<2;i++)
		{
			y=i*75+307;
			DrawRect(0,y,387,y+73,2);	//清可燃气体报警区域
		}				
	}	
	else if(type==3)
	{
		if(mhzonetype==1)
		{
			Fcolor=WHITE;
			Bcolor=RED;				
		}
		else
		{
			Fcolor=BLUE;
			Bcolor=YELLOW;							
		}
		SetColor(Fcolor,Bcolor);
		for(i=0;i<2;i++)
		{
			y=i*75+307;
			DrawRect(400,y,799,y+73,2);	//清灭火显示区域
		}				
	}	

}

//故障恢复
void Delete_Error() 
{
	uint16 i,j;
	i=0;
	j=0;
	while(i<pError)
	{
		if((ErrorList[i].sign==m_Record.sign) && 
		(ErrorList[i].panel==m_Record.panel) &&
        (ErrorList[i].loop==m_Record.loop) &&
        (ErrorList[i].addr==m_Record.addr) &&
        (ErrorList[i].type==m_Record.type) && 
        (ErrorList[i].type1==m_Record.type1)) 
		{	
			j=0x55;
			break;
		}
		i++;
	}
	if(j==0x55)
	{
		//找到恢复项,将后面的往前移
		while(i<pError)
		{
            ErrorList[i].sign=ErrorList[i+1].sign;
            ErrorList[i].panel=ErrorList[i+1].panel;
            ErrorList[i].loop=ErrorList[i+1].loop;
            ErrorList[i].addr=ErrorList[i+1].addr;
            ErrorList[i].type=ErrorList[i+1].type;
            ErrorList[i].type1=ErrorList[i+1].type1;

            ErrorList[i].year=ErrorList[i+1].year;
            ErrorList[i].month=ErrorList[i+1].month;
            ErrorList[i].day=ErrorList[i+1].day;
            ErrorList[i].hour=ErrorList[i+1].hour;
            ErrorList[i].minute=ErrorList[i+1].minute;
            ErrorList[i].second=ErrorList[i+1].second;
			ErrorList[i].confirm=ErrorList[i+1].confirm;
			i++;
		}
		if(pError>0)	pError--;
		//刷新显示
		if((m_View==90 || m_View==91 || m_View==33) && hjzonetype==2)
		{
			hjzonetype=0;
			Qxsq(0);	//清灭火显示区
		}
		if(Total_Error>0) Total_Error--;
		DisplayTotal(4);	//刷新故障总数
		if(Total_Error==0)
		{
			//刷新声光
			m_deng[0] &=0xF7;  //清故障灯
			if(m_Voice==0 && pFire==0 && pRun==0 && pKFire==0)
				m_deng[0] &=0xEF;   //清消音灯
			if(m_Voice==1)
			{
				voice_out(0);
			}
			gpio_out(3,0);		//故障继电器			
		}		
	}	

}

// void GetTongXinBan_count_index(uint16 loop,uint16 addr)
// {
// 	uint8 i,j;

// }




void WriteToErrorZone()
{
    if(pError<499)
    {
        ErrorList[pError].sign=m_Record.sign;
        ErrorList[pError].panel=m_Record.panel;
        ErrorList[pError].loop=m_Record.loop;
        ErrorList[pError].addr=m_Record.addr;
        ErrorList[pError].type=m_Record.type;
        ErrorList[pError].type1=m_Record.type1;

        ErrorList[pError].year=D_Year;
        ErrorList[pError].month=D_Month;
        ErrorList[pError].day=D_Day;
        ErrorList[pError].hour=D_Hour;
        ErrorList[pError].minute=D_Minute;
		ErrorList[pError].second=D_Second;
		ErrorList[pError].confirm=0;
        pError++;
 		m_ErrorAuto=0;	
   }    


}

//写入外部Can命令
void WriteToWCANZone() 
{
	cansendmsg.source=1;
	cansendmsg.add=m_PanelAddr;
	cansendmsg.type=0x40;
	cansendmsg.length=6;
	cansendmsg.rdata[0]=m_Record.sign;
	cansendmsg.rdata[1]=m_Record.loop;
	cansendmsg.rdata[2]=m_Record.addr;
	cansendmsg.rdata[3]=m_Record.type;
	cansendmsg.rdata[4]=m_Record.type1;
	cansendmsg.rdata[5]=m_Record.type2;
	xQueueSend(wcansend_queue,&cansendmsg,0);	
}

//板故障检测
void BoardCheck()
{
    uint8 i;
    for(i=1;i<33;i++)
    {
        if((m_Board[i] & 0x80)==0x80)
        {
            //已登记
            if((m_Board[i] & 0x20)==0x20)
            {
                //收到数据
                m_Board[i] &=0xDF;
                if((m_Board[i] & 0x40)==0x40)
                {
                    //当前有故障,判断恢复
                    m_Board[i]++;
                    if((m_Board[i] &0x1F)>2)
                    {
                        //恢复
                        m_Board[i] &= 0x80;
                        m_Record.panel=0;
                        m_Record.sign=0x8A;  //其它故障恢复
                        m_Record.loop=0x01;  //各板故障
                        m_Record.addr=i;   
                        m_Record.type=0;
						m_Record.type1=0;
			            //WriteToOtherZone(); //写入数据传送区
                        WriteHisRecord(0);//写入历史记录
			            m_Record.sign=0x89;  //其它故障
                        Delete_Error(); //故障恢复
                    }
                    
                }
                else
                {
                    //当前没有故障
                    m_Board[i] &=0x80;
                }
            }
            else
            {
                //未收到数据
                if((m_Board[i] & 0x40)==0x40)
                {
                    //当前有故障
                    m_Board[i] &=0xC0;
                }
                else
                {
                    //当前没有故障
                    m_Board[i]++;
                    if((m_Board[i] &0x1F)>5)	//9
                    {
                        //报故障
                        m_Board[i] &=0xC0;
                        m_Board[i] |=0x40;
                        GZSG(); //故障声光
                        //写故障区
                        m_Record.panel=0;
                        m_Record.sign=0x89;  //其它故障
                        m_Record.loop=0x01;  //各板故障
                        m_Record.addr=i;   
                        m_Record.type=0;
						m_Record.type1=0;
			            WriteToErrorZone();//写入故障区
			            WriteHisRecord(0);//写入历史记录
			            //WriteToOtherZone(); //写入数据传送区
                    }
                }
            }
        }
    }
	//判断黑匣子状态
	if((hxzstate & 0x80)==0x80)
	{
		//收到数据
		hxzstate &=0x7F;
		if((hxzstate & 0x40)==0x40)
		{
			//当前有故障,判断恢复
			hxzstate++;
			if((hxzstate &0x1F)>1)
			{
				//恢复
				hxzstate = 0x00;
				m_Record.panel=0;
				m_Record.sign=0x8A;  //其它故障恢复
				m_Record.loop=0x0A;  //运行数据存储单元故障
				m_Record.addr=0;   
				m_Record.type=0;
				WriteHisRecord(0);//写入历史记录
				WriteToWCANZone(); //写入外部CAN发送区
				m_Record.sign=0x89;  //其它故障
				Delete_Error(); //故障恢复
			}
			
		}
		else
		{
			//当前没有故障
			hxzstate =0x00;
		}
	}
	else
	{
		//未收到数据
		if((hxzstate & 0x40)==0x40)
		{
			//当前有故障
			hxzstate =0x40;
		}
		else
		{
			//当前没有故障
			hxzstate++;
			if((hxzstate &0x1F)>4)	
			{
				//报故障
				hxzstate =0x40;
				GZSG(); //故障声光
				//写故障区
				m_Record.panel=0;
				m_Record.sign=0x89;  //其它故障
				m_Record.loop=0x0A;  //运行数据存储单元故障
				m_Record.addr=0;   
				m_Record.type=0;
				WriteToErrorZone();//写入故障区
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);//写入历史记录
			}
		}
	}	
}


void WriteToHxzZone()
{

	if(m_Record.sign==0x80 && Total_Fire==1)
		 HxzList[phxz1][0]= 0xA1;	//首火警事件 0xA1
	else
		 HxzList[phxz1][0]= m_Record.sign;
	
	HxzList[phxz1][1]= m_Record.panel;
	HxzList[phxz1][2]= m_Record.loop;
	HxzList[phxz1][3]= m_Record.addr;
	HxzList[phxz1][4]= m_Record.type;
	if(m_Record.sign==0x04)
	{
		HxzList[phxz1][5]= m_Record.year;
		HxzList[phxz1][6]= m_Record.month;
		HxzList[phxz1][7]= m_Record.day;
		HxzList[phxz1][8]= m_Record.hour;		
		HxzList[phxz1][9]= m_Record.minute;		
		HxzList[phxz1][10]= m_Record.second;			
	}
	else
	{
		HxzList[phxz1][5]= D_Year;
		HxzList[phxz1][6]= D_Month;
		HxzList[phxz1][7]= D_Day;
		HxzList[phxz1][8]= D_Hour;		
		HxzList[phxz1][9]= D_Minute;		
		HxzList[phxz1][10]= D_Second;		
	}
	HxzList[phxz1][11]= m_Record.confirm;
	
	phxz1++;
	if(phxz1>255)
		phxz1=0;
	hxznum++;
	if(hxznum>255)
		hxznum=256;
	
}


void DealKey()
{
	if(keyvalue>0)
	{
		if(m_LcdLightStatus==0)
		{
			//当前背光关
			m_LCDback=0; //液晶背光计数
			LcdCtrl(1,0);  //开背光
			m_LcdLightStatus=1;	//液晶背光标志
			keyvalue=0;
		}
		else
		{
            m_LCDback=0; //液晶背光计数
			if(keyvalue==Key_Reset)
            {
                //复位键
	            keyvalue=0;
				if(m_View<5 || m_View==90 || m_View==91 || m_View==92 || m_View==93)
				{
		            m_View1=9;
		            m_View=81;
					m_FirstView=0;
				}
				else
					key=Key_Esc;
            }
			if(keyvalue==Key_Reset1)
            {
                //可燃气体复位键
	            keyvalue=0;
				if(m_View<5 || m_View==90 || m_View==91 || m_View==92 || m_View==93)
				{
		            m_View1=8;
		            m_View=81;
					m_FirstView=0;
				}
				else
					key=Key_Esc;
            }
			else if(keyvalue==Key_Silence)
			{
				//消音键
				keyvalue=0;
				if(m_Voice!=0)
				{
					m_Voice=0;
					voice_out(0);
					m_deng[0]|=0x10;	//置消音灯	
				}
				specialkey=Key_Silence;
			}
			else if(keyvalue==Key_VoiceOut || keyvalue==Key_SelfTest
				|| keyvalue==Key_RunAct || keyvalue==Key_JC)
			{
				specialkey=keyvalue;
				keyvalue=0;
			}
			else
			{
				key=keyvalue;
				keyvalue=0;
			}
		}
	}
	//判断特殊按键
	if(specialkey==0)	return;
	switch(specialkey)
	{
	case Key_SelfTest:
		if(m_View<5 || m_View==90 || m_View==91 || m_View==92 || m_View==93)
		{
			m_View1=61;		//自检界面
			m_View=81;
			m_FirstView=0;	
			specialkey=0;
		}
		else
		{
			key=Key_Esc;	//退出当前页	
		}
		break;
	case Key_VoiceOut:
		if(m_View<5 || m_View==90 || m_View==91 || m_View==92 || m_View==93)
		{
			specialkey=0;
			m_View1=7;	  	//启停声光页面
			m_View=81;
			m_FirstView=0;

		}
		else
		{
			key=Key_Esc;	//退出当前页	
		}
		break;
	case Key_JC:
		if(m_View<5 || m_View==90 || m_View==91 || m_View==92 || m_View==93)
		{
			specialkey=0;	
			m_View=102;
			m_FirstView=0;	
			
			m_Record.sign=0xA0;		//特殊事件
			m_Record.panel=m_PanelAddr;
			m_Record.confirm=129;	//检查功能按钮动作
			WriteToHxzZone();	//写黑匣子发送区				
		}
		else if(m_View==102)
		{
			key=Key_Esc;	//退出当前页
			specialkey=0;			
		}
		else
			key=Key_Esc;	//退出当前页
		break;
	case Key_RunAct:	//联动启动
		specialkey=0;
		//下传联动启动命令
		m_CanCommand.sign=0x01;  
		m_CanCommand.loop=0x52;
		m_CanCommand.addr=0;
		m_CanCommand.type=0;
		WriteCanCommand();
	
		m_Record.sign=0xA0;		//特殊事件
		m_Record.panel=m_PanelAddr;
		m_Record.confirm=130;	//联动启动按钮动作
		WriteToHxzZone();	//写黑匣子发送区		
		break;
	case Key_Silence:	//消音按键
		specialkey=0;		
		if(m_PanelType)
		{
			//主机向从机发消音命令
			m_Record.sign=0x01;  
			m_Record.loop=0x0C;	 //消音命令
			m_Record.addr=0;
			m_Record.type=0;
			m_Record.addr=0;   
			WriteToWCANZone(); //写入外部CAN发送区
		}
		break;	
	}
}


uint8 GBtoAsc(uint8* addr)
{
	uint8 char1,num;
	num=0;
	while(1)
	{
		char1=*addr; addr++;
		if(char1==0x00)
			break;
		else
		{
			HistoryBuf[num]=char1;
			num++;
		}	
	}
	return num;
}


uint8 ReadOneNote(uint8 panel,uint8 loop,uint8 addr)
{
	
	uint8 loop1,addr1,r;
	uint32 locate;
	NoteBuf[32]=0;
	addr1=addr;
	r=0;
	if(panel==0)
	{
		if(loop<17)
		{
			r=1;
			locate=E_NoteBegin+((loop-1)<<14)+(addr1<<6);
		}
	}
	else
	{
		if(panel<9)
		{
			if(loop<17)
			{
				r=1;
				loop1=loop;
				locate=E_NoteBegin+0x40000*panel+0x4000*(loop1-1)+(addr1<<6);
			}
		}
	}
	if(r)
		SPI_FLASH_HighSpeedReadCont(locate,32,NoteBuf);
	return r;
}

//显示一行注释
void OutOneNote(uint16 x,uint16 y,uint8 panel,uint8 loop,uint8 addr)
{
	uint8 r;
	r=ReadOneNote(panel,loop,addr);
	if(r)
		TextOut(x,y,NoteBuf,0);	

}


//打印数据
void m_Print()
{
    
    uint8 i,num,loc,bError;
	INT32U pAddr;
	uint8 buf[16];
	uint8 result=0;
	uint8 type=0;
	if(m_PrintSet==0) 
	{
		return; //禁止打印,退出	
	}
	if(m_PrintInit==0)
	{
		m_PrintInit=1;
		
		SendFrame4[0]=0x1B;	//设置行间距
		SendFrame4[1]=0x33;
		SendFrame4[2]=0x22;
		//SendFrame4[3]=0x1C;	//进入汉字打印方式
		//SendFrame4[4]=0x26;
		TotalSendCount4=3;
		//uart4Send();	//好像无效，去掉
		return;			
	}
	if(PrintHisNum>0)
	{
		//有历史记录需要打印
		pAddr=PrintHisAddr1;
		PrintHisNum--;
		result=2;
		//修改指针
		if(PrintHisType==1)
		{
			if(PrintHisAddr1==E_FireBegin)
	               PrintHisAddr1=E_FireEnd;
	           else
	               PrintHisAddr1=PrintHisAddr1-16;
		}
		else if(PrintHisType==2)
		{
			if(PrintHisAddr1==E_RunBegin)
				PrintHisAddr1=E_RunEnd;
			else
				PrintHisAddr1=PrintHisAddr1-16;
		}
		if(PrintHisType==3)
		{
			if(PrintHisAddr1==E_KFireBegin)
	               PrintHisAddr1=E_KFireEnd;
	           else
	               PrintHisAddr1=PrintHisAddr1-16;
		}
		else
		{
			if(PrintHisAddr1==E_OtherBegin)
				PrintHisAddr1=E_OtherEnd;
			else
				PrintHisAddr1=PrintHisAddr1-16;
		} 

	}
	else if(pPrintFire!=pFireHistory)
	{
		//有火警需打印
		pAddr=pPrintFire;
		result=1;
		//修改指针
		pPrintFire +=16;
		if(pPrintFire>E_FireEnd)
			pPrintFire=E_FireBegin;	
	}
	else if(pPrintKFire!=pKFireHistory)
	{
		//有可燃气体报警需打印
		pAddr=pPrintKFire;
		result=1;
		//修改指针
		pPrintKFire +=16;
		if(pPrintKFire>E_KFireEnd)
			pPrintKFire=E_KFireBegin;	
	}
	else if(pPrintRun!=pRunHistory)
	{
		//有联动需打印
		pAddr=pPrintRun;
		result=1;
		//修改指针
		pPrintRun +=16;
		if(pPrintRun>E_RunEnd)
			pPrintRun=E_RunBegin;	
	}
	else if(pPrintOther!=pOtherHistory)
	{
		//有其它记录需打印
		pAddr=pPrintOther;
		result=1;
		//修改指针
		pPrintOther +=16;
		if(pPrintOther>E_OtherEnd)
			pPrintOther=E_OtherBegin;	
	}

	if(result)
	{
		//读一条记录
		SPI_FLASH_HighSpeedReadCont(pAddr,14,buf);
		if(result==2)
		{
			//打印历史记录
			if(buf[0]==0xFF)
			{
				PrintHisNum=0;
				return;	
			}
		}
		//打印时间
		SendFrame4[0]=(buf[5]/10)+0x30;
		SendFrame4[1]=(buf[5]%10)+0x30;
		SendFrame4[2]='/';
		SendFrame4[3]=(buf[6]/10)+0x30;
		SendFrame4[4]=(buf[6]%10)+0x30;
		SendFrame4[5]='/';
		SendFrame4[6]=(buf[7]/10)+0x30;
		SendFrame4[7]=(buf[7]%10)+0x30;
		SendFrame4[8]=' ';
		SendFrame4[9]=(buf[8]/10)+0x30;
		SendFrame4[10]=(buf[8]%10)+0x30;
		SendFrame4[11]=':';
		SendFrame4[12]=(buf[9]/10)+0x30;
		SendFrame4[13]=(buf[9]%10)+0x30;
		SendFrame4[14]=':';
		SendFrame4[15]=(buf[10]/10)+0x30;
		SendFrame4[16]=(buf[10]%10)+0x30;
		SendFrame4[17]=0x0d;	//回车
		bError=0;
		switch(buf[0])
		{
		case 0x80:
			if((buf[4] & 0x1F)==0x07)
				num=GBtoAsc("监管 ");
			else 
				num=GBtoAsc("火警 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=3;		//打印地址及类型及注释
			break;
		case 0x7A:
			num=GBtoAsc("低限报警 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=1;		//打印地址及类型及注释
			break;			
		case 0x7B:
			num=GBtoAsc("高限报警 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=1;		//打印地址及类型及注释
			break;			
		case 0x83:
			num=GBtoAsc("启动 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=1;		//打印地址及类型及注释
			break;
		case 0x84:
			num=GBtoAsc("停止 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=1;		//打印地址及类型及注释
			break;
		case 0x85:
			num=GBtoAsc("反馈 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=1;		//打印地址及类型及注释
			break;
		case 0x86:
			num=GBtoAsc("反馈撤销 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=1;		//打印地址及类型及注释
			break;
		case 0x8F:
			if(buf[4]==0x01)
				num=GBtoAsc("喷洒启动 ");
			else if(buf[4]==0x02)
				num=GBtoAsc("喷洒反馈 ");
			else if(buf[4]==0x03 || buf[4]==0x04 || buf[4]==0x0B )
				num=GBtoAsc("灭火启动 ");
			else if(buf[4]==0x05 || buf[4]==0x06 || buf[4]==0x07 )
				num=GBtoAsc("灭火停止 ");
			else if(buf[4]==0x08 )
				num=GBtoAsc("声光启动 ");
			else if(buf[4]==0x02 )
				num=GBtoAsc("声光停止 ");
			else if(buf[4]==0x09 )
				num=GBtoAsc("放气指示 ");
				
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=4;		//打印灭火地址
			break;
		case 0x01:
			num=GBtoAsc("开机");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=2;		
			break;
		case 0x02:
			num=GBtoAsc("自检");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=2;		
			break;
		case 0x03:
			num=GBtoAsc("复位");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=2;		
			break;
	    case 0x81:
		case 0x61:
			bError=1;
			num=GBtoAsc("故障 ");
			for(i=0;i<num;i++)
				SendFrame4[i+18]=HistoryBuf[i];
			loc=num+18;	
			type=3;		//打印地址及类型及注释
			break;
		case 0x87:
			loc=18;
			if(buf[1]!=0)
			{
				SendFrame4[loc]=buf[1]/10+0x30;
				loc++;	
				SendFrame4[loc]=buf[1]%10+0x30;
				loc++;	
				SendFrame4[loc]='-';
				loc++;	
			}
			SendFrame4[loc]=buf[2]/100+0x30;
			loc++;	
			SendFrame4[loc]=(buf[2]%100)/10+0x30;
			loc++;	
			SendFrame4[loc]=buf[2]%10+0x30;;
			loc++;
			num=GBtoAsc("回路故障");
			for(i=0;i<num;i++)
				SendFrame4[i+loc]=HistoryBuf[i];
			loc=loc+num;
			type=2;		
			break;
		case 0x89:
			loc=18;
			if(buf[1]!=0)
			{
				SendFrame4[loc]=buf[1]/10+0x30;	//机器
				loc++;	
				SendFrame4[loc]=buf[1]%10+0x30;
				loc++;	
				SendFrame4[loc]='-';
				loc++;						
			}
			if(buf[2]==1)
			{
				//板故障
				if(buf[3]<9)
				{
					SendFrame4[loc]=buf[3]/10+0x30;	//板号
					loc++;	
					SendFrame4[loc]=buf[3]%10+0x30;
					loc++;
					num=GBtoAsc("回路板故障");						
					for(i=0;i<num;i++)
						SendFrame4[i+loc]=HistoryBuf[i];
					loc=loc+num;
					type=2;		
				}
				else if(buf[3]<17)
				{
					SendFrame4[loc]=(buf[3]-16)/10+0x30;	//板号
					loc++;	
					SendFrame4[loc]=(buf[3]-16)%10+0x30;
					loc++;
					num=GBtoAsc("灭火盘故障");						
					for(i=0;i<num;i++)
						SendFrame4[i+loc]=HistoryBuf[i];
					loc=loc+num;
					type=2;							
				}
				else
				{
					SendFrame4[loc]=(buf[3]-16)/10+0x30;	//板号
					loc++;	
					SendFrame4[loc]=(buf[3]-16)%10+0x30;
					loc++;
					num=GBtoAsc("多线盘故障");						
					for(i=0;i<num;i++)
						SendFrame4[i+loc]=HistoryBuf[i];
					loc=loc+num;
					type=2;							
				}
			}
			else if(buf[2]==2)
			{
				num=GBtoAsc("电源通讯故障");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==3)
			{
				num=GBtoAsc("主电故障");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==4)
			{
				num=GBtoAsc("备电故障");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==5)
			{
				num=GBtoAsc("声光开路");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==6)
			{
				num=GBtoAsc("声光短路");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==7)
			{
				SendFrame4[loc]=buf[3]/10+0x30;	
				loc++;	
				SendFrame4[loc]=buf[3]%10+0x30;
				loc++;
				num=GBtoAsc("机器故障");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;
			}						
			else if(buf[2]==8)
			{
				num=GBtoAsc("欠压故障");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==9)
			{
				if(buf[3]==2)
					num=GBtoAsc("电源输出2故障");		
				else
					num=GBtoAsc("电源输出1故障");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==0x0A)
			{
				num=GBtoAsc("运行数据存储单元故障");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else if(buf[2]==0x0C)
			{
				num=GBtoAsc("电池低压");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				type=2;						
			}
			else
				type=0;
			break;
		default:
			type=0;	//不打印		
			break;			
		}
		if(type==4)
		{
			//打印其他机器的灭火信息
			if(buf[1]!=0)
			{
				SendFrame4[loc]=buf[1]/10+0x30;	//机器
				loc++;	
				SendFrame4[loc]=buf[1]%10+0x30;
				loc++;	
				SendFrame4[loc]='-';
				loc++;						
			}
			if(buf[2]==101)
			{
				num=GBtoAsc("灭火");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
				SendFrame4[loc]=buf[3]+0x30;			//地址
				loc++;	
				SendFrame4[loc]=199;	//区
				loc++;	
				SendFrame4[loc]=248;
				loc++;
			}
		}
		else if(type==3)
		{
			//打印地址、类型及注释
			if(buf[1]!=0)
			{
				SendFrame4[loc]=buf[1]/10+0x30;	//机器
				loc++;	
				SendFrame4[loc]=buf[1]%10+0x30;
				loc++;	
				SendFrame4[loc]='-';
				loc++;						
			}
			if(buf[2]==101)
			{
				num=GBtoAsc("灭火");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
			}
			else if(buf[2]==102)
			{
				num=GBtoAsc("多线");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
			}
			else
			{
				SendFrame4[loc]=buf[2]/100+0x30;		//回路
				loc++;	
				SendFrame4[loc]=(buf[2]%100)/10+0x30;
				loc++;	
				SendFrame4[loc]=buf[2]%10+0x30;;
				loc++;					
			}
			SendFrame4[loc]='-';
			loc++;
			SendFrame4[loc]=buf[3]/100+0x30;			//地址
			loc++;	
			SendFrame4[loc]=(buf[3]%100)/10+0x30;
			loc++;	
			SendFrame4[loc]=buf[3]%10+0x30;
			loc++;
			SendFrame4[loc]=' ';
			loc++;
			if(buf[2]<100)
			{
				//总线地址的火警或故障需打印类型
				switch(buf[4]& 0x1F)
				{
					case 01:	num=GBtoAsc("光电感烟");	break;
					case 02:	num=GBtoAsc("电子感温");	break;
					case 03:	num=GBtoAsc("烟温复合");	break;
					case 04:	num=GBtoAsc("手动按钮");	break;
					
					case 10:	num=GBtoAsc("消火栓钮");	break;
					case 11:	num=GBtoAsc("输入模块");	break;
					case 12:	num=GBtoAsc("输出模块");	break;
					case 13:	num=GBtoAsc("输入输出");	break;
					
					case 14:	num=GBtoAsc("火灾显示盘");	break;
					case 15:	num=GBtoAsc("声光警报");	break;
					
					case 20:	num=GBtoAsc("短路隔离器");	break;
					case 21:	num=GBtoAsc("紧急启停");	break;
					case 22:	num=GBtoAsc("手自动模块");	break;

					case 25:	num=GBtoAsc("CO探测器");	break;
					case 26:	num=GBtoAsc("H2探测器");	break;
					case 28:	num=GBtoAsc("H2探测器");	break;
					
					default:	num=GBtoAsc("未知类型");	break;
		
				}
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
			}
			else if(bError)
			{
				//判断是否需要打印多线的故障类型
				if(buf[0]==0x61)
				{
					switch(buf[5])
					{
						case 0x01:	num=GBtoAsc("启动线路开路");	break;
						case 0x02:	num=GBtoAsc("启动线路短路");	break;
						case 0x03:	num=GBtoAsc("反馈线路开路");	break;
						default:	num=0;		break;
					}
					if(num>0)
					{
						for(i=0;i<num;i++)
							SendFrame4[i+loc]=HistoryBuf[i];
						loc=loc+num;
					}				
				}
			}
			SendFrame4[loc]=0x0d;	//回车
			loc++;
			//读一条注释
			if(ReadOneNote(buf[1],buf[2],buf[3]))
			{
				i=0;
				while(i<32 && NoteBuf[i]!=0xFF)
				{
					SendFrame4[loc]=NoteBuf[i];
					loc++;
					i++;	
				}
			}
			SendFrame4[loc]=' ';	//空格
			loc++;
			SendFrame4[loc]=0x0a;	//打印并换行
			TotalSendCount4=loc+1;
			uart4Send();			
		}
		else if(type==1)
		{
			if(buf[1]!=0)
			{
				SendFrame4[loc]=buf[1]/10+0x30;	//机器
				loc++;	
				SendFrame4[loc]=buf[1]%10+0x30;
				loc++;	
				SendFrame4[loc]='-';
				loc++;						
			}
			if(buf[2]==101)
			{
				num=GBtoAsc("灭火");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
			}
			else if(buf[2]==102)
			{
				num=GBtoAsc("多线");						
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;
			}
			else
			{
				SendFrame4[loc]=buf[2]/100+0x30;		//回路
				loc++;	
				SendFrame4[loc]=(buf[2]%100)/10+0x30;
				loc++;	
				SendFrame4[loc]=buf[2]%10+0x30;;
				loc++;					
			}
			SendFrame4[loc]='-';
			loc++;						
			SendFrame4[loc]=buf[3]/100+0x30;			//地址
			loc++;	
			SendFrame4[loc]=(buf[3]%100)/10+0x30;
			loc++;	
			SendFrame4[loc]=buf[3]%10+0x30;
			loc++;
			if(buf[2]<100)
			{
				SendFrame4[loc]=' ';
				loc++;						
				//总线联动打印类型
				switch(buf[4]& 0x1F)
				{
					case 01:	num=GBtoAsc("光电感烟");	break;
					case 02:	num=GBtoAsc("电子感温");	break;
					case 03:	num=GBtoAsc("烟温复合");	break;
					case 04:	num=GBtoAsc("手动按钮");	break;
					
					case 10:	num=GBtoAsc("消火栓钮");	break;
					case 11:	num=GBtoAsc("输入模块");	break;
					case 12:	num=GBtoAsc("输出模块");	break;
					case 13:	num=GBtoAsc("输入输出");	break;
					
					case 14:	num=GBtoAsc("火灾显示盘");	break;
					case 15:	num=GBtoAsc("声光警报");	break;
					
					case 20:	num=GBtoAsc("短路隔离器");	break;
					case 21:	num=GBtoAsc("紧急启停");	break;					
					case 22:	num=GBtoAsc("手自动模块");	break;

					case 25:	num=GBtoAsc("CO探测器");	break;
					case 26:	num=GBtoAsc("H2探测器");	break;
					case 28:	num=GBtoAsc("H2探测器");	break;

					default:	num=GBtoAsc("未知类型");	break;
		
				}
				for(i=0;i<num;i++)
					SendFrame4[i+loc]=HistoryBuf[i];
				loc=loc+num;

			}
			SendFrame4[loc]=0x0d;	//回车
			loc++;
			//读一条注释
			if(ReadOneNote(buf[1],buf[2],buf[3]))
			{
				i=0;
				while(i<32 && NoteBuf[i]!=0xFF)
				{
					SendFrame4[loc]=NoteBuf[i];
					loc++;
					i++;	
				}
			}
			SendFrame4[loc]=' ';	//空格
			loc++;
			SendFrame4[loc]=0x0a;	//打印并换行
			TotalSendCount4=loc+1;
			uart4Send();										
		}
		else if(type==2)
		{
			SendFrame4[loc]=0x0d;	//回车
			loc++;
			SendFrame4[loc]=' ';	//空格
			loc++;
			SendFrame4[loc]=0x0a;	//打印并换行
			TotalSendCount4=loc+1;
			uart4Send();			
		}
 	}

}

//机器故障检测
void MachineCheck()
{
    uint8 i;
	if(m_PanelType==0)	return;
    for(i=1;i<PANEL_NET_MAX;i++)
    {
        if((m_Panel[i] & 0x80)==0x80)
        {
            //已登记
            if((m_Panel[i] & 0x20)==0x20)
            {
                //收到数据
                m_Panel[i] &=0xDF;
                if((m_Panel[i] & 0x40)==0x40)
                {
                    //当前有故障,判断恢复
                    m_Panel[i]++;
                    if((m_Panel[i] &0x1F)>1)
                    {
                        //恢复
                        m_Panel[i] &= 0x80;
                        m_Record.panel=0;
                        m_Record.sign=0x8A;  //其它故障恢复
                        m_Record.loop=0x07;  //机器故障
                        m_Record.addr=i;   
                        m_Record.type=0;
						m_Record.type1=0;
			            //WriteToOtherZone(); //写入数据传送区
                        WriteHisRecord(0);//写入历史记录
			            m_Record.sign=0x89;  //其它故障
                        Delete_Error(); //故障恢复
                    }
                    
                }
                else
                {
                    //当前没有故障
                    m_Panel[i] &=0x80;
                }
            }
            else
            {
                //未收到数据
                if((m_Panel[i] & 0x40)==0x40)
                {
                    //当前有故障
                    m_Panel[i] &=0xC0;
                }
                else
                {
                    //当前没有故障
					if(i!=m_PanelAddr)
					{
						m_Panel[i]++;
						if((m_Panel[i] &0x1F)>5)
						{
							//报故障
							m_Panel[i] &=0xC0;
							m_Panel[i] |=0x40;
							GZSG(); //故障声光
							//写故障区
							m_Record.panel=0;
							m_Record.sign=0x89;  //其它故障
							m_Record.loop=0x07;  //机器故障
							m_Record.addr=i;   
							m_Record.type=0;
							m_Record.type1=0;
							WriteToErrorZone();//写入故障区
							WriteHisRecord(0);//写入历史记录
							//WriteToOtherZone(); //写入数据传送区
						}
					}
                }
            }
        }
    }
}

void CommToPower()
{
	uint8 bReceive;
	uint8 i,num,sum;
	uint16 status;
	//先判断通讯故障及恢复
	bReceive=0;
	if(m_LastPowerCommand!=0)
	{
		
		if(ReceiveFrame5[0]==0x5A && ReceiveFrame5[1]==0x33)
		{
			num=ReceiveFrame5[2];
			sum=0;
			for(i=0;i<num+3;i++)
				sum+=ReceiveFrame5[i];
			if(sum==ReceiveFrame5[i])
			{
				bReceive=1;
				for(i=0;i<num+4;i++)
					PowerData[i]=ReceiveFrame5[i];
				bRevPowerData=1;
				
				if(m_PowerComStatus>0x7F)
				{
					m_PowerComStatus++;
					if(m_PowerComStatus>0x82)
					{
						m_PowerComStatus=0;
						//写入历史记录
						m_Record.panel=0;
						m_Record.sign=0x8A;  //其它故障恢复
						m_Record.loop=0x02;  //电源通讯故障
						m_Record.addr=0;   
						m_Record.type=0;
						m_Record.type1=0;
						WriteToWCANZone(); //写入外部CAN发送区
						WriteHisRecord(0);	//写入历史记录
						m_Record.sign=0x89;  //其它故障
						Delete_Error(); //故障恢复
					}	
				}
				else m_PowerComStatus=0;			
			}

		} 
		if(bReceive==0)
		{
			//未接收到正确数据
			if(m_PowerComStatus<0x80 )
			{
				m_PowerComStatus++;
				if(m_PowerComStatus>0x19)
				{
					m_PowerComStatus=0x80;
					GZSG(); //故障声光
					//写故障区
					m_Record.panel=0;
					m_Record.sign=0x89;  //其它故障
					m_Record.loop=0x02;  //电源通讯故障
					m_Record.addr=0;   
					m_Record.type=0;
					m_Record.type1=0;
					//写入故障区
					WriteToErrorZone();
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(0);//写入历史记录
				}
			}
			else m_PowerComStatus=0x80;
			
		}
	}
	if(bReceive && ReceiveFrame5[1]==0x33)
	{
		//接收正确,判断电源状态
		status = ((uint16)ReceiveFrame5[3]<<8) | ReceiveFrame5[4];
		if(status==m_PowerRev)
		{
			//判断主电
			if((m_PowerRev & 0x04)==0x04 && (m_PowerStatus &0x04) ==0x00)
			{
				//主电故障
				GZSG(); //故障声光
				m_deng[2] |=0x80;
				m_deng[0] |=0x02;
				m_deng[0] &=0xFE; 	
				//写故障区
				m_Record.panel=0;
				m_Record.sign=0x89;  //其它故障
				m_Record.loop=0x03;  //主电故障
				m_Record.addr=0;   
				m_Record.type=0;
				m_Record.type1=0;
				//写入故障区
				WriteToErrorZone();
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);//写入历史记录
			}	
			if((m_PowerRev & 0x04)==0x00 && (m_PowerStatus &0x04) ==0x04)
			{
				//主电恢复
				m_deng[2] &=0x7F;
				m_deng[0] &=0xFD;
				m_deng[0] |=0x01;
				//写入历史记录
				m_Record.panel=0;
				m_Record.sign=0x8A;  //其它故障恢复
				m_Record.loop=0x03;  //主电故障
				m_Record.addr=0;   
				m_Record.type=0;
				m_Record.type1=0;
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);	//写入历史记录
				m_Record.sign=0x89;  //其它故障
				Delete_Error(); //故障恢复
//					powLowtimes=0;	//欠压故障计数
//					if(bPowerLow)
//					{
//						m_Record.panel=0;
//						m_Record.sign=0x8A;  //其它故障恢复
//						m_Record.loop=0x08;  //欠压
//						m_Record.addr=0;   
//						m_Record.type=0;
//						WriteToWCANZone(); //写入外部CAN发送区
//						WriteHisRecord(0);	//写入历史记录
//						m_Record.sign=0x89;  //其它故障
//						Delete_Error(); //故障恢复
//						bPowerLow=0;	//欠压故障标志
//					}					
			}	
			//判断备电
			if((m_PowerRev & 0x08)==0x08 && (m_PowerStatus &0x08) ==0x00)
			{
				//备电故障
				GZSG(); //故障声光
				m_deng[1] |=0x80;
				//写故障区
				m_Record.panel=0;
				m_Record.sign=0x89;  //其它故障
				m_Record.loop=0x04;  //备电故障
				m_Record.addr=0;   
				m_Record.type=0;
				m_Record.type1=0;
				//写入故障区
				WriteToErrorZone();
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);//写入历史记录
			}	
			if((m_PowerRev & 0x08)==0x00 && (m_PowerStatus &0x08) ==0x08)
			{
				//备电恢复
				m_deng[1] &=0x7F;
				m_Record.panel=0;
				m_Record.sign=0x8A;  //其它故障恢复
				m_Record.loop=0x04;  //备电故障
				m_Record.addr=0;   
				m_Record.type=0;
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);	//写入历史记录
				m_Record.sign=0x89;  //其它故障
				Delete_Error(); //故障恢复
			}
			//判断电源输出
			if((m_PowerRev & 0x70)>0 && (m_PowerStatus & 0x70) ==0x00)
			{
				//电源输出故障
				GZSG(); //故障声光
				//写故障区
				m_Record.panel=0;
				m_Record.sign=0x89;  //其它故障
				m_Record.loop=0x09;  //电源输出故障
				m_Record.addr=0;   
				m_Record.type=0;
				m_Record.type1=0;
				//写入故障区
				WriteToErrorZone();
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);//写入历史记录
			}	
			if((m_PowerRev & 0x70)==0x00 && (m_PowerStatus &0x70)>0 )
			{
				//电源输出恢复
				m_Record.panel=0;
				m_Record.sign=0x8A;  //其它故障恢复
				m_Record.loop=0x09;  //电源输出
				m_Record.addr=0;   
				m_Record.type=0;
				m_Record.type1=0;
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);	//写入历史记录
				m_Record.sign=0x89;  //其它故障
				Delete_Error(); //故障恢复
			}	
			//判断欠压
			if((m_PowerRev & 0x80)==0x80 && (m_PowerStatus &0x80) ==0x00)
			{
				//欠压故障
				GZSG(); //故障声光
				//写故障区
				m_Record.panel=0;
				m_Record.sign=0x89;  //其它故障
				m_Record.loop=0x08;  //欠压
				m_Record.addr=0;   
				m_Record.type=0;
				m_Record.type1=0;
				//写入故障区
				WriteToErrorZone();
				WriteToWCANZone(); //写入外部CAN发送区
				WriteHisRecord(0);//写入历史记录
				bPowerLow=1;	//欠压故障标志
				//发送欠压故障命令
				//m_CanCommand.sign=0x01;  
				//m_CanCommand.loop=0x05;
				//m_CanCommand.addr=0;
				//m_CanCommand.type=0;
				//WriteCanCommand();
			}	
			if((m_PowerRev & 0x80)==0x00 && (m_PowerStatus &0x80) ==0x80)
			{
				//欠压恢复
				m_Record.panel=0;
				m_Record.sign=0x8A;  //其它故障恢复
				m_Record.loop=0x08;  //欠压
				m_Record.addr=0;   
				m_Record.type=0;
				m_Record.type1=0;
				WriteToWCANZone();  //写入外部CAN发送区
				WriteHisRecord(0);	//写入历史记录
				m_Record.sign=0x89;  //其它故障
				Delete_Error(); //故障恢复
				bPowerLow=0;	//欠压故障标志
				//发送欠压故障恢复命令
				//m_CanCommand.sign=0x01;  
				//m_CanCommand.loop=0x06;
				//m_CanCommand.addr=0;
				//m_CanCommand.type=0;
				//WriteCanCommand();
			}
			m_PowerStatus=m_PowerRev;	//刷新状态

		}
		else
		{
			m_PowerRev=status;	
		}
	}
	//发送下次巡检
	m_LastPowerCommand=m_PowerCommand;
	SendFrame5[0]=0xA5;
	SendFrame5[1]=m_LastPowerCommand;
    SendFrame5[2]=m_LastPowerCommand;
    SendFrame5[3]=PowerLowValue;
	num=0;
	for(i=0;i<4;i++)
		num+=SendFrame5[i];	
    SendFrame5[4]=num;
	TotalSendCount5=5;
    uart5Send();
	//重新开始接收
	ReceiveCount5=0;  //接收字节数	
}

void DealZdsd()
{
	//判断自动手动锁，锁带自复位,使用1个锁控制
	if(LOCK1_SET())
	{
		//当前未按下
		if((m_suo & 0x80)==0x80)
		{
			m_suo++;
			if(m_suo>0x89)
			{
				m_suo=0x00;	
			}	
		}
		else
			m_suo=0;		
	}
	else
	{
		//当前按下
		if((m_suo & 0x80)==0x00)
		{
			m_suo++;
			if(m_suo>20)
			{
				m_suo=0x80;	//按下
				if(m_ZdSd & 0x80)
				{
					m_ZdSd=0x40;
					m_Record.sign=0x05; //手自动切换
					m_Record.panel=m_ZdSd;
					m_Record.loop=0;  
					m_Record.addr=0;   
					m_Record.type=0;
					WriteHisRecord(0); //写入其它记录
					
					m_Record.sign=0xA0;		//特殊事件
					m_Record.panel=m_PanelAddr;
					m_Record.confirm=125;	//手动状态
					WriteToHxzZone();	//写黑匣子发送区					
					m_deng[1] |=0x04;
					m_deng[1] &=0xF7;
					
					//保存自动手动状态，写入SPI FALSH
					SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
					Flash_Sector_Erase(1,1);   //清空
					HistoryBuf[17]=m_ZdSd;
					Auto_Add_Write(0x1000,HistoryBuf,32);	//写入	
				}
				else
				{
					m_ZdSd=0xC0;
					m_Record.sign=0x05; //手自动切换
					m_Record.panel=m_ZdSd;
					m_Record.loop=0;  
					m_Record.addr=0;   
					m_Record.type=0;
					WriteHisRecord(0); //写入其它记录
					
					m_Record.sign=0xA0;		//特殊事件
					m_Record.panel=m_PanelAddr;
					m_Record.confirm=126;	//自动状态
					WriteToHxzZone();	//写黑匣子发送区	

					if(bldqq)	//有联动请求标志
					{
						bldqq=0;
						//下传联动启动命令
						m_CanCommand.sign=0x01;  
						m_CanCommand.loop=0x52;
						m_CanCommand.addr=0;
						m_CanCommand.type=0;
						WriteCanCommand();					
					}
					
					m_deng[1] |=0x08;
					m_deng[1] &=0xFB;
					
					//保存自动手动状态，写入SPI FALSH
					SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
					Flash_Sector_Erase(1,1);   //清空
					HistoryBuf[17]=m_ZdSd;
					Auto_Add_Write(0x1000,HistoryBuf,32);	//写入							
				}					
			}	
		}
		else
			m_suo=0x80;	
	}
	
	
//	//判断自动手动锁，锁带自复位
//	if(m_ZdSd & 0x80)
//	{
//		//当前自动状态
//		zdsdtimes1=0;
//		//判断转手动
//		if(LOCK1_SET())
//		{
//			zdsdtimes2++;
//			if(zdsdtimes2>5)
//			{
//				zdsdtimes2=0;
//				m_ZdSd=0x40;
//				m_Record.sign=0x05; //手自动切换
//				m_Record.panel=m_ZdSd;
//				m_Record.loop=0;  
//				m_Record.addr=0;   
//				m_Record.type=0;
//				WriteHisRecord(0); //写入其它记录
//				
//				m_Record.sign=0xA0;		//特殊事件
//				m_Record.panel=m_PanelAddr;
//				m_Record.confirm=125;	//手动状态
//				WriteToHxzZone();	//写黑匣子发送区					
//				m_deng[1] |=0x04;
//				m_deng[1] &=0xF7;
//				
//				//保存自动手动状态，写入SPI FALSH
//				SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
//				Flash_Sector_Erase(1,1);   //清空
//				HistoryBuf[17]=m_ZdSd;
//				Auto_Add_Write(0x1000,HistoryBuf,32);	//写入				
//			}
//		}
//		else
//		{
//			zdsdtimes2=0;
//		}
//	}
//	else
//	{
//		//当前手动状态
//		zdsdtimes2=0;
//		//判断转自动
//		if(LOCKK_SET())
//		{
//			zdsdtimes1++;
//			if(zdsdtimes1>5)
//			{
//				zdsdtimes1=0;
//				m_ZdSd=0xC0;
//				m_Record.sign=0x05; //手自动切换
//				m_Record.panel=m_ZdSd;
//				m_Record.loop=0;  
//				m_Record.addr=0;   
//				m_Record.type=0;
//				WriteHisRecord(0); //写入其它记录
//				
//				m_Record.sign=0xA0;		//特殊事件
//				m_Record.panel=m_PanelAddr;
//				m_Record.confirm=125;	//手动状态
//				WriteToHxzZone();	//写黑匣子发送区	

//				if(bldqq)	//有联动请求标志
//				{
//					bldqq=0;
//					//下传联动启动命令
//					m_CanCommand.sign=0x01;  
//					m_CanCommand.loop=0x52;
//					m_CanCommand.addr=0;
//					m_CanCommand.type=0;
//					WriteCanCommand();					
//				}
//				
//				m_deng[1] |=0x08;
//				m_deng[1] &=0xFB;
//				
//				//保存自动手动状态，写入SPI FALSH
//				SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
//				Flash_Sector_Erase(1,1);   //清空
//				HistoryBuf[17]=m_ZdSd;
//				Auto_Add_Write(0x1000,HistoryBuf,32);	//写入				
//			}
//		}
//		else
//		{
//			zdsdtimes1=0;
//		}		
//	}
	
}

//自动启动或停止
void AutoRunOrStop()
{
    uint16 i,qdtime,tztime; 
	INT8U sign,panel,loop,addr,ss;
    //if((m_ZdSd & 0x80)==0x00 && bRunAuto==0x00) return; //自动不允许
    if(pQdZone==0)  return;
    for(i=0;i<pQdZone;i++)
    {
		sign=QdZone[i][0];
		panel=QdZone[i][1];
		loop=QdZone[i][2];
		addr=QdZone[i][3];
		qdtime=QdZone[i][4];
		tztime=QdZone[i][5];
        if(sign==0)
        {
			if(qdtime==0)
            {
                if(m_ZdSd & 0x80)
				{
					//自动允许
					if(panel==0)
						ss=0x02;
					else
						ss=0x32;	//向区域机下传联动启动
				}
				else
				{
					//自动禁止，下传请求
					if(panel==0)
						ss=0x42;
					else
						ss=0x34;	//向区域机下传联动请求
					bldqq=1;		//有联动请求标志
				}
				if(panel==0)
				{
					sign=0x80; 
					//自动启动
					m_CanCommand.sign=ss;  
					m_CanCommand.loop=loop;
					m_CanCommand.addr=addr;
					m_CanCommand.type=0;
					WriteCanCommand();
					if(tztime==0)
						sign=0xC0;  //不用停止 
					QdZone[i][0]=sign;
				}
				else
				{
					sign=0x80; 
					//自动启动
					m_Record.sign=ss;
					m_Record.loop=loop;
					m_Record.addr=addr;
					m_Record.type=panel;
					WriteToWCANZone();
					if(tztime==0)
						sign=0xC0;  //不用停止 
					QdZone[i][0]=sign;
				}
            }
            else
            {
                qdtime--;
				QdZone[i][4]=qdtime;
            }
                
        }
        else if(sign==0x80)
        {
            if(tztime==0)
            {
				if(m_ZdSd & 0x80)
				{
					//自动允许
					if(panel==0)
						ss=0x03;
					else
						ss=0x33;	//向区域机下传联停止
				}
				else
				{
					//自动禁止，直接下传停止，不下传停止请求
					if(panel==0)
						ss=0x03;
					else
						ss=0x33;	
				}
				if(panel==0)
				{
					sign=0xC0; 
					//自动停止
					m_CanCommand.sign=ss;  
					m_CanCommand.loop=loop;
					m_CanCommand.addr=addr;
					m_CanCommand.type=0;
					WriteCanCommand();
					QdZone[i][0]=sign;
				}
				else
				{
					sign=0xC0; 
					//自动停止
					m_Record.sign=ss;
					m_Record.loop=loop;
					m_Record.addr=addr;
					m_Record.type=panel;
					WriteToWCANZone();
					QdZone[i][0]=sign;				
				}
					
            }
            else
            {
                tztime--;
				QdZone[i][5]=tztime;
            }
        }
    }
} 

uint16 GetKeRanQiUploadConcentration(uint8_t addr);
void UpdateKrFireConcentration(uint8_t addr, uint8_t type, uint16 concentration);
void FillKrFireConcentration(void);
/* 解析完整的可燃气CAN文件并更新登记状态、报警/故障位和浓度缓存。file由调用方传入，允许后台使用独立缓冲区，不必占用界面的CanFile。 */
/* file是48字节独立CAN文件缓存；本函数只解析并更新缓存，不在这里绘图，后台和界面可以共用解析结果。 */
void SyncKeRanQiUploadFromCanFile(const uint8 *file);
void TriggerTongXinBanRegisterQuery(void);

void RefreshLel()
{
	uint16 ll,lel;
	uint8 r;
	if(pKFire>0)
	{
		if(krndsend[0]==0x80)
		{
			krndsend[0]=0;
			r=0;
			if(Fcolor !=WHITE || Bcolor !=BLUE)
			{
				SetColor(WHITE,BLUE);
				r=1;
			}
			ll=200;
			if(krndsend[2]==25)
				TextOut(ll,456,"CO:3-",0);
			else
				TextOut(ll,456,"H2:3-",0);
			
			ll+=60;
			OutOneChar1(ll,456,krndsend[1]+0x30,0);
			ll+=24;
			lel =  (uint16)krndsend[3] <<8;
			lel |=krndsend[4];
			lel &=0x3FFF;
			SetKeRanQiUploadConcentration(krndsend[1], lel);
			UpdateKrFireConcentration(krndsend[1], krndsend[2], lel);
			SetKeRanQiUploadBit(krndsend[1], KERANQI_UPLOAD_REGISTER, 1);
			if(krndsend[2]==25)
			{
				if(lel>999) lel=999;
				OutThreeChar1(ll,456,lel,0);
				TextOut(ll+36,456,"ppm ",0);
			}
			else
			{
				if(lel>199) lel=199;
				OutThreeChar1(ll,456,lel,0);
				TextOut(ll+36,456,"%LEL",0);
			}
			
			if(r) 	SetColor(Fcolor,Bcolor); 	
		}
	}
}

void Refresh()
{
	uint8 i;
	if(rtc_read())
	{
		//秒定时到
		if(m_View !=61)
			ClockOut(572,456);	//时钟输出

		//备份时钟,用于读关机记录
		RTC_BKP2=RTC_DATE;
		RTC_BKP3=RTC_TIME;
				
		if(isRegister==0x80)
			isRegister=0x81;
		
		if(m_SelfTestTimes>0) m_SelfTestTimes--;
		if(BrocastTimes<255) BrocastTimes++;	//广播时间计数
		if(WBrocastTimes<255) WBrocastTimes++;	//外部广播时间计数	
		AutoRunOrStop();  //自动启动或停止
		
		if(m_LcdLightStatus==0)
		{
			if(pFire>0 || pError>0 || pRun>0)
		    {
			    m_LCDback=0; //液晶背光计数
				LcdCtrl(1,0);  //开背光
				m_LcdLightStatus=1;	//液晶背光标志

			}
		}
        //判断是否需要自动退出
		if(bTiaoshi==0)
		{
			if((m_View!=90) &&(m_View!=91) && (m_View!=92) && (m_View!=0) && (m_View!=21)) 	//编程页面/自动登记/探测器曲线也不退出
			{
				m_AutoEscTime++; //自动退出时间
			}
			else
				m_AutoEscTime=0;
		}
        ////////////////////////////////////////////////
        //判断自动翻页
        if(m_View==90)
        {
            if(bConfirm==0)
			{
				if(pFire>3 || (pFire==0 && pError>3)) m_FireAuto++;
				if(pRun>3 || (pRun==0 && pldError>3))  m_RunAuto++;
				if(pKFire>2 || (pKFire==0 && pkrError>2)) m_KrFireAuto++;
				if(pmhRun>2 || (pmhRun==0 && pmhError>2)) m_mhRunAuto++;
			}
        }
        if(m_View==91 ||m_View==33)
        {
            if(bConfirm==0)
			{
				if(pError>3) m_FireAuto++;
				if(pldError>3)  m_RunAuto++;
				if(pkrError>2) m_KrFireAuto++;
				if(pmhError>2) m_mhRunAuto++;
			}            
        }
        if(m_View==92 ||m_View==32)
        {
            if(Total_Isolate>8) m_DisableAuto++;
        }
		if(m_View==66 || krq_SnedDelayFlag == 1)
			TestKrKzqTimeDelay++;	

		if(bTestQx)
			TestQxTimes++;
		
		if((D_Second %2)==1)
		{
			//2秒处理一次电源通讯
			CommToPower();
		}

		if((D_Second % 10)==1)
        {
            //10秒标志
            BoardCheck();   //板故障检测
	        //液晶背光判断
	        m_LCDback++; //液晶背光计数
			if(m_LcdLightStatus)
			{
			    if(m_LCDback>17 && pFire==0 && pError==0 && pRun==0)
				{
					LcdCtrl(0,0);  //关背光
					m_LcdLightStatus=0;	//液晶背光标志
					m_LCDback=20;
				}
			}
        }
        if((D_Second % 15)==2)
        {
            MachineCheck();   //机器故障检测
		}
		m_Print();
		RefreshLel();

	}
	DealKey();
	DealLed();
	DealZdsd();
	
	if(m_AutoEscTime>59)
    {
		key=Key_Esc;	//退出当前页
    }

	
	if(bPowerLow)
	{
		//有欠压，则一直响故障声
        voice_out(1);
	    m_deng[0] &=0xEF;   //消音灯
		m_deng[0] &=0xFD;	//清备电运行灯
		m_deng[1] |=0x80;	//置备电故障灯
	}
}


void WriteToFireZone()
{
    if(pFire<255)
    {
        FireList[pFire].sign=m_Record.sign;
        FireList[pFire].panel=m_Record.panel;
        FireList[pFire].loop=m_Record.loop;
        FireList[pFire].addr=m_Record.addr;
        FireList[pFire].type=m_Record.type;
        FireList[pFire].type1=m_Record.type1;

        FireList[pFire].year=D_Year;
        FireList[pFire].month=D_Month;
        FireList[pFire].day=D_Day;
        FireList[pFire].hour=D_Hour;
        FireList[pFire].minute=D_Minute;
		FireList[pFire].second=D_Second;
		FireList[pFire].confirm=0;
        pFire++;
 		m_FireAuto=0;	
   }    
}

uint16 GetKeRanQiUploadConcentration(uint8_t addr)
{
	if(addr == 0 || addr > KERANQI_UPLOAD_CONC_COUNT)
		return 0;
	return KeRanQiNongDu_value[addr-1] & 0x3FFF;
}

void UpdateKrFireConcentration(uint8_t addr, uint8_t type, uint16 concentration)
{
	uint8 i;
	if(addr == 0 || addr > KERANQI_UPLOAD_CONC_COUNT)
		return;
	concentration &= 0x3FFF;
	for(i=0;i<pKFire;i++)
	{
		if((KFireList[i].loop==3) &&
		   (KFireList[i].addr==addr) &&
		   (KFireList[i].type==type) &&
		   ((KFireList[i].sign==0x7A) || (KFireList[i].sign==0x7B)))
		{
			KFireList[i].type1=(uint8)(concentration>>8);
			KFireList[i].type2=(uint8)concentration;
		}
	}
}

void FillKrFireConcentration(void)
{
	uint16 concentration;
	if(m_Record.loop!=3 || m_Record.addr==0 || m_Record.addr>KERANQI_UPLOAD_CONC_COUNT)
		return;
	concentration=((uint16)m_Record.type1<<8) | m_Record.type2;
	concentration &= 0x3FFF;
	if(concentration==0)
	{
		concentration=GetKeRanQiUploadConcentration(m_Record.addr);
		if(concentration>0)
		{
			m_Record.type1=(uint8)(concentration>>8);
			m_Record.type2=(uint8)concentration;
		}
	}
}

uint8 NormalizeKrDeviceType(uint8 type)
{
	type &= 0x1F;
	if(type==28 || type==29)
		type=26;
	return type;
}
void WriteToKrFireZone()
{
    uint8 i;
    uint8 tt=0;
	FillKrFireConcentration();
	if(m_Record.loop==3 && m_Record.addr>0 && m_Record.addr<=KERANQI_UPLOAD_CONC_COUNT)
	{
		SetKeRanQiUploadBit(m_Record.addr, KERANQI_UPLOAD_REGISTER, 1);
		SetKeRanQiUploadBit(m_Record.addr, KERANQI_UPLOAD_ALARM, 1);
		if(m_Record.sign==0x7A)
			SetKeRanQiUploadBit(m_Record.addr, KERANQI_UPLOAD_LOW_ALARM, 1);
		else if(m_Record.sign==0x7B)
			SetKeRanQiUploadBit(m_Record.addr, KERANQI_UPLOAD_HIGH_ALARM, 1);
	}
	//首先查找联动区是否有本条
	if(pKFire>0)
	{
		for(i=0;i<pKFire;i++)
		{
			if(KFireList[i].sign==m_Record.sign &&
			   KFireList[i].panel==m_Record.panel &&
			   KFireList[i].loop==m_Record.loop &&
			   KFireList[i].addr==m_Record.addr)
			{
				//找到
				tt=1;
				switch(KFireList[i].sign)
				{
					case 0x7A:
					{
						if(Total_Fire1>0)
							Total_Fire1--;
						DisplayTotal(6);
					}
					break;
					case 0x7B:
					{
						if(Total_Fire2>0)
							Total_Fire2--;
						DisplayTotal(7);
					}
					break;
				}
				break;
			}
		 }    
	}
	if(tt==0 && pKFire<63)
    {
        //未找到,则写入联动区
		KFireList[pKFire].sign=m_Record.sign;
        KFireList[pKFire].panel=m_Record.panel;
        KFireList[pKFire].loop=m_Record.loop;
        KFireList[pKFire].addr=m_Record.addr;
        KFireList[pKFire].type=m_Record.type;
        KFireList[pKFire].type1=m_Record.type1;
        KFireList[pKFire].type2=m_Record.type2;

        KFireList[pKFire].year=D_Year;
        KFireList[pKFire].month=D_Month;
        KFireList[pKFire].day=D_Day;
        KFireList[pKFire].hour=D_Hour;
        KFireList[pKFire].minute=D_Minute;
        KFireList[pKFire].second=D_Second;
        pKFire++;
    }
    
}



//分类型存储故障数量
void SaveErrNum(uint8 type)
{
	uint8 tt,j;
	tt= m_Record.type & 0x1F;
	switch(tt)
	{
	case 1:		j=0;	break;
	case 2:		j=1;	break;
	case 4:		j=2;	break;
		
	case 10:	j=3;	break;
	case 11:	j=4;	break;
	case 12:	j=5;	break;
	case 13:	j=6;	break;

	case 14:	j=7;	break;
	case 15:	j=8;	break;

	case 20:	j=9;	break;
	case 21:	j=10;	break;
						
	default:	j=12;	break;
	}
	if(type==0)
		sberr[j]++;	//设备故障数
	else
	{
		if(sberr[j]>0)
			sberr[j]--;
	}
}


uint8 IsModuleFeedbackCancelUpload(void)
{
    uint16 i;
    uint8 type;

    type = m_Record.type & 0x1F;
    if(m_Record.sign != 0x86 || type < 11 || type > 13)
        return 0;

    for(i=0; i<pRun; i++)
    {
        if(RunList[i].panel==m_Record.panel &&
           RunList[i].loop==m_Record.loop &&
           RunList[i].addr==m_Record.addr &&
           RunList[i].sign==0x85)
        {
            return 1;
        }
    }

    return 0;
}
void WriteToRunZone()
{
    uint16 i;
    uint8 tt=0;
	//首先查找联动区是否有本条
	if(pRun>0)
	{
		for(i=0;i<pRun;i++)
		{
			if(RunList[i].panel==m_Record.panel &&
			   RunList[i].loop==m_Record.loop &&
			   RunList[i].addr==m_Record.addr)
			{
				//找到
				tt=1;
				switch(RunList[i].sign)
				{
					case 0x83:
					{
						if(m_Record.sign==0x83)
							Total_Act--;
						else if(m_Record.sign==0x84)
						{
							RunList[i].sign=0x8B; //启动后停止
							if(m_ActNoAnswer>0)		m_ActNoAnswer--;
						}
						else if(m_Record.sign==0x85)
						{
							RunList[i].sign=0x8A; //启动并反馈
							m_ActAndAnswer++; //启动并反馈总数
						}
						else 
							RunList[i].sign=0x83;
					}
					break;
					case 0x84:
					{
						if(m_Record.sign==0x84)
							Total_Act++;
						else 
							RunList[i].sign=m_Record.sign;
					}
					break;
					case 0x85:
					{
						if(m_Record.sign==0x83)
						{
							RunList[i].sign=0x8A; //启动并反馈
							m_ActAndAnswer++; //启动并反馈总数
						}
						else if(m_Record.sign==0x85)
							Total_Answer--; 
						else if(m_Record.sign==0x86)
							RunList[i].sign=0x8C; //反馈后撤销
					}
					break;
					case 0x86:
					{
						if(m_Record.sign==0x86)
							Total_Answer++;
						else 
							RunList[i].sign=m_Record.sign;
					}
					break;
					case 0x8A: //启动并反馈
					{
						if(m_Record.sign==0x83)
							Total_Act--;
						else if(m_Record.sign==0x84)
							RunList[i].sign=0x85; 
						else if(m_Record.sign==0x85)
							Total_Answer--; 
						else 
						{
							RunList[i].sign=0x83;
							m_ActAndAnswer--; //启动并反馈总数
						}
					}
					break;
					case 0x8B: //启动后停止
					{
						if(m_Record.sign==0x84)
							Total_Act++;
						else if(m_Record.sign==0x86)
							RunList[i].sign=0x8B; 
						else 
							RunList[i].sign=m_Record.sign;
					}
					break;
					case 0x8C: //反馈后撤销
					{
						if(m_Record.sign==0x86)
							Total_Answer++;
						else if(m_Record.sign==0x84)
							RunList[i].sign=0x8C; 
						else 
							RunList[i].sign=m_Record.sign;
					}
					break;
					
				}
				break;
			}
		 }    
	}
	if(tt)	
	{	
		ldzonetype=0;		
		m_RunAuto=0;
	}	
	if(tt==0 && pRun<255)
    {
        //未找到,则写入联动区
		RunList[pRun].sign=m_Record.sign;
        RunList[pRun].panel=m_Record.panel;
        RunList[pRun].loop=m_Record.loop;
        RunList[pRun].addr=m_Record.addr;
        RunList[pRun].type=m_Record.type;

        RunList[pRun].year=D_Year;
        RunList[pRun].month=D_Month;
        RunList[pRun].day=D_Day;
        RunList[pRun].hour=D_Hour;
        RunList[pRun].minute=D_Minute;
        RunList[pRun].second=D_Second;
        pRun++;
		m_RunAuto=0;
    }
    
}

void WriteToMhRunZone()
{
    uint16 i;
    uint8 tt=0;
	if(m_Record.sign==0x8F)
	{
		//气体灭火类
		switch(m_Record.type)
		{
		case 0x03:
		case 0x04:
		case 0x0B:
			m_Record.type |=0x80;	//需要显示延时中
			RunDelayNum++;	//启动延时的总数，用于灯状态
			break;
		case 0x01:
		case 0x02:
		case 0x05:
		case 0x06:
		case 0x07:
			//需要查找联动区，去除延时中标识
			if(pmhRun>0)
			{
				for(i=0;i<pmhRun;i++)
				{
					if(MhRunList[i].sign==0x8F && 
					   MhRunList[i].panel==m_Record.panel &&
					   MhRunList[i].loop==m_Record.loop &&
					   MhRunList[i].addr==m_Record.addr)
					{
						//找到
						mhzonetype=0;
						if(MhRunList[i].type==0x83 || MhRunList[i].type==0x84 || MhRunList[i].type==0x8B)
						{
							MhRunList[i].type &=0x0F;
							if(RunDelayNum>0)
								RunDelayNum--;	//启动延时的总数，用于灯状态
							
						}
					}
				 }    
			}
			break;
		}
	}
	
	if(tt==0 && pmhRun<63)
    {
        //未找到,则写入联动区
		MhRunList[pmhRun].sign=m_Record.sign;
        MhRunList[pmhRun].panel=m_Record.panel;
        MhRunList[pmhRun].loop=m_Record.loop;
        MhRunList[pmhRun].addr=m_Record.addr;
        MhRunList[pmhRun].type=m_Record.type;

        MhRunList[pmhRun].year=D_Year;
        MhRunList[pmhRun].month=D_Month;
        MhRunList[pmhRun].day=D_Day;
        MhRunList[pmhRun].hour=D_Hour;
        MhRunList[pmhRun].minute=D_Minute;
        MhRunList[pmhRun].second=D_Second;
        pmhRun++;
    }
    
}

void pdxhsfh(uint8 panel,uint8 loop,uint8 addr)
{
	if(panel==0 && loop==102 && addr==1)
	{
		//发送消火栓反馈命令
		m_CanCommand.sign=0x01;  //下传消火栓点灯命令
		m_CanCommand.loop=0x0B;
		m_CanCommand.addr=0;
		m_CanCommand.type=0;
		WriteCanCommand();
	}		
}

//其他机复位，删除故障
void OtherReset(uint8 panel) 
{
    uint16 i,j;
    i=0;
    j=0;
    //删除故障
    while(j<pError)
    {
        if(ErrorList[j].panel!=panel)
        {
            ErrorList[i].sign=ErrorList[j].sign;
            ErrorList[i].panel=ErrorList[j].panel;
            ErrorList[i].loop=ErrorList[j].loop;
            ErrorList[i].addr=ErrorList[j].addr;
            ErrorList[i].type=ErrorList[j].type;
			ErrorList[i].type1=ErrorList[j].type1;

            ErrorList[i].year=ErrorList[j].year;
            ErrorList[i].month=ErrorList[j].month;
            ErrorList[i].day=ErrorList[j].day;
            ErrorList[i].hour=ErrorList[j].hour;
            ErrorList[i].minute=ErrorList[j].minute;
            ErrorList[i].second=ErrorList[j].second;
			ErrorList[i].confirm=ErrorList[j].confirm;
			i++;
        }
        j++;
    }
	if(i!= pError)
	{
		pError=i;
		Total_Error=pError;
		DisplayTotal(4);	//刷新故障总数
		if(m_View==91 || m_View==33)
		{
			m_FirstView=0;  //刷新页面
		}
		if(Total_Error==0)
		{
			//刷新声光
			m_deng[0] &=0xF7;  //清故障灯
			if(m_Voice==0 && pFire==0 && pRun==0)
				m_deng[0] &=0xEF;   //清消音灯
			if(m_Voice==1)
			{
				voice_out(0);
			}
			gpio_out(3,0);		//故障继电器									
		}
	}
    
}


void m_Reset()
{    
	
	uint8_t i;
	Fcolor=WHITE;
	Bcolor=NAVY;	
	SetColor(Fcolor,Bcolor);
	Lcdclr();
	OutChars(6,10,"系统正在复位，请稍候——",1);
	for(i=0;i<25;i++)
	{
		vTaskDelay(100);
		fwdgt_counter_reload(); // 喂狗
	}
	OutChars(6,10,"   复位已完成！         ",1);	
	for(i=0;i<15;i++)
	{
		vTaskDelay(100);
		fwdgt_counter_reload(); // 喂狗
	}
	
	MainTaskInit();
	InitDeviceRecordSystem();
	TongXinBan_value[0][0] = 0x01;
	ResetKeRanQiUploadStatus();
	ResetTongXinBanRunStatus();
	
	//发送复位命令
	m_CanCommand.sign=0x01;  
	m_CanCommand.loop=0x01;
	m_CanCommand.addr=0;
	m_CanCommand.type=0;
	WriteCanCommand();
	
	m_Record.sign=0x03; //复位
    m_Record.year=D_Year;
    m_Record.month=D_Month;
    m_Record.day=D_Day;
    m_Record.hour=D_Hour;
    m_Record.minute=D_Minute;
    m_Record.second=D_Second;
    WriteHisRecord(0); //写入开机记录
	
	if(m_OhterReset!=0x80) //被其他机器复位标志
	{
	    m_Record.sign=0x01;  
	    m_Record.loop=0x01;
	    m_Record.addr=0;
	    m_Record.type=0;
	    //发送外部复位命令
	    if(m_PanelType)
	    {
	        m_Record.addr=1;  //主机发送标志
	    }
	    WriteToWCANZone(); //写入外部CAN发送区
	}
    m_OhterReset=0x00;
	
}

//单独复位可燃气体
void m_ResetKrkzq()	
{
	//需从故障区中删除可燃气体的数据，清空可燃气体报警区，再下发复位命令
    uint16 i,j,num;
    i=0;
    j=0;
	num=0;
    //删除故障
    while(j<pError)
    {
        if(ErrorList[j].sign==0x81 && ErrorList[j].loop==3)
		{
			//是可燃气体探测器
			num++;
		}
		else
        {
            ErrorList[i].sign=ErrorList[j].sign;
            ErrorList[i].panel=ErrorList[j].panel;
            ErrorList[i].loop=ErrorList[j].loop;
            ErrorList[i].addr=ErrorList[j].addr;
            ErrorList[i].type=ErrorList[j].type;
			ErrorList[i].type1=ErrorList[j].type1;

            ErrorList[i].year=ErrorList[j].year;
            ErrorList[i].month=ErrorList[j].month;
            ErrorList[i].day=ErrorList[j].day;
            ErrorList[i].hour=ErrorList[j].hour;
            ErrorList[i].minute=ErrorList[j].minute;
            ErrorList[i].second=ErrorList[j].second;
			ErrorList[i].confirm=ErrorList[j].confirm;
			i++;
        }
        j++;
    }
	if(i!= pError)
	{
		pError=i;
		Total_Error=pError;
		DisplayTotal(4);	//刷新故障总数
		if(Total_Error==0)
		{
			//刷新声光
			m_deng[0] &=0xF7;  //清故障灯
			if(m_Voice==0 && pFire==0 && pRun==0 && pKFire==0)
				m_deng[0] &=0xEF;   //清消音灯
			if(m_Voice==1)
			{
				voice_out(0);
			}
			gpio_out(3,0);		//故障继电器									
		}
	}
	if(Total_KError>=num)
		Total_KError-=num;
	else
		Total_KError=0;
	
	if(Total_KError==0)
		m_deng[2] &=0xDF;
	
	//清可燃气体报警区
	pKFire=0;
	ResetKeRanQiUploadStatus();
	m_deng[2] &=0xFE;	//可燃气体报警灯
	gpio_out(4,0);	//继电器
	Total_Fire1=0;
	DisplayTotal(6);	//显示总数
	gpio_out(5,0);	//继电器
	Total_Fire2=0;
	DisplayTotal(7);	//显示总数
	if(m_Voice==3)
	{
		if(pFire==0)
		{
			m_Voice=0;
			voice_out(0);
		}
	}
		
	//下发命令
	m_CanCommand.sign=0x01;  
	m_CanCommand.loop=0x0C;
	m_CanCommand.addr=0;
	m_CanCommand.type=0;
	WriteCanCommand();
	
	FirstPageView(); //首页显示
	
}


uint16 hebingtime(void)
{
	uint16 tt,tt1;
	tt= (uint16)((D_Month & 0x0F)<<12);
	tt1=(uint16)((D_Day & 0x1F)<<6);
	tt |=tt1;
	tt |=(D_Hour & 0x1F);
	return tt;
}

//写入单独的故障区
uint8 WriteToSingleErr()
{
	uint8 r;
	r=0;
	switch(m_Record.loop)
	{
	case 1:		//气灭的总线设备故障
	case 2:
	case 101:	//气灭的直控故障
		if(pmhError<127)
		{
			MhErrList[pmhError].sign=m_Record.sign;
			MhErrList[pmhError].panel=m_Record.panel;
			MhErrList[pmhError].loop=m_Record.loop;
			MhErrList[pmhError].addr=m_Record.addr;
			MhErrList[pmhError].type=m_Record.type;
			MhErrList[pmhError].type1=m_Record.type1;

			MhErrList[pmhError].year=D_Year;
			MhErrList[pmhError].month=D_Month;
			MhErrList[pmhError].day=D_Day;
			MhErrList[pmhError].hour=D_Hour;
			MhErrList[pmhError].minute=D_Minute;
			MhErrList[pmhError].second=D_Second;
			MhErrList[pmhError].confirm=0;
			pmhError++;
			r=1;
	   }		
		
		 
	    break;
	case 3:		//可燃气体的故障
		if(pkrError<63)
		{
			KrErrList[pkrError].sign=m_Record.sign;
			KrErrList[pkrError].panel=m_Record.panel;
			KrErrList[pkrError].loop=m_Record.loop;
			KrErrList[pkrError].addr=m_Record.addr;
			KrErrList[pkrError].type=m_Record.type;
			KrErrList[pkrError].type1=m_Record.type1;
			KrErrList[pkrError].type2=m_Record.type2;

			KrErrList[pkrError].year=D_Year;
			KrErrList[pkrError].month=D_Month;
			KrErrList[pkrError].day=D_Day;
			KrErrList[pkrError].hour=D_Hour;
			KrErrList[pkrError].minute=D_Minute;
			KrErrList[pkrError].second=D_Second;
			KrErrList[pkrError].confirm=0;
			pkrError++;
			r=1;
	   }				
		break;
//	case 4:		//火灾报警的总线设备故障
//	case 5:
//		if(phjError<255)
//		{
//			HjErrList[phjError].sign=m_Record.sign;
//			HjErrList[phjError].panel=m_Record.panel;
//			HjErrList[phjError].loop=m_Record.loop;
//			HjErrList[phjError].addr=m_Record.addr;
//			HjErrList[phjError].type=m_Record.type;
//			HjErrList[phjError].type1=m_Record.type1;

//			HjErrList[phjError].year=D_Year;
//			HjErrList[phjError].month=D_Month;
//			HjErrList[phjError].day=D_Day;
//			HjErrList[phjError].hour=D_Hour;
//			HjErrList[phjError].minute=D_Minute;
//			HjErrList[phjError].second=D_Second;
//			HjErrList[phjError].confirm=0;
//			phjError++;
//	   }						
//		break;
	case 6:		//消防联动的总线设备故障
	case 102:	//多线的故障
		if(pldError<255)
		{
			LdErrList[pldError].sign=m_Record.sign;
			LdErrList[pldError].panel=m_Record.panel;
			LdErrList[pldError].loop=m_Record.loop;
			LdErrList[pldError].addr=m_Record.addr;
			LdErrList[pldError].type=m_Record.type;
			LdErrList[pldError].type1=m_Record.type1;

			LdErrList[pldError].year=D_Year;
			LdErrList[pldError].month=D_Month;
			LdErrList[pldError].day=D_Day;
			LdErrList[pldError].hour=D_Hour;
			LdErrList[pldError].minute=D_Minute;
			LdErrList[pldError].second=D_Second;
			LdErrList[pldError].confirm=0;
			pldError++;
			r=1;
	   }		
		break;	
	}		
	return r;
}


uint8 Delete_SingleError()
{
	uint16 i,j;
	uint8 r;
	r=0;
	i=0;
	j=0;	
	switch(m_Record.loop)
	{
	case 1:		//气灭的总线设备故障
	case 2:
	case 101:	//气灭的直控故障
		while(i<pmhError)
		{
			if((MhErrList[i].sign==m_Record.sign) && 
			(MhErrList[i].panel==m_Record.panel) &&
			(MhErrList[i].loop==m_Record.loop) &&
			(MhErrList[i].addr==m_Record.addr) &&
			(MhErrList[i].type==m_Record.type) && 
			(MhErrList[i].type1==m_Record.type1)) 
			{	
				j=0x55;
				break;
			}
			i++;
		}
		if(j==0x55)
		{
			//找到恢复项,将后面的往前移
			while(i<pmhError)
			{
				MhErrList[i].sign=MhErrList[i+1].sign;
				MhErrList[i].panel=MhErrList[i+1].panel;
				MhErrList[i].loop=MhErrList[i+1].loop;
				MhErrList[i].addr=MhErrList[i+1].addr;
				MhErrList[i].type=MhErrList[i+1].type;
				MhErrList[i].type1=MhErrList[i+1].type1;

				MhErrList[i].year=MhErrList[i+1].year;
				MhErrList[i].month=MhErrList[i+1].month;
				MhErrList[i].day=MhErrList[i+1].day;
				MhErrList[i].hour=MhErrList[i+1].hour;
				MhErrList[i].minute=MhErrList[i+1].minute;
				MhErrList[i].second=MhErrList[i+1].second;
				MhErrList[i].confirm=MhErrList[i+1].confirm;
				i++;
			}
			if(pmhError>0)
				pmhError--;
			if((m_View==90 || m_View==91 || m_View==33) && mhzonetype==2)
			{
				mhzonetype=0;
				Qxsq(3);	//清灭火显示区
			}				
		}
		r=1;
		break;
	case 3:		//可燃气体的故障
		while(i<pkrError)
		{
			if((KrErrList[i].sign==m_Record.sign) && 
			(KrErrList[i].panel==m_Record.panel) &&
			(KrErrList[i].loop==m_Record.loop) &&
			(KrErrList[i].addr==m_Record.addr) &&
			(NormalizeKrDeviceType(KrErrList[i].type)==NormalizeKrDeviceType(m_Record.type)) && 
			(KrErrList[i].type1==m_Record.type1)) 
			{	
				j=0x55;
				break;
			}
			i++;
		}
		if(j==0x55)
		{
			//找到恢复项,将后面的往前移
			while(i<pkrError)
			{
				KrErrList[i].sign=KrErrList[i+1].sign;
				KrErrList[i].panel=KrErrList[i+1].panel;
				KrErrList[i].loop=KrErrList[i+1].loop;
				KrErrList[i].addr=KrErrList[i+1].addr;
				KrErrList[i].type=KrErrList[i+1].type;
				KrErrList[i].type1=KrErrList[i+1].type1;
				KrErrList[i].type2=KrErrList[i+1].type2;

				KrErrList[i].year=KrErrList[i+1].year;
				KrErrList[i].month=KrErrList[i+1].month;
				KrErrList[i].day=KrErrList[i+1].day;
				KrErrList[i].hour=KrErrList[i+1].hour;
				KrErrList[i].minute=KrErrList[i+1].minute;
				KrErrList[i].second=KrErrList[i+1].second;
				KrErrList[i].confirm=KrErrList[i+1].confirm;
				i++;
			}
			if(pkrError>0)
				pkrError--;
			if((m_View==90 || m_View==91 || m_View==33) && krzonetype==2)
			{
				krzonetype=0;
				Qxsq(2);	//清可燃显示区
			}
			//判断可燃气体故障恢复
			if(Total_KError>0)
				Total_KError--;
			if(Total_KError==0)
				m_deng[2] &=0xDF;
		}			
		r=1;
		break;
//	case 4:		//火灾报警的总线设备故障
//	case 5:
//		while(i<phjError)
//		{
//			if((HjErrList[i].sign==m_Record.sign) && 
//			(HjErrList[i].panel==m_Record.panel) &&
//			(HjErrList[i].loop==m_Record.loop) &&
//			(HjErrList[i].addr==m_Record.addr) &&
//			(HjErrList[i].type==m_Record.type) && 
//			(HjErrList[i].type1==m_Record.type1)) 
//			{	
//				j=0x55;
//				break;
//			}
//			i++;
//		}
//		if(j==0x55)
//		{
//			//找到恢复项,将后面的往前移
//			while(i<phjError)
//			{
//				HjErrList[i].sign=HjErrList[i+1].sign;
//				HjErrList[i].panel=HjErrList[i+1].panel;
//				HjErrList[i].loop=HjErrList[i+1].loop;
//				HjErrList[i].addr=HjErrList[i+1].addr;
//				HjErrList[i].type=HjErrList[i+1].type;
//				HjErrList[i].type1=HjErrList[i+1].type1;

//				HjErrList[i].year=HjErrList[i+1].year;
//				HjErrList[i].month=HjErrList[i+1].month;
//				HjErrList[i].day=HjErrList[i+1].day;
//				HjErrList[i].hour=HjErrList[i+1].hour;
//				HjErrList[i].minute=HjErrList[i+1].minute;
//				HjErrList[i].second=HjErrList[i+1].second;
//				HjErrList[i].confirm=HjErrList[i+1].confirm;
//				i++;
//			}
//			if(phjError>0)
//				phjError--;
//			if(m_View==90 && hjzonetype==2)
//			{
//				hjzonetype=0;
//				Qxsq(0);	//清火警显示区
//			}				
//		}			
//		break;
	case 6:		//消防联动的总线设备故障
	case 102:	//多线的故障
		while(i<pldError)
		{
			if((LdErrList[i].sign==m_Record.sign) && 
			(LdErrList[i].panel==m_Record.panel) &&
			(LdErrList[i].loop==m_Record.loop) &&
			(LdErrList[i].addr==m_Record.addr) &&
			(LdErrList[i].type==m_Record.type) && 
			(LdErrList[i].type1==m_Record.type1)) 
			{	
				j=0x55;
				break;
			}
			i++;
		}
		if(j==0x55)
		{
			//找到恢复项,将后面的往前移
			while(i<pldError)
			{
				LdErrList[i].sign=LdErrList[i+1].sign;
				LdErrList[i].panel=LdErrList[i+1].panel;
				LdErrList[i].loop=LdErrList[i+1].loop;
				LdErrList[i].addr=LdErrList[i+1].addr;
				LdErrList[i].type=LdErrList[i+1].type;
				LdErrList[i].type1=LdErrList[i+1].type1;

				LdErrList[i].year=LdErrList[i+1].year;
				LdErrList[i].month=LdErrList[i+1].month;
				LdErrList[i].day=LdErrList[i+1].day;
				LdErrList[i].hour=LdErrList[i+1].hour;
				LdErrList[i].minute=LdErrList[i+1].minute;
				LdErrList[i].second=LdErrList[i+1].second;
				LdErrList[i].confirm=LdErrList[i+1].confirm;
				i++;
			}
			if(pldError>0)
				pldError--;
			if((m_View==90 || m_View==91 || m_View==33) && ldzonetype==2)
			{
				ldzonetype=0;
				Qxsq(1);	//清联动显示区
			}				
		}			
		r=1;
		break;
	}
	if(r==1)
	{		
		if(Total_Error>0) Total_Error--;
		DisplayTotal(4);	//刷新故障总数
		if(Total_Error==0)
		{
			//刷新声光
			m_deng[0] &=0xF7;  //清故障灯
			if(m_Voice==0 && pFire==0 && pRun==0 && pKFire==0)
				m_deng[0] &=0xEF;   //清消音灯
			if(m_Voice==1)
			{
				voice_out(0);
			}
			gpio_out(3,0);		//故障继电器			
		}
	}
	return r;
}


//Can接收数据处理
void m_Can_Receive()	
{
	uint8 type1;
	uint8 tt,panel,j,loop,addr,r;
	uint16 i,num,shu;
	INT8U *dptr;

	BaseType_t xReturn;  
	xReturn=xQueueReceive(canrev_queue,&msg,0);
	if(pdTRUE == xReturn)
	{
	  	if(msg.source==0)
		{
			//内部CAN
			if(msg.type==0x40)
			{
				m_Record.sign=msg.rdata[0];
				m_Record.panel=0;
				m_Record.loop=msg.rdata[1];
				m_Record.addr=msg.rdata[2];
				m_Record.type=msg.rdata[3];
				m_Record.type1=msg.rdata[4];
				m_Record.type2=msg.rdata[5];

				if(m_Record.loop==3)
					m_Record.type=NormalizeKrDeviceType(m_Record.type);

				if(krq_SnedFlag == 1 && m_Record.loop == 3)
				{
					krq_SnedFlag = 0;
					m_Record.sign = 0x71;
					m_Record.panel = m_PanelAddr;
					WriteToWCANZone();
				}

				//处理数据
				switch(m_Record.sign)  {
				case 0x80: //火警
					HJSG();     //火警声光
					WriteToFireZone();//写入火警区
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(1);//写入历史记录 
					HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_ALARM,1);
					break;
				case 0x7A:	//低警
				case 0x7B:  //高警
					KBJSG();     //可燃气体报警声光
					WriteToKrFireZone();//写入火警区
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_KeRanQi])//0612
					{	
						if(m_Record.sign == 0x7A)
						{
							TongXinBan_value[DEVICE_KeRanQi][m_Record.addr-1] |= KERANQI_UPLOAD_LOW_ALARM;
						}
						else
						{
							TongXinBan_value[DEVICE_KeRanQi][m_Record.addr-1] |= KERANQI_UPLOAD_LOW_ALARM;
							TongXinBan_value[DEVICE_KeRanQi][m_Record.addr-1] |= KERANQI_UPLOAD_HIGH_ALARM;
						}
						TongXinBan_value[0][0] |= STATUS_ALARM;
					}
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(3);//写入可燃气体报警历史记录	
					HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_ALARM,1);
					break;
				case 0x81: //故障
				case 0x61: //多线故障
					GZSG(); //故障声光
					r=WriteToSingleErr();	//写入单独的故障区
					if(r==0)
					{
						WriteToErrorZone();//写入故障区
						
						HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_FAULT,1);

					}
					if(m_Record.sign==0x81)
						HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_FAULT,1);
					if(m_Record.sign==0x61)
						SyncDuoXianUploadState(m_Record.loop,m_Record.addr,STATUS_FAULT,1);
					else if((m_Record.type & 0x1F)==15 || (m_Record.loop==4 && m_Record.addr==SgAddr))
						SyncShengGuangUploadState(m_Record.loop,m_Record.addr,STATUS_FAULT,1);
					
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(0);//写入历史记录
					//判断声光故障
					if(m_Record.loop==4 && m_Record.addr==SgAddr)
					{
						sggznum++;
					}
					if(m_Record.sign==0x81)
						SaveErrNum(0);//分类型存储故障数量
					break;        
				case 0x82: //故障恢复
				case 0x62: //多线故障恢复
				{
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(0);//写入历史记录
					//判断声光故障恢复
					if(m_Record.loop==4 && m_Record.addr==SgAddr)
					{
						if(sggznum>0)
							sggznum--;	
					}
					if(m_Record.sign==0x82)
						SaveErrNum(1);//分类型存储故障数量
					m_Record.sign=m_Record.sign-1;
					r=Delete_SingleError();

					HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_FAULT,0);
					if(m_Record.sign==0x61)
						SyncDuoXianUploadState(m_Record.loop,m_Record.addr,STATUS_FAULT,0);
					else if((m_Record.type & 0x1F)==15 || (m_Record.loop==4 && m_Record.addr==SgAddr))
						SyncShengGuangUploadState(m_Record.loop,m_Record.addr,STATUS_FAULT,0);

					if(r==0)
						Delete_Error(); //故障恢复
					break;
				}
				case 0x83:	//启动
				{
					if(Total_Act==0)
					{
						m_ActNoAnswer=1;
						m_ActAndAnswer=0;
						m_RunTimes=20;		//启动后需要常亮的时间
					}
					else
						m_ActNoAnswer++; //启动未反馈总数
					WriteToRunZone();//写入联动区
					
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_DuoXian])//0612
					{	
						TongXinBan_value[DEVICE_DuoXian][m_Record.addr-1] |= STATUS_START;
						TongXinBan_value[0][0] |= STATUS_START;
					}
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(2);//写入历史记录
					HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_START,1);
					//判断总线声光启动
					if(m_Record.loop==4 && m_Record.addr==SgAddr)
					{
						sgqdnum++;
					}
					QDSG();     //启动声光
					break;
				}
				case 0x84:	//停止
				{
					//判断总线声光故障恢复
					if(m_Record.loop==4 && m_Record.addr==SgAddr)
					{
						if(sgqdnum>0)
							sgqdnum--;	
					}
					WriteToRunZone();//写入联动区
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_DuoXian])//0612
					{	
						TongXinBan_value[DEVICE_DuoXian][m_Record.addr-1] &= ~STATUS_START;
						TongXinBan_value[0][0]  &= ~STATUS_START;
					}
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(2);//写入历史记录
					HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_START,0);
					if(Total_Act>0)	Total_Act--;
					DisplayTotal(1);	//刷新动作总数
					break;	
				}
				case 0x85: //反馈
				{
					WriteToRunZone();//写入联动区
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_DuoXian])//0612
					{	
						TongXinBan_value[DEVICE_DuoXian][m_Record.addr-1] |= STATUS_FEEDBACK;
						TongXinBan_value[0][0]  |= STATUS_FEEDBACK;
					}
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(2);//写入历史记录
					FKSG();     //反馈声光
					pdxhsfh(0,m_Record.loop,m_Record.addr);
					HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_FEEDBACK,1);
					break;
				}
				case 0x86:	//撤销
				{
					r=IsModuleFeedbackCancelUpload();
					WriteToRunZone();//写入联动区
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_DuoXian])//0612
					{	
						TongXinBan_value[DEVICE_DuoXian][m_Record.addr-1] &= ~STATUS_FEEDBACK;
						TongXinBan_value[0][0]  &= ~STATUS_FEEDBACK;
					}
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(2);//写入历史记录
					if(Total_Answer>0)	Total_Answer--;
					if(Total_Answer==0)	m_deng[0] &=0xBF;	//清反馈灯
					HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_FEEDBACK,0);
					if(r)
						HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_FEEDBACK,1);
					DisplayTotal(2);	//刷新反馈总数
					break;	
				}
				case 0x87: //回路故障
				{
					GZSG(); //故障声光
					m_Record.type=0;
					WriteToErrorZone();//写入故障区
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(0);//写入历史记录
					break;        
				}
				case 0x88: //回路故障恢复
				{
					m_Record.type=0;
					WriteHisRecord(0);//写入历史记录
					WriteToWCANZone(); //写入外部CAN发送区
					m_Record.sign=0x87;
					Delete_Error(); //故障恢复
					break;
				}
				case 0x89:	//其它故障，主要是上传24V输出故障
				{
					GZSG(); //故障声光
					WriteToErrorZone();//写入故障区
					WriteToWCANZone(); //写入外部CAN发送区
					WriteHisRecord(0);//写入历史记录					
					break;
				}
				case 0x8A: //其它故障恢复
				{
					WriteHisRecord(0);//写入历史记录
					WriteToWCANZone(); //写入外部CAN发送区
					m_Record.sign=0x89;
					Delete_Error(); //故障恢复
					break;
				}
				case 0x8F:	//气体灭火类
				{
					switch(m_Record.type)
					{
					case 0x01:	//喷洒启动
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_QiMieFenQu])
					{	
						TongXinBan_value[DEVICE_QiMieFenQu][m_Record.addr-1] &= ~QIMIE_UPLOAD_STATE_MASK;
						TongXinBan_value[DEVICE_QiMieFenQu][m_Record.addr-1] |= STATUS_UPLOAD_PENSA;
						TongXinBan_value[0][0] |= STATUS_UPLOAD_PENSA;
					}
						WriteToWCANZone(); //写入外部CAN发送区
						QDSG();     //启动声光
						break;
					case 0x02:	//喷洒反馈
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_QiMieFenQu])
					{	
						TongXinBan_value[DEVICE_QiMieFenQu][m_Record.addr-1] &= ~QIMIE_UPLOAD_STATE_MASK;
						TongXinBan_value[DEVICE_QiMieFenQu][m_Record.addr-1] |= STATUS_FEEDBACK;
						TongXinBan_value[0][0] |= STATUS_FEEDBACK;
					}
						WriteToWCANZone(); //写入外部CAN发送区
						FKSG();     //反馈声光
						//喷洒声输出
						voice_out(6);
						break;
					case 0x03:	//灭火现场急启
					case 0x04:	//灭火面板急启
					case 0x08:	//声光启动
					case 0x0A:	//放气指示输出
					case 0x0B:	//灭火自动启动
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
					if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_QiMieFenQu])
					{	TongXinBan_value[DEVICE_QiMieFenQu][m_Record.addr-1] &= ~QIMIE_UPLOAD_STATE_MASK;
						TongXinBan_value[DEVICE_QiMieFenQu][m_Record.addr-1] |= KERANQI_UPLOAD_FIRE2;
						TongXinBan_value[0][0] |= KERANQI_UPLOAD_FIRE2;
					}
						WriteToWCANZone(); //写入外部CAN发送区
						QDSG();     //启动声光
						fengjiflag=1;
						TingZhiFengJi();
						QiDongShengGuang();
						break;
					case 0x05:	
					case 0x06:	
					case 0x07:	
					case 0x09:	
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
						if(m_Record.addr>0 && m_Record.addr<=TongXinBan_REGISTER_COUNT[DEVICE_QiMieFenQu])
						{
							TongXinBan_value[DEVICE_QiMieFenQu][m_Record.addr-1] &= ~QIMIE_UPLOAD_STATE_MASK;
						}
						WriteToWCANZone(); //写入外部CAN发送区
						if(Total_Act>0)	Total_Act--;
						DisplayTotal(1);	//刷新动作总数
						break;
					}
				}
				break;
				case 0x97:	//已解除屏蔽
				case 0x98:	//已屏蔽
				case 0x99:	//已登记
				case 0x9A:	//已解除登记
				case 0x9B:	//返回总线对应
				case 0xA0:	//返回查询数据
				case 0xA1:	//返回背景电流
				case 0xA3:	//返回总线对应关系
				case 0xA4:	//返回查询灭火延时
				case 0xA5:	//返回设置灭火延时
				{
					//命令的反馈
					m_Return=0x80;   //命令反馈标志
					m_CanReturn.sign=m_Record.sign;
					m_CanReturn.loop=m_Record.loop;
					m_CanReturn.addr=m_Record.addr;
					m_CanReturn.type=m_Record.type;
					m_CanReturn.type1=m_Record.type1;

					if(m_Record.sign==0x99 || m_Record.sign==0x9A)
					{
						TriggerTongXinBanRegisterQuery();
					}

					break;
				}
				}
						
			}
		}
		else
		{
			//外部CAN
			m_Record.sign=msg.rdata[0];
			m_Record.panel=msg.add;
			m_Record.loop=msg.rdata[1];
			m_Record.addr=msg.rdata[2];
			m_Record.type=msg.rdata[3];
			m_Record.type1=msg.rdata[4];			
			m_Record.type2=msg.rdata[5];			
			if(m_Record.loop==3)
				m_Record.type=NormalizeKrDeviceType(m_Record.type);
			if(m_Record.sign==0x01 && m_Record.loop==0x01)	//复位命令	
			{
				if(m_PanelType)
				{
					//本机为主机，则清除其它机器的故障数据
					OtherReset(m_Record.panel);  //其他机器复位
				}
				else
				{
					//本机为从机，判断是否为其它主机复位
					if(m_Record.addr==0x01)
					{
						m_OhterReset=0x80; //被其他机器复位标志
						m_Reset();  //复位
					}	
				}
			}
			else
			{
				if(m_Record.sign==0x32 || m_Record.sign==0x33)	//跨机启动停止
				{
					if(m_Record.type==m_PanelAddr)
					{
						//是本机地址的命令
						m_CanCommand.sign=m_Record.sign-0x30;  
						m_CanCommand.loop=m_Record.loop;
						m_CanCommand.addr=m_Record.addr;
						m_CanCommand.type=0;
						WriteCanCommand();
					}				
				}
				else if(m_Record.sign==0x41)
				{
					//查询手自动状态
					if(m_Record.loop==m_PanelAddr)
						WBrocastTimes=20;	//快速广播状态
				}
				else if(m_PanelType==0)	
				{
					if(m_Record.sign==0x01)
					{
						if(m_Record.loop==0x0C)
						{
							//主机下发消音命令
							if(m_Voice!=0)
							{
								m_Voice=0;
								voice_out(0);
								m_deng[0]|=0x10;	//置消音灯	
							}						
						}
					}
					else if(m_Record.sign==0x35)
					{
						//收到时钟同步命令
						D_Year=msg.rdata[1];
						D_Month=msg.rdata[2];
						D_Day=msg.rdata[3];
						D_Hour=msg.rdata[4];
						D_Minute=msg.rdata[5];
						D_Second=msg.rdata[6];

						rtc_setup(); 
						
					}
					else if(m_Record.sign==0x72)
					{
						if(m_Record.loop == m_PanelAddr)
						{
							m_CanCommand.sign=0x14;
							m_CanCommand.loop=3;
							m_CanCommand.addr=0;
							m_CanCommand.type=0;
							krq_SnedFlag = 1;
							WriteCanCommand();
						}
					}
					return;	//本机为从机，退出
				}
				//本机为主机，处理数据
				switch(m_Record.sign)  {
				case 0x01: //其它机器发送
				{
					if(m_Record.loop==0x03)
					{
						//从机自检
						r=0;
						if(Fcolor !=WHITE || Bcolor !=BLUE)
						{
							SetColor(WHITE,BLUE);
							r=1;
						}
						OutTwoChar1(400,456,m_Record.panel,0);
						TextOut(424,456,"机自检...",0);
						if(r) 	SetColor(Fcolor,Bcolor);
					}
					break;
				}
				case 0x05: //其它机器自检完成
				{
					r=0;
					if(Fcolor !=WHITE || Bcolor !=BLUE)
					{
						SetColor(WHITE,BLUE);
						r=1;
					}
					TextOut(400,456,"              ",0);
					if(r) 	SetColor(Fcolor,Bcolor);
					break;
				}
				case 0x70: //查询指定机器的可燃气浓度
				case 0x72:
				{
					if (m_Record.loop == m_PanelAddr)//判断是否为本机
					{
						m_CanCommand.sign=0x14;  	//查可燃气体探测器信息
						m_CanCommand.loop=3;
						m_CanCommand.addr=0;
						m_CanCommand.type=0;
						krq_SnedFlag = 1;
						WriteCanCommand();//外can接收指令后，通过内can查询可燃气浓度
					}					
					break;
				}
				case 0x71: //返回可燃气浓度查询
				{
					if(m_Record.loop==3 && m_Record.addr>0 && m_Record.addr<=6)
					{
						shu = ((uint16)m_Record.type1 << 8) | m_Record.type2;
						shu &= 0x3FFF;
						SetKeRanQiUploadConcentration(m_Record.addr, shu);
						SetKeRanQiUploadBit(m_Record.addr, KERANQI_UPLOAD_REGISTER, 1);
						UpdateKrFireConcentration(m_Record.addr, m_Record.type, shu);

						if(m_View==66)
						{
							tt=m_Record.addr+4;
							j=19;
							OutChars(tt,j,"是",0);

							j=26;
							if(m_Record.type==25)
								OutChars(tt,j,"CO探测器",0);
							else if(m_Record.type==26)
								OutChars(tt,j,"H2探测器",0);
							else
								OutChars(tt,j,"未知类型",0);

							j=36;
							OutChars(tt,j,"正常",0);

							j=47;
							if(m_Record.type==25)
							{
								num=shu/100;
								if(num>9)
									OutTwoChar(tt,j,num,0);
								else
									OutOneChar(tt,j+1,num+0x30,0);
								OutTwoChar(tt,j+2,shu%100,0);
								OutChars(tt,j+4,"ppm",0);
							}
							else if(m_Record.type==26)
							{
								num=shu/100;
								if(num>1)
									num=1;
								OutOneChar(tt,j,num+0x30,0);
								OutTwoChar(tt,j+1,shu%100,0);
								OutChars(tt,j+3,"%LEL",0);
							}
						}
					}
					break;
				}
				case 0x80: //火警
				{
					HJSG();     //火警声光
					WriteToFireZone();//写入火警区
					WriteHisRecord(1);//写入历史记录 
					break;
				}
				case 0x7A:	//低警
				case 0x7B:  //高警
					KBJSG();     //可燃气体报警声光
					WriteToKrFireZone();//写入火警区
					WriteHisRecord(3);//写入可燃气体报警历史记录 				
					break;
				case 0x81: //故障
				case 0x61: //多线故障
				{
					
					GZSG(); //故障声光					
					r=WriteToSingleErr();	//写入单独的故障区
					if(r==0)
					{
						WriteToErrorZone();//写入故障区
					}
					WriteHisRecord(0);//写入历史记录
					break;        
				}
				case 0x82: //故障恢复
				case 0x62: //多线故障恢复
				{
					WriteHisRecord(0);//写入历史记录
					m_Record.sign=m_Record.sign-1;
					r=Delete_SingleError();
					if(r==0)
						Delete_Error(); //故障恢复					
					break;
				}
				case 0x83:	//启动
				{
					if(Total_Act==0)
					{
						m_ActNoAnswer=1;
						m_ActAndAnswer=0;
						m_RunTimes=20;		//启动后需要常亮的时间
					}
					else
						m_ActNoAnswer++; //启动未反馈总数
					WriteToRunZone();//写入联动区
					WriteHisRecord(2);//写入历史记录
					QDSG();     //启动声光
					break;
				}
				case 0x84:	//停止
				{
					WriteToRunZone();//写入联动区
					WriteHisRecord(2);//写入历史记录
					if(Total_Act>0)	Total_Act--;
					DisplayTotal(1);	//刷新动作总数
					break;	
				}
				case 0x85: //反馈
				{
					WriteToRunZone();//写入联动区
					WriteHisRecord(2);//写入历史记录
					FKSG();     //反馈声光
					break;
				}
				case 0x86:	//撤销
				{
					r=IsModuleFeedbackCancelUpload();
					WriteToRunZone();//写入联动区
					WriteHisRecord(2);//写入历史记录
					if(Total_Answer>0)	Total_Answer--;
					if(Total_Answer==0)	m_deng[0] &=0xBF;	//清反馈灯
					DisplayTotal(2);	//刷新反馈总数
					break;	
				}
				case 0x87: //回路故障
				{
					GZSG(); //故障声光
					m_Record.type=0;
					WriteToErrorZone();//写入故障区
					WriteHisRecord(0);//写入历史记录
					break;        
				}
				case 0x88: //回路故障恢复
				{
					m_Record.type=0;
					WriteHisRecord(0);//写入历史记录
					m_Record.sign=0x87;
					Delete_Error(); //故障恢复
					break;
				}
				case 0x89: //其它故障
				{
					GZSG(); //故障声光
					m_Record.addr=0;
					m_Record.type=0;
					WriteToErrorZone();//写入故障区
					WriteHisRecord(0);//写入历史记录
					break;        
				}
				case 0x8A: //其它故障恢复
				{
					m_Record.addr=0;
					m_Record.type=0;
					WriteHisRecord(0);//写入历史记录
					m_Record.sign=0x89;
					Delete_Error(); //故障恢复
					break;
				}
				case 0x8F:	//气体灭火类
				{
					switch(m_Record.type)
					{
					case 0x01:	//喷洒启动
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
						QDSG();     //启动声光
						break;
					case 0x02:	//喷洒反馈
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
						FKSG();     //反馈声光
						//喷洒声输出
						voice_out(6);						
						break;
					case 0x03:	
					case 0x04:	
					case 0x08:	
					case 0x0A:	
					case 0x0B:	
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
						QDSG();     //启动声光
						break;
					case 0x05:	
					case 0x06:	
					case 0x07:	
					case 0x09:	
						WriteHisRecord(2);//写入历史记录
						WriteToMhRunZone();//写入联动区
						if(Total_Act>0)	Total_Act--;
						DisplayTotal(1);	//刷新动作总数
						break;
					}
					break;
				}			
				case 0x91:	//屏蔽
				{
					//首先读出全部数据，查找是否已存在，如果不存在则写入
					num=Total_Isolate*8+8;	//需读出总数
					SPI_FLASH_HighSpeedReadCont(E_DisableAddr,num,(INT8U *)flashbuf);
					tt=0;
					dptr=(INT8U *)(flashbuf+8);
					if(Total_Isolate>0)
					{
						for(i=0;i<Total_Isolate;i++)
						{
							panel=*dptr;
							dptr++;
							loop=*dptr;
							dptr++;
							addr=*dptr;
							dptr+=6;
							if(panel==m_Record.panel && loop==m_Record.loop && addr==m_Record.addr)
							{
								tt=1;
								break;
							}
						}
					}
					if(tt==0)
					{
						
						//写入记录
						*dptr=m_Record.panel;
						dptr++;
						*dptr=m_Record.loop;
						dptr++;
						*dptr=m_Record.addr;
						dptr++;
						*dptr=m_Record.type;	//增加存储类型，月/日/时合成2个字节
						dptr++;
						*dptr=D_Year;
						dptr++;
						shu=hebingtime();
						*dptr=(shu & 0xFF00)>>8;
						dptr++;
						*dptr=shu & 0xFF;
						dptr++;
						*dptr=D_Minute;
						if(Total_Isolate<255)
							Total_Isolate++;
						dptr=(INT8U *)(flashbuf+2);
						*dptr=Total_Isolate;
						m_deng[0] |=0x80;	//屏蔽灯
						//写入
						Flash_Sector_Erase(30,30);	//清除屏蔽块
						num=Total_Isolate*8+8;	//需写入总数
						Auto_Add_Write(E_DisableAddr,(INT8U *)flashbuf,num);
						DisplayTotal(5);	//刷新屏蔽总数
						HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_SHIELD,1);
						if(bHaveSendDis)
						{
							//写入CRT实时发送区
							DisableList[m_pWriteDis].sign=0x91;	//屏蔽
							DisableList[m_pWriteDis].panel=m_Record.panel;
							DisableList[m_pWriteDis].loop=m_Record.loop;
							DisableList[m_pWriteDis].addr=m_Record.addr;
							DisableList[m_pWriteDis].type=m_Record.type;
			
							DisableList[m_pWriteDis].year=D_Year;
							DisableList[m_pWriteDis].month=D_Month;
							DisableList[m_pWriteDis].day=D_Day;
							DisableList[m_pWriteDis].hour=D_Hour;
							DisableList[m_pWriteDis].minute=D_Minute;
							DisableList[m_pWriteDis].second=D_Second;
							m_pWriteDis++;
							if(m_pWriteDis>99)	m_pWriteDis=0;
							m_SendDisNum=Total_Isolate;
						}
					}
					break;
				}
				case 0x92:	//解除屏蔽
				{
					
					//首先读出全部数据，查找是否已存在，如果存在则删除
					num=Total_Isolate*8+8;	//需读出总数
					SPI_FLASH_HighSpeedReadCont(E_DisableAddr,num,(INT8U *)flashbuf);
					dptr=(INT8U *)(flashbuf+8);
					tt=0;
					if(Total_Isolate>0)
					{
						i=0;
						while(i<Total_Isolate)
						{
							panel=*dptr;
							dptr++;
							loop=*dptr;
							dptr++;
							addr=*dptr;
							dptr+=6;
							if(panel==m_Record.panel && loop==m_Record.loop && addr==m_Record.addr)
							{
								tt=1;
								dptr-=8;
								break;
							}
							i++;
						}
					}
					if(tt==1)
					{
						//找到,将后边的往前移
						while(i<Total_Isolate)
						{
							for(j=0;j<8;j++)
							{
								panel=*(dptr+8);
								*dptr=panel;
								dptr++;
							}
							i++;
						}
						if(Total_Isolate>0)    Total_Isolate--;
						if(Total_Isolate==0)	m_deng[0] &=0x7F;  //清屏蔽灯
						dptr=(INT8U *)(flashbuf+2);
						*dptr=Total_Isolate;
						//写入
						Flash_Sector_Erase(30,30);	//清除屏蔽块
						num=Total_Isolate*8+8;	//需写入总数
						Auto_Add_Write(E_DisableAddr,(INT8U *)flashbuf,num);
						DisplayTotal(5);	//刷新屏蔽总数
						HandleDeviceState(m_Record.loop,m_Record.addr,STATUS_SHIELD,0);
						//写入CRT实时发送区
						DisableList[m_pWriteDis].sign=0x92;	//解除屏蔽
						DisableList[m_pWriteDis].panel=m_Record.panel;
						DisableList[m_pWriteDis].loop=m_Record.loop;
						DisableList[m_pWriteDis].addr=m_Record.addr;
						DisableList[m_pWriteDis].type=m_Record.type;
						
						DisableList[m_pWriteDis].year=D_Year;
						DisableList[m_pWriteDis].month=D_Month;
						DisableList[m_pWriteDis].day=D_Day;
						DisableList[m_pWriteDis].hour=D_Hour;
						DisableList[m_pWriteDis].minute=D_Minute;
						DisableList[m_pWriteDis].second=D_Second;
						m_pWriteDis++;
						if(m_pWriteDis>99)	m_pWriteDis=0;
					}
					break;
				}
				}
	        }
		}

	}
}



void MainView_Deal() //主界面
{
    if(key==Key_Menu)  //进入主菜单
    {
		m_View=1;
		m_FirstView=0;
    }
    if(pFire>0 ||pRun>0 || pKFire>0)
	{
		m_View=90;
		m_FirstView=0;
		DisFireNum=0;
		DisRunNum=0;
		HaveDisplay=0;
		HaveDisplayRun=0;
		DisKrFireNum=0;
		HaveDisplayKrFire=0;
	}
	else if(pError>0)
	{
		m_View=91;
		m_FirstView=0;
	}
	else if(Total_Isolate>0)
	{
	    m_View=92;
        m_FirstView=0;
	}

    key=0;
  
}

//主菜单选择
void MainMenu_Deal()
{

    if(m_FirstView!=m_View)
	{
        m_FirstView=m_View;		
		DrawPicture1(1,0,25,800,455,0,25);
	}
	if(key==Key_Esc)
    {
		FirstPageView(); //首页显示
    }
    if(key==0x31 )
    {
        //配置查询
		m_View=3;
		m_FirstView=0;        
    }
    else if(key==0x32 )
    {
        //设置需要输入密码
        m_View1=2;
        m_View=81;
		m_FirstView=0;
    }
    else if(key==0x33)
    {
        //安装需要输入密码
        m_View1=5;
        m_View=81;
		m_FirstView=0;
    }
    else if(key==0x34 )
    {
        //测试需要输入密码
        m_View1=4;
        m_View=81;
		m_FirstView=0;
    }
	else if(key==0x35)
    {
        //系统需要输入密码
		m_View1=6;
		m_View=81;
		m_FirstView=0;        
    }
    else if(key==0x36)
    {
        //帮助
		m_View=7;
		m_FirstView=0;        
    }
    key=0;
}


//密码输入
void Code_Input()
{
	if(m_FirstView!=m_View)
	{

		m_FirstView=m_View;

		Windows(100,100,700,500,"输入密码");
		if(m_View1==2)
			OutChars(5,10,"请输入设置密码：",1);
		else if(m_View1==4)
			OutChars(5,10,"请输入测试密码：",1);
		else if(m_View1==5)
			OutChars(5,10,"请输入安装密码：",1);
		else if(m_View1==6)
			OutChars(5,10,"请输入系统密码：",1);
		else if(m_View1==8)
			OutChars(5,10,"请输入可燃气体复位密码：",1);
		else if(m_View1==9)
			OutChars(5,10,"请输入复位密码：",1);
		else if(m_View1==10)
			OutChars(5,10,"请输入自检密码：",1);
		else if(m_View1==7)
		{
			if(m_SoundStatus==3)
				OutChars(5,6,"当前声光已输出,请输入密码停止：",1);
			else		
				OutChars(5,10,"请输入声光启动密码：",1);
		}
		else if(m_View1>40 && m_View1<50)
		{
			OutChars(5,10,"请输入二级密码：",1);
		}
		OutOneChar(7,12,'_',1);
        m_Input[0]=0xFF;
        m_Input[1]=0xFF;
        m_Input[2]=0xFF;
        m_Input[3]=0xFF;
        m_Input[4]=0xFF;
        m_Input[5]=0xFF;
			
	}
    if(key==Key_Esc)
    {
		FirstPageView(); //首页显示
    }
    if(key==Key_BackSpace)
    {
        m_Input[0]=0xFF;
        m_Input[1]=0xFF;
        m_Input[2]=0xFF;
        m_Input[3]=0xFF;
        m_Input[4]=0xFF;
        m_Input[5]=0xFF;
        OutChars(7,12,"_     ",1);
    }
    if(key>0x29 && key<0x3A)
    {
        if(m_Input[0]==0xFF)
        {
            m_Input[0]=key;
            OutChars(7,12,"*_",1);
        }
        else if(m_Input[1]==0xFF)
        {
            m_Input[1]=key;
            OutChars(7,13,"*_",1);
        }
        else if(m_Input[2]==0xFF)
        {
            m_Input[2]=key;
			if(m_View1>20 && m_View1<29)
			{
				//需要输入6位
	            OutChars(7,14,"*_",1);
			}
			else
			{
	            OutOneChar(7,14,'*',1);
	            //判断密码
	            if(m_View1==2 || m_View1==4 || m_View1==5 || m_View1==6 || m_View1==61)
	            {
	               //设置、测试、联动页面
		            if((m_Input[0]==0x31 && m_Input[1]==0x31 && m_Input[2]==0x39) ||  (m_Input[0]==m_Code1[0] && m_Input[1]==m_Code1[1] && m_Input[2]==m_Code1[2]))
					{
	                    m_View=m_View1;
	                    m_FirstView=0;
	                }
					else
					{
						//输入错误
				        m_Input[0]=0xFF;
				        m_Input[1]=0xFF;
				        m_Input[2]=0xFF;
				        OutChars(7,12,"_  ",1);					
					}
				}
	            else if(m_View1==9 || m_View1==8)
	            {
		            if((m_Input[0]==0x31 && m_Input[1]==0x31 && m_Input[2]==0x39) ||  (m_Input[0]==m_Code1[0] && m_Input[1]==m_Code1[1] && m_Input[2]==m_Code1[2]))
					{	                
						if(m_View1==9)
							m_Reset();  //复位
						else
							m_ResetKrkzq();	//复位可燃气体
					}
					else
					{
						//输入错误
				        m_Input[0]=0xFF;
				        m_Input[1]=0xFF;
				        m_Input[2]=0xFF;
				        OutChars(7,12,"_  ",1);					
					}
	            }
				else if(m_View1==7)
				{
		            if((m_Input[0]==0x31 && m_Input[1]==0x31 && m_Input[2]==0x39) || (m_Input[0]==m_Code1[0] && m_Input[1]==m_Code1[1] && m_Input[2]==m_Code1[2]))
					{
						if(m_SoundStatus==3)
						{
							m_SoundStatus=0;	//声光停止
							m_deng[1] &=0xFE;
							m_CanCommand.sign=0x01;  //停止声光命令
							m_CanCommand.loop=0x0A;
							m_CanCommand.addr=0;
							m_CanCommand.type=0;
							WriteCanCommand();
						}
						else if(m_SoundStatus==0)
						{
							m_SoundStatus=3;	//声光启动
							m_deng[1] |=0x01;
							m_CanCommand.sign=0x01;  //启动声光命令
							m_CanCommand.loop=0x09;
							m_CanCommand.addr=0;
							m_CanCommand.type=0;
							WriteCanCommand();
						}
						FirstPageView(); //首页显示
					}
					else
					{
						//输入错误
				        m_Input[0]=0xFF;
				        m_Input[1]=0xFF;
				        m_Input[2]=0xFF;
				        OutChars(7,12,"_  ",1);					
	
					}
	            }
			}
        }
		else if(m_Input[3]==0xFF)
        {
            m_Input[3]=key;
            OutChars(7,15,"*_",1);
        }
		else if(m_Input[4]==0xFF)
        {
            m_Input[4]=key;
            OutChars(7,16,"*_",1);
        }
		else if(m_Input[5]==0xFF)
        {
            m_Input[5]=key;
	        OutOneChar(7,17,'*',1);
	        if(m_View1>40 && m_View1<50)
			{
					//设置的二级密码
		            if((m_Input[0]==m_Code[0] && m_Input[1]==m_Code[1] && m_Input[2]==m_Code[2] && m_Input[3]==m_Code[3] && m_Input[4]==m_Code[4] && m_Input[5]==m_Code[5]) 
					|| (m_Input[0]==0x30 && m_Input[1]==0x30 && m_Input[2]==0x30 && m_Input[3]==0x30 && m_Input[4]==0x30 && m_Input[5]==0x30 ))
					{
	                    m_View=m_View1;
	                    m_FirstView=0;
	                }
					else
					{
						//输入错误
				        m_Input[0]=0xFF;
				        m_Input[1]=0xFF;
				        m_Input[2]=0xFF;
				        m_Input[3]=0xFF;
				        m_Input[4]=0xFF;
				        m_Input[5]=0xFF;
				        OutChars(7,12,"_     ",1);					
					}
			}
        }

    }
    key=0;   
}

//设置页面
void SetView_Deal()
{
    if(m_FirstView!=m_View)
	{

		m_FirstView=m_View;
		DrawPicture1(3,0,25,800,455,0,25);
		//1.手动启停设备
		//2.部件屏蔽
		//3.设置时间
		//4.设置联动编程
		//5.设置汉字注释

	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=1;
			m_FirstView=0;
		}
		break;
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		{
	        //不需要输入二级密码
			m_View=11+key-0x31;
			m_FirstView=0;
		}
		break;
    }
    key=0;
}

//安装页面
void InstallView_Deal()
{
    if(m_FirstView!=m_View)
	{

		m_FirstView=m_View;
		DrawPicture1(4,0,25,800,455,0,25);
		//1.部件自动登记
		//2.部件手动登记
		//3.开关打印机
		//4.灭火区延时时间
		//5.声光广播轮响时间

	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=1;
			m_FirstView=0;
		}
		break;
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		{
	        //不需要输入二级密码
			m_View=21+key-0x31;
			m_FirstView=0;
		}
		break;
    }
    key=0;
}

//测试页面
void TestView_Deal()
{
    if(m_FirstView!=m_View)
	{

		m_FirstView=m_View;
		DrawPicture1(5,0,25,800,455,0,25);
		//1.探测模拟曲线
		//2.回路信号浏览
		//3.部件数据查询
		//4.电池状态查询
		//5.可燃气体信息	//运行模式选择

	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=1;
			m_FirstView=0;
		}
		break;
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		{
	        //不需要输入二级密码
			m_View=62+key-0x31;
			m_FirstView=0;
		}
		break;
    }
    key=0;	
}

//系统页面
void SysView_Deal()
{
    if(m_FirstView!=m_View)
	{
		
		m_FirstView=m_View;
		DrawPicture1(6,0,25,800,455,0,25);
		//1.系统配置
		//2.组网配置
		//3.清除处理
		//4.设置一级密码
		//5.设置二级密码
		//6.USB文件读写		

	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=1;
			m_FirstView=0;
		}
		break;
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		{
	        ///需要输入二级密码
			m_View1=41+key-0x31;
			m_View=81;
			m_FirstView=0;
		}
		break;
    }
    key=0;		
}

//帮助页面
void Help()
{
    if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
		DrawPicture1(7,0,25,800,455,0,25);
	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=1;
			m_FirstView=0;
		}
		break;
    }
    key=0;	
}

//设置时钟
void SetClock() 
{
	uint8 i;
    if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
        Windows(100,100,700,500,"设置时钟");
		OutChars(5,10,"请依次输入当前时间。",1);
		OutChars(7,9,"年:",1);
		OutChars(7,22,"月:",1);
		OutChars(7,35,"日:",1);
		OutChars(9,9,"时:",1);
		OutChars(9,22,"分:",1);
		OutChars(9,35,"秒:",1);

		OutOneChar(7,14,'_',1);
				
		m_Cursor=14; //光标位置
		m_Cursor1=7;
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=2;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	}
	if(key>0x29 && key<0x3A && m_Cursor<44 )
	{
		//有键按下
		OutOneChar(m_Cursor1,m_Cursor,key,1);
		switch(m_Cursor)
		{
			case 14:
			case 27:
			case 40:
			{
				m_Cursor++;
				OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
				m_Input[0]=key-0x30;
			}
			break;
			case 15:
			{
				if(m_Cursor1==7)
				{
					//年
					i= m_Input[0]*10+key-0x30;
					if(i<99)
					{
						m_Cursor=27;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
						m_Input[4]=i;
			
					}
					else
					{
						OutOneChar(m_Cursor1,m_Cursor,' ',1); //空格
						m_Cursor--;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
					}
				}
				else
				{
					//时
					i= m_Input[0]*10+key-0x30;
					if(i<24)
					{
						m_Cursor=27;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
						m_Input[7]=i;
					}
					else
					{
						OutOneChar(m_Cursor1,m_Cursor,' ',1); //空格
						m_Cursor--;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
					}
				}

			}
			break;
			case 28:
			{
				if(m_Cursor1==7)
				{
					//月
					i= m_Input[0]*10+key-0x30;
					if(i!=0 && i<13)
					{
						m_Cursor=40;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
						m_Input[5]=i;
					}
					else
					{
						OutOneChar(m_Cursor1,m_Cursor,' ',1); //空格
						m_Cursor--;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
					}
				}
				else
				{
					//分
					i= m_Input[0]*10+key-0x30;
					if(i<61)
					{
						m_Cursor=40;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
						m_Input[8]=i;
					}
					else
					{
						OutOneChar(m_Cursor1,m_Cursor,' ',1); //空格
						m_Cursor--;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
					}
				}
			}
			break;
			case 41:
			{
				if(m_Cursor1==7)
				{
					//日
					i= m_Input[0]*10+key-0x30;
					if(i!=0 && i<32)
					{
						m_Cursor=14;
						m_Cursor1=9;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
						m_Input[6]=i;
					}
					else
					{
						OutOneChar(m_Cursor1,m_Cursor,' ',1); //空格
						m_Cursor--;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
					}
				}
				else
				{
					//秒
					i= m_Input[0]*10+key-0x30;
					if(i<61)
					{
						m_Cursor=54;
						m_Input[9]=i;
								
						D_Year=m_Input[4];
						D_Month=m_Input[5];
						D_Day=m_Input[6];
						D_Hour=m_Input[7];
						D_Minute=m_Input[8];
						D_Second=m_Input[9];

						rtc_setup(); 
						
						if(m_PanelType)
						{
							//主机需发送时间同步命令
							bTimeSync=1;
						}							
						m_Record.sign=0xA0;		//特殊事件
						m_Record.panel=m_PanelAddr;
						m_Record.confirm=131;	//调整时钟
						WriteToHxzZone();	//写黑匣子发送区
						OutChars(12,20,"设置已完成。",1);
					}
					else
					{
						OutOneChar(m_Cursor1,m_Cursor,' ',1); //空格
						m_Cursor--;
					    OutOneChar(m_Cursor1,m_Cursor,'_',1); //光标
					}				
				}

			}
			break;

		}

	}
	key=0;	

}

//自动登记
void AutoRegister()
{
	uint16 i,j,num;
	uint8 panel,k;
	INT8U *dptr;
	INT8U *dptr1;
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
        Windows(100,100,700,500,"自动登记");
		OutChars(9,20,"请按确认键开始登记。",1);
		isRegister=0;
	}
	if(isRegister==0x81)
	{
		isRegister=0x80;
		if(RegisterTimes>0)
		{ 
			RegisterTimes--;
			OutTwoChar(9,30,RegisterTimes,1);
		}
		else
		{
			OutChars(9,19,"登记已完成！               ",1);
			isRegister=0;
		}
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=5;
		m_FirstView=0;
		isRegister=0;
		m_Reset();  //自动登记后复位
	}
	break;
	case Key_Enter:  
	{
		if(isRegister==0)
		{
		    isRegister=0x80;
			RegisterTimes=61;
		    //发送自动登记命令
		    m_CanCommand.sign=0x01;  //自动登记
			m_CanCommand.loop=0x02;
			m_CanCommand.addr=0; 
			m_CanCommand.type=0;
			WriteCanCommand();

			OutChars(9,19,"命令已发出,60秒后登记完成！",1);
			//清除屏蔽
            //首先读出全部数据，查找是否本机的屏蔽，如果是则删除
			num=Total_Isolate*8+8;	//需读出总数
			SPI_FLASH_HighSpeedReadCont(E_DisableAddr,num,(INT8U *)flashbuf);
			dptr=(INT8U *)(flashbuf+8);
			dptr1=(INT8U *)(flashbuf+8);
			i=0;
			j=0;
            while(i<Total_Isolate)
            {
                panel=*dptr;
				if(panel!=0)
                {
					//需要移动
	                for(k=0;k<8;k++)
					{
						panel=*dptr;
						*dptr1=panel;
						dptr++;
						dptr1++;
					}
					j++;
                }
				else
				{
					dptr+=8;
                }
				i++;
            }
			Total_Isolate=j;
            if(Total_Isolate==0)	m_deng[0] &=0x7F;  //清屏蔽灯
			dptr=(INT8U *)(flashbuf+2);
			*dptr=Total_Isolate;
            //写入
			Flash_Sector_Erase(30,30);	//清除屏蔽块
			num=Total_Isolate*8+8;	//需写入总数
			Auto_Add_Write(E_DisableAddr,(INT8U *)flashbuf,num);
			DisplayTotal(5);	//刷新屏蔽总数
		}				
	}
	break;
	}
	key=0;	
		
}


//手动登记
void ManualRegister()
{
	uint16 loop,addr,type;
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
        Windows(100,100,700,500,"手动登记");
		OutChars(4,10,"请输入3位回路,3位地址及2位类型,类型信息为:",0);
		OutChars(5,14,"01光电感烟 02电子感温 03烟温复合 04手动按钮 ",0);
		OutChars(6,14,"10消火栓钮 11输入模块 12输出模块 13输入输出",0);
		OutChars(7,14,"14显示盘   15声光警报 20短路隔离 21紧急启停按钮",0);
		OutChars(8,14,"22手自动   25CO探测器 26H2探测器",0);
		OutChars(9,10,"向上键为登记,向下键为解除。",0);
		OutChars(12,10,"回路:",1);
		OutChars(12,24,"地址:",1);
		OutChars(12,38,"类型:",1);
		
		OutOneChar(12,17,'_',1);
		m_Cursor=17; //光标位置
		m_Return=0;
	}
	if(m_Return==0x80)   //命令反馈标志
    {        
        m_Return=0x00;
        if(m_CanReturn.sign==0x99) //已登记 
        {
			OutChars(10,4,"    地址已登记!        ",1);
        }
        else if(m_CanReturn.sign==0x9A) //已解除
        {
			OutChars(10,4,"    已解除登记!        ",1);
        } 
    }
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=5;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	case Key_Up:  // 登记
	{
		if(m_Cursor==52-4)
		{
			m_CanCommand.sign=0x04;  //手动登记
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=m_Input[5];
			m_CanCommand.type=m_Input[6];
			WriteCanCommand();
			OutChars(10,4,"登记命令已发出!    ",1);
		}
	}
	break;
	case Key_Down:  //解除登记
	{
		if(m_Cursor==50-4 || m_Cursor==49-4 || m_Cursor==52-4 )
		{
			m_CanCommand.sign=0x05;  //手动解除登记
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=m_Input[5];
			m_CanCommand.type=0;
			WriteCanCommand();
			OutChars(10,4,"解除登记命令已发出!",1);				
		}		
	}
	break;  
	}
	if(key>0x29 && key<0x3A && m_Cursor<52-4 )
	{
		//有键按下
		switch(m_Cursor)
		{
		    case 17:
		    case 18:
		    case 31:
		    case 32:
			case 45:
		    {
				OutOneChar(12,m_Cursor,key,1);
				if(m_Cursor==45)   //类型第一位
					m_Input[6]=key-0x30;
			    else if(m_Cursor==31 ||m_Cursor==32 )
					m_Input[m_Cursor-28]=key-0x30;	
				else
					m_Input[m_Cursor-17]=key-0x30;	
				m_Cursor++;
				OutOneChar(12,m_Cursor,'_',1);	 //光标
			    
		    }
		    break;
		    case 19:
		    {
			    OutOneChar(12,m_Cursor,key,1);
				//回路

					loop=m_Input[0]*100+m_Input[1]*10+key-0x30;
			    if(loop>0 && loop<33)	 
			    {
			        m_Input[2]=loop;
			        m_Cursor=31;
			  		OutOneChar(12,m_Cursor,'_',1);	 //光标
			    }
				else
				{
					//输入错误
					OutChars(12,17,"_  ",1);
					m_Cursor=17; //光标位置
				}
			}
			break;
		    case 33:
		    {
			    OutOneChar(12,m_Cursor,key,1);
				//地址
				addr=m_Input[3]*100+m_Input[4]*10+key-0x30;
				if(addr>0 && addr<221)
				{
					m_Input[5]=addr;
					m_Cursor=45;
					OutOneChar(12,m_Cursor,'_',1);	 //光标
				}
				else
				{
					//输入错误
					OutChars(12,31,"_  ",1);
					m_Cursor=31; //光标位置
				}
		    }
		    break;
		    case 46:	//类型第2位
		    {
				//类型
				OutOneChar(12,m_Cursor,key,1);
				type=m_Input[6]*10+key-0x30;
				if(type>0 && type<27)
				{
				    m_Input[6]=type;
			    	m_Cursor=48;
				}
				else
				{
					//输入错误
					OutChars(12,45,"_ ",1);
					m_Cursor=45; //光标位置	
				}
		    }
		    break;
		}	
	}
	key=0;
}


//地址屏蔽
void ManualIsolate()
{
	uint16 i,loop,addr;
	uint8 tt,panel,j;
	uint16 num,shu;
	INT8U *dptr;
	if(m_FirstView!=m_View)
	{
	    m_FirstView=m_View;
	    Windows(100,100,700,500,"地址屏蔽");
		OutChars(5,10,"请输入3位回路和3位地址",1);
		OutChars(6,10,"向上键为屏蔽，",1);
		OutChars(7,10,"向下键为解除屏蔽。",1);
		OutChars(9,14,"回路:",1);
		OutChars(9,28,"地址:",1);
	
		OutOneChar(9,21,'_',1);
		m_Cursor=21; //光标位置
    }
	if(m_Return==0x80)   //命令反馈标志
    {        
        m_Return=0x00;
        if(m_CanReturn.sign==0x98) //已屏蔽 
        {
			OutChars(12,20,"地址已屏蔽!    ",1);//add_test
            //首先读出全部数据，查找是否已存在，如果不存在则写入
			num=Total_Isolate*8+8;	//需读出总数
			SPI_FLASH_HighSpeedReadCont(E_DisableAddr,num,(INT8U *)flashbuf);
            tt=0;
			dptr=(INT8U *)(flashbuf+8);
            if(Total_Isolate>0)
            {
                for(i=0;i<Total_Isolate;i++)
                {
                    panel=*dptr;
					dptr++;
					loop=*dptr;
					dptr++;
					addr=*dptr;
					dptr+=6;
					if(panel==0 && loop==m_CanReturn.loop && addr==m_CanReturn.addr)
                    {
                        tt=1;
                        break;
                    }
                }
            }
            if(tt==0)
            {
                
                //写入记录
                *dptr=0;
				dptr++;
                *dptr=m_CanReturn.loop;
				dptr++;
                *dptr=m_CanReturn.addr;
				dptr++;
                *dptr=m_CanReturn.type;	//增加存储类型，月/日/时合成2个字节
				dptr++;
                *dptr=D_Year;
				dptr++;
				shu=hebingtime();
                *dptr=(shu & 0xFF00)>>8;
				dptr++;
                *dptr=shu & 0xFF;
				dptr++;
                *dptr=D_Minute;
                if(Total_Isolate<255)
                    Total_Isolate++;
				dptr=(INT8U *)(flashbuf+2);
				*dptr=Total_Isolate;
                m_deng[0] |=0x80;	//屏蔽灯
                //写入
				Flash_Sector_Erase(30,30);	//清除屏蔽块
				num=Total_Isolate*8+8;	//需写入总数
				Auto_Add_Write(E_DisableAddr,(INT8U *)flashbuf,num);
				DisplayTotal(5);	//刷新屏蔽总数
				HandleDeviceState(m_CanReturn.loop,m_CanReturn.addr,STATUS_SHIELD,1);
				if(bHaveSendDis)
				{
					//写入CRT实时发送区
					DisableList[m_pWriteDis].sign=0x91;	//屏蔽
	        		DisableList[m_pWriteDis].panel=0;
	        		DisableList[m_pWriteDis].loop=m_CanReturn.loop;
	        		DisableList[m_pWriteDis].addr=m_CanReturn.addr;
	        		DisableList[m_pWriteDis].type=m_CanReturn.type;
	
	        		DisableList[m_pWriteDis].year=D_Year;
	        		DisableList[m_pWriteDis].month=D_Month;
	        		DisableList[m_pWriteDis].day=D_Day;
	        		DisableList[m_pWriteDis].hour=D_Hour;
	        		DisableList[m_pWriteDis].minute=D_Minute;
					DisableList[m_pWriteDis].second=D_Second;
	        		m_pWriteDis++;
					if(m_pWriteDis>99)	m_pWriteDis=0;
					m_SendDisNum=Total_Isolate;
				}
				//写入外部CAN
				m_Record.sign=0x91;	//屏蔽
				m_Record.loop=m_CanReturn.loop;
				m_Record.addr=m_CanReturn.addr;
				m_Record.type=m_CanReturn.type;
				WriteToWCANZone(); //写入外部CAN发送区
				WriteToHxzZone();	//写黑匣子发送区

            }
        }
        else if(m_CanReturn.sign==0x97) //已解除
        {
			OutChars(12,20,"屏蔽已解除!    ",1);
            //首先读出全部数据，查找是否已存在，如果存在则删除
			num=Total_Isolate*8+8;	//需读出总数
			SPI_FLASH_HighSpeedReadCont(E_DisableAddr,num,(INT8U *)flashbuf);
			dptr=(INT8U *)(flashbuf+8);
            tt=0;
            if(Total_Isolate>0)
            {
                i=0;
                while(i<Total_Isolate)
                {
                    panel=*dptr;
					dptr++;
					loop=*dptr;
					dptr++;
					addr=*dptr;
					dptr+=6;
					if(panel==0 && loop==m_CanReturn.loop && addr==m_CanReturn.addr)
                    {
                        tt=1;
						dptr-=8;
                        break;
                    }
                    i++;
                }
            }
            if(tt==1)
            {
                //找到,将后边的往前移
                while(i<Total_Isolate)
                {
                    for(j=0;j<8;j++)
					{
						panel=*(dptr+8);
						*dptr=panel;
						dptr++;
					}
                    i++;
                }
				if(Total_Isolate>0)    Total_Isolate--;
                if(Total_Isolate==0)	m_deng[0] &=0x7F;  //清屏蔽灯
				dptr=(INT8U *)(flashbuf+2);
				*dptr=Total_Isolate;
                //写入
				Flash_Sector_Erase(30,30);	//清除屏蔽块
				num=Total_Isolate*8+8;	//需写入总数
				Auto_Add_Write(E_DisableAddr,(INT8U *)flashbuf,num);
				DisplayTotal(5);	//刷新屏蔽总数
				HandleDeviceState(m_CanReturn.loop,m_CanReturn.addr,STATUS_SHIELD,0);
				//写入CRT实时发送区
				DisableList[m_pWriteDis].sign=0x92;	//解除屏蔽
	        	DisableList[m_pWriteDis].panel=0;
	        	DisableList[m_pWriteDis].loop=m_CanReturn.loop;
	        	DisableList[m_pWriteDis].addr=m_CanReturn.addr;
	        	DisableList[m_pWriteDis].type=m_CanReturn.type;
				
				DisableList[m_pWriteDis].year=D_Year;
	        	DisableList[m_pWriteDis].month=D_Month;
	        	DisableList[m_pWriteDis].day=D_Day;
	        	DisableList[m_pWriteDis].hour=D_Hour;
	        	DisableList[m_pWriteDis].minute=D_Minute;
				DisableList[m_pWriteDis].second=D_Second;
	        	m_pWriteDis++;
				if(m_pWriteDis>99)	m_pWriteDis=0;
				//写入外部CAN
				m_Record.sign=0x92;	//解除屏蔽
				m_Record.loop=m_CanReturn.loop;
				m_Record.addr=m_CanReturn.addr;
				m_Record.type=m_CanReturn.type;
				WriteToWCANZone(); //写入外部CAN发送区
				WriteToHxzZone();	//写黑匣子发送区

            }
            
        } 
    }

	switch(key)
	{
	case Key_Esc:
	{
		m_View=2;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	case Key_Up:  //屏蔽
	{
		if(m_Cursor==49)
		{
		    m_CanCommand.sign=0x06;  //屏蔽
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=m_Input[5];
			m_CanCommand.type=0;
			WriteCanCommand();
			OutChars(12,20,"命令已发出!",1);
		}
	}
	break;
	case Key_Down:  //解屏蔽
	{
		if(m_Cursor==49)
		{
		    m_CanCommand.sign=0x07;  //解除屏蔽
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=m_Input[5];
			m_CanCommand.type=0;
			WriteCanCommand();
			OutChars(12,20,"命令已发出!",1);
		}				
	}
	break;  
	}
	if(key>0x29 && key<0x3A && m_Cursor<50 )
	{
		//有键按下
		switch(m_Cursor)
		{
		    case 21:
		    case 22:
		    case 35:
		    case 36:
		    {
				OutOneChar(9,m_Cursor,key,1);
			    if(m_Cursor>23)
					m_Input[m_Cursor-32]=key-0x30;	
				else
					m_Input[m_Cursor-21]=key-0x30;	
				m_Cursor++;
				OutOneChar(9,m_Cursor,'_',1);	 //光标
			    
		    }
		    break;
		    case 23:
		    {
			    OutOneChar(9,m_Cursor,key,1);
				//回路
				loop=m_Input[0]*100+m_Input[1]*10+key-0x30;
			    if(loop>0 && loop<33)
			    {
			        m_Input[2]=loop;
			        m_Cursor=35;
			  		OutOneChar(9,m_Cursor,'_',1);	 //光标
			    }
				else
				{
					//输入错误
					OutChars(9,21,"_  ",1);
					m_Cursor=21; //光标位置
				}
			}
			break;
		    case 37:
		    {
			    OutOneChar(9,m_Cursor,key,1);
				//地址
				addr=m_Input[3]*100+m_Input[4]*10+key-0x30;
			    if(addr>0 && addr<201)
			    {
			        m_Input[5]=addr;
			        m_Cursor=49;
			    }
				else
				{
					//输入错误
					OutChars(9,35,"_  ",1);
					m_Cursor=35; //光标位置
				}
		    }
		    break;
		}	
	}
	key=0;
}

//系统配置
void SetSystem()
{
    uint8 i,num;
    if(m_FirstView!=m_View)
	{
        m_FirstView=m_View;
        Windows(100,100,700,500,"系统配置");
		OutChars(3,8,"请输入系统配置参数，按确认键存储。",1);
		OutChars(5,8,"当前回路板总数  ,重设:",1);
		OutChars(7,8,"当前灭火盘总数  ,重设:",1);
		OutChars(9,8,"当前多线盘总数  ,重设:",1);
		OutTwoChar(5,22,m_LoopBoardNum,1);
		OutTwoChar(7,22,m_MiehuoNum,1);
		OutTwoChar(9,22,m_DirectNum,1);

		OutOneChar(5,33,'_',1);
		m_Cursor=33; //光标位置
		m_Cursor1=5; //光标位置
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=6;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
		m_FirstView=0;
	}
	break;
	case Key_Enter:
	{
		if(m_Cursor==40)
		{
			m_LoopBoardNum=m_Input[2];
			m_MiehuoNum=m_Input[3];
			m_DirectNum=m_Input[4];
			//初始化各板
			for(i=0;i<32;i++)
				m_Board[i]=0;
			for(i=0;i<m_LoopBoardNum;i++)
				m_Board[i+1]=0x80;
			for(i=0;i<m_MiehuoNum;i++)
				m_Board[i+9]=0x80;
			for(i=0;i<m_DirectNum;i++)
				m_Board[i+17]=0x80;
			//写入SPI FALSH
			SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
			Flash_Sector_Erase(1,1);   //清空
			HistoryBuf[1]=m_LoopBoardNum;	//修改
			HistoryBuf[2]=m_MiehuoNum;
			HistoryBuf[3]=m_DirectNum;
			Auto_Add_Write(0x1000,HistoryBuf,32);	//写入

			OutTwoChar(5,22,m_LoopBoardNum,1);
			OutTwoChar(7,22,m_MiehuoNum,1);
			OutTwoChar(9,22,m_DirectNum,1);

			OutChars(12,10,"设置已完成!",1);
		}
	}
	break;
	}
	if(key>0x2F && key<0x3A && m_Cursor<40 )
	{
		if(m_Cursor==33)
		{
			 OutOneChar(m_Cursor1,33,key,1);
			 m_Input[0]=key-0x30;
			 m_Cursor++;
			 OutOneChar(m_Cursor1,34,'_',1);
		}
		else
		{
			OutOneChar(m_Cursor1,m_Cursor,key,1);
			num=m_Input[0]*10+key-0x30;
			if(m_Cursor1==5)
			{
				if(num<9)
				{
					m_Cursor1=7;
					m_Cursor=33;
					OutOneChar(7,33,'_',1);
					m_Input[2]=num;	
				}
				else
				{
					m_Cursor1=5;
					m_Cursor=33;
					OutChars(5,33,"_ ",1);					
				}
			}
			else if(m_Cursor1==7)
			{
				if(num<9)
				{
					m_Cursor1=9;
					m_Cursor=33;
					OutOneChar(9,33,'_',1);
					m_Input[3]=num;	
				}
				else
				{
					m_Cursor1=7;
					m_Cursor=33;
					OutChars(7,33,"_ ",1);					
				}
			}
			else
			{
				if(num<9)
				{
					m_Cursor=40;
					m_Input[4]=num;	
				}
				else
				{
					m_Cursor1=9;
					m_Cursor=33;
					OutChars(9,33,"_ ",1);					
				}				
			}
				
		}
	}
	key=0;

}



//网络配置
void SetNet()
{
    uint8 i,num;
    if(m_FirstView!=m_View)
	{
        m_FirstView=m_View;
        Windows(100,100,700,500,"网络配置");
		if(m_PanelType)
		{
			OutChars(3,8,"当前已设为主机,本机地址  ,带从机数  。",1);
			OutTwoChar(3,42,m_PanelNum,1);
		}
		else
		{
			OutChars(3,8,"当前已设为从机,本机地址  。",1);			
		}
		OutTwoChar(3,31,m_PanelAddr,1);
		OutChars(4,8,"重新设置,主机输入1,从机输入0:",1);
		OutChars(6,10,"主机从机:",1);
		OutChars(8,10,"本机地址:",1);
		OutChars(10,10,"从机数量:",1);
		OutOneChar(6,22,'_',1);
		m_Cursor=22; //光标位置
		m_Cursor1=6; //光标位置
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=6;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
		m_FirstView=0;
	}
	break;
	case Key_Enter:
	{
		if(m_Cursor==40)
		{
			m_PanelType=m_Input[2];
			m_PanelAddr=m_Input[3];
			m_PanelNum=m_Input[4];
			//写入SPI FALSH
			SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
			Flash_Sector_Erase(1,1);   //清空
			HistoryBuf[8]=m_PanelAddr;	//修改
			HistoryBuf[9]=m_PanelType;
			HistoryBuf[10]=m_PanelNum;
			Auto_Add_Write(0x1000,HistoryBuf,32);	//写入
			if(m_PanelType)
			{
				OutChars(3,8,"当前已设为主机,本机地址  ,带从机数  。",1);
				OutTwoChar(3,42,m_PanelNum,1);
			}
			else
			{
				OutChars(3,8,"当前已设为从机,本机地址  。           ",1);			
			}
			OutTwoChar(3,31,m_PanelAddr,1);
			//配置
			for(i=0;i<PANEL_NET_MAX;i++)
			{
				m_Panel[i]=0x00;	
			}
			if(m_PanelType)
			{
				//主机
				for(i=1;i<m_PanelNum+1;i++)
					m_Panel[i]=0x80;	
			}
			OutChars(12,10,"设置已完成!",1);
		}
	}
	break;
	}
	if(key>0x2F && key<0x3A && m_Cursor<40 )
	{
		if(m_Cursor==22)
		{
			 if(m_Cursor1==6)
			 {
			 	if(key==0x30 || key==0x31)
				{
					OutOneChar(m_Cursor1,22,key,1);
					m_Input[2]=key-0x30;
					m_Cursor1=8;
					OutOneChar(m_Cursor1,22,'_',1);
				}
			 }
			 else
			 {
				 OutOneChar(m_Cursor1,22,key,1);
				 m_Input[0]=key-0x30;
				 m_Cursor++;
				 OutOneChar(m_Cursor1,23,'_',1);
			}
		}
		else
		{
			OutOneChar(m_Cursor1,m_Cursor,key,1);
			num=m_Input[0]*10+key-0x30;
			if(m_Cursor1==8)
			{
				if(num<=PANEL_NET_MAX)
				{
					m_Cursor1=10;
					m_Cursor=22;
					OutOneChar(10,22,'_',1);
					m_Input[3]=num;	
				}
				else
				{
					m_Cursor1=8;
					m_Cursor=22;
					OutChars(8,22,"_ ",1);					
				}
			}
			else if(m_Cursor1==10)
			{
				if(num<=PANEL_SLAVE_MAX)
				{
					m_Input[4]=num;	
					m_Cursor=40;
				}
				else
				{
					m_Cursor1=10;
					m_Cursor=22;
					OutChars(10,22,"_ ",1);					
				}
			}
				
		}
	}
	key=0;

}


//打印机设置
void SetPrint()
{
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
        Windows(100,100,700,500,"开关打印机");
		OutChars(4,8,"设置打印机工作方式，",1);
		OutChars(5,8,"向上键为允许打印，",1);
		OutChars(6,8,"向下键为禁止。",1);
		OutChars(9,8,"当前打印机:",1);
		if(m_PrintSet>0)
		{
			OutChars(9,20,"允许打印",1);	
		}
		else
		{
			OutChars(9,20,"禁止打印",1);
		}
		OutOneChar(9,28,'_',1);
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=5;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	case Key_Up:  // 允许打印
	{
		//写入SPI FALSH
		SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
		Flash_Sector_Erase(1,1);   //清空
		HistoryBuf[5]=1;
		Auto_Add_Write(0x1000,HistoryBuf,32);	//写入
		OutChars(9,20,"允许打印",1);

		m_PrintSet=1;
		m_PrintInit=0; //打印机初始化标志
		pPrintFire=pFireHistory; 	//火警打印指针
		pPrintRun=pRunHistory;		//联动打印指针
		pPrintOther=pOtherHistory;	//其它打印指针
	}
	break;
	case Key_Down:  //禁止打印
	{
		m_PrintSet=0;
		//写入SPI FALSH
		SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
		Flash_Sector_Erase(1,1);   //清空
		HistoryBuf[5]=0;
		Auto_Add_Write(0x1000,HistoryBuf,32);	//写入
		OutChars(9,20,"禁止打印",1);
	}
	break;  
	}
	key=0;
}

//显示设备类型
void OutOneType(uint16 x,uint16 y,uint8 type)
{
	switch(type)
	{
		case 01:	TextOut(x,y,"光电感烟",0);	break;
		case 02:	TextOut(x,y,"电子感温",0);	break;
		case 03:	TextOut(x,y,"烟温复合",0);	break;
		case 04:	TextOut(x,y,"手动按钮",0);	break;
		
		case 10:	TextOut(x,y,"消火栓钮",0);	break;
		case 11:	TextOut(x,y,"输入模块",0);	break;
		case 12:	TextOut(x,y,"输出模块",0);	break;
		case 13:	TextOut(x,y,"输入输出",0);	break;
		
		case 14:	TextOut(x,y,"火灾显示盘",0);	break;
		case 15:	TextOut(x,y,"声光警报",0);	break;

		case 20:	TextOut(x,y,"短路隔离器",0);	break;
		case 21:	TextOut(x,y,"紧急启停",0);	break;
		case 22:	TextOut(x,y,"手自动模块",0);	break;

		case 25:	TextOut(x,y,"CO探测器",0);	break;		
		case 26:	TextOut(x,y,"H2探测器",0);	break;
					case 27:	TextOut(x,y,"可燃气体",0);	break;
					case 28:	TextOut(x,y,"H2探测器",0);	break;
					case 29:	TextOut(x,y,"H2探测器",0);	break;		
		
		default:	TextOut(x,y,"未知类型",0);	break;
	}
}

//显示多线故障类型
void OutOneErrorType(uint16 x,uint16 y,uint8 type)
{
	switch(type)
	{
		case 0x01:	TextOut(x,y,"启动开路",0);	break;
		case 0x02:	TextOut(x,y,"启动短路",0);	break;
		case 0x03:	TextOut(x,y,"反馈开路",0);	break;
		default:	TextOut(x,y,"故障",0);	break;
	}
}

//显示总线地址故障类型
void OutOneErrorType1(uint16 x,uint16 y,uint8 type,uint8 type1)
{
	switch(type1)
	{
		case 0x01:	TextOut(x,y,"通讯故障",0);	break;
		case 0x02:
			if(type==11 ||type==13)
				TextOut(x,y,"输入开路",0);
			else
				TextOut(x,y,"故障",0);	
			break;
		case 0x04:	
			if(type==1 || type==3)
				TextOut(x,y,"污染故障",0);	
			else
				TextOut(x,y,"输出故障",0);	
			break;			
		case 0x08:	TextOut(x,y,"24V故障",0);	
			break;
		default:
			TextOut(x,y,"故障",0);
			break;
	}
}


//显示一条联动
void OutOneRun(u8 jb)
{    
	u16 locatey;
	
	m_RunAuto=0;  //联动自动翻页
	if(jb==1)
	{
		Bcolor=RED;
		Fcolor=WHITE;
		SetColor(Fcolor,Bcolor);
	}
	else
	{
		Fcolor=BLUE;
		Bcolor=YELLOW;
		SetColor(Fcolor,Bcolor);
	}	
	
	locatey=(HaveDisplayRun-1)*75+53;

	OutOneChar1(400,locatey,'N',0);
	OutThreeChar1(412,locatey,DisRunNum,0);
	OutOneChar1(448,locatey,':',0);
	
	if(jb==1)
	{
		HistoryBuf[0]=RunList[DisRunNum-1].panel;
		HistoryBuf[1]=RunList[DisRunNum-1].loop;
		HistoryBuf[2]=RunList[DisRunNum-1].addr;
		OutAddr(470,locatey,HistoryBuf);	   //显示地址
		HistoryBuf[3]=RunList[DisRunNum-1].type;
		
		switch(RunList[DisRunNum-1].sign)
		{   
			case 0x83:	TextOut(640,locatey,"启动      ",0);	break;
			case 0x84:  TextOut(640,locatey,"停止      ",0);	break;
			case 0x85:  TextOut(640,locatey,"反馈      ",0);	break;
			case 0x86:  TextOut(640,locatey,"反馈撤销  ",0);	break;
			case 0x8A:  TextOut(640,locatey,"启动并反馈",0);	break;
			case 0x8B:  TextOut(640,locatey,"启动后停止",0);	break;
			case 0x8C:  TextOut(640,locatey,"反馈后撤销",0);	break;
			case 0x8D:  TextOut(640,locatey,"启动无反馈",0);	break;
		}
		locatey+=24;
		
		if(HistoryBuf[1]<33)
		{
			//总线地址显示类型
			OutOneType(640,locatey,RunList[DisRunNum-1].type & 0x1F);	//显示类型
		}
		//显示时间
		HistoryBuf[0]=RunList[DisRunNum-1].year;
		HistoryBuf[1]=RunList[DisRunNum-1].month;
		HistoryBuf[2]=RunList[DisRunNum-1].day;
		HistoryBuf[3]=RunList[DisRunNum-1].hour;
		HistoryBuf[4]=RunList[DisRunNum-1].minute;
		HistoryBuf[5]=RunList[DisRunNum-1].second;
		OutTime(400,locatey,HistoryBuf,0);   //显示时间

		if(RunList[DisRunNum-1].loop<16)
		{
			locatey+=24;  
			//位置:	
			OutOneNote(400,locatey,RunList[DisRunNum-1].panel,RunList[DisRunNum-1].loop,RunList[DisRunNum-1].addr);
		}
	}
	else
	{
		HistoryBuf[0]=LdErrList[DisRunNum-1].panel;
		HistoryBuf[1]=LdErrList[DisRunNum-1].loop;
		HistoryBuf[2]=LdErrList[DisRunNum-1].addr;
		OutAddr(470,locatey,HistoryBuf);	   //显示地址
		locatey+=24;
		if(LdErrList[DisRunNum-1].sign==0x81)
		{
			OutOneType(660,locatey-24,LdErrList[DisRunNum-1].type & 0x1F);	//显示类型
			OutOneErrorType1(660,locatey,LdErrList[DisRunNum-1].type & 0x1F ,LdErrList[DisRunNum-1].type1);//显示总线地址故障类型
		}
		else
			OutOneErrorType(660,locatey,LdErrList[DisRunNum-1].type1);	//显示故障类型
		
		//显示时间
		HistoryBuf[0]=LdErrList[DisRunNum-1].year;
		HistoryBuf[1]=LdErrList[DisRunNum-1].month;
		HistoryBuf[2]=LdErrList[DisRunNum-1].day;
		HistoryBuf[3]=LdErrList[DisRunNum-1].hour;
		HistoryBuf[4]=LdErrList[DisRunNum-1].minute;
		HistoryBuf[5]=LdErrList[DisRunNum-1].second;
		OutTime(400,locatey,HistoryBuf,0);   //显示时间		
	}
}



//显示一条火警
void OutOneFire(u16 no,u8 disno,u8 disno1,u8 jb)
{    
    u8 type;
	u16 locatey,num;
	u8 buf[6];
    
	m_FireAuto=0; //火警自动翻页
	if(jb==1)
	{
		if(disno==disno1)
		{	
			Fcolor=RED;
			Bcolor=WHITE;	
		}
		else
		{	
			Fcolor=WHITE;
			Bcolor=RED;	
		}
		SetColor(Fcolor,Bcolor);
		
		locatey=(disno-1)*75+53;

		//清显示区
		DrawRect(0,locatey,387,locatey+73,2);////////////////////////白色区域

		locatey++;

		if(no==1)
		{
			TextOut(0,locatey,"首警:",0);
		}
		else
		{	
			OutOneChar1(0,locatey,'N',0);
			OutThreeChar1(12,locatey,no,0);
			OutOneChar1(48,locatey,':',0);
		}
		
		HistoryBuf[0]=FireList[no-1].panel;
		HistoryBuf[1]=FireList[no-1].loop;
		HistoryBuf[2]=FireList[no-1].addr;	
		OutAddr(70,locatey,HistoryBuf);	   //显示地址
		
		type =FireList[no-1].type & 0x1F;
		OutOneType(260,locatey,type);	//显示类型
		
		locatey+=24;
		HistoryBuf[0]=FireList[no-1].year;
		HistoryBuf[1]=FireList[no-1].month;
		HistoryBuf[2]=FireList[no-1].day;
		HistoryBuf[3]=FireList[no-1].hour;
		HistoryBuf[4]=FireList[no-1].minute;
		HistoryBuf[5]=FireList[no-1].second;	
		OutTime(0,locatey,HistoryBuf,0);   //显示时间

		switch(FireList[no-1].confirm)
		{
		case 1:
			
			TextOut(250,locatey,"已确认真警",0);
			break;
		case 2:
			TextOut(250,locatey,"已确认误报",0);
			break;
		case 3:
			TextOut(250,locatey,"已确认调试",0);
			break;
		default:
			TextOut(250,locatey,"未确认火警",0);
			break;
		}
		
		locatey+=24;
		//位置:
		OutOneNote(0,locatey,FireList[no-1].panel,FireList[no-1].loop,FireList[no-1].addr);
		//TextOut(0,locatey,"测试一下汉字显示信息能否显示完全",0);
		if(disno==disno1)
		{
			Fcolor=WHITE;
			Bcolor=RED;	
			SetColor(Fcolor,Bcolor);
		}
	}
	else if(jb==2)
	{
		if(disno==disno1)
		{	
			Fcolor=YELLOW;
			Bcolor=BLUE;	
		}
		else
		{	
			Fcolor=BLUE;
			Bcolor=YELLOW;
		}
		SetColor(Fcolor,Bcolor);
//		Fcolor=BLUE;
//		Bcolor=YELLOW;
//		SetColor(Fcolor,Bcolor);	
		
		locatey=(disno-1)*75+53;
					
		//清显示区
		DrawRect(0,locatey,387,locatey+73,2);////////////////////////白色区域
		locatey++;
		
		OutOneChar1(0,locatey,'N',0);
		OutThreeChar1(12,locatey,no,0);
		OutOneChar1(48,locatey,':',0);

		//显示时间
		HistoryBuf[0]=ErrorList[no-1].year;
		HistoryBuf[1]=ErrorList[no-1].month;
		HistoryBuf[2]=ErrorList[no-1].day;
		HistoryBuf[3]=ErrorList[no-1].hour;
		HistoryBuf[4]=ErrorList[no-1].minute;
		HistoryBuf[5]=ErrorList[no-1].second;
		OutTime(0,locatey+24,HistoryBuf,0);   //显示时间		
		
		switch(ErrorList[no-1].confirm)
		{
		case 1:
			TextOut(260,locatey+24,"故障已处理",0);
			break;
		default:
			TextOut(260,locatey+24,"故障未处理",0);
			break;
		}
		
		switch(ErrorList[no-1].sign)
		{
			case 0x81: //地址故障
				HistoryBuf[0]=ErrorList[no-1].panel;
				HistoryBuf[1]=ErrorList[no-1].loop;
				HistoryBuf[2]=ErrorList[no-1].addr;
				OutAddr(60,locatey,HistoryBuf);	   //显示地址
				OutOneType(184,locatey,ErrorList[no-1].type & 0x1F);	//显示类型
				OutOneErrorType1(284,locatey,ErrorList[no-1].type & 0x1F ,ErrorList[no-1].type1);//显示总线地址故障类型
				locatey+=48;
				//位置:
				OutOneNote(0,locatey,ErrorList[no-1].panel,ErrorList[no-1].loop,ErrorList[no-1].addr);	
				break;
			case 0x87: //回路故障
			{
				if(ErrorList[no-1].panel==0)
				{
					OutThreeChar1(70,locatey,ErrorList[no-1].loop,0);
					TextOut(106,locatey,"回路故障",0);					
				}
				else
				{
					TextOut(94,locatey,"-   回路故障",0);						
					OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
					OutThreeChar1(106,locatey,ErrorList[no-1].loop,0);
				}
			}
			break;
			case 0x89: //其它故障
			{
				if(ErrorList[no-1].loop==1)
				{
					//板故障
					num= ErrorList[no-1].addr;
					if(ErrorList[no-1].panel==0)
					{
						OutTwoChar1(70,locatey,num,0);
						TextOut(94,locatey,"号回路板故障",0);
					}
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						OutOneChar1(94,locatey,'-',0);
						OutTwoChar1(106,locatey,num,0);
						TextOut(130,locatey,"号回路板故障",0);
					}
				}
				else if(ErrorList[no-1].loop==2)
				{
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"电源通讯故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机电源通讯故障",0);
					}
				}
				else if(ErrorList[no-1].loop==3)
				{
					//主电故障
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"主电故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机主电故障",0);
					}
				}
				else if(ErrorList[no-1].loop==4)
				{
					//备电故障
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"备电故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机备电故障",0);
					}
				}
				else if(ErrorList[no-1].loop==5)
				{
					//声光开路故障
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"声光开路",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机声光开路",0);
					}
				}
				else if(ErrorList[no-1].loop==6)
				{
					//声光短路故障
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"声光短路",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机声光短路",0);
					}
				}
				else if(ErrorList[no-1].loop==7)
				{
					//机器故障
					OutTwoChar1(70,locatey,ErrorList[no-1].addr,0);
					TextOut(94,locatey,"号机器故障",0);
				}
				else if(ErrorList[no-1].loop==8)
				{
					//欠压故障
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"备电欠压故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机备电欠压",0);
					}
				}
				else if(ErrorList[no-1].loop==9)
				{
					//电源输出故障
					if(ErrorList[no-1].addr==2)
					{
						if(ErrorList[no-1].panel==0)
							TextOut(70,locatey,"联动电源输出短路",0);
						else
						{
							OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
							TextOut(94,locatey,"号机联动电源输出短路",0);
						}
					}
					else
					{
						if(ErrorList[no-1].panel==0)
							TextOut(70,locatey,"电源输出1故障",0);
						else
						{
							OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
							TextOut(94,locatey,"号机电源输出1故障",0);
						}
					}
				}
				else if(ErrorList[no-1].loop==0x0A)
				{
					//运行数据存储单元故障
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"运行数据存储单元故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机运行数据存储单元故障",0);
					}
				}
				else if(ErrorList[no-1].loop==0x0C)
				{
					//电池低压故障
					if(ErrorList[no-1].panel==0)
						TextOut(70,locatey,"电池低压故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机电池低压",0);
					}
				}
			}
			break;			
		}			
	}
	else if(jb==3)
	{
		Fcolor=WHITE;
		Bcolor=BLACK;
		SetColor(Fcolor,Bcolor);		
		locatey=(disno-1)*75+54;
					
		OutOneChar1(0,locatey,'N',0);
		OutThreeChar1(12,locatey,no,0);
		OutOneChar1(48,locatey,':',0);
		
		//屏蔽需要倒序显示
		num= Total_Isolate + 1 - no;
		//读一条屏蔽
		SPI_FLASH_HighSpeedReadCont(E_DisableAddr+num*8,8,HistoryBuf);	
		OutAddr(70,locatey,HistoryBuf);	   //显示地址
		OutOneType(260,locatey,HistoryBuf[3] & 0x1F);	//显示类型

		locatey+=24;		
		//显示时间
		buf[0]=HistoryBuf[4];	//年
		num=(u16) (HistoryBuf[5]<<8) | HistoryBuf[6];
		buf[1]=(num & 0xF000)>>12;  //月
		buf[2]=(num & 0x0FC0)>>6;  	//日
		buf[3]=num & 0x003F; 		//时
		buf[4]=HistoryBuf[7];
		buf[5]=0;
		OutTime(0,locatey,buf,1);   //显示时间
		TextOut(260,locatey,"屏蔽",0);
		
	}
}



//显示一条可燃气体
void OutOneKrFire(u8 jb)
{    
	u16 locatey;
	u16 lel;
	u8 num;
	
	m_KrFireAuto=0;  
	if(jb==1)
	{
		Bcolor=RED;
		Fcolor=WHITE;
		SetColor(Fcolor,Bcolor);
	}
	else
	{
		Fcolor=BLUE;
		Bcolor=YELLOW;
		SetColor(Fcolor,Bcolor);
	}
	
	locatey=(HaveDisplayKrFire-1)*75+307;

	OutOneChar1(0,locatey,'N',0);
	OutThreeChar1(12,locatey,DisKrFireNum,0);
	OutOneChar1(48,locatey,':',0);
	
	if(jb==1)
	{
		if(DisKrFireNum==1)
		{
			TextOut(0,locatey,"首警:",0);
		}
		else
		{
			OutOneChar1(0,locatey,'N',0);
			OutThreeChar1(12,locatey,DisKrFireNum,0);
			OutOneChar1(48,locatey,':',0);
		}
		
		HistoryBuf[0]=KFireList[DisKrFireNum-1].panel;
		HistoryBuf[1]=KFireList[DisKrFireNum-1].loop;
		HistoryBuf[2]=KFireList[DisKrFireNum-1].addr;
		OutAddr(70,locatey,HistoryBuf);	   //显示地址
		HistoryBuf[3]=KFireList[DisKrFireNum-1].type;
		lel =  (uint16)KFireList[DisKrFireNum-1].type1 <<8;
		lel |=KFireList[DisKrFireNum-1].type2;
		lel &=0x3FFF;
		if(lel==0 && KFireList[DisKrFireNum-1].loop==3)
		{
			lel=GetKeRanQiUploadConcentration(KFireList[DisKrFireNum-1].addr);
			if(lel>0)
			{
				KFireList[DisKrFireNum-1].type1=(uint8)(lel>>8);
				KFireList[DisKrFireNum-1].type2=(uint8)lel;
			}
		}
		if(HistoryBuf[3]==25)
		{
			TextOut(260,locatey,"CO",0);
			num=lel/100;
			if(num>9)
				OutTwoChar1(250,locatey+24,num,0);
			else
				OutOneChar1(262,locatey+24,num+0x30,0);
			OutTwoChar1(274,locatey+24,lel%100,0);
			TextOut(298,locatey+24,"ppm",0);
		}
		else if(HistoryBuf[3]==26)
		{
			TextOut(260,locatey,"H2",0);
			num=lel/100;
			if(num>1)
				num=1;
			OutOneChar1(262,locatey+24,num+0x30,0);
			OutTwoChar1(274,locatey+24,lel%100,0);
			TextOut(298,locatey+24,"%LEL",0);
		}
		
		if(KFireList[DisKrFireNum-1].sign==0x7A)
			TextOut(284,locatey,"低限报警",0);
		else if(KFireList[DisKrFireNum-1].sign==0x7B)
			TextOut(284,locatey,"高限报警",0);	
			
		locatey+=24;
		//显示时间
		HistoryBuf[0]=KFireList[DisKrFireNum-1].year;
		HistoryBuf[1]=KFireList[DisKrFireNum-1].month;
		HistoryBuf[2]=KFireList[DisKrFireNum-1].day;
		HistoryBuf[3]=KFireList[DisKrFireNum-1].hour;
		HistoryBuf[4]=KFireList[DisKrFireNum-1].minute;
		HistoryBuf[5]=KFireList[DisKrFireNum-1].second;
		OutTime(0,locatey,HistoryBuf,0);   //显示时间

		locatey+=24;  
		//位置:	
		OutOneNote(0,locatey,KFireList[DisKrFireNum-1].panel,KFireList[DisKrFireNum-1].loop,KFireList[DisKrFireNum-1].addr);
	}
	else
	{
		OutOneChar1(0,locatey,'N',0);
		OutThreeChar1(12,locatey,DisKrFireNum,0);
		OutOneChar1(48,locatey,':',0);

		HistoryBuf[0]=KrErrList[DisKrFireNum-1].panel;
		HistoryBuf[1]=KrErrList[DisKrFireNum-1].loop;
		HistoryBuf[2]=KrErrList[DisKrFireNum-1].addr;
		OutAddr(70,locatey,HistoryBuf);	   //显示地址
		OutOneType(260,locatey,KrErrList[DisKrFireNum-1].type & 0x1F);		//显示类型

		locatey+=24;
		//显示时间
		HistoryBuf[0]=KrErrList[DisKrFireNum-1].year;
		HistoryBuf[1]=KrErrList[DisKrFireNum-1].month;
		HistoryBuf[2]=KrErrList[DisKrFireNum-1].day;
		HistoryBuf[3]=KrErrList[DisKrFireNum-1].hour;
		HistoryBuf[4]=KrErrList[DisKrFireNum-1].minute;
		HistoryBuf[5]=KrErrList[DisKrFireNum-1].second;
		OutTime(0,locatey,HistoryBuf,0);   //显示时间
		OutOneErrorType1(260,locatey,KrErrList[DisKrFireNum-1].type & 0x1F ,KrErrList[DisKrFireNum-1].type1);	//显示故障类型

		locatey+=24;  
		//位置:	
		OutOneNote(0,locatey,KrErrList[DisKrFireNum-1].panel,KrErrList[DisKrFireNum-1].loop,KrErrList[DisKrFireNum-1].addr);
	}
}

void DisFireZone()
{
	if(pFire>0)
	{
		if(hjzonetype==1)
		{
			//显示后续火警
			if(pFire>DisFireNum)
			{   
				if(HaveDisplay<3)
				{
					DisFireNum++;
					HaveDisplay++;
					if(HaveDisplay==1)
					{
						CurDisplay=HaveDisplay;
						CurDisNum=DisFireNum;
					}
					OutOneFire(DisFireNum,HaveDisplay,CurDisplay,1);
					bDisplaying=1;
				}
				else
					bDisplaying=0;
			}
			else
				bDisplaying=0;
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(266,26,"(报警)",0);
			hjzonetype=1;				
			Qxsq(0);
			DisFireNum=1;
			HaveDisplay=1;
			CurDisplay=HaveDisplay;
			CurDisNum=DisFireNum;
			OutOneFire(DisFireNum,HaveDisplay,CurDisplay,1);
		}
	}
	else if(pError>0)
	{
		if(hjzonetype==2)
		{
			//显示后续火警区故障
			if(pError>DisFireNum)
			{   
				if(HaveDisplay<3)
				{
					DisFireNum++;
					HaveDisplay++;
					if(HaveDisplay==1)
					{
						CurDisplay=HaveDisplay;
						CurDisNum=DisFireNum;
					}
					OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
					bDisplaying=1;
				}
				else
					bDisplaying=0;
			}
			else
				bDisplaying=0;
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(266,26,"(故障)",0);
			hjzonetype=2;				
			Qxsq(0);
			DisFireNum=1;
			HaveDisplay=1;
			CurDisplay=HaveDisplay;
			CurDisNum=DisFireNum;
			OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
		}
	}
	else if(Total_Isolate>0)
	{
		if(hjzonetype==3)
		{
			//显示后续火警区屏蔽
			if(Total_Isolate>DisFireNum)
			{   
				if(HaveDisplay<3)
				{
					DisFireNum++;
					HaveDisplay++;
					OutOneFire(DisFireNum,HaveDisplay,CurDisplay,3);
				}
			}
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(266,26,"(屏蔽)",0);
			hjzonetype=3;					
			Qxsq(0);
			DisFireNum=1;
			HaveDisplay=1;
			OutOneFire(DisFireNum,HaveDisplay,CurDisplay,3);
		}
	}
	else
	{
		DisFireNum=0;
		HaveDisplay=0;
		CurDisplay=HaveDisplay;
		CurDisNum=DisFireNum;	
		hjzonetype=0;				
	}	

}

void DisKrFireZone()
{
	if(pKFire>0)
	{
		if(krzonetype==1)
		{
			//显示后续可燃气体报警
			if(pKFire>DisKrFireNum )
			{
				if(HaveDisplayKrFire<2)
				{
					DisKrFireNum++;
					HaveDisplayKrFire++;
					OutOneKrFire(1);
				}
			}	
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(266,280,"(报警)",0);
			krzonetype=1;
			Qxsq(2);
			DisKrFireNum=1;
			HaveDisplayKrFire=1;
			OutOneKrFire(1);
		}
	}
	else if(pkrError>0)
	{
		if(krzonetype==2)
		{
			//显示后续可燃气体故障
			if(pkrError>DisKrFireNum )
			{
				if(HaveDisplayKrFire<2)
				{
					DisKrFireNum++;
					HaveDisplayKrFire++;
					OutOneKrFire(2);
				}
			}	
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(266,280,"(故障)",0);
			krzonetype=2;
			Qxsq(2);
			DisKrFireNum=1;
			HaveDisplayKrFire=1;
			OutOneKrFire(2);
		}		
	}
	else
	{
		DisKrFireNum=0;
		HaveDisplayKrFire=0;	
		krzonetype=0;		
	}	
}

//显示一条灭火信息
void OutOneMh(u8 jb)
{    
	u16 locatey;
	
	m_mhRunAuto=0;
	if(jb==1)
	{
		Bcolor=RED;
		Fcolor=WHITE;
		SetColor(Fcolor,Bcolor);
	}
	else
	{
		Fcolor=BLUE;
		Bcolor=YELLOW;
		SetColor(Fcolor,Bcolor);
	}
	
	locatey=(HaveDisplayMh-1)*75+307;

	OutOneChar1(400,locatey,'N',0);
	OutThreeChar1(412,locatey,DisMhNum,0);
	OutOneChar1(448,locatey,':',0);
	
	if(jb==1)
	{
		HistoryBuf[0]=MhRunList[DisMhNum-1].panel;
		HistoryBuf[1]=MhRunList[DisMhNum-1].loop;
		HistoryBuf[2]=MhRunList[DisMhNum-1].addr;
		OutAddr(470,locatey,HistoryBuf);	   //显示地址
		HistoryBuf[3]=MhRunList[DisMhNum-1].type;
		switch(MhRunList[DisMhNum-1].sign)
		{   
			case 0x83:	TextOut(640,locatey,"启动      ",0);	break;
			case 0x84:  TextOut(640,locatey,"停止      ",0);	break;
			case 0x85:  TextOut(640,locatey,"反馈      ",0);	break;
			case 0x86:  TextOut(640,locatey,"反馈撤销  ",0);	break;
			case 0x8A:  TextOut(640,locatey,"启动并反馈",0);	break;
			case 0x8B:  TextOut(640,locatey,"启动后停止",0);	break;
			case 0x8C:  TextOut(640,locatey,"反馈后撤销",0);	break;
			case 0x8D:  TextOut(640,locatey,"启动无反馈",0);	break;
			case 0x8F:	//气体灭火类
			{
				switch(HistoryBuf[3] & 0x0F)
				{
				case 0x01:	TextOut(640,locatey,"气体喷洒启动",0);	break;
				case 0x02:	TextOut(640,locatey,"气体喷洒反馈",0);	break;
				case 0x03:	TextOut(640,locatey,"灭火现场急启",0);	break;
				case 0x04:	TextOut(640,locatey,"灭火面板急启",0);	break;
				case 0x05:	TextOut(640,locatey,"灭火急启断开",0);	break;
				case 0x06:	TextOut(640,locatey,"灭火现场急停",0);	break;
				case 0x07:	TextOut(640,locatey,"灭火面板急停",0);	break;
				case 0x08:	TextOut(640,locatey,"声光启动    ",0);	break;
				case 0x09:	TextOut(640,locatey,"声光停止    ",0);	break;
				case 0x0A:	TextOut(640,locatey,"放气指示输出",0);	break;
				case 0x0B:	TextOut(640,locatey,"灭火自动启动",0);	break;
				}
			}
			break;
		}
			
		locatey+=24;
		//显示时间
		HistoryBuf[0]=MhRunList[DisMhNum-1].year;
		HistoryBuf[1]=MhRunList[DisMhNum-1].month;
		HistoryBuf[2]=MhRunList[DisMhNum-1].day;
		HistoryBuf[3]=MhRunList[DisMhNum-1].hour;
		HistoryBuf[4]=MhRunList[DisMhNum-1].minute;
		HistoryBuf[5]=MhRunList[DisMhNum-1].second;
		OutTime(400,locatey,HistoryBuf,0);   //显示时间
		
		HistoryBuf[3]=MhRunList[DisMhNum-1].type;
		if(MhRunList[DisMhNum-1].sign==0x8F)
		{
			if(HistoryBuf[3]==0x83 || HistoryBuf[3]==0x84 || HistoryBuf[3]==0x8B)
			{
				//显示延时中
				TextOut(640,locatey,"延时中...",0);
			}
		}
		else if(HistoryBuf[1]<33)
		{
			//总线地址显示类型
			OutOneType(640,locatey,MhRunList[DisMhNum-1].type & 0x1F);	//显示类型
			locatey+=24;  
			//位置:	
			OutOneNote(400,locatey,MhRunList[DisMhNum-1].panel,MhRunList[DisMhNum-1].loop,MhRunList[DisMhNum-1].addr);
		}

	}
	else
	{
		HistoryBuf[0]=MhErrList[DisMhNum-1].panel;
		HistoryBuf[1]=MhErrList[DisMhNum-1].loop;
		HistoryBuf[2]=MhErrList[DisMhNum-1].addr;
		OutAddr(470,locatey,HistoryBuf);	   //显示地址
		locatey+=24;
		if(MhErrList[DisMhNum-1].sign==0x81)
			OutOneErrorType1(660,locatey,MhErrList[DisMhNum-1].type & 0x1F ,MhErrList[DisMhNum-1].type1);//显示总线地址故障类型
		else
			OutOneErrorType(660,locatey,MhErrList[DisMhNum-1].type1);	//显示故障类型
		
		//显示时间
		HistoryBuf[0]=MhErrList[DisMhNum-1].year;
		HistoryBuf[1]=MhErrList[DisMhNum-1].month;
		HistoryBuf[2]=MhErrList[DisMhNum-1].day;
		HistoryBuf[3]=MhErrList[DisMhNum-1].hour;
		HistoryBuf[4]=MhErrList[DisMhNum-1].minute;
		HistoryBuf[5]=MhErrList[DisMhNum-1].second;
		OutTime(400,locatey,HistoryBuf,0);   //显示时间

	}
}


void DisMhLdZone()
{
	if(pmhRun>0)
	{
		if(mhzonetype==1)
		{
			//显示后续气灭联动
			if(pmhRun>DisMhNum )
			{
				if(HaveDisplayMh<2)
				{
					DisMhNum++;
					HaveDisplayMh++;
					OutOneMh(1);
				}
			}	
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(666,280,"(灭火)",0);
			mhzonetype=1;
			Qxsq(3);
			DisMhNum=1;
			HaveDisplayMh=1;
			OutOneMh(1);
		}
	}
	else if(pmhError>0)
	{
		if(mhzonetype==2)
		{
			//显示后续气灭故障
			if(pmhError>DisMhNum )
			{
				if(HaveDisplayMh<2)
				{
					DisMhNum++;
					HaveDisplayMh++;
					OutOneMh(2);
				}
			}	
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(666,280,"(故障)",0);
			mhzonetype=2;
			Qxsq(3);
			DisMhNum=1;
			HaveDisplayMh=1;
			OutOneMh(2);
		}		
	}
	else
	{
		DisMhNum=0;
		HaveDisplayMh=0;	
		mhzonetype=0;		
	}	
}

void displayzonenum()
{
	u16 fc,bc;
	fc=Fcolor;
	bc=Bcolor;
	Fcolor=WHITE;
	Bcolor=BLACK;
	SetColor(Fcolor,Bcolor);

	TextOut(5,26,"   ",0);
	TextOut(405,26,"   ",0);
	TextOut(5,280,"   ",0);
	TextOut(405,280,"   ",0);
	
	switch(optzonenum)
	{
	case 1:
		TextOut(5,26,"==>",0);
		break;
	case 2:
		TextOut(405,26,"==>",0);
		break;
	case 3:
		TextOut(5,280,"==>",0);
		break;
	case 4:
		TextOut(405,280,"==>",0);
		break;		
	}		
	Fcolor=fc;
	Bcolor=bc;
	SetColor(Fcolor,Bcolor);
	
}

void DisLdZone()
{
	if(pRun>0)
	{
		if(ldzonetype==1)
		{
			//显示后续火警
			if(pRun>DisRunNum)
			{   
				if(HaveDisplayRun<3)
				{
					DisRunNum++;
					HaveDisplayRun++;
					OutOneRun(1);
				}
			}
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(666,26,"(联动)",0);
			ldzonetype=1;				
			Qxsq(1);
			DisRunNum=1;
			HaveDisplayRun=1;
			OutOneRun(1);
		}
	}
	else if(pldError>0)
	{
		if(ldzonetype==2)
		{
			//显示后续联动区故障
			if(pldError>DisRunNum)
			{   
				if(HaveDisplayRun<3)
				{
					DisRunNum++;
					HaveDisplayRun++;
					OutOneRun(2);
				}
			}
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			TextOut(666,26,"(故障)",0);
			ldzonetype=2;				
			Qxsq(1);
			DisRunNum=1;
			HaveDisplayRun=1;
			OutOneRun(2);
		}
	}
	else
	{
		DisRunNum=0;
		HaveDisplayRun=0;
		ldzonetype=0;				
	}	
}

void zidongfanye(u8 fanyetype)
{
	u8 i;
	if(fanyetype==1)
	{
		if(hjzonetype==1)
		{
			if(HaveDisplay>CurDisplay)
			{
				//当前反显的需更新
				OutOneFire(CurDisNum,CurDisplay,0,1);
				CurDisNum++;
				CurDisplay++;
				OutOneFire(CurDisNum,CurDisplay,CurDisplay,1);
			}
			else if(pFire>CurDisNum && CurDisplay==3) 
			{
				//上滚一条
				CurDisNum++;
				for(i=2;i<3;i++)
					OutOneFire(CurDisNum+i-3,i,0,1);

				OutOneFire(CurDisNum,3,3,1);
			}
			else if(pFire>3)
			{
				//回首页
				Qxsq(0); 
				DisFireNum=1;
				HaveDisplay=1;
				CurDisplay=HaveDisplay;
				CurDisNum=DisFireNum;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,1);
			}
		}
		else if(hjzonetype==2)
		{
//			if(pError>DisFireNum && HaveDisplay==3) 
//			{
//				Qxsq(0);	//清火警显示区
//				DisFireNum--;
//				HaveDisplay=1;
//				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
//			}		
//			else if(pError>3)
//			{
//				//回第一条
//				Qxsq(0);	//清火警显示区
//				DisFireNum=1;
//				HaveDisplay=1;
//				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
//			}	
			if(HaveDisplay>CurDisplay)
			{
				//当前反显的需更新
				OutOneFire(CurDisNum,CurDisplay,0,2);
				CurDisNum++;
				CurDisplay++;
				OutOneFire(CurDisNum,CurDisplay,CurDisplay,2);
			}
			else if(pError>CurDisNum && CurDisplay==3) 
			{
				//上滚一条
				CurDisNum++;
				for(i=1;i<3;i++)
					OutOneFire(CurDisNum+i-3,i,0,2);

				OutOneFire(CurDisNum,3,3,2);
			}
			else if(pError>3)
			{
				//回首页
				Qxsq(0); 
				DisFireNum=1;
				HaveDisplay=1;
				CurDisplay=HaveDisplay;
				CurDisNum=DisFireNum;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
			}			
		}			
		else if(hjzonetype==3)
		{
			if(Total_Isolate>DisFireNum && HaveDisplay==3) 
			{
				Qxsq(0);	//清火警显示区
				DisFireNum--;
				HaveDisplay=1;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,3);
			}		
			else if(Total_Isolate>3)
			{
				//回第一条
				Qxsq(0);	//清火警显示区
				DisFireNum=1;
				HaveDisplay=1;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,3);
			}				
		}		
	}
	else if(fanyetype==2)
	{
		if(ldzonetype==1)
		{
			if(pRun>DisRunNum && HaveDisplayRun==3) 
			{
				Qxsq(1);	//清联动显示区
				DisRunNum--;
				HaveDisplayRun=1;
				OutOneRun(1);
			}		
			else if(pRun>3)
			{
				//回第一条
				Qxsq(1);	//清联动显示区
				DisRunNum=1;
				HaveDisplayRun=1;
				OutOneRun(1);
			}
		}
		else if(ldzonetype==2)
		{
			if(pldError>DisRunNum && HaveDisplayRun==3) 
			{
				Qxsq(1);	//清联动显示区
				DisRunNum--;
				HaveDisplayRun=1;
				OutOneRun(2);
			}		
			else if(pldError>3)
			{
				//回第一条
				Qxsq(1);	//清联动显示区
				DisRunNum=1;
				HaveDisplayRun=1;
				OutOneRun(2);
			}				
		}		
	}
	else if(fanyetype==3)
	{
		if(krzonetype==1)
		{
			if(pKFire>DisKrFireNum && HaveDisplayKrFire==2) 
			{
				Qxsq(2);	//清可燃显示区
				HaveDisplayKrFire=1;
				OutOneKrFire(1);
			}		
			else if(pKFire>2)
			{
				//回第一条
				Qxsq(2);	//清可燃显示区
				DisKrFireNum=1;
				HaveDisplayKrFire=1;
				OutOneKrFire(1);
			}
		}
		else if(krzonetype==2)
		{
			if(pkrError>DisKrFireNum && HaveDisplayKrFire==2) 
			{
				Qxsq(2);	//清可燃显示区
				HaveDisplayKrFire=1;
				OutOneKrFire(2);
			}		
			else if(pkrError>2)
			{
				//回第一条
				Qxsq(2);	//清可燃显示区
				DisKrFireNum=1;
				HaveDisplayKrFire=1;
				OutOneKrFire(2);
			}				
		}		
	}	
	else if(fanyetype==4)
	{
		if(mhzonetype==1)
		{
			if(pmhRun>DisMhNum && HaveDisplayMh==2) 
			{
				Qxsq(3);	//清灭火显示区
				HaveDisplayMh=1;
				OutOneMh(1);
			}		
			else if(pmhRun>2)
			{
				//回第一条
				Qxsq(3);	//清灭火显示区
				DisMhNum=1;
				HaveDisplayMh=1;
				OutOneMh(1);
			}
		}
		else if(mhzonetype==2)
		{
			if(pmhError>DisMhNum && HaveDisplayMh==2) 
			{
				Qxsq(3);	//清灭火显示区
				HaveDisplayMh=1;
				OutOneMh(2);
			}		
			else if(pmhError>2)
			{
				//回第一条
				Qxsq(3);	//清灭火显示区
				DisMhNum=1;
				HaveDisplayMh=1;
				OutOneMh(2);
			}				
		}
	}
}

//火警显示页面
void FireDisplay()
{
	u8 i,r;
	u16 y;
	if(m_FirstView!=m_View)
	{

		//首次显示
		m_FirstView=m_View;
		
		FirePageView();
		bConfirm=0;
        m_FireAuto=0; //火警自动翻页
        m_RunAuto=0;  //联动自动翻页
		m_KrFireAuto=0;
		m_mhRunAuto=0;
		
		//每个区域分级别显示
		hjzonetype=0;
		DisFireZone();
		ldzonetype=0;
		DisLdZone();
		krzonetype=0;
		DisKrFireZone();
		mhzonetype=0;
		DisMhLdZone();

		
		optzonenum=1;	//可操作的分区号
		displayzonenum();
		
    }
    else
    {
        //显示后续火警区
        DisFireZone();
		DisLdZone();
		DisKrFireZone();		
		DisMhLdZone();
				
    }
    //判断自动翻页
    if((pFire>3 || (pFire==0 && pError>3)) && m_FireAuto>9 && bConfirm==0)
    {
        m_FireAuto=0; //火警自动翻页
        zidongfanye(1);
    }
    if((pRun>3 || (pRun==0 && pldError>3)) && m_RunAuto>9)
    {
        m_RunAuto=0; //联动自动翻页
		zidongfanye(2);
    }
	if((pKFire>2 || (pKFire==0 && pkrError>2)) && m_KrFireAuto>9)
	{
        m_KrFireAuto=0; //
		zidongfanye(3);			
	}
    if((pmhRun>2 || (pmhRun==0 && pmhError>2)) && m_mhRunAuto>9 && bConfirm==0)
    {
        m_mhRunAuto=0; //灭火自动翻页
		zidongfanye(4);
    }

	if(bConfirm==1)
	{
		r=0;
		if(pFire>0)
		{
			if(key>0x30 && key<0x34)
			{
				//确认火警
				FireList[CurDisNum-1].confirm=key-0x30;	//确认火警
				OutOneFire(CurDisNum,CurDisplay,CurDisplay,1);	//更新信息
				bConfirm=0;
				//刷新联动页
				r=1;
			}
			else if(key==Key_Esc)
			{
				bConfirm=0;
				//刷新联动页
				r=1;
			}
		}
		else if(pError>0)
		{
			if(key==0x31)
			{
				//确认故障
				ErrorList[CurDisNum-1].confirm=1;	//确认故障
				OutOneFire(CurDisNum,CurDisplay,CurDisplay,2);	//更新信息
				bConfirm=0;
				//刷新联动页
				r=1;
			}
			else if(key==Key_Esc)
			{
				bConfirm=0;
				key=0;
				//刷新联动页
				r=1;
			}
		}
		if(r)
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			DrawRect(410,285,799,452,2);	//整体显示区域
			TextOut(450,280,"气体灭火控制器信息",0);
			y=75*0+305;
			DrawLine(380,y,800,y);
			mhzonetype=0;
			DisMhLdZone();
		}
	}

    //判断按键
	switch(key)
	{
	case Key_Menu:  //操作
	{
		m_View=1;
		m_FirstView=0;		
	}
	break;
	case Key_Up:  //火警前页
	{
		if(optzonenum==1)
		{
			//操作火警区
			if(hjzonetype==1 && pFire>0)
			{
				Qxsq(0); 
				DisFireNum=1;
				HaveDisplay=1;
				CurDisplay=HaveDisplay;
				CurDisNum=DisFireNum;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,1);
				m_FireAuto=0; //火警自动翻页	
			}
			else if(hjzonetype==2 && pError>0)
			{
				Qxsq(0); 
				DisFireNum=1;
				HaveDisplay=1;
				CurDisplay=HaveDisplay;
				CurDisNum=DisFireNum;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
			}
			else if(hjzonetype==3 && Total_Isolate>3)
			{
				Qxsq(0); 
				DisFireNum=1;
				HaveDisplay=1;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,3);				
			}
		}
		else if(optzonenum==2)
		{
			//操作联动区
			if(ldzonetype==1 && pRun>3)
			{
				Qxsq(1);	//清联动显示区
				DisRunNum=1;
				HaveDisplayRun=1;
				OutOneRun(1);
				m_RunAuto=0;  //联动自动翻页
			}
			else if(ldzonetype==2 && pldError>3)
			{
				Qxsq(1);	//清联动显示区
				DisRunNum=1;
				HaveDisplayRun=1;
				OutOneRun(2);
			}
		}
		else if(optzonenum==3)
		{
			//操作可燃气体报警区
			if(krzonetype==1 && pKFire>2)
			{
				Qxsq(2);	//清可燃显示区
				DisKrFireNum=1;
				HaveDisplayKrFire=1;
				OutOneKrFire(1);
				m_KrFireAuto=0;  
			}
			else if(krzonetype==2 && pkrError>2)
			{
				Qxsq(2);	//清可燃显示区
				DisKrFireNum=1;
				HaveDisplayKrFire=1;
				OutOneKrFire(2);
				m_KrFireAuto=0;  
			}
		}
		else if(optzonenum==4)
		{
			//操作灭火区
			if(mhzonetype==1 && pmhRun>2)
			{
				Qxsq(3);	//清灭火显示区
				DisMhNum=1;
				HaveDisplayMh=1;
				OutOneMh(1);
			}
			else if(mhzonetype==2 && pmhError>2)
			{
				Qxsq(3);	//清灭火显示区
				DisMhNum=1;
				HaveDisplayMh=1;
				OutOneMh(2);
			}
		}

	}		
	break;
	case Key_Down: //火警翻页
	{
		if(optzonenum==1)
		{
			//操作火警区
			zidongfanye(1);				
		}
		else if(optzonenum==2)
		{
			//操作联动区
			zidongfanye(2);
		}
		else if(optzonenum==3)
		{
			//操作可燃气体报警区
			zidongfanye(3);			
		}
		else if(optzonenum==4)
		{
			//操作灭火区
			zidongfanye(4);
		}
	}
	break;
	case Key_Confirm:	//火警确认
	{
		if(bConfirm==0)
		{
			if(pFire>0 && optzonenum==1)
			{
				Fcolor=WHITE;
				Bcolor=DGRAY;
				SetColor(Fcolor,Bcolor);
				win.x=410;
				win.y=285;
				win.with=380;
				win.hight=165;
				win.title=NULL;
				win.state=NULL;
				GUI_WindowsDraw(&win);
				TextOut(415,310,"确认火警信息",0);
				HistoryBuf[0]=FireList[CurDisNum-1].panel;
				HistoryBuf[1]=FireList[CurDisNum-1].loop;
				HistoryBuf[2]=FireList[CurDisNum-1].addr;
				TextOut(415,340,"当前火警地址：",0);
				OutAddr(590,340,HistoryBuf);	//显示地址
				TextOut(440,370,"真实火警按1,误报火警按2,",0);
				TextOut(440,400,"调试火警按3,按取消键退出。",0);
				bConfirm=1;
				
				Fcolor=WHITE;
				Bcolor=RED;
				SetColor(Fcolor,Bcolor);
			}
			else if(pError>0 && optzonenum==1)
			{
				Fcolor=WHITE;
				Bcolor=DGRAY;
				SetColor(Fcolor,Bcolor);
				win.x=410;
				win.y=285;
				win.with=380;
				win.hight=165;
				win.title=NULL;
				win.state=NULL;
				GUI_WindowsDraw(&win);
				TextOut(415,310,"确认故障信息",0);
				TextOut(415,340,"当前故障序号：",0);
				OutThreeChar1(590,340,CurDisNum,0);	//显示序号
				TextOut(440,370,"故障已处理请按1,",0);
				TextOut(440,400,"未处理按取消键。",0);
				bConfirm=1;
				
				Fcolor=WHITE;
				Bcolor=RED;
				SetColor(Fcolor,Bcolor);				
			}
		}
	}
	break;
	case Key_Before:  //联动前页
	{
		if(optzonenum>0)
			optzonenum--;	//可操作的分区号
		if(optzonenum==0)
			optzonenum=4;
		displayzonenum();
	}
	break;
	case Key_Next: //联动翻页
	{
		optzonenum++;	//可操作的分区号
		if(optzonenum>4)
			optzonenum=1;
		displayzonenum();
	}
	break;
	}
	if(pFire==0 && pRun==0 && pKFire==0 && pmhRun==0)
	{
	    //退出
	    FirstPageView(); //首页显示
	    //清音响
	    if(m_Voice==3)
	    {
	        voice_out(0);
            m_Voice=0;
	    }
	}
	key=0;	
}

//显示一条故障
void OutOneError(u16 no,u8 disno,u8 disno1)
{    
    u8 num;
	u16 locatey;
	
	if(disno==disno1)
	{	
		Fcolor=YELLOW;
		Bcolor=BLUE;	
	}
	else
	{	
		Fcolor=BLUE;
		Bcolor=YELLOW;
	}
	SetColor(Fcolor,Bcolor);
	
	locatey=disno*55+17;

	//清显示区
	DrawRect(0,locatey,800,locatey+50,2);	////////////


	OutOneChar1(4,locatey,'N',0);
	OutThreeChar1(16,locatey,no,0);
	OutOneChar1(52,locatey,':',0);
	
	//显示时间
	HistoryBuf[0]=ErrorList[no-1].year;
	HistoryBuf[1]=ErrorList[no-1].month;
	HistoryBuf[2]=ErrorList[no-1].day;
	HistoryBuf[3]=ErrorList[no-1].hour;
	HistoryBuf[4]=ErrorList[no-1].minute;
	HistoryBuf[5]=ErrorList[no-1].second;
	OutTime(4,locatey+25,HistoryBuf,0);   //显示时间

	switch(ErrorList[no-1].confirm)
	{
	case 1:
		TextOut(680,locatey,"故障已处理",0);
		break;
	default:
		TextOut(680,locatey,"未处理故障",0);
		break;
	}

    switch(ErrorList[no-1].sign)
    {
    	case 0x81: //地址故障
		case 0x61: //多线故障
        {
			HistoryBuf[0]=ErrorList[no-1].panel;
			HistoryBuf[1]=ErrorList[no-1].loop;
			HistoryBuf[2]=ErrorList[no-1].addr;
			OutAddr(70,locatey,HistoryBuf);	   //显示地址
			if(ErrorList[no-1].sign==0x81)
			{
				OutOneType(300,locatey,ErrorList[no-1].type & 0x1F);	//显示类型
				// OutOneErrorType1(500,locatey,ErrorList[no-1].type & 0x1F ,ErrorList[no-1].type1);//显示总线地址故障类型
				OutOneErrorType1(450,locatey,ErrorList[no-1].type & 0x1F ,ErrorList[no-1].type1);//显示总线地址故障类型
			}
			else
			{
				OutOneErrorType(300,locatey,ErrorList[no-1].type1);//显示多线故障类型
			}
        }
        break;
        case 0x87: //回路故障
        {
	        if(ErrorList[no-1].panel==0)
			{
				OutThreeChar1(70,locatey,ErrorList[no-1].loop,0);
				TextOut(106,locatey,"回路故障",0);					
			}
			else
			{
				TextOut(94,locatey,"-   回路故障",0);						
				OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
				OutThreeChar1(106,locatey,ErrorList[no-1].loop,0);
			}
        }
        break;
        case 0x89: //其它故障
        {
				if(ErrorList[no-1].loop==1)
                {
                    //板故障
	                num= ErrorList[no-1].addr;
					if(ErrorList[no-1].panel==0)
					{
						if(num<9)
						{
							OutTwoChar1(70,locatey,num,0);
		                    TextOut(94,locatey,"号回路板故障",0);
						}
						else if(num<17)
						{
							OutTwoChar1(70,locatey,num-8,0);
		                    TextOut(94,locatey,"号灭火盘故障",0);
						}
						else
						{
							OutTwoChar1(70,locatey,num-16,0);
		                    TextOut(94,locatey,"号多线盘故障",0);
						}
					}
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						OutOneChar1(94,locatey,'-',0);
						if(num<9)
						{
							OutTwoChar1(106,locatey,num,0);
		                    TextOut(130,locatey,"号回路板故障",0);
						}
						else if(num<17)
						{
							OutTwoChar1(106,locatey,num-8,0);
		                    TextOut(130,locatey,"号灭火盘故障",0);
						}
						else
						{
							OutTwoChar1(106,locatey,num-16,0);
		                    TextOut(130,locatey,"号扩展板故障",0);
						}
					}
                }
				else if(ErrorList[no-1].loop==2)
                {
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"电源通讯故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机电源通讯故障",0);
					}
                }
				else if(ErrorList[no-1].loop==3)
                {
                    //主电故障
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"主电故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机主电故障",0);
					}
                }
                else if(ErrorList[no-1].loop==4)
                {
                    //备电故障
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"备电故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机备电故障",0);
					}
                }
                else if(ErrorList[no-1].loop==5)
                {
                    //声光开路故障
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"声光开路",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机声光开路",0);
					}
                }
                else if(ErrorList[no-1].loop==6)
                {
                    //声光短路故障
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"声光短路",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机声光短路",0);
					}
                }
				else if(ErrorList[no-1].loop==7)
                {
                    //机器故障
	                OutTwoChar1(70,locatey,ErrorList[no-1].addr,0);
	                TextOut(94,locatey,"号机器故障",0);
                }
				else if(ErrorList[no-1].loop==8)
                {
                    //欠压故障
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"备电欠压故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机备电欠压",0);
					}
                }
				else if(ErrorList[no-1].loop==9)
                {
                    //电源输出故障
					if(ErrorList[no-1].addr==2)
					{
						if(ErrorList[no-1].panel==0)
							TextOut(70,locatey,"联动电源输出短路",0);
						else
						{
							OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
							TextOut(94,locatey,"号机联动电源输出短路",0);
						}
					}
					else
					{
						if(ErrorList[no-1].panel==0)
							TextOut(70,locatey,"电源输出1故障",0);
						else
						{
							OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
							TextOut(94,locatey,"号机电源输出1故障",0);
						}
					}
                }
				else if(ErrorList[no-1].loop==0x0A)
                {
                    //运行数据存储单元故障
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"运行数据存储单元故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机运行数据存储单元故障",0);
					}
                }
				else if(ErrorList[no-1].loop==0x0C)
                {
                    //电池低压故障
					if(ErrorList[no-1].panel==0)
	                	TextOut(70,locatey,"电池低压故障",0);
					else
					{
						OutTwoChar1(70,locatey,ErrorList[no-1].panel,0);
						TextOut(94,locatey,"号机电池低压",0);
					}
                }
       }
        break;
    }
	

    //地址故障显示位置
    if(ErrorList[no-1].sign==0x81)
    {
        //位置:
		OutOneNote(400,locatey+25,ErrorList[no-1].panel,ErrorList[no-1].loop,ErrorList[no-1].addr);
		//TextOut(400,locatey+25,"测试一下汉字显示信息能否显示完全",0);
    }
}

//故障显示页面
void ErrorDisplay()
{
	u8 i,r;
	u16 y;
	if(pError==0 && m_View==91)
	{
		//故障恢复
		FirstPageView(); //首页显示 
		return;			
	}
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
		Fcolor=BLUE;
		Bcolor=YELLOW;
		SetColor(Fcolor,Bcolor);
		DrawRect(0,25,800,455,2);	//整体显示区域
		
		TextOut(10,28,"故障信息",1);

		bConfirm=0;
			
		Fcolor=WHITE;
		Bcolor=YELLOW;
		SetColor(Fcolor,Bcolor);
		for(i=0;i<8;i++)
		{
			y=i*55+70;
			DrawLine(0,y,800,y);
		}
		Fcolor=BLUE;
		Bcolor=YELLOW;
		SetColor(Fcolor,Bcolor);

    	if(pError>0)
		{
	    	Display_Number=1;
	    	HaveDisplay=1;
			CurDisplay=HaveDisplay;
			CurDisNum=Display_Number;
	    	OutOneError(Display_Number,HaveDisplay,CurDisplay);
	    	m_ErrorAuto=0; //故障自动翻页
			bRefreshErrorDisplay=0;	//刷新故障显示区
    	}
		else if(m_View==33)
		{
			OutChars(10,14,"当前没有故障。",0);	
		}	
    }
    else
    {
        //判断是否有后续故障
		if(pError>Display_Number)
		{
			if(HaveDisplay<7)
			{	
				//未页满
				Display_Number++;
				HaveDisplay++;
				OutOneError(Display_Number,HaveDisplay,CurDisplay);  //输出一行故障
				bDisplaying=1;
			}	
			else
				bDisplaying=0;
		}
		else
			bDisplaying=0;
	}
	//判断自动翻页
    if(pError>7 && m_ErrorAuto>7 && bConfirm==0)
    {
        m_ErrorAuto=0; //故障自动翻页
        key=Key_Down;
    }
	if(bRefreshErrorDisplay && RefreshErrorTimes>200)
	{
		bRefreshErrorDisplay=0;
		RefreshErrorTimes=0;
		m_FirstView=0;
	}
	//如果正在显示，则不处理翻页键
	if(bDisplaying && key==Key_Down)
		return;

	if(bConfirm==1)
	{
		r=0;
		if(key==0x31)
		{
			//确认故障
			ErrorList[CurDisNum-1].confirm=1;
			bConfirm=0;
			//刷新联动页
			r=1;
		}
		else if(key==Key_Esc)
		{
			bConfirm=0;
			//刷新联动页
			r=1;
		}
		if(r)
		{
			m_FirstView=0;  //回首页
		}
	}

	//判断按键
	switch(key)
	{
	case Key_Menu:  //操作
	{
		m_View=1;
		m_FirstView=0;		
	}
	break;
	case Key_Up:  //首页
	{
		m_FirstView=0;		
	}
	break;
	case Key_Down: //翻页
	{
		if(HaveDisplay>CurDisplay)
		{
			//当前反显的需更新
			OutOneError(CurDisNum,CurDisplay,0);
			CurDisNum++;
			CurDisplay++;
			OutOneError(CurDisNum,CurDisplay,CurDisplay);
		}
		else if(pError>CurDisNum && CurDisplay==7) //每页显示7行
		{
			Fcolor=BLUE;
			Bcolor=YELLOW;
			SetColor(Fcolor,Bcolor);
			DrawRect(0,25,800,455,2);	//整体显示区域
			
			TextOut(10,28,"故障信息",1);
				
			Fcolor=WHITE;
			Bcolor=YELLOW;
			SetColor(Fcolor,Bcolor);
			for(i=0;i<8;i++)
			{
				y=i*55+70;
				DrawLine(0,y,800,y);
			}
			Fcolor=BLUE;
			Bcolor=YELLOW;
			SetColor(Fcolor,Bcolor);

			CurDisNum++;
			for(i=0;i<7;i++)
			{
				OutOneError(CurDisNum+i-6,i+1,CurDisplay);
			}			
	        m_ErrorAuto=0; //故障自动翻页
		}
		else if(pError>7)
		{
		    m_FirstView=0;  //回首页
		}		
	}
	break;
	case Key_Confirm:	//故障确认
	{
		if(bConfirm==0 && pError>0)
		{
			Fcolor=WHITE;
			Bcolor=DGRAY;
			SetColor(Fcolor,Bcolor);
			win.x=375;
			win.y=265;
			win.with=425;
			win.hight=190;
			win.title=NULL;
			win.state=NULL;
			GUI_WindowsDraw(&win);
			TextOut(400,310,"确认故障信息",0);
			TextOut(400,340,"当前故障序号：",0);
			OutThreeChar1(560,340,CurDisNum,0);
			TextOut(400,400,"故障已处理请按1,未处理按退格键。",0);
			bConfirm=1;
		}
	}

	break;
	}
	if(m_View==33 && key==Key_Esc)
	{
		m_View=3;
		m_FirstView=0;		
	}
	key=0;	

}

//查询页面
void FindView_Deal()
{
    if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;

		DrawPicture1(2,0,25,800,455,0,25);
//		Windows(100,100,700,500,"查询选项");
//		OutChars(3,15,"请按数字键选择功能：",1);
//		OutChars(7,24,"1.查询登记地址",1);
//		OutChars(8,24,"2.查询屏蔽地址",1);
//		OutChars(9,24,"3.当前故障",1);
//		OutChars(10,24,"4.火警历史记录",1);
//		OutChars(11,24,"5.联动历史记录",1);
//		OutChars(12,24,"6.其它历史记录",1);
//		OutChars(13,24,"7.查询联动编程",1);
//		OutChars(14,24,"8.查询位置信息",1);

	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=1;
			m_FirstView=0;
		}
		break;
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:    //燃气报警历史记录
		{
			m_View=31+key-0x31;
			m_FirstView=0;				
		}
		break;
    }
    key=0;
}

//显示一条屏蔽
void OutOneDisable()
{    
	u16 locatey,num;
	u8 buf[6];
	
	//屏蔽需要倒序显示
	num= Total_Isolate + 1 - Display_Number;
	//读一条屏蔽
	SPI_FLASH_HighSpeedReadCont(E_DisableAddr+num*8,8,HistoryBuf);	
	locatey=HaveDisplay*50+21;

	OutOneChar1(4,locatey,'N',0);
	OutThreeChar1(16,locatey,Display_Number,0);
	OutOneChar1(52,locatey,':',0);

	OutAddr(70,locatey,HistoryBuf);	   //显示地址
	TextOut(200,locatey,"地址屏蔽",0);
	
	//显示类型
	OutOneType(350,locatey,HistoryBuf[3] & 0x1F);	//显示类型
	
	//显示时间
	buf[0]=HistoryBuf[4];	//年
	num=(u16) (HistoryBuf[5]<<8) | HistoryBuf[6];
	buf[1]=(num & 0xF000)>>12;  //月
	buf[2]=(num & 0x0FC0)>>6;  	//日
	buf[3]=num & 0x003F; 		//时
	buf[4]=HistoryBuf[7];
	buf[5]=0;
	
	OutTime(4,locatey+24,buf,1);   //显示时间
    //位置:
	//OutOneNote(400,locatey+24,HistoryBuf[0],HistoryBuf[1],HistoryBuf[2]);

}


//屏蔽显示
void DisableDisplay()
{
	u8 i;
	u16 y;
	if(Total_Isolate==0 && m_View==92)
	{
		//没有屏蔽
		FirstPageView(); //首页显示 
		return;			
	}
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
    	Fcolor=YELLOW;
		Bcolor=DGRAY;
		SetColor(Fcolor,Bcolor);
		DrawRect(0,25,800,455,2);	//整体显示区域
		
		TextOut(10,28,"屏蔽信息",1);

		Fcolor=LGRAY;
		Bcolor=DGRAY;
		SetColor(Fcolor,Bcolor);
		for(i=0;i<8;i++)
		{
			y=i*50+70;
			DrawLine(0,y,800,y);
		}
    	Fcolor=YELLOW;
		Bcolor=DGRAY;
		SetColor(Fcolor,Bcolor);

		if(Total_Isolate>0)
		{
	    	Display_Number=1;
	    	HaveDisplay=1;
	    	OutOneDisable();
	        m_DisableAuto=0; //屏蔽自动翻页
    	}
		else if(m_View==32)
		{
			OutChars(10,14,"当前没有屏蔽。",0);	
		}	
    }
    else
    {
        //判断是否有后续故障
		if(Total_Isolate>Display_Number)
		{
			if(HaveDisplay<8)
			{	
				//未页满
				Display_Number++;
				HaveDisplay++;
				OutOneDisable();  //输出一行屏蔽
				bDisplaying=1;
			}	
			else
				bDisplaying=0;
		}
		else
			bDisplaying=0;
	}
	//判断自动翻页
    if(Total_Isolate>8 && m_DisableAuto>7)
    {
        m_DisableAuto=0; //屏蔽自动翻页
        key=Key_Down;
    }
	//如果正在显示，则不处理翻页键
	if(bDisplaying && key==Key_Down)
		return;
	//判断按键
	switch(key)
	{
	case Key_Menu:  //操作
	{
		m_View=1;
		m_FirstView=0;		
	}
	break;
	case Key_Up:  //首页
	{
		m_FirstView=0;		
	}
	break;
	case Key_Down: //翻页
	{
		if(Total_Isolate>Display_Number && HaveDisplay==8) //每页显示8行
		{
			Fcolor=YELLOW;
			Bcolor=DGRAY;
			SetColor(Fcolor,Bcolor);
			DrawRect(0,25,800,455,2);	//整体显示区域
			
			TextOut(10,28,"屏蔽信息",1);
				
			Fcolor=LGRAY;
			Bcolor=DGRAY;
			SetColor(Fcolor,Bcolor);
			for(i=0;i<8;i++)
			{
				y=i*50+70;
				DrawLine(0,y,800,y);
			}
			Fcolor=YELLOW;
			Bcolor=DGRAY;
			SetColor(Fcolor,Bcolor);
			
			Display_Number=Display_Number-8;
			HaveDisplay=1;
			OutOneDisable();  //输出一行屏蔽			
	        m_DisableAuto=0; //屏蔽自动翻页
		}
		else
		{
		    m_FirstView=0;  //回首页
		}		
	}
	break;
	}
	if(m_View==32 && key==Key_Esc)
	{
		//查询屏蔽页面
		m_View=3;
		m_FirstView=0;		
	}
	key=0;	

}


// 记录指定回路的所有设备
void RecordLoopDevices(uint8 loop_num)
{
    uint8 i;
    uint8 type, type1, index1, type2, index2;
    
    for(i=1; i<151; i++)
    {
        if(CanFile[i+3] & 0x80)  // 有设备
        {
            type = NormalizeKrDeviceType(CanFile[i+3]);
            type1 = GetTongXinBan_type_index(type);
            // 检查设备是否已经被记录过
            if(device_index_map[loop_num][i].recorded == 0)
            {

				// 只记录1-6回路的前10个同类型设备
				if(loop_num >= 1 && loop_num <= 6 && 
				type1 > DEVICE_KongZhiQi && type1 < DEVICE_TYPE_COUNT && 
				type_device_count[type1] < 10)
				{
					// 计算索引（从1开始）
					if(loop_num == 3 &&
					   i >= 1 &&
					   i <= KERANQI_UPLOAD_CONC_COUNT &&
					   type1 == DEVICE_KeRanQi)
						index1 = i;
					else
						index1 = type_device_count[type1] + 1;
					
					// 保存设备映射关系
					device_index_map[loop_num][i].type = type;
					device_index_map[loop_num][i].index = index1;
					device_index_map[loop_num][i].recorded = 1;
					
					// 设置设备登记状态位为1
					HandleDeviceState(loop_num, i, STATUS_REGISTER, 1);
					
					type_device_count[type1]++;
				}	
			}
            else
            {
                // 设备已经记录过，更新状态位（保持登记状态为1）
                // 这里可以更新其他可能变化的状态，比如报警状态等
                type1 = device_index_map[loop_num][i].type;
                index1 = device_index_map[loop_num][i].index;
                
                if(type1 > 0 && index1 > 0 && index1 <= 10)
                {
                    // 确保登记状态位为1
                    HandleDeviceState(loop_num, i, STATUS_REGISTER, 1);
                }
            }
				
        }
		else
		{
			// 如果这个地址没有设备，但之前记录过，可能需要清除登记状态
			if(device_index_map[loop_num][i].recorded == 1)
			{
				type2 = device_index_map[loop_num][i].type;
				index2 = device_index_map[loop_num][i].index;
				
				if(type2 > 0 && index2 > 0 && index2 <= 10)
				{
					// 清除登记状态位
					HandleDeviceState(loop_num, i, STATUS_REGISTER, 0);
					
					// 标记为未记录
					device_index_map[loop_num][i].recorded = 0;
					device_index_map[loop_num][i].type = 0;
					device_index_map[loop_num][i].index = 0;
					
					// 注意：这里不清除type_device_count，因为我们不知道是否有其他设备占用这个索引
					// 如果需要精确管理，需要更复杂的逻辑
				}
			}
		}
    }
}



void TriggerTongXinBanRegisterQuery(void)
{
	need_query_loops = 1;
	boot_query_register_loop = 1;
	boot_query_register_wait = 0;
	boot_query_register_delay = 1500;
	boot_query_register_retry = 0;
	boot_query_register_failed_mask = 0;
	boot_query_register_rescan_delay = 0;
}

/* 上电后台扫描1到6回路的设备登记信息，不依赖用户打开登记界面。每个回路等待回复；失败最多重试后记录下来，扫描结束后再延时重扫失败回路。 */
void QueryRegisterOnBoot(void)
{
	if(need_query_loops == 0)
	{
		if(boot_query_register_failed_mask == 0)
			return;

		/* Retry an unsuccessful startup scan after a short cooldown. */
		/* 每次主循环加1；主循环约2毫秒一次，因此30000次约为60秒。延时期间不重复占用CAN总线。 */
if(boot_query_register_rescan_delay < 30000)
		{
			boot_query_register_rescan_delay++;
			return;
		}

		need_query_loops = 1;
		boot_query_register_loop = 1;
		boot_query_register_wait = 0;
		boot_query_register_delay = 0;
		boot_query_register_retry = 0;
		boot_query_register_failed_mask = 0;
		boot_query_register_rescan_delay = 0;
	}

	/* 31是手动登记界面。该页面也要读写CanFile，因此后台扫描先暂停，避免抢走界面的响应帧。 */
if(m_View == 31)
		return;

	if(boot_query_register_loop < 1)
		boot_query_register_loop = 1;

	if(boot_query_register_loop > 6)
	{
		need_query_loops = 0;
		boot_query_register_wait = 0;
		boot_query_register_delay = 0;
		boot_query_register_retry = 0;
		boot_query_register_rescan_delay = 0;
		return;
	}

	/* 气体数据被Modbus读取后提高了优先级：若当前没有在等回路查询回复，就先让出总线给气体查询。已发出的查询仍继续等完。 */
if(kr_query_priority && !boot_query_register_wait)
		return;

	if(boot_query_register_wait)
	{
		/* 0x80表示CAN文件已收齐。消费前先清标志，避免同一份旧文件被重复处理。 */
if(HaveReciveCanFile == 0x80)
		{
			HaveReciveCanFile = 0x00;
			/* 回包中的回路号必须与当前查询回路一致，确认匹配后才建立该回路的设备登记映射。 */
if(CanFile[2] == boot_query_register_loop)
			{
				RecordLoopDevices(boot_query_register_loop);
				boot_query_register_failed_mask &= (uint8_t)~(1U << (boot_query_register_loop - 1));
				boot_query_register_loop++;
				boot_query_register_retry = 0;
				boot_query_register_wait = 0;
				boot_query_register_delay = 0;
			}
		}
		else
		{
			boot_query_register_delay++;
			/* 等待约500个主循环（约1秒）仍无完整CAN文件，判为本次超时并进入重试流程。 */
if(boot_query_register_delay > 500)
			{
				/* retry从0开始：此条件表示初次请求后已重发两次；仍失败就记录该回路，继续扫描其他回路。 */
if(boot_query_register_retry >= 2)
				{
					boot_query_register_failed_mask |= (uint8_t)(1U << (boot_query_register_loop - 1));
					boot_query_register_loop++;
					boot_query_register_retry = 0;
				}
				else
				{
					boot_query_register_retry++;
				}
				boot_query_register_wait = 0;
				boot_query_register_delay = 0;
			}
		}
		return;
	}

	/* 回路查询之间留出约1500个主循环（约3秒），给控制器和CAN总线留处理时间。 */
if(boot_query_register_delay < 1500)
	{
		boot_query_register_delay++;
		return;
	}

	/* 发送设备/登记信息查询命令：当前目标为boot_query_register_loop，地址0表示查询整回路。 */
m_CanCommand.sign = 0x11;
	m_CanCommand.loop = boot_query_register_loop;
	m_CanCommand.addr = 0;
	m_CanCommand.type = 0;
	HaveReciveCanFile = 0x00;
	WriteCanCommand();
	boot_query_register_wait = 1;
	boot_query_register_delay = 0;
}


/* 后台读取可燃气设备状态和浓度。need_query_kr_register表示有待处理查询，boot_query_kr_wait表示命令已发出、正在等CAN回复；超时或收到回复后结束本次等待。 */
void QueryKeRanQiRegisterOnBoot(void)
{
	if(need_query_kr_register == 0)
		return;

	/* 启动扫描尚未结束时，默认先完成回路登记；只有Modbus明确优先请求气体数据时才打断顺序。 */
if(need_query_loops && !kr_query_priority)
		return;

	/* 66为气体实时显示页，31为手动登记页：这些页面由自身处理共享CAN回复，所以后台不发起新查询。若气体查询已在途则仍允许收尾。 */
if(m_View == 66 || (m_View == 31 && !boot_query_kr_wait))
		return;

	/* 后台查询已发出但尚未完成。FindRegister在此短暂返回，等后台消费或超时清理共享接收文件后再处理界面。 */
if(boot_query_kr_wait)
	{
		if(HaveReciveCanFile == 0x80)
		{
			HaveReciveCanFile = 0x00;
			/* 只接受约定的气体上传文件类型/编号；不匹配的文件不写入气体寄存器缓存。 */
if(CanFile[1] == 6 && CanFile[2] == 3)
			{
				SyncKeRanQiUploadFromCanFile(CanFile);
				need_query_kr_register = 0;
                kr_query_priority = 0;
				boot_query_kr_retry = 0;
			}
			else if(boot_query_kr_retry >= 2)
			{
				need_query_kr_register = 0;
                kr_query_priority = 0;
				boot_query_kr_retry = 0;
			}
			else
			{
				boot_query_kr_retry++;
			}
			boot_query_kr_wait = 0;
			boot_query_kr_delay = 0;
		}
		else
		{
			boot_query_kr_delay++;
			/* 等待约500个主循环（约1秒）没有完整回复，就结束本轮等待并重试，避免状态永久卡在wait。 */
if(boot_query_kr_delay > 500)
			{
				if(boot_query_kr_retry >= 2)
				{
					need_query_kr_register = 0;
                kr_query_priority = 0;
					boot_query_kr_retry = 0;
				}
				else
				{
					boot_query_kr_retry++;
				}
				boot_query_kr_wait = 0;
				boot_query_kr_delay = 0;
			}
		}
		return;
	}

	if(boot_query_kr_delay < 500)
	{
		boot_query_kr_delay++;
		return;
	}

	/* 发送气体数据查询命令，目标为第3回路；发送前清除旧接收标志，防止把上次残留文件当作本次回复。 */
m_CanCommand.sign = 0x14;
	m_CanCommand.loop = 3;
	m_CanCommand.addr = 0;
	m_CanCommand.type = 0;
	HaveReciveCanFile = 0x00;
	WriteCanCommand();
	boot_query_kr_wait = 1;
	boot_query_kr_delay = 0;
}

//查登记
/* 手动登记界面与后台查询共用CAN接收缓冲区。若后台气体查询正在等待回复，暂缓本次界面处理，避免一份回复被错误地当成另一种命令的结果。 */
void FindRegister()
{
	uint8 i;
    uint16 x,y,num;
	uint8 type;
	uint8 buf[32];

	/* Let an in-flight background gas query release the shared CAN file buffer first. */
	if(boot_query_kr_wait)
		return;

    // // 新增：如果需要查询1-6回路，则自动开始查询
    // if(need_query_loops && m_FirstView != m_View)
    // {
    //     // 开始查询第1回路
    //     m_Input[2] = 1;
    //     m_CanCommand.sign = 0x11;  
    //     m_CanCommand.loop = m_Input[2];
    //     m_CanCommand.addr = 0;
    //     m_CanCommand.type = 0;
    //     WriteCanCommand();
    //     HaveReciveCanFile = 0;
        
    //     // 清除显示区域
    //     DrawRect(0,70,800,455,2);
        
    //     // 显示正在查询的回路
    //     OutChars(1,20,"自动查询1-6回路中...",1);
    //     OutChars(1,25,"当前查询回路:",1);
    //     OutThreeChar(14,25,m_Input[2],1);
        
    //     need_query_loops = 0; // 开始查询后清除标志
    // }

	if(m_FirstView !=m_View )
    {
        m_FirstView=m_View;
 		Fcolor=WHITE;
		Bcolor=NAVY;
		SetColor(Fcolor,Bcolor);
		DrawRect(0,25,800,455,2);	//整体显示区域
		
		OutChars(1,1,"查询登记",1);
		OutChars(1,10,"回路:",1);

    	OutOneChar(1,16,'_',1); //光标
		m_Cursor=16; //光标位置
		HaveReciveCanFile=0x00;

	}

	if(HaveReciveCanFile==0x80)   //命令反馈标志
    {        
        HaveReciveCanFile=0x00;
		RecordLoopDevices(m_Input[2]);

		//15*10=150
		for(i=0;i<17;i++)
			DrawLine(5+i*48,80,5+i*48,380);
		for(i=0;i<11;i++)
			DrawLine(5,80+i*30,773,80+i*30);
		
		for(i=0;i<10;i++)
			OutThreeChar1(10,82+i*30,i*15+1,0);
		for(i=0;i<32;i++)
			buf[i]=0; 
		num=0;
		
		x=62;
		y=88;


		//总线回路
		for(i=1;i<151;i++)
		{
			//显示类型
			if(CanFile[i+3] & 0x80)
			{
				type=NormalizeKrDeviceType(CanFile[i+3]);
				num++;
				buf[type]++;
				switch(type)
				{
					case 01:	TextOut(x,y,"感烟",2);	break;
					case 02:	TextOut(x,y,"感温",2);	break;
					case 03:	TextOut(x,y,"复合",2);	break;
					case 04:	TextOut(x,y,"手报",2);	break;
					
					case 10:	TextOut(x,y,"消钮",2);	break;
					case 11:	TextOut(x,y,"输入",2);	break;
					case 12:	TextOut(x,y,"输出",2);	break;
					case 13:	TextOut(x,y,"控制",2);	break;
					
					case 14:	TextOut(x,y,"显示",2);	break;
					case 15:	TextOut(x,y,"声光",2);	break;
					
					case 20:	TextOut(x,y,"短隔",2);	break;
					case 21:	TextOut(x,y,"启停",2);	break;
					case 22:	TextOut(x,y,"切换",2);	break;
					
					case 25:	TextOut(x,y,"CO  ",2);	break;
					case 26:	TextOut(x,y,"H2  ",2);	break;
					case 27:	TextOut(x,y,"可燃",2);	break;
					case 28:	TextOut(x,y,"H2  ",2);	break;
					case 29:	TextOut(x,y,"H2  ",2);	break;
					
					default:	TextOut(x,y,"未知",2);	break;	
				}
			}
			if(i%15==0)
			{
				x=62;
				y+=30;
			}
			else
				x+=48;
		}

		OutThreeChar1(350,40,CanFile[2],0);
		TextOut(386,40,"回路登记总数为   ,具体如下:",0);
		OutThreeChar1(554,40,num,0);		


		x=10;
		y=390;
		for(i=1;i<30;i++)
		{						
	
			if(buf[i]>0)
			{
				switch(i)
				{
					case 01:	TextOut(x,y,"光电感烟:",0);	break;
					case 02:	TextOut(x,y,"光电感温:",0);	break;
					case 03:	TextOut(x,y,"烟温复合:",0);	break;
					case 04:	TextOut(x,y,"手动按钮:",0);	break;

					case 10:	TextOut(x,y,"消火栓钮:",0);	break;
					case 11:	TextOut(x,y,"输入模块:",0);	break;
					case 12:	TextOut(x,y,"输出模块:",0);	break;
					case 13:	TextOut(x,y,"输入输出:",0);	break;
					
					case 14:	TextOut(x,y,"  显示盘:",0);	break;
					case 15:	TextOut(x,y,"声光警报:",0);	break;
					
					case 20:	TextOut(x,y,"短路隔离:",0);	break;
					case 21:	TextOut(x,y,"紧急启停:",0);	break;
					case 22:	TextOut(x,y,"手自切换:",0);	break;
					
					case 25:	TextOut(x,y,"CO探测器",0);	break;
					case 26:	TextOut(x,y,"H2探测器",0);	break;
					case 27:	TextOut(x,y,"可燃气体",0);	break;
					case 28:	TextOut(x,y,"H2探测器",0);	break;
					case 29:	TextOut(x,y,"H2探测器",0);	break;
					
					default:	TextOut(x,y,"未知类型:",0);	break;
				}
				x+=108;
				OutThreeChar1(x,y,buf[i],0);
				x+=50;
				if(x>750)
				{
					x=10;
					y+=30;
				}
			}
		}

	}		
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=3;
		m_FirstView=0;

	Fcolor=WHITE;
	Bcolor=BLUE;
	SetColor(Fcolor,Bcolor);
	DrawRect(0,455,800,460,2);
	DrawRect(0,460,572,479,2);
	}
	break;
	case Key_BackSpace:  
	{
		m_View=31;
		m_FirstView=0;
	}
	break;
	case Key_Up:
	{
		if(m_Cursor==30 && m_Input[2]<65)
		{
			m_Input[2]++;
		    //发送查登记命令
		    m_CanCommand.sign=0x11;  
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=0;
			m_CanCommand.type=0;
			WriteCanCommand();
			HaveReciveCanFile=0;
			//显示
			OutThreeChar(1,16,m_Input[2],1);	
			DrawRect(0,70,800,455,2);
		}
	}
	break;	
	case Key_Down:
	{	
		if(m_Cursor==30 && m_Input[2]>1)
		{
			m_Input[2]--;
		    //发送查登记命令
		    m_CanCommand.sign=0x11;  
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=0;
			m_CanCommand.type=0;
			WriteCanCommand();
			HaveReciveCanFile=0;
			//显示
			OutThreeChar(1,16,m_Input[2],1);	
			DrawRect(0,70,800,455,2);
		}
	}
	break;
	
// // 新增：增加一个按键来触发1-6回路自动查询
//     case Key_Enter:  // 确认键用于触发自动查询
//     {
//         need_query_loops = 1;
//         m_FirstView = 0; // 强制刷新界面
//     }
//     break;	

	
	}
	if(key>0x29 && key<0x3A && m_Cursor<50 )
	{
		//有键按下
		OutOneChar(1,m_Cursor,key,1);
		switch(m_Cursor)
		{
		    case 16:
		    {
			    m_Cursor++;
			    OutOneChar(1,m_Cursor,'_',1); //光标
			    m_Input[0]=key-0x30;
		    }
		    break;
		    case 17:
		    {
			    m_Cursor++;
			    OutOneChar(1,m_Cursor,'_',1); //光标
			    m_Input[1]=key-0x30;
		    }
		    break;
		    case 18:
		    {
			    //回路
			    num=m_Input[0]*100+m_Input[1]*10+key-0x30;
			    if(num>0 && num<65)
			    {
			        m_Cursor=30;
			        m_Input[2]=num;
		            //发送查登记命令
		            m_CanCommand.sign=0x11;  
			        m_CanCommand.loop=m_Input[2];
			        m_CanCommand.addr=0;
			        m_CanCommand.type=0;
			        WriteCanCommand();
			        
			    }
			    else
				{
					m_Cursor=16;
					OutChars(1,16,"_  ",1);
				}
			}   
		    break;
		}	
	}		
	key=0;
}

//显示一条历史记录
void OutOneHistory(uint8 type)
{
	uint8 num;
	uint8 type1;
	u16 locatey,lel;
	//读一条记录
	SPI_FLASH_HighSpeedReadCont(His_Addr,14,HistoryBuf);
	if(HistoryBuf[0]!=0xFF)
	{
		locatey=HaveDisplay*55+72;
		if(type==1)
		{
			//显示火警记录
			if(HistoryBuf[0]==0x80)
			{
				Display_Number++;	//总显示个数
				OutOneChar1(4,locatey,'N',0);
				OutTwoChar1(16,locatey,Display_Number/100,0);
				OutTwoChar1(40,locatey,Display_Number%100,0);
				OutOneChar1(64,locatey,':',0);
				
				OutAddr(88,locatey,HistoryBuf+1);	   //显示地址
				
				TextOut(250,locatey,"火警",0);
				
				OutOneType(400,locatey,HistoryBuf[4] & 0x1F);	//显示类型
				
				OutTime(570,locatey,HistoryBuf+5,0);   //显示时间
				OutOneNote(100,locatey+25,HistoryBuf[1],HistoryBuf[2],HistoryBuf[3]);
			}
		}
		else if(type==2)
		{
			//显示联动记录
			Display_Number++;	//总显示个数
			OutOneChar1(4,locatey,'N',0);
			OutTwoChar1(16,locatey,Display_Number/100,0);
			OutTwoChar1(40,locatey,Display_Number%100,0);
			OutOneChar1(64,locatey,':',0);
			
			OutAddr(88,locatey,HistoryBuf+1);	   //显示地址
			if(HistoryBuf[2]<33)
				OutOneType(400,locatey,HistoryBuf[4] & 0x1F);	//显示类型
			switch(HistoryBuf[0])
			{   
				case 0x83:  TextOut(250,locatey,"启动",0);	break;
				case 0x84:  TextOut(250,locatey,"停止",0);	break;
				case 0x85:  TextOut(250,locatey,"反馈",0);	break;
				case 0x86:  TextOut(250,locatey,"反馈撤销",0);	break;
				case 0x8F:	//气体灭火类
				{
					switch(HistoryBuf[4])
					{
					case 0x01:	TextOut(250,locatey,"气体喷洒启动",0);	break;
					case 0x02:	TextOut(250,locatey,"气体喷洒反馈",0);	break;
					case 0x03:	TextOut(250,locatey,"灭火现场急启",0);	break;
					case 0x04:	TextOut(250,locatey,"灭火面板急启",0);	break;
					case 0x05:	TextOut(250,locatey,"灭火急启断开",0);	break;
					case 0x06:	TextOut(250,locatey,"灭火现场急停",0);	break;
					case 0x07:	TextOut(250,locatey,"灭火面板急停",0);	break;
					case 0x08:	TextOut(250,locatey,"声光启动    ",0);	break;
					case 0x09:	TextOut(250,locatey,"声光停止    ",0);	break;
					case 0x0A:	TextOut(250,locatey,"放气指示输出",0);	break;
					case 0x0B:	TextOut(250,locatey,"灭火自动启动",0);	break;
					}
				}
				break;
			}
			OutTime(570,locatey,HistoryBuf+5,0);   //显示时间
			OutOneNote(100,locatey+25,HistoryBuf[1],HistoryBuf[2],HistoryBuf[3]);
		}
		else if(type==3)
		{
			//燃气报警
			Display_Number++;	//总显示个数
			OutOneChar1(4,locatey,'N',0);
			OutTwoChar1(16,locatey,Display_Number/100,0);
			OutTwoChar1(40,locatey,Display_Number%100,0);
			OutOneChar1(64,locatey,':',0);
			
			OutAddr(88,locatey,HistoryBuf+1);	   //显示地址
			
			lel =  (uint16)HistoryBuf[11] <<8;
			lel |=HistoryBuf[12];
			lel &=0x3FFF;
			if(HistoryBuf[4]==25)
			{
				TextOut(260,locatey,"CO",0);
				num=lel/100;
				if(num>9)
					OutTwoChar1(450,locatey,num,0);
				else
					OutOneChar1(462,locatey,num+0x30,0);
				OutTwoChar1(474,locatey,lel%100,0);
				TextOut(498,locatey,"ppm",0);
			}
			else if(HistoryBuf[4]==26)
			{
				TextOut(260,locatey,"H2",0);
				num=lel/100;
				if(num>1)
					num=1;
				OutOneChar1(462,locatey,num+0x30,0);
				OutTwoChar1(474,locatey,lel%100,0);
				TextOut(498,locatey,"%LEL",0);
			}
			if(HistoryBuf[0]==0x7A)
				TextOut(284,locatey,"低限报警",0);
			else if(HistoryBuf[0]==0x7B)
				TextOut(284,locatey,"高限报警",0);	
			
			OutTime(570,locatey,HistoryBuf+5,0);   //显示时间
			OutOneNote(100,locatey+25,HistoryBuf[1],HistoryBuf[2],HistoryBuf[3]);
		}
		else
		{
			//显示其它记录
			Display_Number++;	//总显示个数
			OutOneChar1(4,locatey,'N',0);
			OutTwoChar1(16,locatey,Display_Number/100,0);
			OutTwoChar1(40,locatey,Display_Number%100,0);
			OutOneChar1(64,locatey,':',0);

		    switch(HistoryBuf[0])
		    {   
		        case 0x01:  TextOut(100,locatey,"开机",0);	break;
		        case 0x02:  TextOut(100,locatey,"自检",0);	break;
		        case 0x03:  TextOut(100,locatey,"复位",0);	break;
		        case 0x04:  TextOut(100,locatey,"关机",0);	break;
				case 0x05:
					if(HistoryBuf[1]==0xC0)
						TextOut(100,locatey,"自动状态",0);	
					else
						TextOut(100,locatey,"手动状态",0);
					break;					
				case 0x81: //地址故障
				case 0x61: //多线故障
				{
					OutAddr(88,locatey,HistoryBuf+1);	   //显示地址
					if(HistoryBuf[0]==0x81)
					{
						OutOneType(400,locatey,HistoryBuf[4] & 0x1F);	//显示类型
						OutOneErrorType1(250,locatey,HistoryBuf[4] & 0x1F,HistoryBuf[11]);//显示总线地址故障类型	
					}
					else
					{
						OutOneErrorType(400,locatey,HistoryBuf[4]);//显示多线故障类型
					}
					OutOneNote(54,locatey+25,HistoryBuf[1],HistoryBuf[2],HistoryBuf[3]);
				}
				break;
		        case 0x82:  
		        case 0x62:  
				{
					OutAddr(88,locatey,HistoryBuf+1);	   //显示地址
					if(HistoryBuf[0]==0x82)
					{
						OutOneType(400,locatey,HistoryBuf[4] & 0x1F);	//显示类型
						
						type1=HistoryBuf[4] &0x1F;
						switch(HistoryBuf[11])
						{
							case 0x01:	TextOut(250,locatey,"通讯恢复",0);	break;
							case 0x02:	
								if(type1==11 || type1==13)
									TextOut(250,locatey,"输入开路恢复",0);	
								else
									TextOut(250,locatey,"故障恢复",0);	
								break;
							case 0x04:
								if(type1==1 || type1==3)
									TextOut(250,locatey,"污染故障恢复",0);
								else
									TextOut(250,locatey,"输出故障恢复",0);	
								break;
							case 0x08:
								TextOut(250,locatey,"24V故障恢复",0);
								break;
						}
						
					}
					else
					{
						switch(HistoryBuf[11])
						{
							case 0x01:	TextOut(250,locatey,"启动开路恢复",0);	break;
							case 0x02:	TextOut(250,locatey,"启动短路恢复",0);	break;
							case 0x03:	TextOut(250,locatey,"反馈线路恢复",0);	break;
							default:	TextOut(250,locatey,"故障恢复",0);	break;
						}	
					
					}
					OutOneNote(54,locatey+25,HistoryBuf[1],HistoryBuf[2],HistoryBuf[3]);
				}
				break;
				case 0x87:
				{
			        if(HistoryBuf[1]==0)
					{
						OutThreeChar1(100,locatey,HistoryBuf[2],0);
						TextOut(136,locatey,"回路故障",0);
					}
					else
					{
						TextOut(124,locatey,"-   回路故障",0);							
						OutTwoChar1(100,locatey,HistoryBuf[1],0);
						OutThreeChar1(136,locatey,HistoryBuf[2],0);
					}
				}
				break;
				case 0x88:
				{
			        if(HistoryBuf[1]==0)
					{
						OutThreeChar1(100,locatey,HistoryBuf[2],0);
			            TextOut(136,locatey,"回路恢复正常",0);
					}
					else
					{
			            TextOut(124,locatey,"-   回路恢复正常",0);				
						OutTwoChar1(100,locatey,HistoryBuf[1],0);
						OutThreeChar1(136,locatey,HistoryBuf[2],0);
					}
				}
				break;
		        case 0x89: //其它故障
		        {
					if(HistoryBuf[2]==1)
		            {
		                //板故障
			            num= HistoryBuf[3];
						if(HistoryBuf[1]==0)
						{
							if(num<9)
							{
								OutTwoChar1(100,locatey,num,0);
				                TextOut(124,locatey,"号回路板故障",0);
							}
							else if(num<17)
							{
								OutTwoChar1(100,locatey,num-8,0);
				                TextOut(124,locatey,"号灭火盘故障",0);
							}
							else
							{
								OutTwoChar1(100,locatey,num-16,0);
				                TextOut(124,locatey,"号多线盘故障",0);
							}
						}
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							OutOneChar1(124,locatey,'-',0);
							if(num<9)
							{
								OutTwoChar1(136,locatey,num,0);
								TextOut(160,locatey,"号回路板故障",0);
							}
							else if(num<17)
							{
								OutTwoChar1(136,locatey,num-8,0);
				                TextOut(160,locatey,"号灭火盘故障",0);
							}
							else
							{
								OutTwoChar1(136,locatey,num-16,0);
				                TextOut(160,locatey,"号多线盘故障",0);
							}
						}
		            }
					else if(HistoryBuf[2]==2)
		            {
		                //电源通讯故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"电源通讯故障",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机电源通讯故障",0);
						}
		            }
					else if(HistoryBuf[2]==3)
		            {
		                //主电故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"主电故障",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机主电故障",0);
						}
		            }
		            else if(HistoryBuf[2]==4)
		            {
		                //备电故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"备电故障",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机备电故障",0);
						}
		            }
		            else if(HistoryBuf[2]==5)
		            {
		                //声光开路故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"声光开路",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机声光开路",0);
						}
		            }
		            else if(HistoryBuf[2]==6)
		            {
		                //声光短路故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"声光短路",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机声光短路",0);
						}
		            }
					else if(HistoryBuf[2]==7)
		            {
		                //机器故障
			            OutTwoChar1(100,locatey,HistoryBuf[3],0);
						TextOut(124,locatey,"号机器故障",0);
		            }
					else if(HistoryBuf[2]==8)
		            {
		                //欠压故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"备电欠压故障",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机欠压故障",0);
						}
		            }
					else if(HistoryBuf[2]==9)
		            {
		                //电源输出故障
						if(HistoryBuf[3]==2)
						{
							if(HistoryBuf[1]==0)
								TextOut(100,locatey,"联动电源输出短路",0);
							else
							{
								OutTwoChar1(100,locatey,HistoryBuf[1],0);
								TextOut(124,locatey,"号机联动电源输出短路",0);
							}
						}
						else
						{
							if(HistoryBuf[1]==0)
								TextOut(100,locatey,"电源输出1故障",0);
							else
							{
								OutTwoChar1(100,locatey,HistoryBuf[1],0);
								TextOut(124,locatey,"号机电源输出1故障",0);
							}
						}
		            }
					else if(HistoryBuf[2]==0x0A)
		            {
		                //运行数据存储单元故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"运行数据存储单元故障",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机运行数据存储单元故障",0);
						}
		            }
					else if(HistoryBuf[2]==0x0C)
		            {
		                //电池低压
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"电池低压故障",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机电池低压",0);
						}
		            }
		        }
		        break;
		        case 0x8A: //其它故障恢复
		        {
					if(HistoryBuf[2]==1)
		            {
		                //板故障
			            num= HistoryBuf[3];
						if(HistoryBuf[1]==0)
						{
							if(num<9)
							{
								OutTwoChar1(100,locatey,num,0);
				                TextOut(124,locatey,"号回路板故障恢复",0);
							}
							else if(num<17)
							{
								OutTwoChar1(100,locatey,num-8,0);
				                TextOut(124,locatey,"号灭火盘故障恢复",0);
							}
							else
							{
								OutTwoChar1(100,locatey,num-16,0);
				                TextOut(124,locatey,"号多线盘故障恢复",0);
							}
						}
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							OutOneChar1(124,locatey,'-',0);
							if(num<9)
							{
								OutTwoChar1(136,locatey,num,0);
								TextOut(160,locatey,"号回路板故障恢复",0);
							}
							else if(num<17)
							{
								OutTwoChar1(136,locatey,num-8,0);
				                TextOut(160,locatey,"号灭火盘故障恢复",0);
							}
							else
							{
								OutTwoChar1(136,locatey,num-16,0);
				                TextOut(160,locatey,"号多线盘故障恢复",0);
							}
						}
		            }
					else if(HistoryBuf[2]==2)
		            {
		                //电源通讯故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"电源通讯故障恢复",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机电源通讯故障恢复",0);
						}
		            }
					else if(HistoryBuf[2]==3)
		            {
		                //主电故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"主电故障恢复",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机主电故障恢复",0);
						}
		            }
		            else if(HistoryBuf[2]==4)
		            {
		                //备电故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"备电故障恢复",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机备电故障恢复",0);
						}
		            }
		            else if(HistoryBuf[2]==5)
		            {
		                //声光开路故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"声光恢复正常",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机声光恢复正常",0);
						}
		            }
		            else if(HistoryBuf[2]==6)
		            {
		                //声光短路故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"声光恢复正常",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机声光恢复正常",0);
						}
		            }
					else if(HistoryBuf[2]==7)
		            {
		                //机器故障
			            OutTwoChar1(100,locatey,HistoryBuf[3],0);
						TextOut(124,locatey,"号机器故障恢复",0);
		            }
					else if(HistoryBuf[2]==8)
		            {
		                //欠压故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"备电欠压恢复",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机欠压故障恢复",0);
						}
		            }
					else if(HistoryBuf[2]==9)
		            {
		                //电源输出故障
						if(HistoryBuf[3]==2)
						{
							if(HistoryBuf[1]==0)
								TextOut(100,locatey,"电源输出短路恢复",0);
							else
							{
								OutTwoChar1(100,locatey,HistoryBuf[1],0);
								TextOut(124,locatey,"号机电源输出短路恢复",0);
							}
						}
						else
						{
							if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"电源输出恢复",0);
							else
							{
								OutTwoChar1(100,locatey,HistoryBuf[1],0);
								TextOut(124,locatey,"号机电源输出恢复",0);
							}
						}
		            }
					else if(HistoryBuf[2]==0x0A)
		            {
		                //运行数据存储单元故障
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"运行数据存储单元故障恢复",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机运行数据存储单元故障恢复",0);
						}
		            }
					else if(HistoryBuf[2]==0x0C)
		            {
		                //电池低压
						if(HistoryBuf[1]==0)
				            TextOut(100,locatey,"电池低压恢复",0);
						else
						{
							OutTwoChar1(100,locatey,HistoryBuf[1],0);
							TextOut(124,locatey,"号机电池低压恢复",0);
						}
		            }
					
		        }
		        break;
			}
			OutTime(570,locatey,HistoryBuf+5,0);   //显示时间
		}
	}
	else
	{
		bHistoryEnd=1;
	}

}

//查历史记录
void FindHistory(uint8 type)
{
	u8 i;
	u16 y,F1,B1;
	if(m_FirstView !=m_View)
    {
        m_FirstView=m_View;
		if(type==1 || type==3)
		{
			Fcolor=WHITE;
			Bcolor=RED;	
		}
		else if(type==2)
		{
			Fcolor=BLACK;
			Bcolor=CYAN;			
		}
		else
		{
			Fcolor=WHITE;
			Bcolor=DGRAY;
		}
		SetColor(Fcolor,Bcolor);
		DrawRect(0,25,800,455,2);	//整体显示区域
		
		for(i=0;i<8;i++)
		{
			y=i*55+70;
			DrawLine(0,y,800,y);
		}
		
		if(type==1)
		{
			TextOut(10,28,"火警历史记录",1);
			//显示首地址
			if(pFireHistory==E_FireBegin)
				His_Addr=E_FireEnd;
			else
				His_Addr=pFireHistory-16;
		}
		else if(type==2)
		{
			TextOut(10,28,"联动历史记录",1);
			//显示首地址
			if(pRunHistory==E_RunBegin)
				His_Addr=E_RunEnd;
			else
				His_Addr=pRunHistory-16;
		}
		else if(type==3)
		{
			TextOut(10,28,"可燃气体报警历史记录",1);
			//显示首地址
			if(pKFireHistory==E_KFireBegin)
				His_Addr=E_KFireEnd;
			else
				His_Addr=pKFireHistory-16;
		}
		else 
		{
			TextOut(10,28,"其它历史记录",1);
			//显示首地址
			if(pOtherHistory==E_OtherBegin)
				His_Addr=E_OtherEnd;
			else
				His_Addr=pOtherHistory-16;
		}

		HaveDisplay=0;		//本页显示个数
		Display_Number=0;	//总显示个数
		bHistoryEnd=0;
		PrintHisAddr=His_Addr;	//打印首地址
		Display_Number1=Display_Number;	//用于打印本页时重新显示本页
		m_NumofEnd=0;		//历史记录中连续出现的0xFF
		bDispWin=0;
    } 
    else
    {
        while(HaveDisplay<7 && bHistoryEnd==0)
        {   
            OutOneHistory(type);  //输出一行历史记录
			if(bHistoryEnd==0) 		//有显示数据
			{
				HaveDisplay++;
				m_NumofEnd=0;		//历史记录中连续出现的0xFF
			}
			if(type==1)
			{
				if(bHistoryEnd && His_Addr!=E_FireEnd)
				{
					m_NumofEnd++;		//历史记录中连续出现的0xFF
					if(m_NumofEnd<3)
						bHistoryEnd=0;	
				}
				if(His_Addr==E_FireBegin)
	                His_Addr=E_FireEnd;
	            else
	                His_Addr=His_Addr-16;
			}
			else if(type==2)
			{
				if(bHistoryEnd && His_Addr!=E_RunEnd)
				{
					m_NumofEnd++;		//历史记录中连续出现的0xFF
					if(m_NumofEnd<3)
						bHistoryEnd=0;	
				}
				if(His_Addr==E_RunBegin)
					His_Addr=E_RunEnd;
				else
					His_Addr=His_Addr-16;
			}
			else if(type==3)
			{
				if(bHistoryEnd && His_Addr!=E_KFireEnd)
				{
					m_NumofEnd++;		//历史记录中连续出现的0xFF
					if(m_NumofEnd<3)
						bHistoryEnd=0;	
				}
				if(His_Addr==E_KFireBegin)
	                His_Addr=E_KFireEnd;
	            else
	                His_Addr=His_Addr-16;
			}
			else
			{
				if(bHistoryEnd && His_Addr!=E_OtherEnd)
				{
					m_NumofEnd++;		//历史记录中连续出现的0xFF
					if(m_NumofEnd<3)
						bHistoryEnd=0;	
				}
				if(His_Addr==E_OtherBegin)
					His_Addr=E_OtherEnd;
				else
					His_Addr=His_Addr-16;
			} 
		}
    }
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=3;
		m_FirstView=0;
	}
	break;
	case Key_Up:
	{
	    if(bDispWin==0)
			m_FirstView=0;
	}
	break;
	case Key_Down:
	{
	    if(bDispWin==0)
		{
			if(HaveDisplay==7)
			{            				
				DrawRect(0,72,800,455,2);	//整体显示区域
				
				for(i=0;i<8;i++)
				{
					y=i*55+70;
					DrawLine(0,y,800,y);
				}
				HaveDisplay=0;
			}
			PrintHisAddr=His_Addr;	//打印首地址
			Display_Number1=Display_Number;	//用于打印本页时重新显示本页
		}
	}
	break;
	case Key_Enter:
	{
		if(bDispWin==0)
		{
			if(m_PrintSet>0)
			{
				F1=Fcolor;
				B1=Bcolor;
				Fcolor=WHITE;
				Bcolor=DGRAY;
				SetColor(Fcolor,Bcolor);
				win.x=425;
				win.y=325;
				win.with=375;
				win.hight=130;
				win.title=NULL;
				win.state=NULL;
				GUI_WindowsDraw(&win);
				TextOut(450,370,"确定要打印当前信息吗？",0);
				TextOut(450,400,"按确认键打印，按修改键退出。",0);
				bDispWin=1;
				Fcolor=F1;
				Bcolor=B1;
				SetColor(Fcolor,Bcolor);
			}
		}
		else
		{
			//打印当前页
			bDispWin=0;
			PrintHisAddr1=PrintHisAddr;
			PrintHisNum=10;
			PrintHisType=type;
			//重新显示本页
			DrawRect(0,72,800,455,2);	//整体显示区域
			for(i=0;i<8;i++)
			{
				y=i*55+70;
				DrawLine(0,y,800,y);
			}
			HaveDisplay=0;			
			His_Addr=PrintHisAddr;	//本页显示首地址	
			Display_Number=Display_Number1;
		}
	}
	break;
	case Key_BackSpace:
	{
		if(bDispWin)
		{
			bDispWin=0;
			//重新显示本页
			DrawRect(0,72,800,455,2);	//整体显示区域
			for(i=0;i<8;i++)
			{
				y=i*55+70;
				DrawLine(0,y,800,y);
			}
			HaveDisplay=0;			
			His_Addr=PrintHisAddr;	//本页显示首地址				
			Display_Number=Display_Number1;
		}
	}
	break;
	}
	key=0;

}

//自检
void SelfTest()
{
	if(m_FirstView!=m_View)
    {
        m_FirstView=m_View;

		Fcolor=WHITE;
		Bcolor=BLUE;
		SetColor(Fcolor,Bcolor);
		Lcdclr();

		TextOut(10,7,"系统自检",1);  
		OutChars(15,20,"系统自检，请稍候",0);

		m_SelfTestTimes=15;
		m_SetTestRefrshLcd=0;		//自检时刷屏标志

		//发送自检命令
	    m_CanCommand.sign=0x01;  
	    m_CanCommand.loop=0x03;
	    m_CanCommand.addr=0;
	    m_CanCommand.type=0;
	    WriteCanCommand();
		
	    m_Record.sign=0x02; //自检
	    m_Record.panel=0;
        m_Record.loop=0;  
	    m_Record.addr=0;   
	    m_Record.type=0;
		m_Record.year=D_Year;
        m_Record.month=D_Month;
        m_Record.day=D_Day;
        m_Record.hour=D_Hour;
        m_Record.minute=D_Minute;
 		WriteHisRecord(0); //写入自检记录
		
		
		if(m_PanelType==0)
		{
			m_Record.sign=0x01;  
			m_Record.loop=0x03;	 //从机自检向主机发送
			m_Record.addr=0;
			m_Record.type=0;
			m_Record.addr=0;   
			WriteToWCANZone(); //写入外部CAN发送区
		}
		m_Record.sign=0xA0;		//特殊事件
		m_Record.panel=m_PanelAddr;
		m_Record.confirm=123;	//自检
		WriteToHxzZone();	//写黑匣子发送区

	    gpio_out(0,0);	//系统故障灯

    }
    if(m_SelfTestTimes>10)
    {
        //火警声
		voice_out(3);
    }
    else if(m_SelfTestTimes>5)
    {
        //联动声
	     voice_out(2);
    }
    else
    {
        //故障声
	     voice_out(1);
    }
	if(m_SetTestRefrshLcd==0 && m_SelfTestTimes==12)
	{
		//改背景
		Fcolor=GREEN;
		Bcolor=GREEN;
		SetColor(Fcolor,Bcolor);
		Lcdclr();
		m_SetTestRefrshLcd=1;
	}
	if(m_SetTestRefrshLcd==1 && m_SelfTestTimes==9)
	{
		//改背景
	    Fcolor=RED;	
		Bcolor=RED;
		SetColor(Fcolor,Bcolor);
		Lcdclr();
		m_SetTestRefrshLcd=2;
	}
	if(m_SetTestRefrshLcd==2 && m_SelfTestTimes==6)
	{
		//改背景
	    Fcolor=YELLOW;	
		Bcolor=YELLOW;
		SetColor(Fcolor,Bcolor);
		Lcdclr();
		m_SetTestRefrshLcd=3;
	}
	if(m_SetTestRefrshLcd==3 && m_SelfTestTimes==3)
	{
		//改背景
        Fcolor=WHITE;	
		Bcolor=BLUE;
		SetColor(Fcolor,Bcolor);
		Lcdclr();
		m_SetTestRefrshLcd=0;
		TextOut(10,7,"系统自检",1);  
		OutChars(15,20,"系统自检，请稍候",0);
	}
    if(key==Key_Esc || m_SelfTestTimes==0)
    {
        m_SelfTestTimes=0;
		FirstPageView(); //首页显示

        gpio_out(0,1);	//系统故障灯
	    voice_out(0);
		
		m_Record.sign=0x05; //自检完成
	    m_Record.panel=0;
        m_Record.loop=0;  
	    m_Record.addr=0;   
	    m_Record.type=0;
		m_Record.year=D_Year;
        m_Record.month=D_Month;
        m_Record.day=D_Day;
        m_Record.hour=D_Hour;
        m_Record.minute=D_Minute;
        WriteToWCANZone(); //写入外部CAN发送区
    }
}

//未实现功能
void NoFunction()
{
    if(m_FirstView!=m_View)
	{
        m_FirstView=m_View;
		
		Windows(100,100,700,500,"未实现此功能");
		OutChars(12,15,"未实现此功能，请退出。",0);

	}
	if(key==Key_Esc)
    {
		FirstPageView(); //首页显示
    }
	key=0;	 
}

void outputDecimal(uint8 h,uint8 l) //将m_PanelAddr转换为对应的ascll码，并显示到屏幕上
{  // value = 0~64
    if (m_PanelAddr >= 10) {
        // 输出十位
        OutOneChar(h, l, '0' + (m_PanelAddr / 10), 1);

        // 输出个位
        OutOneChar(h, l + 1, '0' + (m_PanelAddr % 10), 1);
    } else {

        // 输出个位
        OutOneChar(h, l, '0' + m_PanelAddr, 1);
    }
}

//手动启停设备
void ManualAct()
{
	uint16 loop,addr;
	if(m_FirstView!=m_View)
	{
	    m_FirstView=m_View;
	    Windows(100,100,700,500,"手动启停总线设备");		
		OutChars(3,8,"本机地址：",1);
		outputDecimal(3,20);

		OutChars(4,10,"请各输入3位机器、回路、地址",1);
		OutChars(6,10,"向上键为启动，",1);
		OutChars(7,10,"向下键为停止。",1);
		OutChars(9,4,"机器:",1);
		OutChars(9,16,"回路:",1);
		OutChars(9,30,"地址:",1);
	
		OutOneChar(9,11,'_',1);
		m_Cursor=11; //光标位置
    }

	switch(key)
	{
	case Key_Esc:
	{
		m_View=2;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	case Key_Up:  //启动
	{
		if(m_Cursor==49)
		{
			if (m_PanelAddr1==m_PanelAddr)
			{//启动本机设备
				m_CanCommand.sign=0x02;  //手动启动
				m_CanCommand.loop=m_Input[2];
				m_CanCommand.addr=m_Input[5];
				m_CanCommand.type=0;
				WriteCanCommand();
				OutChars(12,14,"启动命令已发出!",1);
			} else
			{//向从机设备发送启动指令
				m_Record.sign=0x32;  //手动启动
				m_Record.loop=m_Input[2];
				m_Record.addr=m_Input[5];
				m_Record.type=m_Input[8];
				WriteToWCANZone();
				OutChars(12,14,"启动命令已发出!",1);
			}

		}
	}
	break;
	case Key_Down:  //解屏蔽
	{
		if(m_Cursor==49)
		{
			if (m_PanelAddr1==m_PanelAddr)
			{//停止本机设备
				m_CanCommand.sign=0x03;  //手动停止
				m_CanCommand.loop=m_Input[2];
				m_CanCommand.addr=m_Input[5];
				m_CanCommand.type=0;
				WriteCanCommand();
				OutChars(12,14,"停止命令已发出!",1);
			} else
			{
				m_Record.sign=0x33;  //手动停止
				m_Record.loop=m_Input[2];
				m_Record.addr=m_Input[5];
				m_Record.type=m_Input[8];
				WriteToWCANZone();
				OutChars(12,14,"停止命令已发出!",1);
			}
		}				
	}
	break;  
	}
	if(key>0x29 && key<0x3A && m_Cursor<50 )
	{
		//有键按下
		switch(m_Cursor)
		{
			case 11:
			case 12://机器
		
		    case 23:
		    case 24://回路

		    case 37:
		    case 38://地址
		    {
				OutOneChar(9,m_Cursor,key,1);
				if(m_Cursor>25)
					m_Input[m_Cursor-34]=key-0x30;//地址
			    else if(m_Cursor>13)
					m_Input[m_Cursor-23]=key-0x30;//回路	
				else
					m_Input[m_Cursor-5]=key-0x30;//机器	
				m_Cursor++;
				OutOneChar(9,m_Cursor,'_',1);	 //光标
			    
		    }
		    break;

		    case 13:
		    {
			    OutOneChar(9,m_Cursor,key,1);
				//机器
				m_PanelAddr1=m_Input[6]*100+m_Input[7]*10+key-0x30;
			    if(m_PanelAddr1>0 && m_PanelAddr1<=PANEL_NET_MAX)
			    {
			        m_Input[8]=m_PanelAddr1;
			        m_Cursor=23;
			  		OutOneChar(9,m_Cursor,'_',1);	 //光标
			    }
				else
				{
					//输入错误
					OutChars(9,11,"_  ",1);
					m_Cursor=11; //光标位置
				}
			}
			break;

		    case 25:
		    {
			    OutOneChar(9,m_Cursor,key,1);
				//回路
				loop=m_Input[0]*100+m_Input[1]*10+key-0x30;
			    if((loop>0 && loop<33) || loop==101 || loop==102)
			    {
			        m_Input[2]=loop;
			        m_Cursor=37;
			  		OutOneChar(9,m_Cursor,'_',1);	 //光标
			    }
				else
				{
					//输入错误
					OutChars(9,23,"_  ",1);
					m_Cursor=23; //光标位置
				}
			}
			break;

		    case 39:
		    {
			    OutOneChar(9,m_Cursor,key,1);
				//地址
				addr=m_Input[3]*100+m_Input[4]*10+key-0x30;
			    if(addr>0 && addr<201)
			    {
			        m_Input[5]=addr;
			        m_Cursor=49;
			    }
				else
				{
					//输入错误
					OutChars(9,37,"_  ",1);
					m_Cursor=37; //光标位置
				}
		    }
		    break;
		}	
	}
	key=0;
}

//设置密码
void SetCode()
{
	if(m_FirstView !=m_View)
    {
        m_FirstView=m_View;
 		Windows(100,100,700,500,"设置二级密码");
		
		OutChars(5,8,"请输入六位密码后按'确认'键。",1);
    	OutOneChar(9,17,'_',1); //光标
		m_Cursor=17; //光标位置
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=6;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
		m_FirstView=0;
	}
	break;
	case Key_Enter:
	{
		if(m_Cursor==50)
		{
			m_Code[0]=m_Input[0];
			m_Code[1]=m_Input[1];
			m_Code[2]=m_Input[2];
			m_Code[3]=m_Input[3];
			m_Code[4]=m_Input[4];
			m_Code[5]=m_Input[5];
			//写入SPI FALSH
			SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
			Flash_Sector_Erase(1,1);   //清空
			HistoryBuf[11]=m_Code[0];	//二级密码
			HistoryBuf[12]=m_Code[1];	//二级密码
			HistoryBuf[13]=m_Code[2];	//二级密码
			HistoryBuf[14]=m_Code[3];	//二级密码
			HistoryBuf[15]=m_Code[4];	//二级密码
			HistoryBuf[16]=m_Code[5];	//二级密码

			Auto_Add_Write(0x1000,HistoryBuf,32);	//写入
			OutChars(12,10,"设置已完成，请牢记密码!",1);
		}		
	}
	break;
	}
	if(key>0x29 && key<0x3A && m_Cursor<50 )
	{
		//有键按下
		OutOneChar(9,m_Cursor,key,1);
		switch(m_Cursor)
		{
		    case 17:
		    {
			    m_Cursor++;
			    OutOneChar(9,m_Cursor,'_',1); //光标
			    m_Input[0]=key;
		    }
		    break;
		    case 18:
		    {
			    m_Cursor++;
			    OutOneChar(9,m_Cursor,'_',1); //光标
			    m_Input[1]=key;
		    }
		    break;
		    case 19:
		    {
			    m_Cursor++;
			    OutOneChar(9,m_Cursor,'_',1); //光标
			    m_Input[2]=key;
		    }
		    break;
		    case 20:
		    {
			    m_Cursor++;
			    OutOneChar(9,m_Cursor,'_',1); //光标
			    m_Input[3]=key;
		    }
		    break;
		    case 21:
		    {
			    m_Cursor++;
			    OutOneChar(9,m_Cursor,'_',1); //光标
			    m_Input[4]=key;
		    }
		    break;
		    case 22:
		    {
			    m_Input[5]=key;
				m_Cursor=50;
			}   
		    break;
		}	
	}		
	key=0;
}

//设置密码
void SetCode1()
{
	if(m_FirstView !=m_View)
    {
        m_FirstView=m_View;
 		Windows(100,100,700,500,"设置一级密码");
		
		OutChars(5,8,"请输入三位密码后按'确认'键。",1);
    	OutOneChar(9,17,'_',1); //光标
		m_Cursor=17; //光标位置
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=6;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
		m_FirstView=0;
	}
	break;
	case Key_Enter:
	{
		if(m_Cursor==50)
		{
			m_Code1[0]=m_Input[0];
			m_Code1[1]=m_Input[1];
			m_Code1[2]=m_Input[2];
			//写入SPI FALSH
			SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
			Flash_Sector_Erase(1,1);   //清空
			HistoryBuf[23]=m_Code1[0];	//一级密码
			HistoryBuf[24]=m_Code1[1];	//一级密码
			HistoryBuf[25]=m_Code1[2];	//一级密码

			Auto_Add_Write(0x1000,HistoryBuf,32);	//写入
			OutChars(12,10,"设置已完成，请牢记密码!",1);
		}		
	}
	break;
	}
	if(key>0x29 && key<0x3A && m_Cursor<50 )
	{
		//有键按下
		OutOneChar(9,m_Cursor,key,1);
		switch(m_Cursor)
		{
		    case 17:
		    {
			    m_Cursor++;
			    OutOneChar(9,m_Cursor,'_',1); //光标
			    m_Input[0]=key;
		    }
		    break;
		    case 18:
		    {
			    m_Cursor++;
			    OutOneChar(9,m_Cursor,'_',1); //光标
			    m_Input[1]=key;
		    }
		    break;
		    case 19:
		    {
			    m_Input[2]=key;
				m_Cursor=50;
		    }
		    break;
		}	
	}		
	key=0;
}

//设置灭火延时
void SetMieTime()
{
	uint8 num;
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
        Windows(100,100,700,500,"灭火延时设置");
		OutChars(4,10,"请先输入两位灭火分区号后",1);
		OutChars(5,10,"再输入两位延时时间，",1);
		OutChars(6,10,"然后按确定键设置完成。",1);
		OutChars(7,10,"分区:",1);
		OutOneChar(7,16,'_',1);
		m_Cursor=16; //光标位置
		m_Cursor1=7; //光标位置
	}

	if(m_Return==0x80)   //命令反馈标志
    {        
        m_Return=0x00;
        if(m_CanReturn.sign==0xA4) //返回查询 
        {
			OutChars(9,10,"当前灭火延时时间为  s，重设:",1);
			OutTwoChar(9,28,m_CanReturn.addr,1);
			OutChars(11,10,"延时:    s",1);			
			OutOneChar(11,16,'_',1);
			m_Cursor=16; //光标位置
			m_Cursor1=11; //光标位置
        }
        else if(m_CanReturn.sign==0xA5) //返回设置
        {
			OutTwoChar(9,28,m_CanReturn.addr,1);
			OutChars(12,10,"设置已完成!     ",1);
        } 
    }
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=5;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;	
	case Key_Enter:
	{
		if(m_Cursor==38)
		{
			m_CanCommand.sign=0x18;  //设置灭火延迟
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=m_Input[3];
			m_CanCommand.type=0;
			WriteCanCommand();					
			OutChars(12,10,"设置命令已发出!",1);
		}
	}
	break;
	}
	if(key>0x2F && key<0x3A && m_Cursor<38 )
	{
		if(m_Cursor==16)
		{
			 OutOneChar(m_Cursor1,16,key,1);
			 m_Input[0]=key-0x30;
			 m_Cursor++;
			 OutOneChar(m_Cursor1,17,'_',1);
		}
		else
		{
			OutOneChar(m_Cursor1,m_Cursor,key,1);
			num=m_Input[0]*10+key-0x30;
			if(m_Cursor1==7)
			{
				if(num<17 && num>0)
				{
					m_Cursor=40;
					m_Input[2]=num;	
					//发送查询命令
					m_CanCommand.sign=0x17;  //查询灭火延迟
					m_CanCommand.loop=m_Input[2];
					m_CanCommand.addr=0;
					m_CanCommand.type=0;
					WriteCanCommand();					
				}
				else
				{
					m_Cursor1=7;
					m_Cursor=16;
					OutChars(7,16,"_ ",1);					
				}
			}
			else if(m_Cursor1==11)
			{
				if(num<61)
				{
					m_Input[3]=num;	
					m_Cursor=38;
				}
				else
				{
					m_Cursor1=11;
					m_Cursor=16;
					OutChars(11,16,"_ ",1);					
				}
			}
				
		}
	}
	key=0;

}

//设置声光广播交替启动
void SetSGGB()
{
    uint8 num;
    if(m_FirstView!=m_View)
	{
        m_FirstView=m_View;
        Windows(100,100,700,500,"声光广播工作方式");
		if(bInturn)
		{
			OutChars(4,4,"当前为交替启动,声光启动  s,广播启动  s。",1);
			OutTwoChar(4,27,VoiceRunTime,1);
			OutTwoChar(4,39,BroadcastRunTime,1);
		}
		else
		{
			OutChars(4,12,"当前已设为声光广播分别启动。",1);			
		}
		OutChars(5,4,"重新设置,交替启动输入1,分别启动输入0:",1);
		OutChars(7,14,"工作方式:",1);
		OutChars(9,15,"声光(s):",1);
		OutChars(11,15,"广播(s):",1);
		OutOneChar(7,24,'_',1);
		m_Cursor=24; //光标位置
		m_Cursor1=7; //光标位置
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=5;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
		m_FirstView=0;
	}
	break;
	case Key_Enter:
	{
		if(m_Cursor==42)
		{
			bInturn=m_Input[2];
			VoiceRunTime=m_Input[3];
			BroadcastRunTime=m_Input[4];
			//写入SPI FALSH
			SPI_FLASH_HighSpeedReadCont(0x1000,32,HistoryBuf);	 //读出
			Flash_Sector_Erase(1,1);   //清空
			HistoryBuf[20]=bInturn;	//修改
			HistoryBuf[21]=VoiceRunTime;
			HistoryBuf[22]=BroadcastRunTime;
			Auto_Add_Write(0x1000,HistoryBuf,32);	//写入
			if(bInturn)
			{
				OutChars(4,4,"当前为交替启动,声光启动  s,广播启动  s。",1);
				OutTwoChar(4,27,VoiceRunTime,1);
				OutTwoChar(4,39,BroadcastRunTime,1);
			}
			else
			{
				OutChars(4,4,"当前已设为声光广播分别启动。            ",1);			
			}
			//配置
			OutChars(15,19,"设置已完成!",1);
		}
	}
	break;
	}
	if(key>0x2F && key<0x3A && m_Cursor<50 )
	{
		if(m_Cursor==24)
		{
			 if(m_Cursor1==7)
			 {
			 	if(key==0x30 || key==0x31)
				{
					OutOneChar(m_Cursor1,24,key,1);
					m_Input[2]=key-0x30;
					m_Cursor1=9;
					OutOneChar(m_Cursor1,24,'_',1);
				}
			 }
			 else
			 {
				 OutOneChar(m_Cursor1,24,key,1);
				 m_Input[0]=key-0x30;
				 m_Cursor++;
				 OutOneChar(m_Cursor1,25,'_',1);
			}
		}
		else
		{
			OutOneChar(m_Cursor1,m_Cursor,key,1);
			num=m_Input[0]*10+key-0x30;
			if(m_Cursor1==9)
			{
				if(num>5 && num<100)
				{
					m_Cursor1=11;
					m_Cursor=24;
					OutOneChar(11,24,'_',1);
					m_Input[3]=num;	
				}
				else
				{
					m_Cursor1=9;
					m_Cursor=24;
					OutChars(9,24,"_ ",1);					
				}
			}
			else if(m_Cursor1==11)
			{
				if(num>5 && num<100)
				{
					m_Input[4]=num;	
					m_Cursor=52;
				}
				else
				{
					m_Cursor1=11;
					m_Cursor=24;
					OutChars(11,24,"_ ",1);					
				}
			}				
		}
	}
	key=0;

}

//进入调试模式
void Tiaoshi()
{
	uint8 r;
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
        Windows(100,100,700,500,"进入调试模式");
		OutChars(5,10,"禁止或退出调试方式",1);
		OutChars(6,10,"向上键为进入调试，",1);
		OutChars(7,10,"向下键为退出。",1);
		OutChars(10,16,"当前模式:",1);
		if(bTiaoshi>0)
		{
			OutChars(10,26,"调试模式",1);	
		}
		else
		{
			OutChars(10,26,"正常运行",1);
		}
		OutOneChar(10,34,'_',1);
	}
	switch(key)
	{
	case Key_Esc:  
	{
		m_View=4;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	case Key_Up:  
	{
		OutChars(10,26,"调试模式",1);
		bTiaoshi=1;
		r=0;
		if(Fcolor !=WHITE || Bcolor !=BLUE)
		{
			SetColor(WHITE,BLUE);
			r=1;
		}
		TextOut(0,455,"调试模式",0);
		if(r) 	SetColor(Fcolor,Bcolor);
		
	}
	break;
	case Key_Down: 
	{
		bTiaoshi=0;
		OutChars(10,26,"正常运行",1);
		r=0;
		if(Fcolor !=WHITE || Bcolor !=BLUE)
		{
			SetColor(WHITE,BLUE);
			r=1;
		}
		TextOut(0,455,"正常运行",0);
		if(r) 	SetColor(Fcolor,Bcolor);
	}
	break;  
	}
	key=0;
}

//清除操作
void Qccl()
{
	uint16 i,num;
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
		Windows(100,100,700,500,"清除操作");
		OutChars(3,10,"请按数字键选择功能：",1);
		OutChars(5,14,"1.清除全部编程",1);
		OutChars(6,14,"2.清除本机注释",1);
		OutChars(7,14,"3.清除集中机注释",1);
	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=6;
			m_FirstView=0;
		}
		break;
		case 0x31:
			//清除本机编程
			OutChars(10,12,"清除编程,剩余",1);
			num=256;
			for(i=512;i<768;i++)
			{
				Flash_Sector_Erase(i,i);
				num--;
				OutTwoChar(10,25,num/100,1);
				OutTwoChar(10,27,num%100,1);
				fwdgt_counter_reload(); // 喂狗			
			}
			OutChars(10,12,"编程信息清除完成。      ",1);
			//m_LogicCycle=0;
			break;
		case 0x32:
			OutChars(10,12,"清除本机注释,剩余",1);
			num=50;
			for(i=1024;i<1074;i++)
			{
				Flash_Sector_Erase(i,i);
				num--;
				OutTwoChar(10,29,num/100,1);
				OutTwoChar(10,31,num%100,1);
				fwdgt_counter_reload(); // 喂狗			
			}
			OutChars(10,12,"本机注释清除完成。       ",1);
			break;
		case 0x33:
			//清除集中机注释
			OutChars(10,7,"清除集中机注释,剩余",1);
			num=500;
			for(i=1074;i<1574;i++)
			{
				Flash_Sector_Erase(i,i);
				num--;
				OutTwoChar(10,26,num/100,1);
				OutTwoChar(10,28,num%100,1);
				fwdgt_counter_reload(); // 喂狗			
			}
			OutChars(10,12,"集中机注释清除完成。         ",1);
			break;
    }
    key=0;
}

//USB文件读写
void SetUsbOpt()
{
	uint16 num;
	BaseType_t xReturn;
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
		Windows(100,100,700,500,"USB文件读写");
		OutChars(4,10,"请按数字键选择功能：",1);
		OutChars(6,14,"1.导入汉字注释文件",1);
		OutChars(7,14,"2.导入联动编程文件",1);
		OutChars(8,14,"3.导出汉字注释信息",1);
		OutChars(9,14,"4.导出联动编程信息",1);
		sendtousb_msg.flag=0;
	}
	
	xReturn=xQueueReceive(sendtomain_queue,&usbmsg1,0);
	if(pdTRUE == xReturn)
	{
		if(usbmsg1.flag==0x80)
		{
			OutChars(15-3,15,"USB读取错误。",1);
			sendtousb_msg.flag=0;
		}
		else if(usbmsg1.flag==0x81)
		{
			OutChars(15-3,15,"读取文件错误。",1);
			sendtousb_msg.flag=0;
		}
		else if(usbmsg1.flag==0x86)
		{
			OutChars(15-3,15,"创建文件错误。",1);
			sendtousb_msg.flag=0;
		}
		else if(usbmsg1.flag==0x82)
		{
			OutChars(15-3,15,"写入注释：   %。",1);
			OutThreeChar(15-3,25,usbmsg1.num,1);
			if(usbmsg1.num>=100)
				sendtousb_msg.flag=0;
		}
		else if(usbmsg1.flag==0x83)
		{
			OutChars(15-3,15,"写入编程：   %。",1);
			if(usbmsg1.num<100)
				OutThreeChar(15-3,25,usbmsg1.num,1);
			else
			{
				OutThreeChar(15-3,25,100,1);
				sendtousb_msg.flag=0;
			}
		}
		else if(usbmsg1.flag==0x84)
		{
			OutChars(15-3,15,"导出注释：   %。",1);
			OutThreeChar(15-3,25,usbmsg1.num,1);
			if(usbmsg1.num>=100)
				sendtousb_msg.flag=0;
		}
		else if(usbmsg1.flag==0x85)
		{
			OutChars(15-3,15,"导出编程：   %。",1);
			OutThreeChar(15-3,25,usbmsg1.num,1);
			if(usbmsg1.num>=100)
				sendtousb_msg.flag=0;
		}
			
	}
    switch(key)
    {
		case Key_Esc:
		{
			m_View=6;
			m_FirstView=0;
		}
		break;
		case 0x31:		
			//导入汉字注释
			if(sendtousb_msg.flag==0)
			{
				sendtousb_msg.flag=1;
				sendtousb_msg.num=0;
				xQueueSend(sendtousb_queue,&sendtousb_msg,0);//发送消息让usb任务读取u盘汉字注释
			}
			break;
		case 0x32:
			//导入联动编程
			if(sendtousb_msg.flag==0)
			{
				sendtousb_msg.flag=2;
				sendtousb_msg.num=0;
				xQueueSend(sendtousb_queue,&sendtousb_msg,0);//发送消息让usb任务读取u盘联动编程
			}
			break;
		case 0x33:
			//导出汉字注释
			if(sendtousb_msg.flag==0)
			{
				sendtousb_msg.flag=3;
				sendtousb_msg.num=0;
				xQueueSend(sendtousb_queue,&sendtousb_msg,0);
			}
			break;
		case 0x34:
			//导出联动编程
			if(sendtousb_msg.flag==0)
			{
				sendtousb_msg.flag=4;
				sendtousb_msg.num=0;
				xQueueSend(sendtousb_queue,&sendtousb_msg,0);
			}
			break;
    }
    key=0;
	
}

//电源数据
void TestPower()
{
	uint8 sign;
	uint16 powda;
	if(m_FirstView!=m_View)
	{
	    m_FirstView=m_View;
 		Windows(100,100,700,500,"电源测试数据");
		
		OutChars(4,8,"电源状态:",0);
		
		OutChars(7,8,"电池电量:",0);
		OutChars(8,8,"电池1电压:",0);
		OutChars(9,8,"电池2电压:",0);
		OutChars(10,8,"输出电压:",0);
		OutChars(11,8,"输出电流:",0);
		

    }
	if(bRevPowerData==0x01)   //命令反馈标志
    {        
        bRevPowerData=0x00;
		sign=PowerData[4];
		if(sign & 0x04)
			OutChars(4,18,"主电故障,",0);
		else
			OutChars(4,18,"主电正常,",0);
		if(sign & 0x08)
			OutChars(4,27,"备电故障,",0);
		else
			OutChars(4,27,"备电正常,",0);		
		if(sign & 0x70)
			OutChars(4,36,"输出故障,",0);
		else
			OutChars(4,36,"输出正常,",0);		
		if(sign & 0x80)
			OutChars(4,45,"备电已欠压,",0);
		else
			OutChars(4,45,"备电未欠压,",0);	
		sign=PowerData[3];
		if(sign & 0x01)
			OutChars(5,18,"电池1故障,",0);
		else
			OutChars(5,18,"电池1正常,",0);
		if(sign & 0x02)
			OutChars(5,28,"电池2故障",0);
		else
			OutChars(5,28,"电池2正常",0);		
		
		OutChars(7,20,"   %",0);
		if(PowerData[6]>99)
			OutThreeChar(7,20,PowerData[6],0);
		else
			OutTwoChar(7,21,PowerData[6],0);

		powda=PowerData[8]*256 + PowerData[7];
		OutChars(8,20,"  . V",0);
		OutTwoChar(8,20,powda/10,0);
		OutOneChar(8,23,powda%10 + 0x30,0);
		
		powda=PowerData[10]*256 + PowerData[9];
		OutChars(9,20,"  . V",0);
		OutTwoChar(9,20,powda/10,0);
		OutOneChar(9,23,powda%10 + 0x30,0);
		
		powda=PowerData[12]*256 + PowerData[11];		
		OutChars(10,20,"  . V",0);
		OutTwoChar(10,20,powda/10,0);
		OutOneChar(10,23,(powda%10)+0x30,0);
				
		powda=PowerData[13];		
		OutChars(11,20,"  . A",0);
		OutTwoChar(11,20,powda/10,0);
		OutOneChar(11,23,(powda%10)+0x30,0);

    }
	switch(key)
	{
	case Key_Esc:
	{
		m_View=4;
		m_FirstView=0;
	}
	break;
	}
	key=0;
}

//处理与黑匣子通讯
void CommToHxz()	
{
	uint8 i,j,k,sum,total;
	uint16 num;

	if(rcv_frame0)
	{
		rcv_frame0=0;
		//判断累加和
		total=ReceiveFrame0[2]+3;
		sum=0;
		for(i=0;i<total;i++)
			sum+=ReceiveFrame0[i];
		if(sum==ReceiveFrame0[total])
		{
			//判断命令
			hxzstate |=0x80;	//置收到黑匣子数据标志
			if(ReceiveFrame0[3]==0x02)
			{
				//重发命令
				ReceiveCount0=0;
				uart0Send();
			}
			else if(ReceiveFrame0[3]==0x01)
			{
				//读命令
				ReceiveCount0=0;
 				if(hxznum>0)
				{
					if(hxznum>10)
						total=10;
					else
						total=hxznum;
					
					sum=0;
					SendFrame0[0]=0x40;
					sum+=SendFrame0[0];
					SendFrame0[1]=0x7D;	//控制器
					sum+=SendFrame0[1];
					SendFrame0[2]=total*12 + 1;	//数据长度			
					sum+=SendFrame0[2];
					SendFrame0[3]=0x04;	//发送事件
					sum+=SendFrame0[3];
					j=4;
					for(i=0;i<total;i++)
					{
						for(k=0;k<12;k++)
						{
							SendFrame0[j]=HxzList[phxz2][k];
							sum+=SendFrame0[j];
							j++;
						}
						phxz2++;
						if(phxz2>255)
							phxz2=0;
						hxznum--;
					}
					SendFrame0[j]= sum;
					j++;
					SendFrame0[j]=0x41;	//结束符
					j++;
					TotalSendCount0=j;
					uart0Send();
				}
				else
				{
					//没有要发送的事件
					SendFrame0[0]=0x40;
					SendFrame0[1]=0x7D;	//控制器
					SendFrame0[2]=0x01;	//数据长度			
					SendFrame0[3]=0x03;	//应答命令
					SendFrame0[4]=0xC1;	//字节0~字节3的校验和 
					SendFrame0[5]=0x41;	//结束符
					TotalSendCount0=6;
					uart0Send();					
				}
			}
		}
	}	
}	

//计算CRC校验
void calccrc(uchar crcbuf)
{
	uchar i,TT;
	crc=crc ^ crcbuf;
	for(i=0;i<8;i++)
	{
		TT=crc&1;
		crc=crc>>1;
		crc=crc&0x7fff;
		if (TT==1)
			crc=crc^0xa001;
	}
//	for(i=0;i<8;i++)
//	{
//		TT=((crcbuf<<i) & 0x80) ^ ((crc & 0x8000 ) >> 8);
//		crc=crc<<1;
//		if (TT!=0)
//			crc=crc^0xa001;
//	}
}

uint8 CommToDisable1()	//判断通过新协议CRT发送屏蔽
{
	INT8U i,buf[8];
	INT32U pAddr;
	INT16U total;
	if(bHaveSendDis==0)
	{
		if(m_SendDisNum<Total_Isolate)
		{
			//需要初始发送屏蔽
			pAddr=E_DisableAddr+m_SendDisNum*8+8;
			SPI_FLASH_HighSpeedReadCont(pAddr,8,buf);
			
			HistoryBuf[0]=0x91;
			if(buf[0]==0)
				HistoryBuf[1]=m_PanelAddr;	//机器号
			else
				HistoryBuf[1]=buf[0];
			
			HistoryBuf[2]= buf[1];	//回路
			HistoryBuf[3]= buf[2];	//地址
			HistoryBuf[4]= 0;	//类型
			
			for(i=0;i<5;i++)
                HistoryBuf[i+5]=buf[i+3];
			HistoryBuf[10] =0;	//秒
			
			m_SendDisNum++;
			if(m_SendDisNum>=Total_Isolate)
				bHaveSendDis=1;	
			return 1;
		}
		else
		{
			bHaveSendDis=1;	
			return 0;	
		}	
	}
	else if(m_pCRTDis !=m_pWriteDis)
	{
			//有实时的屏蔽或解除需发送
			HistoryBuf[0]=DisableList[m_pCRTDis].sign;
			if(DisableList[m_pCRTDis].panel==0)
				HistoryBuf[1]=m_PanelAddr;	//机器号
			else
	        	HistoryBuf[1]=DisableList[m_pCRTDis].panel;
	        HistoryBuf[2]=DisableList[m_pCRTDis].loop;
	        HistoryBuf[3]=DisableList[m_pCRTDis].addr;
	        HistoryBuf[4]=DisableList[m_pCRTDis].type;
	        HistoryBuf[5]=DisableList[m_pCRTDis].year;
	        HistoryBuf[6]=DisableList[m_pCRTDis].month;
	        HistoryBuf[7]=DisableList[m_pCRTDis].day;
	        HistoryBuf[8]=DisableList[m_pCRTDis].hour;
	        HistoryBuf[9]=DisableList[m_pCRTDis].minute;
	        HistoryBuf[10]=DisableList[m_pCRTDis].second;

	        m_pCRTDis++;
			if(m_pCRTDis>29)	m_pCRTDis=0;	
			return 1;
	}
	return 0;	
}

uint8 Gettype(uint8 type)
{
	uint8 tt=0;
	switch(type)
	{
		case 01:	tt=23;	break;
		case 02:	tt=31;	break;
		case 03:	tt=50;	break;
		case 04:	tt=61;	break;
		case 05:	tt=75;	break;		//电缆接口
		case 06:	tt=75;	break;		//探测接口
		case 07:	tt=71;	break;
		case 8:		tt=72;	break;
		case 9:		tt=75;	break;
		
		case 10:	tt=62;	break;
		case 11:	tt=72;	break;
		case 12:	tt=73;	break;
		case 13:	tt=74;	break;
		case 14:	tt=71;	break;
		case 15:	tt=71;	break;
		case 16:	tt=10;	break;
		case 17:	tt=82;	break;
		case 18:	tt=86;	break;
		case 19:	tt=71;	break;
		case 20:	tt=71;	break;
		case 25:	tt=102;	break;
		case 26:	tt=102;	break;
		case 28:	tt=102;	break;
		
		default:	tt=0;	break;
	}
	return tt;
}

//准备图显、电气监控设备发送帧数据
void  PrepareSend(uint8 eventtype,uint8 source)
{
	uint8 i,matter;
	uint8 devicetype;
	uint8 pa,s1,s2;	//s1,s2为状态代码
	pa=eventtype;
	s1=0x00;
	s2=0x00;
	switch(HistoryBuf[0])
	{
	case 0x80:
		if((HistoryBuf[4] & 0x1F)==0x07)
			matter=70;
		else
			matter=3;
		devicetype=Gettype(HistoryBuf[4] & 0x1F);
		s1=0x08;
		s2=0x00;
		break;
	case 0x83:
		matter=19;
		if(HistoryBuf[2]<100)
			devicetype=Gettype(HistoryBuf[4] & 0x1F);
		else
			devicetype=0;
		s1=0x10;
		s2=0x00;
		break;
	case 0x84:
		matter=29;
		if(HistoryBuf[2]<100)
			devicetype=Gettype(HistoryBuf[4] & 0x1F);
		else
			devicetype=0;
		s1=0x00;
		s2=0x00;
		break;
	case 0x85:
		matter=26;
		if(HistoryBuf[2]<100)
			devicetype=Gettype(HistoryBuf[4] & 0x1F);
		else
			devicetype=0;
		s1=0x20;
		s2=0x00;
		break;
	case 0x86:
		matter=28;
		if(HistoryBuf[2]<100)
			devicetype=Gettype(HistoryBuf[4] & 0x1F);
		else
			devicetype=0;
		s1=0x00;
		s2=0x00;
		break;
	case 0x7A:	//燃气低警
		matter=5;
		devicetype=Gettype(HistoryBuf[4] & 0x1F);
		s1=0x08;
		s2=0x00;
		break;
	case 0x7B:	//燃气高警
		matter=6;
		devicetype=Gettype(HistoryBuf[4] & 0x1F);
		s1=0x08;
		s2=0x00;
		break;	
	case 0x01:
		matter=120;
		devicetype=1;
		s1=0x00;
		s2=0x00;
		break;
	case 0x02:
		matter=123;
		devicetype=1;
		s1=0x00;
		s2=0x00;
		break;
	case 0x03:
		if(source==1)
		{
			//复位命令电气控制装置发送设置数据帧
			matter=0;
			//WriteEleSend(1);
		}
		else
		{
			matter=122;
			devicetype=1;
			pa=1;
		}
		s1=0x00;
		s2=0x00;
		break;
	case 0x04:
		matter=121;
		devicetype=1;
		s1=0x00;
		s2=0x00;
		break;
	case 0x05:
		if(HistoryBuf[1]==0xC0)
			matter=126;
		else
			matter=125;		
		devicetype=1;
		pa=1;
		s1=0x00;
		s2=0x00;
		break;
	case 0x81:	//故障
		if(HistoryBuf[2]<100)
		{
			//总线部件故障
			devicetype=Gettype(HistoryBuf[4] & 0x1F);
		}
		else
			devicetype=0;			
		matter=80;
		s1=0x80;
		s2=0x00;
		break;
	case 0x82:	//故障恢复
		if(HistoryBuf[2]<100)
		{
			//总线部件故障
			devicetype=Gettype(HistoryBuf[4] & 0x1F);
		}
		else
			devicetype=0;			
		matter=100;
		s1=0x00;
		s2=0x00;
		break;
	case 0x87:	//回路故障
		devicetype=13;			
		matter=85;
		break;
	case 0x88:	//回路故障恢复
		devicetype=13;			
		matter=105;
		s1=0x80;
		s2=0x00;
		break;
	case 0x89:	//其它故障
		if(HistoryBuf[2]==1)	//回路板，扩展板故障
		{
			devicetype=0;			
			matter=81;
		}
		else if(HistoryBuf[2]==2)	//电源通讯故障
		{
			devicetype=158;			
			matter=81;			
		}
		else if(HistoryBuf[2]==3)	//主电故障
		{
			devicetype=14;			
			matter=82;			
		}
		else if(HistoryBuf[2]==4)  //备电故障
		{
			devicetype=15;			
			matter=83;			
		}
		else if(HistoryBuf[2]==5)  //声光开路
		{
			devicetype=17;			
			matter=87;			
		}
		else if(HistoryBuf[2]==6)  //声光短路
		{
			devicetype=17;			
			matter=87;			
		}
		else if(HistoryBuf[2]==7)  //机器故障
		{
			devicetype=1;			
			matter=81;			
		}
		else if(HistoryBuf[2]==8)  //欠压故障
		{
			devicetype=15;			
			matter=80;			
		}
		else if(HistoryBuf[2]==9)  //电源输出故障
		{
			devicetype=211;			
			matter=80;			
		}
		else if(HistoryBuf[2]==0x0A)  //黑匣子故障
		{
			devicetype=18;			
			matter=80;			
		}
		else if(HistoryBuf[2]==0x0A)  //多线盘/总线盘故障
		{
			devicetype=212;			
			matter=80;			
		}
		s1=0x80;
		s2=0x00;
		break;
	case 0x8A:	//其它故障恢复
		if(HistoryBuf[2]==1)	//回路板，扩展板故障
		{
			devicetype=0;			
			matter=101;
		}
		else if(HistoryBuf[2]==2)	//电源通讯故障
		{
			devicetype=158;			
			matter=101;			
		}
		else if(HistoryBuf[2]==3)	//主电故障
		{
			devicetype=14;			
			matter=102;			
		}
		else if(HistoryBuf[2]==4)  //备电故障
		{
			devicetype=15;			
			matter=103;			
		}
		else if(HistoryBuf[2]==5)  //声光开路
		{
			devicetype=17;			
			matter=107;			
		}
		else if(HistoryBuf[2]==6)  //声光短路
		{
			devicetype=17;			
			matter=107;			
		}
		else if(HistoryBuf[2]==7)  //机器故障
		{
			devicetype=1;			
			matter=101;			
		}
		else if(HistoryBuf[2]==8)  //欠压故障
		{
			devicetype=15;			
			matter=100;			
		}
		else if(HistoryBuf[2]==9)  //电源输出故障
		{
			devicetype=211;			
			matter=100;			
		}
		else if(HistoryBuf[2]==0x0A)  //黑匣子故障
		{
			devicetype=18;			
			matter=100;			
		}
		else if(HistoryBuf[2]==0x0A)  //多线盘/总线盘故障
		{
			devicetype=212;			
			matter=100;			
		}
		s1=0x00;
		s2=0x00;
		break;	
	case 0x91:	//屏蔽
		matter=72;
		devicetype=0;
		s1=0x00;
		s2=0x01;
		break;
	case 0x92:	//屏蔽解除
		matter=73;
		devicetype=0;
		s1=0x00;
		s2=0x00;
		break;
	default:
		matter=0;
	}
	if(matter>0)
	{
		if(source==0)
		{
			//图显
			for(i=0;i<4;i++)
			{
				FecbusSendFrame[i].type=0x00;
				FecbusSendFrame[i].target=FecbusTarget;
				FecbusSendFrame[i].pa=pa;
				FecbusSendFrame[i].source=FecbusSoure;
				FecbusSendFrame[i].mnum=fwater;
				if(i<3)
					FecbusSendFrame[i].tnum=i+1;
				else 
					FecbusSendFrame[i].tnum=0;
			}
			FecbusSendFrame[0].dlc=0x08;
			if(pa==1)
				FecbusSendFrame[0].da[0]=0x05;	//通告紧急事件
			else
				FecbusSendFrame[0].da[0]=0x06;	//通告一般事件
			if(HistoryBuf[1]==0)
				FecbusSendFrame[0].da[1]=m_PanelAddr;
			else
				FecbusSendFrame[0].da[1]=HistoryBuf[1];
			FecbusSendFrame[0].da[2]=HistoryBuf[2];
			FecbusSendFrame[0].da[3]=HistoryBuf[3];
			FecbusSendFrame[0].da[4]=0;
			FecbusSendFrame[0].da[5]=devicetype;
			FecbusSendFrame[0].da[6]=0;
			FecbusSendFrame[0].da[7]=matter;
			
			FecbusSendFrame[1].dlc=0x08;
			FecbusSendFrame[1].da[0]=0x00;	//事件代码高位
			FecbusSendFrame[1].da[1]=s1;	//状态代码
			FecbusSendFrame[1].da[2]=s2;
			FecbusSendFrame[1].da[3]=HistoryBuf[5];	//byte_to_bcd(HistoryBuf[5]);	//年
			FecbusSendFrame[1].da[4]=HistoryBuf[6];	//byte_to_bcd(HistoryBuf[6]);
			FecbusSendFrame[1].da[5]=HistoryBuf[7];	//byte_to_bcd(HistoryBuf[7]);
			FecbusSendFrame[1].da[6]=HistoryBuf[8];	//byte_to_bcd(HistoryBuf[8]);
			FecbusSendFrame[1].da[7]=HistoryBuf[9];	//byte_to_bcd(HistoryBuf[9]);
			
			FecbusSendFrame[2].dlc=0x01;
			FecbusSendFrame[2].da[0]=HistoryBuf[10];	//byte_to_bcd(HistoryBuf[10]); //秒
			
			FecbusSendFrame[3].dlc=0x02;
			FecbusSendFrame[3].da[0]=0x0F; //结束帧
			FecbusSendFrame[3].da[1]=0x00;
			
			fwater++;
			if(fwater>63)	fwater=1;
			
			bNeedSendFecbus=1;
			FecbusSendNum=4;
			FecbusSend1=0;
			RepeatTimes=0;	
		}	
		else
		{
			//电气控制装置
//			for(i=0;i<4;i++)
//			{
//				FecbusEleSendFrame[i].type=0x00;
//				FecbusEleSendFrame[i].target=0x00;		//广播
//				FecbusEleSendFrame[i].pa=pa;
//				FecbusEleSendFrame[i].source=FecbusSoure;
//				FecbusEleSendFrame[i].mnum=fwater;
//				if(i<3)
//					FecbusEleSendFrame[i].tnum=i+1;
//				else 
//					FecbusEleSendFrame[i].tnum=0;
//			}
//			FecbusEleSendFrame[0].dlc=0x08;
//			if(pa==1)
//				FecbusEleSendFrame[0].da[0]=0x05;	//通告紧急事件
//			else
//				FecbusEleSendFrame[0].da[0]=0x06;	//通告一般事件
//			if(HistoryBuf[1]==0)
//				FecbusEleSendFrame[0].da[1]=m_PanelAddr;
//			else
//				FecbusEleSendFrame[0].da[1]=HistoryBuf[1];
//			FecbusEleSendFrame[0].da[2]=HistoryBuf[2];
//			FecbusEleSendFrame[0].da[3]=HistoryBuf[3];
//			FecbusEleSendFrame[0].da[4]=0;
//			FecbusEleSendFrame[0].da[5]=devicetype;
//			FecbusEleSendFrame[0].da[6]=0;
//			FecbusEleSendFrame[0].da[7]=matter;
//			
//			FecbusEleSendFrame[1].dlc=0x08;
//			FecbusEleSendFrame[1].da[0]=0x00;	//事件代码高位
//			FecbusEleSendFrame[1].da[1]=0x00;	//状态代码为0
//			FecbusEleSendFrame[1].da[2]=0x00;
//			FecbusEleSendFrame[1].da[3]=HistoryBuf[5];	//年
//			FecbusEleSendFrame[1].da[4]=HistoryBuf[6];
//			FecbusEleSendFrame[1].da[5]=HistoryBuf[7];
//			FecbusEleSendFrame[1].da[6]=HistoryBuf[8];
//			FecbusEleSendFrame[1].da[7]=HistoryBuf[9];
//			
//			FecbusEleSendFrame[2].dlc=0x01;
//			FecbusEleSendFrame[2].da[0]=HistoryBuf[10]; //秒
//			
//			FecbusEleSendFrame[3].dlc=0x02;
//			FecbusEleSendFrame[3].da[0]=0x0F; //结束帧
//			FecbusEleSendFrame[3].da[1]=0x00;
//			
//			Elefwater++;
//			if(Elefwater>63)	Elefwater=1;
//			
//			bNeedSendEleFecbus=1;
//			FecbusEleSendNum=4;
//			FecbusEleSend1=0;
//			EleRepeatTimes=0;				
		}			
	}
	
}

// void InitDeviceList() {
//     for (int i = 0; i < DEVICE_TYPE_COUNT; i++) {
//         TongXinBan_device_count[i] = 0;
//         for (int j = 0; j < MAX_DEVICES_PER_TYPE; j++) {
//             TongXinBan_device_list[i][j].registered = 0;
//             TongXinBan_device_list[i][j].fault = 0;
//         }
//     }
// }

// // 登记设备
// void RegisterDevice(uint8_t type, uint16_t loop, uint16_t addr) {
//     uint8_t type_index = GetTongXinBan_type_index(type, loop, addr);
//     if (type_index >= DEVICE_TYPE_COUNT) {
//         return;  // 无效的设备类型
//     }
    
//     // 检查是否已经登记过
//     for (int i = 0; i < TongXinBan_device_count[type_index]; i++) {
//         if (TongXinBan_device_list[type_index][i].loop == loop && 
//             TongXinBan_device_list[type_index][i].addr == addr) {
//             return;  // 已登记，不重复登记
//         }
//     }
    
//     // 登记新设备
//     if (TongXinBan_device_count[type_index] < MAX_DEVICES_PER_TYPE) {
//         TongXinBan_device_list[type_index][TongXinBan_device_count[type_index]].loop = loop;
//         TongXinBan_device_list[type_index][TongXinBan_device_count[type_index]].addr = addr;
//         TongXinBan_device_list[type_index][TongXinBan_device_count[type_index]].type = type;
//         TongXinBan_device_list[type_index][TongXinBan_device_count[type_index]].registered = 1;
//         TongXinBan_device_count[type_index]++;
//     }
// }

// // 根据回路和地址查找设备索引
// int8_t FindDeviceIndex(uint8_t type, uint16_t loop, uint16_t addr) {
//     uint8_t type_index = GetTongXinBan_type_index(type, loop, addr);
//     if (type_index >= DEVICE_TYPE_COUNT) {
//         return -1;  // 无效的设备类型
//     }
    
//     // 在已登记的设备中查找
//     for (int i = 0; i < TongXinBan_device_count[type_index]; i++) {
//         if (TongXinBan_device_list[type_index][i].loop == loop && 
//             TongXinBan_device_list[type_index][i].addr == addr &&
//             TongXinBan_device_list[type_index][i].registered == 1) {
//             return i;  // 找到设备，返回索引（0-based）
//         }
//     }
    
//     return -1;  // 未找到设备
// }

// // 更新通信板故障状态
// void UpdateTongXinBanFaultStatus(uint16_t loop, uint16_t addr, uint8_t type) {
//     uint8_t type_index = GetTongXinBan_type_index(type, loop, addr);
//     if (type_index >= DEVICE_TYPE_COUNT) {
//         return;
//     }
    
//     int8_t device_index = FindDeviceIndex(type, loop, addr);
//     if (device_index < 0) {
//         return;  // 设备未登记
//     }
    
//     // 设置故障位（第2位）
//     TongXinBan_value[type_index][device_index] |= 0x04;
    
//     // 更新设备列表中的故障状态
//     TongXinBan_device_list[type_index][device_index].fault = 1;
// }

// // 清除通信板故障状态
// void ClearTongXinBanFaultStatus(uint16_t loop, uint16_t addr, uint8_t type) {
//     uint8_t type_index = GetTongXinBan_type_index(type, loop, addr);
//     if (type_index >= DEVICE_TYPE_COUNT) {
//         return;
//     }
    
//     int8_t device_index = FindDeviceIndex(type, loop, addr);
//     if (device_index < 0) {
//         return;  // 设备未登记
//     }
    
//     // 清除故障位（第2位）
//     TongXinBan_value[type_index][device_index] &= ~0x04;
    
//     // 更新设备列表中的故障状态
//     TongXinBan_device_list[type_index][device_index].fault = 0;
// }


void GetRegisterIndex(uint16 reg_addr)
{
	uint16 i;
	for (int i = 0; i < DEVICE_TYPE_COUNT; i++) {
        if (reg_addr >= TongXinBan_START_ADDR[i] && 
            reg_addr < TongXinBan_START_ADDR[i] + TongXinBan_REGISTER_COUNT[i]) {
            ModbusDeviceType = (TongXinBanDeviceType)i;
            ModbusDeviceIndex = reg_addr - TongXinBan_START_ADDR[i];
			return;// 找到后立即返回
        }
    }
	// 如果未找到，设置为无效值
	ModbusDeviceType = 0xFF;
    ModbusDeviceIndex = 0xFF;
}

/* Modbus读取通讯板寄存器时从这里取值。气体状态/浓度寄存器被读取后，安排一次后台气体数据刷新；浓度地址直接读气体浓度缓存。 */
uint16 GetTongXinBanRegisterValue(uint16 reg_addr)
{
	if((reg_addr >= TongXinBan_START_ADDR[DEVICE_KeRanQi] &&
	    reg_addr < TongXinBan_START_ADDR[DEVICE_KeRanQi] + TongXinBan_REGISTER_COUNT[DEVICE_KeRanQi]) ||
	   (reg_addr >= KERANQI_UPLOAD_CONC_START && reg_addr < TONGXINBAN_TOTAL_REGISTER_COUNT))
	{
		/* Prioritize the gas snapshot when Modbus polls gas status or concentration. */
		/* 不覆盖实时显示/手动登记界面的CAN流程；普通Modbus读取气体寄存器时才创建后台刷新请求。 */
if(m_View != 66 && m_View != 31 && kr_query_priority == 0)
		{
			kr_query_priority = 1;
			if(need_query_kr_register == 0)
			{
				need_query_kr_register = 1;
				boot_query_kr_retry = 0;
			}
			boot_query_kr_delay = 500;
		}
		if(reg_addr >= KERANQI_UPLOAD_CONC_START && reg_addr < TONGXINBAN_TOTAL_REGISTER_COUNT)
			return KeRanQiNongDu_value[reg_addr - KERANQI_UPLOAD_CONC_START];
	}

	GetRegisterIndex(reg_addr);
	if(ModbusDeviceType == 0xFF || ModbusDeviceIndex == 0xFF)
	{
		return 0;
	}
	return TongXinBan_value[ModbusDeviceType][ModbusDeviceIndex];
}
/* 处理通讯板Modbus请求。FC03响应必须包含请求数量对应的全部寄存器；超出当前映射但地址仍合法的寄存器由取值函数返回0，不能缩短响应，否则主站会因帧长度不符而丢弃。 */
void CommToTongXinBan()
{
	uint16 i,j,actual_reg_num,current_addr,value;
	uint8 crch,crcl,result,result1,type,idx,TongXinBan_data[256];
	uint32 pAddr;
	uint8 buf[17];
	static uint8_t rcv_timeout_cnt1 = 0;
//	static uint8_t rcv_timeout_cnt1 = 0; //0716

	//0716 接收超时重置接收状态
//	if(ReceiveCount1 > 0 && rcv_frame1 == 0)
//	{
//		rcv_timeout_cnt1++;
//		if(rcv_timeout_cnt1 > 50)  //0716 约100ms超时（主循环~2ms一次）
//		{
//			ReceiveCount1 = 0;
//			rcv_timeout_cnt1 = 0;
//		}
//	}
//	else
//	{
//		rcv_timeout_cnt1 = 0;
//	}

	//先判断接收
	/* 接收计数非零表示收到部分请求。主循环每约2ms检查一次，超过50次（约100ms）仍未收完整帧就清掉残留计数，防止下一条8字节请求与残帧拼接。 */
	if(ReceiveCount1 > 0 && rcv_frame1 == 0)
	{
		rcv_timeout_cnt1++;
		if(rcv_timeout_cnt1 > 50)
		{
			ReceiveCount1 = 0;
			rcv_timeout_cnt1 = 0;
		}
	}
	else
	{
		rcv_timeout_cnt1 = 0;
	}
	if(rcv_frame1)
	{
		rcv_frame1=0;
		//判断校验
		crc=0xFFFF;

		for(i=0;i<6;i++)
			calccrc(ReceiveTongXinBan[i]);
		
		if(ReceiveTongXinBan[6]==(crc & 0xFF) && ReceiveTongXinBan[7]==(crc/256))
		{//校验成功		
			Rev_Modbus_Data.addr = ReceiveTongXinBan[0];
			Rev_Modbus_Data.function_code = ReceiveTongXinBan[1];
			Rev_Modbus_Data.register_addr = (ReceiveTongXinBan[2]<<8) + ReceiveTongXinBan[3];
			Rev_Modbus_Data.register_num = (ReceiveTongXinBan[4]<<8) + ReceiveTongXinBan[5];			

			if((Rev_Modbus_Data.addr==m_PanelAddr)&&(Rev_Modbus_Data.function_code==0x03))//判断是否为本机地址
			{
				idx = 0;
				actual_reg_num = Rev_Modbus_Data.register_num;
				/* FC03一次最多读125个寄存器。数量非法返回异常码03；地址区间溢出返回异常码02。 */
if(actual_reg_num == 0 || actual_reg_num > 125 ||
				   ((uint32)Rev_Modbus_Data.register_addr + actual_reg_num) > 0x10000UL)
				{
					TongXinBan_data[idx++] = m_PanelAddr;
					TongXinBan_data[idx++] = Rev_Modbus_Data.function_code | 0x80;
					/* Modbus异常码：03=非法数据值（数量不合法），02=非法数据地址（寄存器范围越过0xFFFF）。 */
TongXinBan_data[idx++] = (actual_reg_num == 0 || actual_reg_num > 125) ? 0x03 : 0x02;
					crc = 0xFFFF;
					for(i=0;i<idx;i++)
						calccrc(TongXinBan_data[i]);
					TongXinBan_data[idx++] = crc & 0xFF;
					TongXinBan_data[idx++] = (crc >> 8) & 0xFF;
					for(i=0;i<idx;i++)
						SendFrame1[i] = TongXinBan_data[i];
					TotalSendCount1 = idx;
					uart1Send();
					return;
				}

				TongXinBan_data[idx++] = m_PanelAddr;
				TongXinBan_data[idx++] = Rev_Modbus_Data.function_code;
				/* 正常03响应的字节数字段。每个寄存器2字节，因此必须依据请求数量，而不是映射表剩余数量计算。 */
TongXinBan_data[idx++] = actual_reg_num * 2;

				/* 按主站请求逐个生成寄存器值；未知地址由GetTongXinBanRegisterValue返回0，保持帧长度完整。 */
for (i = 0; i < actual_reg_num; i++) {
					current_addr = Rev_Modbus_Data.register_addr + i;
					value = GetTongXinBanRegisterValue(current_addr);
					TongXinBan_data[idx++] = (value >> 8) & 0xFF;
					TongXinBan_data[idx++] = value & 0xFF;
				}

				// Send_Modbus_Data.addr = m_PanelAddr;
				// Send_Modbus_Data.function_code = 0x03;
				// Send_Modbus_Data.byte_num = Rev_Modbus_Data.register_num * 2;
				// Send_Modbus_Data.send_data = TongXinBan_value[ModbusDeviceType][ModbusDeviceIndex];

				// SendFrameModBus[0] = Send_Modbus_Data.addr;
				// SendFrameModBus[1] = Send_Modbus_Data.function_code;
				// SendFrameModBus[2] = Send_Modbus_Data.byte_num;
				// SendFrameModBus[3] = Send_Modbus_Data.send_data>>8;
				// SendFrameModBus[4] = Send_Modbus_Data.send_data;		
				crc=0xFFFF;//计算校验
				for(i=0;i<idx;i++)
					calccrc(TongXinBan_data[i]);
				TongXinBan_data[idx++]=crc & 0xFF;
				TongXinBan_data[idx++]=(crc >> 8) & 0xFF;
				for (int i = 0; i < idx; i++) 
				{
           			SendFrame1[i] = TongXinBan_data[i];
        		}
        		TotalSendCount1 = idx;
        		uart1Send();
			}else
			{
				//本机地址错误
				ReceiveCount1 = 0;
				return;
			}

		} else
		{ 
			//校验错误
			ReceiveCount1 = 0;
			return;
		}
	
	}
}

//与图显通讯
void CommToCRT()
{
	uint16 i,j,total;
	uint8 crch,crcl,result,result1,type;
	uint32 pAddr;
	uint8 buf[17];
	//先判断接收
	if(rcv_frame2)
	{
		rcv_frame2=0;
		//判断校验
		total=RevCRTFrm[7]+8;
		crc=0xFFFF;
		for(i=0;i<total;i++)
			calccrc(RevCRTFrm[i]);
		crch=(crc & 0xFF00)>>8;
		crcl=(crc & 0xFF);
		if(RevCRTFrm[i]==crcl && RevCRTFrm[i+1]==crch)     
		{
			bNeedAns=0;
			if(RevCRTFrm[6]==0)
			{
				//单帧数据或通告事件的最后帧
				if(RevCRTFrm[8]==0x21)
				{
					//巡检CRT的应答帧
					bRevAns=1;					
				}
				else if(RevCRTFrm[8]==0x00)
				{
					//心跳的应答，暂不处理
				}
				else if(RevCRTFrm[8]==0x0F && RevCRTFrm[9]==0x00)
				{
					//消防电气控制装置的通告帧,最后帧
					i= EleFrameNum;
					FecbusRevFrame[i].type=RevCRTFrm[1];
					FecbusRevFrame[i].target=RevCRTFrm[2];
					FecbusRevFrame[i].pa=RevCRTFrm[3];
					FecbusRevFrame[i].source=RevCRTFrm[4];
					FecbusRevFrame[i].mnum=RevCRTFrm[5];
					FecbusRevFrame[i].tnum=RevCRTFrm[6];
					
					FecbusRevFrame[i].dlc=RevCRTFrm[7];
					for(j=0;j<8;j++)
						FecbusRevFrame[i].da[j]=RevCRTFrm[8+j];
					
					//判断数据
					for(i=0;i<8;i++)
						buf[i]=FecbusRevFrame[0].da[i];
					for(i=0;i<8;i++)
						buf[i+8]=FecbusRevFrame[1].da[i];		
					buf[16]=FecbusRevFrame[2].da[0];
					if(buf[0]==0x11 || buf[0]==0x12)
					{
						if(buf[8]==131)
						{
							//调整时钟
							D_Year=buf[11];
							D_Month=buf[12];
							D_Day=buf[13];
							D_Hour=buf[14];
							D_Minute=buf[15];
							D_Second=buf[16];

							rtc_setup(); 
							if(m_PanelType)
							{
								//主机需发送时间同步命令
								bTimeSync=1;
								bRevCRTCommDelay=7;
							}							
							m_Record.sign=0xA0;		//特殊事件
							m_Record.panel=m_PanelAddr;
							m_Record.confirm=131;	//调整时钟
							WriteToHxzZone();	//写黑匣子发送区							
						}
						else if(buf[8]==127)
						{
							//消音
						}
					}
				}
			}
			else
			{
				//多帧事件
				i=RevCRTFrm[6]-1;	//帧号
				FecbusRevFrame[i].type=RevCRTFrm[1];
				FecbusRevFrame[i].target=RevCRTFrm[2];
				FecbusRevFrame[i].pa=RevCRTFrm[3];
				FecbusRevFrame[i].source=RevCRTFrm[4];
				FecbusRevFrame[i].mnum=RevCRTFrm[5];
				FecbusRevFrame[i].tnum=RevCRTFrm[6];
				
				FecbusRevFrame[i].dlc=RevCRTFrm[7];
				for(j=0;j<8;j++)
					FecbusRevFrame[i].da[j]=RevCRTFrm[8+j];
				EleFrameNum=RevCRTFrm[6];
				TimeCount3=0;
			}
		}
	}

	//200ms判断一次
	if(TimeCount3<99)
		return;
	TimeCount3=0;
		
	if(bNeedAns)
	{
		if(bRevAns)
		{
			bRevAns=0;
			bNeedAns=0;
			NormalTimesDelay=0;
			
			FecbusSend1=0;
			FecbusSendNum=0;
			bNeedSendFecbus=0;			
		}
		else
		{
			AnsTimesDelay++;
			if(AnsTimesDelay>4)
			{
				//1s时间到
				AnsTimesDelay=0;
				RepeatTimes++;
				if(RepeatTimes<3)
				{
					FecbusSend1=0;
					bNeedSendFecbus=1;
					bNeedAns=0;
				}
				else
				{
					//发送失败
					RepeatTimes=0;
					bNeedAns=0;
					NormalTimesDelay=0;
					
					FecbusSend1=0;
					FecbusSendNum=0;
					bNeedSendFecbus=0;			
				}
			}
		}
	}
	else if(bNeedSendFecbus)
	{
		if(FecbusSend1<FecbusSendNum)
		{
			//发送数据
			SendFrame2[0]=0x7E;
			SendFrame2[1]=FecbusSendFrame[FecbusSend1].type;
			SendFrame2[2]=FecbusSendFrame[FecbusSend1].target;
			SendFrame2[3]=FecbusSendFrame[FecbusSend1].pa;
			SendFrame2[4]=FecbusSendFrame[FecbusSend1].source;
			SendFrame2[5]=FecbusSendFrame[FecbusSend1].mnum;
			SendFrame2[6]=FecbusSendFrame[FecbusSend1].tnum;
			SendFrame2[7]=FecbusSendFrame[FecbusSend1].dlc;
			j=8;
			for(i=0;i<FecbusSendFrame[FecbusSend1].dlc;i++)
			{
				SendFrame2[j]=FecbusSendFrame[FecbusSend1].da[i];
				j++;
			}
			//计算校验
			crc=0xFFFF;
			for(i=0;i<j;i++)
				calccrc(SendFrame2[i]);
			
			SendFrame2[j]=crc & 0xFF;
			j++;
			SendFrame2[j]=crc / 0x100;
			j++;
			SendFrame2[j]=0x7E;					
			j++;
			TotalSendCount2=j;
			uart2Send();				

			FecbusSend1++;
			if(FecbusSend1>=FecbusSendNum)
			{
				//本次数据已发完，需判断接收
				ReceiveCount2=0;
				bNeedAns=1;
				bRevAns=0;
				AnsTimesDelay=0;
				bNeedSendFecbus=0;
			}
		}
	}	
	else
	{
		//正常巡检发送事件	
		result=0;
		if(pCRTFire!=pFireHistory)
		{
			//有火警需发送
			pAddr=pCRTFire;
			result=1;
			type=1;		//紧急事件优先级
			//修改指针
			pCRTFire +=16;
			if(pCRTFire>E_FireEnd)
				pCRTFire=E_FireBegin;	
			
			bSendCRTCommDelay=7;
		}
		else if(pCRTKFire!=pKFireHistory)
		{
			//有火警需发送
			pAddr=pCRTKFire;
			result=1;
			type=1;		//紧急事件优先级
			//修改指针
			pCRTKFire +=16;
			if(pCRTKFire>E_KFireEnd)
				pCRTKFire=E_KFireBegin;	
			
			bSendCRTCommDelay=7;
		}
		else if(pCRTRun!=pRunHistory)
		{
			//有联动需发送
			pAddr=pCRTRun;
			result=1;
			type=1;		//紧急事件优先级
			//修改指针
			pCRTRun +=16;
			if(pCRTRun>E_RunEnd)
				pCRTRun=E_RunBegin;	
		}
		else if(pCRTOther!=pOtherHistory)
		{
			//有其它记录需发送
			pAddr=pCRTOther;
			result=1;
			type=3;		//一般事件优先级
			//修改指针
			pCRTOther +=16;
			if(pCRTOther>E_OtherEnd)
				pCRTOther=E_OtherBegin;	
		}
		if(result)
		{
			//读一条记录
			SPI_FLASH_HighSpeedReadCont(pAddr,12,HistoryBuf);
			//准备发送帧数据
			PrepareSend(type,0);
		}
		else
		{
			 result1=CommToDisable1();	//判断发送屏蔽
			 if(result1)
			 {
				//准备发送帧数据
				PrepareSend(3,0);
			 }
			 else
			 {
				//3秒发送巡检
				NormalTimesDelay++;
				if(NormalTimesDelay>14)
				{
					NormalTimesDelay=0;
					//巡检
					SendFrame2[0]=0x7E;
					SendFrame2[1]=0x00;
					SendFrame2[2]=FecbusTarget;
					SendFrame2[3]=0x02;
					SendFrame2[4]=FecbusSoure;
					SendFrame2[5]=fwater;
					SendFrame2[6]=0x00;
					SendFrame2[7]=0x01;
					SendFrame2[8]=0x21;   
					//计算校验
					crc=0xFFFF;
					for(i=0;i<9;i++)
						calccrc(SendFrame2[i]);
					
					SendFrame2[9]=crc & 0xFF;
					SendFrame2[10]=crc / 0x100;
					SendFrame2[11]=0x7E;					
					TotalSendCount2=12;
					uart2Send();
					
					ReceiveCount2=0;
					bNeedAns=0;
					bRevAns=0;
					RepeatTimes=0;	
					AnsTimesDelay=0;
					
					fwater++;
					if(fwater>63)	fwater=1;
					 
				}
			}
		}				 
	}
}

void SyncKeRanQiUploadFromCanFile(const uint8 *file)
{
	uint8 i,j,type;
	u16 lel;
	if(file[1]!=6 || file[2]!=3)
		return;

	for(i=0;i<6;i++)
	{
		/* 每个气体设备占7字节，从文件偏移3开始；i从0到5，依次对应6个设备槽位。 */
j=i*7+3;
		/* 状态字节bit7表示该槽位设备已登记；未登记则清除登记位并跳过后续状态/浓度解析。 */
if((file[j] & 0x80)==0x80)
		{
			SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_REGISTER, 1);
			type=NormalizeKrDeviceType(file[j]);
		}
		else
		{
			SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_REGISTER, 0);
			continue;
		}

		if((file[j] & 0xC0)==0x80)
		{
			j++;
			if((file[j] & 0x20)==0x20)
			{
				SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_FAULT, 1);
				continue;
			}
			SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_FAULT, 0);
			j++;
			SetKeRanQiUploadAlarm(i+1, ((file[j] & 0x08)==0x08), ((file[j] & 0x04)==0x04));
			j++;
			lel = (uint16)file[j] << 8;
			lel |= file[j+1];
			/* 浓度字段只保留协议定义的14位有效数据，屏蔽状态标志位。 */
lel &= 0x3FFF;
			SetKeRanQiUploadConcentration(i+1, lel);
			UpdateKrFireConcentration(i+1, type, lel);
		}
	}
}

//显示可燃气体实时信息并同步通信板上传寄存器
void DisplayKrqNongDu(void)
{
	uint8 i,j,h,l,num,type;
	u16 lel;
	HaveReciveCanFile=0x00;
	if(CanFile[1]==6 && CanFile[2]==3)
	{
		DrawRect(180,120,700,240,2);
		for(i=0;i<6;i++)
		{
			h=i+5;
			l=19;
			j=i*7+3;
			if((CanFile[j] & 0x80)==0x80)
			{
				OutChars(h,l,"是",0);
				SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_REGISTER, 1);
			}
			else
			{
				OutChars(h,l,"否",0);
				SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_REGISTER, 0);
			}
			l=26;
			if((CanFile[j] & 0x80)==0x80)
			{
				type=NormalizeKrDeviceType(CanFile[j]);
				if(type==25)
					OutChars(h,l,"CO探测器",0);
				else if(type==26)
					OutChars(h,l,"H2探测器",0);
				else if(type==27)
					OutChars(h,l,"可燃气体",0);
				else
					OutChars(h,l,"未知类型",0);
			}
			if((CanFile[j] & 0xC0)==0x80)
			{
				j++;
				l=36;
				if((CanFile[j] & 0x20)==0x20)
				{
					OutChars(h,l,"通讯故障",0);
					SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_FAULT, 1);
				}
				else
				{
					j++;
					SetKeRanQiUploadBit(i+1, KERANQI_UPLOAD_FAULT, 0);
					SetKeRanQiUploadAlarm(i+1, ((CanFile[j] & 0x08)==0x08), ((CanFile[j] & 0x04)==0x04));
					if((CanFile[j] & 0x04)==0x04)
					{
						OutChars(h,l,"高警 ",0);
						l+=5;
					}
					if((CanFile[j] & 0x08)==0x08)
						OutChars(h,l,"低警",0);
					if((CanFile[j] & 0x0C)==0x00)
						OutChars(h,l,"正常",0);

					j++;
					lel = (uint16)CanFile[j] << 8;
					lel |= CanFile[j+1];
					lel &= 0x3FFF;
					SetKeRanQiUploadConcentration(i+1, lel);
					l=47;
					if(type==25)
					{
						num=lel/100;
						if(num>9)
							OutTwoChar(h,l,num,0);
						else
							OutOneChar(h,l+1,num+0x30,0);
						OutTwoChar(h,l+2,lel%100,0);
						OutChars(h,l+4,"ppm",0);
					}
					else if(type==26)
					{
						num=lel/100;
						if(num>1)
							num=1;
						OutOneChar(h,l,num+0x30,0);
						OutTwoChar(h,l+1,lel%100,0);
						OutChars(h,l+3,"%LEL",0);
					}
				}
			}
		}
	}
}

//可燃气体实时信息
void TestKrkzq()
{
	if(m_FirstView!=m_View)
	{
		HaveReciveCanFile = 0;
		m_FirstView=m_View;
		Windows(100,100,700,500,"可燃气体实时信息");
		OutChars(4,8,"本机地址：",1);
		outputDecimal(4,18);
		OutChars(6,8,"输入3位机器号",1);
		OutOneChar(8,22,'_',1);
		m_Cursor=22;
		m_Cursor1=8;
	}

	if(HaveReciveCanFile==0x80)
	{
		DisplayKrqNongDu();
	}

	switch(key)
	{
	case Key_Esc:
		m_View=4;
		m_FirstView=0;
		break;
	case Key_BackSpace:
		m_FirstView=0;
		break;
	case Key_Enter:
		if(m_Cursor==40)
		{
			m_Cursor=0;
			Windows(100,100,700,500,"可燃气体实时信息");
			OutChars(4,10,"地址    登记     类型     状态       浓度值",0);
			OutChars(5,10,"3-01",0);
			OutChars(6,10,"3-02",0);
			OutChars(7,10,"3-03",0);
			OutChars(8,10,"3-04",0);
			OutChars(9,10,"3-05",0);
			OutChars(10,10,"3-06",0);

			if(m_PanelAddr==m_PanelAddr2)
			{
				m_CanCommand.sign=0x14;
				m_CanCommand.loop=3;
				m_CanCommand.addr=0;
				m_CanCommand.type=0;
				WriteCanCommand();
			}
			else
			{
				m_Record.sign=0x72;
				m_Record.loop=m_PanelAddr2;
				m_Record.addr=0;
				m_Record.type=0;
				WriteToWCANZone();
			}
			HaveReciveCanFile=0;
			TestKrKzqTimeDelay=0;
		}
		break;
	}

	if(key>0x29 && key<0x3A && m_Cursor<50)
	{
		switch(m_Cursor)
		{
		case 22:
		case 23:
			OutOneChar(8,m_Cursor,key,1);
			m_Input[m_Cursor-22]=key-0x30;
			m_Cursor++;
			OutOneChar(8,m_Cursor,'_',1);
			break;
		case 24:
			OutOneChar(8,m_Cursor,key,1);
			m_PanelAddr2=m_Input[0]*100+m_Input[1]*10+key-0x30;
			if(m_PanelAddr2>0 && m_PanelAddr2<=PANEL_NET_MAX)
			{
				m_Input[3]=m_PanelAddr2;
				m_Cursor=40;
			}
			else
			{
				OutChars(8,22,"_  ",1);
				m_Cursor=22;
			}
			break;
		}
	}
	key=0;
}
/*
//可燃气体实时信息
void TestKrkzq()
{
	uint8 i,j,h,l,num,type;
	u16 lel;
	if(m_FirstView!=m_View)
	{
	    m_FirstView=m_View;
 		Fcolor=WHITE;
 		Windows(100,100,700,500,"可燃气体实时信息");
		
		OutChars(4,10,"地址    登记     类型     状态       浓度值",0);
		OutChars(5,10,"3-01",0);
		OutChars(6,10,"3-02",0);
		OutChars(7,10,"3-03",0);
		OutChars(8,10,"3-04",0);
		OutChars(9,10,"3-05",0);
		OutChars(10,10,"3-06",0);
		
		m_CanCommand.sign=0x14;  	//查可燃气体探测器信息
		m_CanCommand.loop=3;
		m_CanCommand.addr=0;
		m_CanCommand.type=0;
		WriteCanCommand();
		HaveReciveCanFile=0;
		
		TestKrKzqTimeDelay=0;

    }

	if(HaveReciveCanFile==0x80)   //命令反馈标志
    {        
        HaveReciveCanFile=0x00;
		if(CanFile[1]==6 && CanFile[2]==3)
		{
			//先清空
			DrawRect(180,120,700,240,2);
			for(i=0;i<6;i++)
			{
				h=i+ 5;
				l=19;
				j=i*7 +3;
				if((CanFile[j] & 0x80)==0x80)
					OutChars(h,l,"是",0);
				else
					OutChars(h,l,"否",0);
				l=26;
				if((CanFile[j] & 0x80)==0x80)
				{
					type=NormalizeKrDeviceType(CanFile[j]);
					if(type==25)
						OutChars(h,l,"CO探测器",0);
					else if(type==26)
					OutChars(h,l,"H2探测器",0);
				else if(type==27)
					OutChars(h,l,"可燃气体",0);
				else
					OutChars(h,l,"未知类型",0);
				}	
				if((CanFile[j] & 0xC0)==0x80)
				{
					j++;
					l=36;
					if((CanFile[j] & 0x20)==0x20)
					{
						OutChars(h,l,"通讯故障",0);
					}
					else
					{
						//if((CanFile[j] & 0x01)==0x01)
						{
							//收到数据标志
							j++;
							if((CanFile[j] & 0x04)==0x04)
							{
								OutChars(h,l,"高警 ",0);
								l+=5;
							}
							if((CanFile[j] & 0x08)==0x08)
								OutChars(h,l,"低警",0);
							if((CanFile[j] & 0x0C)==0x00)
								OutChars(h,l,"正常",0);
							
							//显示浓度值
							j++;
							lel =  (uint16)CanFile[j] <<8;
							lel |=CanFile[j+1];
							lel &=0x3FFF;	
							l=47;
							if(type==25)
							{
								num=lel/100;
								if(num>9)
									OutTwoChar(h,l,num,0);
								else
									OutOneChar(h,l+1,num+0x30,0);
								OutTwoChar(h,l+2,lel%100,0);
								OutChars(h,l+4,"ppm",0);
							}
							else if(type==26)
							{
								num=lel/100;
								if(num>1)
									num=1;
								OutOneChar(h,l,num+0x30,0);
								OutTwoChar(h,l+1,lel%100,0);
								OutChars(h,l+3,"%LEL",0);
							}
							
						}
					}
						
				}
			}
		}
		
	}
	if(TestKrKzqTimeDelay>3)
	{
		TestKrKzqTimeDelay=0;
		m_CanCommand.sign=0x14;  	//查可燃气体探测器信息
		m_CanCommand.loop=3;
		m_CanCommand.addr=0;
		m_CanCommand.type=0;
		WriteCanCommand();
		HaveReciveCanFile=0;		
	}
	switch(key)
	{
	case Key_Esc:
	{
		m_View=4;
		m_FirstView=0;
	}
	break;
	}
	key=0;
}*/

//查找是否多个报警
uint8 FindLogi1(uint8 panel,uint8 loop,uint8 addr1,uint8 addr2)
{
    uint16 i;
	INT8U p,l,a;
    uint8 tt=0;
    if(pFire==0 && pRun==0) return 0;
    for(i=0;i<pFire;i++)
    {
		p=FireList[i].panel;
		l=FireList[i].loop;
		a=FireList[i].addr;
        if(p==panel && l==loop && a>addr1 && a<addr2+1)  //2015.05.19更改	a>addr1-1
            tt++; 
    }
	for(i=0;i<pRun;i++)
	{
		if(RunList[i].sign==0x85 || RunList[i].sign==0x8A)
		{
			if(RunList[i].panel==panel && RunList[i].loop==loop && RunList[i].addr>addr1 && RunList[i].addr<addr2+1) 
            	tt++;
		}
	}
    return tt;    
}

//查找是否报警
uint8 FindLogi(uint8 panel,uint8 loop,uint8 addr)
{
    //增加由反馈的启动
	uint16 i;
	INT8U p,l,a;
    if(pFire==0 && pRun==0 && pKFire==0) return 0;
    for(i=0;i<pFire;i++)
    {
		p=FireList[i].panel;
		l=FireList[i].loop;
		a=FireList[i].addr;
        if(p==panel && l==loop && a==addr) 
            return 1;
    }

    for(i=0;i<pKFire;i++)
    {
		p=KFireList[i].panel;
		l=KFireList[i].loop;
		a=KFireList[i].addr;
        if(p==panel && l==loop && a==addr) 
            return 1;
    }

	for(i=0;i<pRun;i++)
	{
		if(RunList[i].sign==0x85 || RunList[i].sign==0x8A)
		{
			if(RunList[i].panel==panel && RunList[i].loop==loop && RunList[i].addr==addr) 
            	return 1;
		}
	}
    return 0;    
}

void WriteRun(uint8 m_panel,uint8 m_loop,uint8 m_addr,uint8 tqd,uint8 ttz)
{
	INT8U ss;
	if(tqd>0)
	{
		//延迟启动
		if(pQdZone<511)
		{
			QdZone[pQdZone][0]=0x00;
			QdZone[pQdZone][1]=m_panel;
			QdZone[pQdZone][2]=m_loop;
			QdZone[pQdZone][3]=m_addr;
			QdZone[pQdZone][4]=tqd;
			QdZone[pQdZone][5]=ttz;
			pQdZone++;
		}		
	}
	else
	{
		//立即启动
		if(m_ZdSd & 0x80)
		{
			//自动允许
			if(m_panel==0)
				ss=0x02;
			else
				ss=0x32;	//向区域机下传联动启动
		}
		else
		{
			//自动禁止，下传请求
			if(m_panel==0)
				ss=0x42;
			else
				ss=0x34;	//向区域机下传联动请求
			bldqq=1;		//有联动请求标志
		}
		if(m_panel==0)
		{
			//发送本机联动
			m_CanCommand.sign=ss;  
			m_CanCommand.loop=m_loop;
			m_CanCommand.addr=m_addr;
			m_CanCommand.type=0;
			WriteCanCommand();	
		}
		else
		{
			m_Record.sign=ss;
			m_Record.loop=m_loop;
			m_Record.addr=m_addr;
			m_Record.type=m_panel;
			WriteToWCANZone();
		}
		//判断是否需要停止
		if(ttz!=0)
		{
			//需要停止
			if(pQdZone<511)
			{
				QdZone[pQdZone][0]=0x80;
				QdZone[pQdZone][1]=m_panel;
				QdZone[pQdZone][2]=m_loop;
				QdZone[pQdZone][3]=m_addr;
				QdZone[pQdZone][4]=0;
				QdZone[pQdZone][5]=ttz;
				pQdZone++;
			}
		}
	}
}



//编程解释
void ProgramIsExplained()
{
	uint16 length,numee;
	uint8 h,li,k,num,tqd,ttz;
	uint8 m_byte;

	uint8 m_operate=1;	//与或暂存,true为与;
	uint8 m_result=1;
	uint8 temp_result=0;
	uint8 m_tempopt;
	uint16 m_count=0;
	uint8 m_panel1,m_loop1,m_address1,m_address2;
	uint8 fire_count;
	uint16 i;		//位置
	//自动不允许，需要下发联动请求
    //if((m_ZdSd & 0x80)==0x00 && bRunAuto==0x00) return; //自动不允许
    //读取一条编程
	//先读本条总数
	SPI_FLASH_HighSpeedReadCont(E_ProgBegin+m_LogicCycle*512,2,m_ProgBuf);
	if(m_ProgBuf[0]==0xFF)
	{
		//没有编程，从头开始查
		//m_TotalLogic=m_LogicCycle;
		m_LogicCycle=0;
		return;
	}
	length=(m_ProgBuf[0]<<8)+m_ProgBuf[1]+4;
	if(length>500)
		length=500;
	//读本条全部编程
	SPI_FLASH_HighSpeedReadCont(E_ProgBegin+m_LogicCycle*512,length,m_ProgBuf);
    //准备读下一条
    m_LogicCycle++;

	h=(m_LogicCycle-1)/8;
	li=(m_LogicCycle-1)%8;
	if((m_Logi[h] & (0x01<<li))!=0)	return;	   //本条已判断过

	//先查找0xEE,前边是多个联动地址
	i=2;
	while(i<length-2)
	{
		if(m_ProgBuf[i]==0xEE)
			break;
		i++;
	}
	numee=i;	//numee前边是要联动的地址
	tqd=m_ProgBuf[i+1];
	ttz=m_ProgBuf[i+2];
    i+=3;	//0xEE后边跟2个字节的时间 	
	length-=4;
	///////////////////
	while(i<length)
	{
		m_byte=m_ProgBuf[i];
		i++;
		switch(m_byte)
		{
			case 0xEA:	//类型A
				fire_count=0;
				temp_result=0;
				m_count=m_ProgBuf[i];//个数
				i++;
				m_panel1=m_ProgBuf[i];//机器
				i++;
				m_loop1=m_ProgBuf[i];//回路
				i++;
				m_address1=m_ProgBuf[i];//地址
				i++;
				if(FindLogi(m_panel1,m_loop1,m_address1))	fire_count++;
				break;
			case 0xE1:	//~
				m_panel1=m_ProgBuf[i];//机器
				i++;
				m_loop1=m_ProgBuf[i];//回路
				i++;
				m_address2=m_ProgBuf[i];//地址2
				i++;
				fire_count=fire_count+FindLogi1(m_panel1,m_loop1,m_address1,m_address2);  //数量
				break;
			case 0xE5:	//类型A中的地址
				m_panel1=m_ProgBuf[i];//机器
				i++;
				m_loop1=m_ProgBuf[i];//回路
				i++;
				m_address1=m_ProgBuf[i];//地址
				i++;
				if(FindLogi(m_panel1,m_loop1,m_address1))	fire_count++;
				break;
			case 0xE6:	//类型A结束
				if(fire_count>=m_count)  temp_result=1;
				if(m_operate)	m_result=m_result && temp_result;
				else	m_result=m_result || temp_result;
				break;
			case 0xE2:	//简单型开始“（”
				temp_result=1;
				m_tempopt=1;
				m_panel1=m_ProgBuf[i];//机器
				i++;
				m_loop1=m_ProgBuf[i];//回路
				i++;
				m_address1=m_ProgBuf[i];//地址
				i++;
				if(m_tempopt)
					temp_result=temp_result && FindLogi(m_panel1,m_loop1,m_address1);
				else
					temp_result=temp_result || FindLogi(m_panel1,m_loop1,m_address1);
				break;
			case 0xE7:	//简单型E0 “*”
				m_tempopt=1;
				m_panel1=m_ProgBuf[i];//机器
				i++;
				m_loop1=m_ProgBuf[i];//回路
				i++;
				m_address1=m_ProgBuf[i];//地址
				i++;
				if(m_tempopt)
					temp_result=temp_result && FindLogi(m_panel1,m_loop1,m_address1);
				else
					temp_result=temp_result || FindLogi(m_panel1,m_loop1,m_address1);
				break;							
			case 0xE8:	//简单型E4 +“+”
				m_tempopt=0;
				m_panel1=m_ProgBuf[i];//机器
				i++;
				m_loop1=m_ProgBuf[i];//回路
				i++;
				m_address1=m_ProgBuf[i];//地址
				i++;
				if(m_tempopt)
					temp_result=temp_result && FindLogi(m_panel1,m_loop1,m_address1);
				else
					temp_result=temp_result || FindLogi(m_panel1,m_loop1,m_address1);
				break;							
			case 0xE3:	//简单型结束“）”
				if(m_operate)	m_result=m_result && temp_result;
				else	m_result=m_result || temp_result;
				break;
			case 0xE0:	//*与
				if(m_operate)	m_result=m_result && temp_result;
				else	m_result=m_result || temp_result;
				m_operate=1;
				break;
			case 0xE4:	//+或
				if(m_operate)	m_result=m_result && temp_result;
				else	m_result=m_result || temp_result;
				m_operate=0;
				break;
			case 0xFE:	//结束
			case 0xFF:	//结束
				if(m_operate)	m_result=m_result && temp_result;
				else	m_result=m_result || temp_result;
				length=0;	//结束循环	
				break;

			}

	}
	////////////////////
	if(m_result)
	{
	    //符合逻辑关系
		//WWDT_Feed(); 	//喂狗
		//符合逻辑关系标志
		m_Logi[h] |=(0x01<<li);
		//得到联动地址
		i=2;
		while(i<numee)
		{
			m_byte=m_ProgBuf[i];
			i++;
			if(m_byte==0xE1)
			{
				//~
				m_panel1=m_ProgBuf[i];
				i++;
				m_loop1=m_ProgBuf[i];
				i++;
				m_address2=m_ProgBuf[i];
				i++;
				//写入发送区						
				num=m_address2-m_address1+1;		
				for(k=1;k<num;k++)
				{
					//写入联动区或判断区
					WriteRun(m_panel1,m_loop1,m_address1+k,tqd,ttz);									
				}					
			}
			else if(m_byte==0xE5)
			{
				//前边是单独地址
				m_panel1=m_ProgBuf[i];
				i++;
				m_loop1=m_ProgBuf[i];
				i++;
				m_address1=m_ProgBuf[i];
				i++;
				//单独地址写入联动区或判断区
				WriteRun(m_panel1,m_loop1,m_address1,tqd,ttz);	
			}
			else
			{
				//~前的地址或第一个地址
				m_panel1=m_byte;
				m_loop1=m_ProgBuf[i];
				i++;					
				m_address1=m_ProgBuf[i];
				i++;
				//单独地址写入联动区或判断区
				WriteRun(m_panel1,m_loop1,m_address1,tqd,ttz);
			}
		}
	}
    
} 

void xsygs(uint x,uint y,uint num)
{
	if(num>999)
	{
		OutTwoChar1(x,y,num/100,2);
		OutTwoChar1(x+16,y,num%100,2);
	}
	else if(num>99)
	{
		OutOneChar1(x,y,(num/100)+0x30,2);
		OutTwoChar1(x+8,y,num%100,2);		
	}
	else if(num>9)
		OutTwoChar1(x,y,num,2);	
	else
		OutOneChar1(x,y,num+0x30,2);	
}

//检查功能
void CheckFuction()
{
    uchar i,j,type,n,m;
	uint x,y,num,num1;
	uchar pbnum[13];	
	
	if(m_FirstView!=m_View)
	{
        m_FirstView=m_View;
		m_deng[1]|=0x40;	//置检查灯
 		Fcolor=WHITE;
		Bcolor=NAVY;
		SetColor(Fcolor,Bcolor);
		DrawRect(0,25,800,455,2);	//整体显示区域
		OutChars(1,2,"设备检查",1);
		//13*5个隔
		for(i=0;i<6;i++)
			DrawLine(200+i*100,50,200+i*100,400);
		for(i=0;i<15;i++)
			DrawLine(200,50+i*25,700,50+i*25);
		//写标题
		TextOut(215,55,"设备类型",2);
		TextOut(315,55,"设计数量",2);
		TextOut(407,55,"正常工作数",2);
		TextOut(515,55,"故障数量",2);
		TextOut(615,55,"屏蔽数量",2);
		
		TextOut(200,410,"正在查询，请稍候——",0);
		
		for(i=0;i<16;i++)
			totalsb[i]=0;	//设计设备数
		
		m_CheckTotal=m_LoopBoardNum;	//检查回路板总数
		m_HaveCheck=1;	//已检查号
		if((m_Board[m_HaveCheck]& 0x40)==0x00)
		{
			//当前没有故障，发送查询
			HaveReciveCanFile=0x00;
			m_CanCommand.sign=0x13;	//查询回路板信息  
			m_CanCommand.loop=m_HaveCheck;
			m_CanCommand.addr=0;
			m_CanCommand.type=0;
			WriteCanCommand();
		}
		else
		{
			//当前有故障，模拟返回数据
			HaveReciveCanFile=0x80;
			CanFile[1]=0x04; //没有数据
			CanFile[2]=m_HaveCheck; //板号
		}
	}
	if(HaveReciveCanFile==0x80)
	{
		//返回数据
		HaveReciveCanFile=0x00;
		if(CanFile[1]==0x05) //返回的查询数据文件
		{
			//字节2：回路板号
			//字节3：感烟登记的数量高字节
			//字节4: 感烟登记的数量低字节
			//字节5: 感温登记的数量高字节
			//字节6: 感温登记的数量低字节
			//...
			j=3;
			for(i=0;i<13;i++)
			{
				x = CanFile[j]*256 + CanFile[j+1];
				totalsb[i] += x;
				j=j+2;
			}

		}
		//准备查下一块板
		m_HaveCheck++;	//已检查号
		if(m_HaveCheck>=m_CheckTotal+1)
		{
			//读出屏蔽数量
			for(i=0;i<13;i++)
				pbnum[i]=0;
			if(Total_Isolate>0)
			{
				
				for(i=0;i<Total_Isolate;i++)
				{
					SPI_FLASH_HighSpeedReadCont(E_DisableAddr+(i+1)*8,8,HistoryBuf);
					type=HistoryBuf[3];
					type &=0x1F;
					switch(type)
					{
					case 1:		j=0;	break;
					case 2:		j=1;	break;
					case 4:		j=2;	break;
						
					case 10:	j=3;	break;
					case 11:	j=4;	break;
					case 12:	j=5;	break;
					case 13:	j=6;	break;

					case 14:	j=7;	break;
					case 15:	j=8;	break;

					case 20:	j=9;	break;
					case 21:	j=10;	break;
										
					default:	j=12;	break;
					}
					pbnum[j]++;
				}
			}
			//已查询完,显示
			for(i=0;i<13;i++)
			{
				//写类型
				x=215;
				y=80+i*25;
				switch(i)
				{
				case 0:	TextOut(x,y,"光电感烟",2);	break;
				case 1:	TextOut(x,y,"电子感温",2);	break;
				case 2:	TextOut(x,y,"手动按钮",2);	break;
				case 3:	TextOut(x,y,"消火栓钮",2);	break;
				case 4:	TextOut(x,y,"输入模块",2);	break;
				case 5:	TextOut(x,y,"输出模块",2);	break;
				case 6:	TextOut(x,y,"输入输出",2);	break;
				case 7:	TextOut(x-8,y,"火灾显示盘",2);	break;
				case 8:	TextOut(x,y,"声光警报",2);	break;
				case 9:	TextOut(x,y,"短路隔离",2);	break;
				case 10:TextOut(x,y,"紧急启停",2);	break;
				case 11:TextOut(x,y,"可燃气体",2);	break;
				case 12:TextOut(x,y,"其它类型",2);	break;				
				}
				//设计数量
				x=331;
				num=totalsb[i];
				xsygs(x,y,num);
				//故障数量
				x=531;
				xsygs(x,y,sberr[i]);
				//屏蔽数量
				x=631;
				xsygs(x,y,pbnum[i]);
				//正常数量
				x=431;
				num1=sberr[i]+pbnum[i];
				if(num>num1)
					xsygs(x,y,num-num1);
				else
					xsygs(x,y,0);			
			}
			TextOut(200,410,"                    ",0);
		}
		else
		{
			m_CanCommand.sign=0x13;	//查询回路板信息  
			m_CanCommand.loop=m_HaveCheck;
			m_CanCommand.addr=0;
			m_CanCommand.type=0;
			WriteCanCommand();				
		}
	}
	if(key==Key_Esc)
    {
		FirstPageView(); //首页显示
		m_deng[1] &=0xBF;	//清检查灯
    }
	key=0;	
	
}

//查灵敏度级别
void TestJibie()
{
	uint16 i,loop,addr;
	uint8 tt,panel,j;
	uint16 num,shu;
	INT8U *dptr;
	if(m_FirstView!=m_View)
	{
	    m_FirstView=m_View;
	    Windows(100,100,700,500,"查灵敏度");
		OutChars(5,10,"请输入3位回路和3位地址后",1);
		OutChars(6,10,"按确认键。",1);
		OutChars(9,14,"回路:",1);
		OutChars(9,28,"地址:",1);
	
		OutOneChar(9,21,'_',1);
		m_Cursor=21; //光标位置
        m_Return=0x00;
    }
	if(m_Return==0x80)   //命令反馈标志
    {        
        m_Return=0x00;
		if(m_CanReturn.sign==0xA0)
		{
			OutTwoChar(12,16,m_CanReturn.loop,1);
			OutOneChar(12,18,'-',1);
			OutThreeChar(12,19,m_CanReturn.addr,1);
			OutChars(12,22,"已设为",1);
			if(m_CanReturn.type==1)
				OutChars(12,28,"1级灵敏度",1);
			else
				OutChars(12,28,"2级灵敏度",1);
		}
            
    }

	switch(key)
	{
	case Key_Esc:
	{
		m_View=4;
		m_FirstView=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	case Key_Enter:  
	{
		if(m_Cursor==49)
		{
		    m_CanCommand.sign=0x0C;  //查灵敏度级别
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=m_Input[5];
			m_CanCommand.type=0;
			WriteCanCommand();
			OutChars(12,20,"命令已发出!",1);
		}
	}
	break;
	}
	if(key>0x29 && key<0x3A && m_Cursor<50 )
	{
		//有键按下
		switch(m_Cursor)
		{
		    case 21:
		    case 22:
		    case 35:
		    case 36:
		    {
				OutOneChar(9,m_Cursor,key,1);
			    if(m_Cursor>23)
					m_Input[m_Cursor-32]=key-0x30;	
				else
					m_Input[m_Cursor-21]=key-0x30;	
				m_Cursor++;
				OutOneChar(9,m_Cursor,'_',1);	 //光标
			    
		    }
		    break;
		    case 23:
		    {
			    OutOneChar(9,m_Cursor,key,1);
				//回路
				loop=m_Input[0]*100+m_Input[1]*10+key-0x30;
			    if(loop>0 && loop<33)
			    {
			        m_Input[2]=loop;
			        m_Cursor=35;
			  		OutOneChar(9,m_Cursor,'_',1);	 //光标
			    }
				else
				{
					//输入错误
					OutChars(9,21,"_  ",1);
					m_Cursor=21; //光标位置
				}
			}
			break;
		    case 37:
		    {
			    OutOneChar(9,m_Cursor,key,1);
				//地址
				addr=m_Input[3]*100+m_Input[4]*10+key-0x30;
			    if(addr>0 && addr<201)
			    {
			        m_Input[5]=addr;
			        m_Cursor=49;
			    }
				else
				{
					//输入错误
					OutChars(9,35,"_  ",1);
					m_Cursor=35; //光标位置
				}
		    }
		    break;
		}	
	}
	key=0;
	
}

void ErrDisKrFireZone()
{
	if(pkrError>0)
	{
		if(krzonetype==2)
		{
			//显示后续可燃气体故障
			if(pkrError>DisKrFireNum )
			{
				if(HaveDisplayKrFire<2)
				{
					DisKrFireNum++;
					HaveDisplayKrFire++;
					OutOneKrFire(2);
				}
			}	
		}
		else
		{
			krzonetype=2;
			Qxsq(2);
			DisKrFireNum=1;
			HaveDisplayKrFire=1;
			OutOneKrFire(2);
		}		
	}
	else
	{
		DisKrFireNum=0;
		HaveDisplayKrFire=0;	
		krzonetype=0;		
	}	
}

void ErrDisFireZone()
{
	if(pError>0)
	{
		if(hjzonetype==2)
		{
			//显示后续火警区故障
			if(pError>DisFireNum)
			{   
				if(HaveDisplay<3)
				{
					DisFireNum++;
					HaveDisplay++;
					if(HaveDisplay==1)
					{
						CurDisplay=HaveDisplay;
						CurDisNum=DisFireNum;
					}
					OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
					bDisplaying=1;
				}
				else
					bDisplaying=0;
			}
			else
				bDisplaying=0;
		}
		else
		{
			hjzonetype=2;				
			Qxsq(0);
			DisFireNum=1;
			HaveDisplay=1;
			CurDisplay=HaveDisplay;
			CurDisNum=DisFireNum;
			OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
		}
	}
	else
	{
		DisFireNum=0;
		HaveDisplay=0;
		CurDisplay=HaveDisplay;
		CurDisNum=DisFireNum;	
		hjzonetype=0;
	}	

}

void ErrDisLdZone()
{
	if(pldError>0)
	{
		if(ldzonetype==2)
		{
			//显示后续联动区故障
			if(pldError>DisRunNum)
			{   
				if(HaveDisplayRun<3)
				{
					DisRunNum++;
					HaveDisplayRun++;
					OutOneRun(2);
				}
			}
		}
		else
		{
			ldzonetype=2;				
			Qxsq(1);
			DisRunNum=1;
			HaveDisplayRun=1;
			OutOneRun(2);
		}
	}
	else
	{
		DisRunNum=0;
		HaveDisplayRun=0;
		ldzonetype=0;				
	}	
}

void ErrDisMhLdZone()
{
	if(pmhError>0)
	{
		if(mhzonetype==2)
		{
			//显示后续气灭故障
			if(pmhError>DisMhNum )
			{
				if(HaveDisplayMh<2)
				{
					DisMhNum++;
					HaveDisplayMh++;
					OutOneMh(2);
				}
			}	
		}
		else
		{
			mhzonetype=2;
			Qxsq(3);
			DisMhNum=1;
			HaveDisplayMh=1;
			OutOneMh(2);
		}		
	}
	else
	{
		DisMhNum=0;
		HaveDisplayMh=0;	
		mhzonetype=0;		
	}	
}

void errzidongfanye(u8 fanyetype)
{
	u8 i;
	if(fanyetype==1)
	{
		if(hjzonetype==2)
		{
//			if(pError>DisFireNum && HaveDisplay==3) 
//			{
//				Qxsq(0);	//清火警显示区
//				DisFireNum--;
//				HaveDisplay=1;
//				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
//			}		
//			else if(pError>3)
//			{
//				//回第一条
//				Qxsq(0);	//清火警显示区
//				DisFireNum=1;
//				HaveDisplay=1;
//				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
//			}				
			if(HaveDisplay>CurDisplay)
			{
				//当前反显的需更新
				OutOneFire(CurDisNum,CurDisplay,0,2);
				CurDisNum++;
				CurDisplay++;
				OutOneFire(CurDisNum,CurDisplay,CurDisplay,2);
			}
			else if(pError>CurDisNum && CurDisplay==3) 
			{
				//上滚一条
				CurDisNum++;
				for(i=1;i<3;i++)
					OutOneFire(CurDisNum+i-3,i,0,2);

				OutOneFire(CurDisNum,3,3,2);
			}
			else if(pError>3)
			{
				//回首页
				Qxsq(0); 
				DisFireNum=1;
				HaveDisplay=1;
				CurDisplay=HaveDisplay;
				CurDisNum=DisFireNum;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
			}			
		}			
	}
	else if(fanyetype==2)
	{
		if(ldzonetype==2)
		{
			if(pldError>DisRunNum && HaveDisplayRun==3) 
			{
				Qxsq(1);	//清联动显示区
				DisRunNum--;
				HaveDisplayRun=1;
				OutOneRun(2);
			}		
			else if(pldError>3)
			{
				//回第一条
				Qxsq(1);	//清联动显示区
				DisRunNum=1;
				HaveDisplayRun=1;
				OutOneRun(2);
			}				
		}		
	}
	else if(fanyetype==3)
	{
		if(krzonetype==2)
		{
			if(pkrError>DisKrFireNum && HaveDisplayKrFire==2) 
			{
				Qxsq(2);	//清可燃显示区
				HaveDisplayKrFire=1;
				OutOneKrFire(2);
			}		
			else if(pkrError>2)
			{
				//回第一条
				Qxsq(2);	//清可燃显示区
				DisKrFireNum=1;
				HaveDisplayKrFire=1;
				OutOneKrFire(2);
			}				
		}		
	}	
	else if(fanyetype==4)
	{
		if(mhzonetype==2)
		{
			if(pmhError>DisMhNum && HaveDisplayMh==2) 
			{
				Qxsq(3);	//清灭火显示区
				HaveDisplayMh=1;
				OutOneMh(2);
			}		
			else if(pmhError>2)
			{
				//回第一条
				Qxsq(3);	//清灭火显示区
				DisMhNum=1;
				HaveDisplayMh=1;
				OutOneMh(2);
			}				
		}
	}
}

void ErrorDisplay1()
{
	u8 r;
	u16 y;	
	if(pError==0 && pmhError==0 && pkrError==0 && pldError==0 && m_View==91)
	{
		//故障恢复
		FirstPageView(); //首页显示 
		return;			
	}
	if(m_FirstView!=m_View)
	{
		m_FirstView=m_View;
		Fcolor=WHITE;
		Bcolor=BLACK;
		SetColor(Fcolor,Bcolor);
		DrawRect(0,25,800,455,2);	//整体显示区域
		TextOut(50,26,"火灾报警控制器故障信息",0);
		TextOut(450,26,"消防联动控制器故障信息",0);
		TextOut(50,280,"可燃气体控制器故障信息",0);
		TextOut(450,280,"气体灭火控制器故障信息",0);

		Bcolor=BLACK;
		Fcolor=LGRAY;
		SetColor(Fcolor,Bcolor);
		DrawRect(391,25,394,455,3);	//画分隔线
		
		y=75*0+52;
		DrawLine(0,y,800,y);
		y=75*3+52;
		DrawLine(0,y,800,y);
		y=75*0+305;
		DrawLine(0,y,800,y);
				
		Fcolor=WHITE;
		Bcolor=BLACK;
		SetColor(Fcolor,Bcolor);
		
		bConfirm=0;
		
		hjzonetype=0;
		ErrDisFireZone();
		ldzonetype=0;
		ErrDisLdZone();
		krzonetype=0;
		ErrDisKrFireZone();
		mhzonetype=0;
		ErrDisMhLdZone();
		
		optzonenum=1;	//可操作的分区号
		displayzonenum();
	}
    else
    {
        //显示后续火警区
        ErrDisFireZone();
		ErrDisLdZone();
		ErrDisKrFireZone();		
		ErrDisMhLdZone();
				
    }
	

	//判断自动翻页
    if(pError>3 && m_FireAuto>9 && bConfirm==0)
    {
        m_FireAuto=0; //火警自动翻页
        errzidongfanye(1);
    }
    if(pldError>3 && m_RunAuto>9)
    {
        m_RunAuto=0; //联动自动翻页
		errzidongfanye(2);
    }
	if(pkrError>2 && m_KrFireAuto>9)
	{
        m_KrFireAuto=0; //
		errzidongfanye(3);			
	}
    if(pmhError>2 && m_mhRunAuto>9 && bConfirm==0)
    {
        m_mhRunAuto=0; //灭火自动翻页
		errzidongfanye(4);
    }
	
	if(bConfirm==1)
	{
		r=0;
		if(key==0x31)
		{
			//确认故障
			ErrorList[CurDisNum-1].confirm=1;	//确认故障
			OutOneFire(CurDisNum,CurDisplay,CurDisplay,2);	//更新信息
			bConfirm=0;
			//刷新联动页
			r=1;
		}
		else if(key==Key_Esc)
		{
			bConfirm=0;
			key=0;
			//刷新联动页
			r=1;
		}
		if(r)
		{
			Fcolor=WHITE;
			Bcolor=BLACK;
			SetColor(Fcolor,Bcolor);
			DrawRect(410,285,799,452,2);	//整体显示区域
			TextOut(450,280,"气体灭火控制器故障信息",0);
			y=75*0+305;
			DrawLine(380,y,800,y);
			mhzonetype=0;
			DisMhLdZone();
		}
	}


    //判断按键
	switch(key)
	{
	case Key_Menu:  //操作
	{
		m_View=1;
		m_FirstView=0;		
	}
	break;
	case Key_Up:  //火警前页
	{
		if(optzonenum==1)
		{
			//操作火警区
			if(hjzonetype==2 && pError>0)
			{
				Qxsq(0); 
				DisFireNum=1;
				HaveDisplay=1;
				CurDisplay=HaveDisplay;
				CurDisNum=DisFireNum;
				OutOneFire(DisFireNum,HaveDisplay,CurDisplay,2);
			}
		}
		else if(optzonenum==2)
		{
			//操作联动区
			if(ldzonetype==2 && pldError>3)
			{
				Qxsq(1);	//清联动显示区
				DisRunNum=1;
				HaveDisplayRun=1;
				OutOneRun(2);
			}
		}
		else if(optzonenum==3)
		{
			//操作可燃气体报警区
			if(krzonetype==2 && pkrError>2)
			{
				Qxsq(2);	//清可燃显示区
				DisKrFireNum=1;
				HaveDisplayKrFire=1;
				OutOneKrFire(2);
				m_KrFireAuto=0;  
			}
		}
		else if(optzonenum==4)
		{
			//操作灭火区
			if(mhzonetype==2 && pmhError>2)
			{
				Qxsq(3);	//清灭火显示区
				DisMhNum=1;
				HaveDisplayMh=1;
				OutOneMh(2);
			}
		}

	}		
	break;
	case Key_Down: //火警翻页
	{
		if(optzonenum==1)
		{
			//操作火警区
			zidongfanye(1);				
		}
		else if(optzonenum==2)
		{
			//操作联动区
			zidongfanye(2);
		}
		else if(optzonenum==3)
		{
			//操作可燃气体报警区
			zidongfanye(3);			
		}
		else if(optzonenum==4)
		{
			//操作灭火区
			zidongfanye(4);
		}
	}
	break;
	case Key_Confirm:	//故障确认
	{
		if(bConfirm==0 && pError>0 && optzonenum==1)
		{
			Fcolor=WHITE;
			Bcolor=DGRAY;
			SetColor(Fcolor,Bcolor);
			win.x=410;
			win.y=285;
			win.with=380;
			win.hight=165;
			win.title=NULL;
			win.state=NULL;
			GUI_WindowsDraw(&win);
			TextOut(415,310,"确认故障信息",0);
			TextOut(415,340,"当前故障序号：",0);
			OutThreeChar1(590,340,CurDisNum,0);	//显示序号
			TextOut(440,370,"故障已处理请按1,",0);
			TextOut(440,400,"未处理按取消键。",0);
			bConfirm=1;
			
			Fcolor=WHITE;
			Bcolor=RED;
			SetColor(Fcolor,Bcolor);
		}
	}
	break;
	case Key_Before:  //联动前页
	{
		if(optzonenum>0)
			optzonenum--;	//可操作的分区号
		if(optzonenum==0)
			optzonenum=4;
		displayzonenum();
	}
	break;
	case Key_Next: //联动翻页
	{
		optzonenum++;	//可操作的分区号
		if(optzonenum>4)
			optzonenum=1;
		displayzonenum();
	}
	break;
	}
	if(m_View==33 && key==Key_Esc)
	{
		m_View=3;
		m_FirstView=0;		
	}
	key=0;	
}

//模拟量曲线
void TestQx()
{
	uint8 i,right;
	uint16 loop,addr,num;
	uint16 CurrentX,CurrentY;
	if(m_FirstView!=m_View)
	{
	    m_FirstView=m_View;
 		Windows(100,100,700,500,"探测器数据曲线");
		
		OutChars(2,14,"回路:",1);
		OutChars(2,28,"地址:",1);
	
		OutOneChar(2,21,'_',1);
		m_Cursor=21; //光标位置

		//画曲线纵坐标及刻度
		for(i=0;i<9;i++)
		{
			if(i>0)
				OutThreeChar1(90,392-i*32,(i-1)*40,0);
			else
				TextOut(90,392,"-40",0);
			if(i>0)
				DrawLine(127,400-i*32,130,400-i*32);
		}
		//画坐标，原点（130,400）    原点为-40
		DrawLine(130,400,570,400);
		DrawLine(130,400,130,130);
		bTestQx=0;
	
    }
	if(m_Return==0x80)   //命令反馈标志
    {        
        m_Return=0x00;
        if(m_CanReturn.sign==0xA0) //返回查曲线 
        {
			if(m_CanReturn.loop==1)
			{
				if(m_TotalDot1<42)
				{
					m_TotalDot1++;
				}
				else
				{
					Fcolor=WHITE;
					Bcolor=NAVY;	
					SetColor(Fcolor,Bcolor);
					DrawRect(131,131,569,399,2);
					m_LastX1=130;	//画曲线时上次的X坐标
					m_TotalDot1=0;	//已画曲线的点数
				}
			}
			if(m_CanReturn.type ==1)
			{
				//感烟
				if(m_CanReturn.loop==1)
				{
					OutChars(18,20,"-   号烟雾值:       ",0);
					OutThreeChar(18,17,m_Input[2],0);
					OutThreeChar(18,21,m_CanReturn.addr,0);	
					OutThreeChar(18,33,m_CanReturn.type1,0);
					//连线
					num=m_CanReturn.type1+40;
					CurrentX=130+m_TotalDot1*10;
					CurrentY=400-num*8/10;
					if(m_TotalDot1==1)
						m_LastY1=CurrentY;	//第1次的值						
					DrawLine(m_LastX1,m_LastY1,CurrentX,CurrentY);
					m_LastX1=CurrentX;
					m_LastY1=CurrentY;
				}
			}
			else if(m_CanReturn.type ==2)
			{
				//感温
				if(m_CanReturn.loop==1)
				{
					OutChars(18,20,"-   号温度值:   ℃",0);
					OutThreeChar(18,17,m_Input[2],0);
					OutThreeChar(18,21,m_CanReturn.addr,0);	
					if(m_CanReturn.type1>128)
					{
						OutOneChar(18,33,'-',0);
						OutTwoChar(18,34,m_CanReturn.type1-128,0);
						num=40-(m_CanReturn.type1-128);
					}						
					else
					{
						OutThreeChar(18,33,m_CanReturn.type1,0);
						num=m_CanReturn.type1+40;
					}
					//连线
					CurrentX=130+m_TotalDot1*10;
					CurrentY=400-num*8/10;
					if(m_TotalDot1==1)
						m_LastY1=CurrentY;	//第1次的值						
					DrawLine(m_LastX1,m_LastY1,CurrentX,CurrentY);
					m_LastX1=CurrentX;
					m_LastY1=CurrentY;
				}
			}
			else
			{
				OutChars(18,20,"-   号无数据        ",0);
				OutThreeChar(18,17,m_Input[2],0);
				OutThreeChar(18,21,m_CanReturn.addr,0);	
			}
        }
    }

	if(bTestQx && TestQxTimes>2)
	{
		TestQxTimes=0;
		m_CanCommand.sign=0x0B;  //查曲线
		m_CanCommand.loop=m_Input[2];
		m_CanCommand.addr=m_Input[5];
		m_CanCommand.type=0;
		WriteCanCommand();		
	}
		
	switch(key)
	{
	case Key_Esc:
	{
		m_View=4;
		m_FirstView=0;
		bTestQx=0;
	}
	break;
	case Key_BackSpace:  
	{
        m_FirstView=0;
	}
	break;
	case Key_Enter:  
	{
		if(m_Cursor==49 && bTestQx==0)			
		{
		    m_CanCommand.sign=0x0B;  //查曲线
			m_CanCommand.loop=m_Input[2];
			m_CanCommand.addr=m_Input[5];
			m_CanCommand.type=0;
			WriteCanCommand();
			m_LastX1=130;	//画曲线时上次的X坐标
			m_TotalDot1=0;	//已画曲线的点数
			m_Cursor=50;
			bTestQx=1;
			TestQxTimes=0;
		}
	}
	break;
	}
	if(key>0x29 && key<0x3A && m_Cursor<50 )
	{
		//有键按下
		switch(m_Cursor)
		{
		    case 21:
		    case 22:
		    case 35:
		    case 36:
		    {
				OutOneChar(2,m_Cursor,key,1);
			    if(m_Cursor>23)
					m_Input[m_Cursor-32]=key-0x30;	
				else
					m_Input[m_Cursor-21]=key-0x30;	
				m_Cursor++;
				OutOneChar(2,m_Cursor,'_',1);	 //光标
			    
		    }
		    break;
		    case 23:
		    {
			    OutOneChar(2,m_Cursor,key,1);
				//回路
				loop=m_Input[0]*100+m_Input[1]*10+key-0x30;
			    if(loop>0 && loop<33)
			    {
			        m_Input[2]=loop;
			        m_Cursor=35;
			  		OutOneChar(2,m_Cursor,'_',1);	 //光标
			    }
				else
				{
					//输入错误
					OutChars(2,21,"_  ",1);
					m_Cursor=21; //光标位置
				}
			}
			break;
		    case 37:
		    {
			    OutOneChar(2,m_Cursor,key,1);
				//地址
				addr=m_Input[3]*100+m_Input[4]*10+key-0x30;
			    if(addr>0 && addr<201)
			    {
			        m_Input[5]=addr;
			        m_Cursor=49;
			    }
				else
				{
					//输入错误
					OutChars(2,35,"_  ",1);
					m_Cursor=35; //光标位置
				}
		    }
		    break;
		}	
	}
	key=0;
		
}


//页面处理
void Main_display()
{
	switch(m_View)
	{
	case 0:  //主界面
		MainView_Deal();
		break;
	case 1:  //主菜单选择
	    MainMenu_Deal();
	    break;
	case 81: //密码输入
	    Code_Input();
	    break;
	case 2:  //设置页面
		SetView_Deal();
		break;
	case 3:  //查询页面
		FindView_Deal();
		break;
	case 4:  //测试页面
		TestView_Deal();
		break;						  
	case 5:	 //安装页面
		InstallView_Deal();
		break;
	case 6:	 //系统页面
		SysView_Deal();
		break;
	case 7:	 //帮助页面
		Help();
		break;
	case 90: //火警显示
		FireDisplay();
		break;
	case 91: //故障显示
		ErrorDisplay1();
		break;
	case 92: //屏蔽显示
		DisableDisplay();
		break;
//	case 93: //监管显示
//		WatchDisplay();
//		break;
	case 11:	 //联动页面
		ManualAct();
		break;	
	case 12:	//地址屏蔽
	    ManualIsolate();
	    break;
	case 13:  //设置时钟
		SetClock();
		break;
//	case 14: //联动编程输入
//		SetProg();
//		break;
//	case 15: //注释信息输入
//		SetNote();
//		break;		
	case 21:  //自动登记
		AutoRegister();
		break;
	case 22:  //手动登记
		ManualRegister();
		break;
	case 23:  //设置打印机
	    SetPrint();
	    break;
	case 24:  //设置灭火延时
	    SetMieTime();
	    break;
	case 25:  //声光广播交替启动
	    SetSGGB();
	    break;
	case 41:  //系统配置
		SetSystem();
		break;	
	case 42:  //组网设置
	    SetNet();
	    break;
	case 43:  //清除操作
		Qccl();
		break;
	case 44:  //设置一级密码
	    SetCode1();
	    break;
	case 45:  //设置二级密码
		SetCode();
		break;
	case 46:  //USB文件读写
		SetUsbOpt();
		break;
	case 31: //查登记
		FindRegister();
		break;
	case 32: //查屏蔽
		DisableDisplay();
		break;
	case 33: //查故障
	    ErrorDisplay1();
	    break;
	case 34: //查火警记录
		FindHistory(1);
		break;
	case 35: //查联动记录
		FindHistory(2);
		break;
	case 36: //查历史记录
		FindHistory(0);
		break;
//	case 37: //查询编程
//		FindLogic();
//		break;
//	case 38: //查询注释
//		FindNote();
//		break;
	case 39: //查燃气报警记录
		FindHistory(3);
		break;
	case 61: //自检
	    SelfTest();
	    break;
	case 62: //模拟量曲线
		TestQx();
		break;
//	case 63: //回路纵览
//		TestZl();
//		break;
	case 64: //查灵敏度级别
		TestJibie();
		break;
	case 65: //电源数据
		TestPower();
		break;
	case 66: //可燃气体实时信息
	    TestKrkzq();
	    break;
	case 102: //检查功能
		CheckFuction();
		break;
	default:	//未实现功能
		NoFunction();
		break;
	}	
}

void MainTaskLoop(void)
{  
	uint8 i = 0;
	fwdgt_counter_reload(); // 喂狗
	
	
	/* 外部CAN中断已收齐一份文件：先更新后台缓存；只有实时信息界面打开时，才复制给界面使用的CanFile。 */
	/* 中断置位表示48字节文件收齐。主循环在非中断环境解析，避免在CAN中断里做较重的业务处理。 */
if(HaveExternalCanFile == 0x80)
	{
		SyncKeRanQiUploadFromCanFile(ExternalCanFile);
		if(m_View == 66)
		{
			for(i=0;i<48;i++)
				CanFile[i] = ExternalCanFile[i];
			HaveReciveCanFile = 0x80;
		}
		HaveExternalCanFile = 0;
	}

	 m_Can_Receive();	//Can接收数据处理
	CommToHxz();		//处理与黑匣子通讯
	CommToCRT();		//与图显通讯
	CommToTongXinBan();	//与通信板通讯
	QueryRegisterOnBoot();	//开机自动查询登记状态
	QueryKeRanQiRegisterOnBoot();	//开机自动查询可燃气登记状态
	Main_display();
	Refresh();		//定时刷新	

	
	if(TimeToProg)
	{
		TimeToProg=0;
		ProgramIsExplained();	//编程解释	
	}
	vTaskDelay(2);	
}


/****************************End*****************************/
