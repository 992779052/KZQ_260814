
//液晶显示头文件

typedef unsigned char uint8;
typedef unsigned short uint16;

/*********************************************************************************************************
** 设置颜色宏定义
*********************************************************************************************************/
#define   BLACK     0x0000                                              /*  黑色    0,   0,   0         */
#define   NAVY      0x000F                                              /*  深蓝色  0,   0, 128         */
#define   DGREEN    0x03E0                                              /*  深绿色  0,  128,  0         */
#define   DCYAN	    0x03EF                                              /*  深青色  0,  128, 128        */
#define   MAROON    0x7800                                              /*  深红色  128,   0,   0       */
#define   PURPLE    0x780F                                              /*  紫色    128,   0, 128       */
#define   OLIVE     0x7BE0                                              /*  橄榄绿  128, 128,   0       */
#define   LGRAY     0xC618                                              /*  灰白色  192, 192, 192       */
#define   DGRAY     0x7BEF                                              /*  深灰色  128, 128, 128       */
#define   BLUE      0x001F                                              /*  蓝色    0,   0, 255         */
#define   GREEN     0x07E0                                              /*  绿色    0, 255,   0         */
#define   CYAN      0x07FF                                              /*  青色    0, 255, 255         */
#define   RED       0xF800                                              /*  红色    255,   0,   0       */
#define   MAGENTA   0xF81F                                              /*  品红    255,   0, 255       */
#define   YELLOW    0xFFE0                                              /*  黄色    255, 255, 0         */
#define   WHITE     0xFFFF                                              /*  白色    255, 255, 255       */
#define	  RED1		0xFC20   										    /*  浅红色  255, 128, 128       */

/* 定义窗口数据结构 */
typedef  struct
{  uint32_t  x;			// 窗口位置(左上角的x坐标)
   uint32_t  y;			// 窗口位置(左上角的y坐标)
    
   uint32_t  with;		// 窗口宽度
   uint32_t  hight;		// 窗口高度
   
   uint8_t   *title;		// 定义标题栏指针 (标题字符为ASCII字符串，最大个数受窗口限制)
   uint8_t   *state;		// 定义状态栏指针 (若为空时则不显示状态栏)   
} WINDOWS;



//设置当前调色板
extern void SetColor(uint16 FC,uint16 BC);

//设置行列间距
extern void SetHLwidth(uint8 H,uint8 L);

//设置光标显示
extern void SetCursor(uint16 x,uint16 y,uint8 type,uint8 en);

//文本输出
extern void TextOut(uint16 x,uint16 y,uint8 *pString,uint8 hzktype);

//点显示
extern void DrawDot(uint8 *addr,uint8 num,uint8 type);

//全屏清屏
extern void Lcdclr(void);

//显示图片
extern void DrawPicture(uint8 num);

//画矩形或填充矩形
extern void DrawRect(uint16 x1,uint16 y1,uint16 x2,uint16 y2,uint8 type);

//液晶背光控制
extern void LcdCtrl(uint8 type,uint8 num);

//指定位置显示时钟
extern void TimeDisplay(uint16 x,uint16 y);

//显示时钟 
extern void ClockOut(uint16 x,uint16 y);


//显示剪切图片
extern void DrawPicture1(uint8 num,uint16 x1,uint16 y1,uint16 x2,uint16 y2,uint16 x,uint16 y);

extern void OutOneChar(uint8 h,uint8 l,uint8 char1,uint8 type);
extern void OutTwoChar(uint8 h,uint8 l,uint8 num,uint8 type);
extern void OutThreeChar(uint8 h,uint8 l,uint16 num,uint8 type);
extern void OutChars(uint8 h,uint8 l,uint8 *pString,uint8 type);

//显示12*24的一个地址
extern void OutAddr(uint16 x,uint16 y,uint8 *pBuf);

//显示时间
extern void OutTime(uint16 x,uint16 y,uint8 *pBuf,uint8 type);

//画窗口
extern void Windows(uint16 x1,uint16 y1,uint16 x2,uint16 y2,uint8 *title);

//画文本框
extern void Edit(uint8 h,uint8 l,uint8 length);

//用前景色连线
void DrawLine(uint16 x1,uint16 y1,uint16 x2,uint16 y2);

//显示两位数字，x,y为坐标 
extern void OutTwoChar1(uint16 x,uint16 y,uint8 num,uint8 type);

//显示一位数字，x,y为坐标 
extern void OutOneChar1(uint16 x,uint16 y,uint8 num,uint8 type);

extern void OutThreeChar1(uint16 x,uint16 y,uint16 num,uint8 type);

extern  uint8_t  GUI_WindowsDraw(WINDOWS *win);
