
//液晶显示程序

#include "HeaderFiles.h"


//设置当前调色板
void SetColor(uint16 FC,uint16 BC)
{
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x40;
	SendFrame3[2]=FC / 0x100;
	SendFrame3[3]=FC & 0xFF;
	SendFrame3[4]=BC / 0x100;
	SendFrame3[5]=BC & 0xFF;
	SendFrame3[6]=0xCC;
	SendFrame3[7]=0x33;
	SendFrame3[8]=0xC3;
	SendFrame3[9]=0x3C;
	TotalSendCount3=10;
	uart3Send();	
}

//设置行列间距
void SetHLwidth(uint8 H,uint8 L)
{
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x41;
	SendFrame3[2]=H;
	SendFrame3[3]=L;
	SendFrame3[4]=0xCC;
	SendFrame3[5]=0x33;
	SendFrame3[6]=0xC3;
	SendFrame3[7]=0x3C;
	TotalSendCount3=8;
	uart3Send();	
}

//设置光标显示
void SetCursor(uint16 x,uint16 y,uint8 type,uint8 en)
{
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x44;
	SendFrame3[2]=en;
	SendFrame3[3]=x / 0x100;
	SendFrame3[4]=x & 0xFF;
	SendFrame3[5]=y / 0x100;
	SendFrame3[6]=y & 0xFF;
	SendFrame3[7]=0x10;
	if(type==0)
		SendFrame3[8]=0x03;
	else
		SendFrame3[8]=0x0E;		
	SendFrame3[9]=0xCC;
	SendFrame3[10]=0x33;
	SendFrame3[11]=0xC3;
	SendFrame3[12]=0x3C;
	TotalSendCount3=13;
	uart3Send();	
	
}

//文本输出
void TextOut(uint16 x,uint16 y,uint8 *pString,uint8 hzktype)
{
	//汉字库类型 
	uint8 i,type,char1;
	switch(hzktype)
	{
	case 0:	 type=0x6F;	break;   //24
	case 1:	 type=0x55;	break;   //32
	case 2:	 type=0x54;	break;   //16
	case 3:	 type=0x6E;	break;   //12
	default: type=0x6F;	break;	 //24
	}
	SendFrame3[0]=0xAA;
	SendFrame3[1]=type;
	SendFrame3[2]=x / 0x100;
	SendFrame3[3]=x & 0xFF;
	SendFrame3[4]=y / 0x100;
	SendFrame3[5]=y & 0xFF;
	i=6;
	while(1)
	{
		char1=*pString; pString++;
		if(char1==0x00 ||char1==0xFF )
			break;
		else
		{
			SendFrame3[i]=char1;
			i++;
		}	
	}
	SendFrame3[i]=0xCC;
	SendFrame3[i+1]=0x33;
	SendFrame3[i+2]=0xC3;
	SendFrame3[i+3]=0x3C;
	TotalSendCount3=i+4;
	uart3Send();	
			
} 


//显示两位数字，h,l为行和列 
void OutTwoChar(uint8 h,uint8 l,uint8 num,uint8 type)
{
	uint16 x1,y1;
	if(type==0)
	{
		x1=l*12;
		y1=h*24;
	}
	else
	{
		x1=l*16;
		y1=h*32;		
	}
	SendFrame3[0]=0xAA;
	if(type==0)
		SendFrame3[1]=0x6F;
	else
		SendFrame3[1]=0x55;
	SendFrame3[2]=x1 / 0x100;
	SendFrame3[3]=x1 & 0xFF;
	SendFrame3[4]=y1 / 0x100;
	SendFrame3[5]=y1 & 0xFF;
	SendFrame3[6]=num /10+0x30;
	SendFrame3[7]=num % 10+0x30;
	SendFrame3[8]=0xCC;
	SendFrame3[9]=0x33;
	SendFrame3[10]=0xC3;
	SendFrame3[11]=0x3C;
	TotalSendCount3=12;
	uart3Send();	
}

//显示两位数字，x,y为坐标 
void OutTwoChar1(uint16 x,uint16 y,uint8 num,uint8 type)
{
	SendFrame3[0]=0xAA;
	if(type==0)
		SendFrame3[1]=0x6F;
	else if(type==2)
		SendFrame3[1]=0x54;	
	else
		SendFrame3[1]=0x55;

	SendFrame3[2]=x / 0x100;
	SendFrame3[3]=x & 0xFF;
	SendFrame3[4]=y / 0x100;
	SendFrame3[5]=y & 0xFF;
	SendFrame3[6]=num /10+0x30;
	SendFrame3[7]=num % 10+0x30;
	SendFrame3[8]=0xCC;
	SendFrame3[9]=0x33;
	SendFrame3[10]=0xC3;
	SendFrame3[11]=0x3C;
	TotalSendCount3=12;
	uart3Send();	
}

//显示三位数字
void OutThreeChar1(uint16 x,uint16 y,uint16 num,uint8 type)
{
	uint8 dbuf[4];
	dbuf[0]=num /100+0x30;
	dbuf[1]=(num %100)/10+0x30;
	dbuf[2]=num % 10+0x30;
	dbuf[3]=0x00;
	TextOut(x,y,dbuf,type);
}

//点显示
void DrawDot(uint8 *addr,uint8 num,uint8 type)
{
	uint8 i,j,char1;
	SendFrame3[0]=0xAA;
	if(type)
		SendFrame3[1]=0x50;	//背景色
	else
		SendFrame3[1]=0x51;	//前景色
	j=2;
	for(i=0;i<num;i++)
	{
		char1=*addr; addr++;
		SendFrame3[j]=char1;
		j++;
		char1=*addr; addr++;
		SendFrame3[j]=char1;
		j++;
		char1=*addr; addr++;
		SendFrame3[j]=char1;
		j++;
		char1=*addr; addr++;
		SendFrame3[j]=char1;
		j++;
	}
	SendFrame3[j]=0xCC;
	SendFrame3[j+1]=0x33;
	SendFrame3[j+2]=0xC3;
	SendFrame3[j+3]=0x3C;
	TotalSendCount3=j+4;
	uart3Send();	
	
}

//全屏清屏
void Lcdclr(void)
{
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x52;
	SendFrame3[2]=0xCC;
	SendFrame3[3]=0x33;
	SendFrame3[4]=0xC3;
	SendFrame3[5]=0x3C;
	TotalSendCount3=6;
	uart3Send();	
	
}

//显示图片
void DrawPicture(uint8 num)
{
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x70;
	SendFrame3[2]=num;
	SendFrame3[3]=0xCC;
	SendFrame3[4]=0x33;
	SendFrame3[5]=0xC3;
	SendFrame3[6]=0x3C;
	TotalSendCount3=7;
	uart3Send();	
	
}

//显示剪切图片
void DrawPicture1(uint8 num,uint16 x1,uint16 y1,uint16 x2,uint16 y2,uint16 x,uint16 y)
{
		SendFrame3[0]=0xAA;
		SendFrame3[1]=0x71;
		SendFrame3[2]=num; 	//图片号
		
		SendFrame3[3]=x1 >> 8;
		SendFrame3[4]=x1 & 0xFF;
		SendFrame3[5]=y1 >> 8;
		SendFrame3[6]=y1 & 0xFF;

		SendFrame3[7]=x2 >> 8;
		SendFrame3[8]=x2 & 0xFF;
		SendFrame3[9]=y2 >> 8;
		SendFrame3[10]=y2 & 0xFF;

		SendFrame3[11]=x >> 8;
		SendFrame3[12]=x & 0xFF;
		SendFrame3[13]=y >> 8;
		SendFrame3[14]=y & 0xFF;

		SendFrame3[15]=0xCC;
		SendFrame3[16]=0x33;
		SendFrame3[17]=0xC3;
		SendFrame3[18]=0x3C;
		TotalSendCount3=19;
		uart3Send();	
}

//画矩形或填充矩形
void DrawRect(uint16 x1,uint16 y1,uint16 x2,uint16 y2,uint8 type)
{
	SendFrame3[0]=0xAA;
	switch(type)
	{
	case 0:	SendFrame3[1]=0x59;	break;	//前景色画矩形
	case 1: SendFrame3[1]=0x69;	break;	//背景色画矩形	
	case 2:	SendFrame3[1]=0x5A;	break;	//背景色填充
	default: SendFrame3[1]=0x5B;	break;	//前景色填充	
	}
	SendFrame3[2]=x1 >> 8;
 	SendFrame3[3]=x1 & 0xFF;
	SendFrame3[4]=y1 >> 8;
 	SendFrame3[5]=y1 & 0xFF;
	SendFrame3[6]=x2 >> 8;
 	SendFrame3[7]=x2 & 0xFF;
	SendFrame3[8]=y2 >> 8;
 	SendFrame3[9]=y2 & 0xFF;


	SendFrame3[10]=0xCC;
	SendFrame3[11]=0x33;
	SendFrame3[12]=0xC3;
	SendFrame3[13]=0x3C;
	TotalSendCount3=14;
	uart3Send();	
	
}

//液晶背光控制
void LcdCtrl(uint8 type,uint8 num)
{
	if(type==0)
	{
		//关背光
		SendFrame3[0]=0xAA;
		SendFrame3[1]=0x5E;
		SendFrame3[2]=0xCC;
		SendFrame3[3]=0x33;
		SendFrame3[4]=0xC3;
		SendFrame3[5]=0x3C;
		TotalSendCount3=6;
		uart3Send();	
	}
	else if(type==1)
	{
		//开背光到最大亮度
		SendFrame3[0]=0xAA;
		SendFrame3[1]=0x5F;
		SendFrame3[2]=0xCC;
		SendFrame3[3]=0x33;
		SendFrame3[4]=0xC3;
		SendFrame3[5]=0x3C;
		TotalSendCount3=6;
		uart3Send();	
	}
	else
	{
		//背光调节
		SendFrame3[0]=0xAA;
		SendFrame3[1]=0x5F;
		SendFrame3[2]=num;
		SendFrame3[3]=0xCC;
		SendFrame3[4]=0x33;
		SendFrame3[5]=0xC3;
		SendFrame3[6]=0x3C;
		TotalSendCount3=7;
		uart3Send();	

	}
}

//好像不支持
void TimeDisplay(uint16 x,uint16 y)
{
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x9B;
	SendFrame3[2]=0xFF;
	SendFrame3[3]=0x00;
	SendFrame3[4]=0x02;
	SendFrame3[5]=BLUE / 0x100;
	SendFrame3[6]=BLUE & 0xFF;
	SendFrame3[7]=x / 0x100;
	SendFrame3[8]=x & 0xFF;
	SendFrame3[9]=y / 0x100;
	SendFrame3[10]=y & 0xFF;
	SendFrame3[11]=0xCC;
	SendFrame3[12]=0x33;
	SendFrame3[13]=0xC3;
	SendFrame3[14]=0x3C;
	TotalSendCount3=15;
	uart3Send();	
	
}

//显示时钟 
void ClockOut(uint16 x,uint16 y)
{
	uint8 r;
	r=0;
	if(Fcolor !=WHITE || Bcolor !=BLUE)
	{
		SetColor(WHITE,BLUE);
		r=1;
	}
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x6F;	//24*12点阵
	SendFrame3[2]=x >> 8;
	SendFrame3[3]=x & 0xFF;
	SendFrame3[4]=y >> 8;
	SendFrame3[5]=y & 0xFF;
	SendFrame3[6]='2';
	SendFrame3[7]='0';
	SendFrame3[8]=D_Year/10+0x30;
	SendFrame3[9]=D_Year%10+0x30;
	SendFrame3[10]='/';
	SendFrame3[11]=D_Month/10+0x30;
	SendFrame3[12]=D_Month%10+0x30;
	SendFrame3[13]='/';
	SendFrame3[14]=D_Day/10+0x30;
	SendFrame3[15]=D_Day%10+0x30;
	SendFrame3[16]=' ';

	SendFrame3[17]=D_Hour/10+0x30;
	SendFrame3[18]=D_Hour%10+0x30;
	SendFrame3[19]=':';
	SendFrame3[20]=D_Minute/10+0x30;
	SendFrame3[21]=D_Minute%10+0x30;
	SendFrame3[22]=':';
	SendFrame3[23]=D_Second/10+0x30;
	SendFrame3[24]=D_Second%10+0x30;

	SendFrame3[25]=0xCC;
	SendFrame3[26]=0x33;
	SendFrame3[27]=0xC3;
	SendFrame3[28]=0x3C;
	TotalSendCount3=29;
	uart3Send();
	
	if(r) 	SetColor(Fcolor,Bcolor);
	
}

//显示12*24的一个地址
void OutAddr(uint16 x,uint16 y,uint8 *pBuf)
{
	uint8 dd,locate;
	dd=*pBuf;
	pBuf++;
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x6F;
	SendFrame3[2]=x >> 8;
	SendFrame3[3]=x & 0xFF;
	SendFrame3[4]=y >> 8;
	SendFrame3[5]=y & 0xFF;
	locate=6;
	if(dd!=0)	//机器号
	{
		SendFrame3[locate]=dd /10+0x30;
		locate++;
		SendFrame3[locate]=dd % 10+0x30;
		locate++;
		SendFrame3[locate]='-';
		locate++;		
	}
	dd=*pBuf;
	pBuf++;
	if(dd==101)	//灭火回路
	{
		SendFrame3[locate]=195;		//灭3580
		locate++;
		SendFrame3[locate]=240;
		locate++;
		SendFrame3[locate]=187;		//火2780
		locate++;
		SendFrame3[locate]=240;
		locate++;
		dd=*pBuf;
		SendFrame3[locate]=dd /10+0x30;
		locate++;
		SendFrame3[locate]=dd % 10+0x30;
		locate++;
		SendFrame3[locate]=199;		//区3988
		locate++;
		SendFrame3[locate]=248;
	}
	else if(dd==102)	//多线回路
	{
		SendFrame3[locate]=182;
		locate++;
		SendFrame3[locate]=224;
		locate++;
		SendFrame3[locate]=207;
		locate++;
		SendFrame3[locate]=223;
		locate++;
		dd=*pBuf;
		pBuf++;
		SendFrame3[locate]=dd /100+0x30;
		locate++;
		SendFrame3[locate]=(dd %100)/10+0x30;
		locate++;
		SendFrame3[locate]=dd % 10+0x30;
	}
	else	//总线回路
	{
		SendFrame3[locate]=dd /100+0x30;
		locate++;
		SendFrame3[locate]=(dd %100)/10+0x30;
		locate++;
		SendFrame3[locate]=dd % 10+0x30;
		locate++;
		SendFrame3[locate]='-';
		locate++;
		dd=*pBuf;
		pBuf++;
		SendFrame3[locate]=dd /100+0x30;
		locate++;
		SendFrame3[locate]=(dd %100)/10+0x30;
		locate++;
		SendFrame3[locate]=dd % 10+0x30;
	}
	locate++;
	SendFrame3[locate]=0xCC;
	locate++;
	SendFrame3[locate]=0x33;
	locate++;
	SendFrame3[locate]=0xC3;
	locate++;
	SendFrame3[locate]=0x3C;
	locate++;
	TotalSendCount3=locate;
	uart3Send();		
}

//显示时间
void OutTime(uint16 x,uint16 y,uint8 *pBuf,uint8 type)
{
	uint8 time;

	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x6F;
	SendFrame3[2]=x / 0x100;
	SendFrame3[3]=x & 0xFF;
	SendFrame3[4]=y / 0x100;
	SendFrame3[5]=y & 0xFF;
	SendFrame3[6]='2';
	SendFrame3[7]='0';
	time=*pBuf;
	pBuf++;
	SendFrame3[8]=time/10+0x30;
	SendFrame3[9]=time%10+0x30;
	SendFrame3[10]='/';
	time=*pBuf;
	pBuf++;
	SendFrame3[11]=time/10+0x30;
	SendFrame3[12]=time%10+0x30;
	SendFrame3[13]='/';
	time=*pBuf;
	pBuf++;
	SendFrame3[14]=time/10+0x30;
	SendFrame3[15]=time%10+0x30;
	SendFrame3[16]=' ';

	time=*pBuf;
	pBuf++;
	SendFrame3[17]=time/10+0x30;
	SendFrame3[18]=time%10+0x30;
	SendFrame3[19]=':';
	time=*pBuf;
	pBuf++;
	SendFrame3[20]=time/10+0x30;
	SendFrame3[21]=time%10+0x30;
	if(type==1)
	{
		//不显示秒
		SendFrame3[22]=0xCC;
		SendFrame3[23]=0x33;
		SendFrame3[24]=0xC3;
		SendFrame3[25]=0x3C;
		TotalSendCount3=26;
	}
	else
	{
		//显示秒
		SendFrame3[22]=':';
		time=*pBuf;
		SendFrame3[23]=time/10+0x30;
		SendFrame3[24]=time%10+0x30;
	
		SendFrame3[25]=0xCC;
		SendFrame3[26]=0x33;
		SendFrame3[27]=0xC3;
		SendFrame3[28]=0x3C;
		TotalSendCount3=29;
	}
	uart3Send();	
 
}

//窗口函数
void Windows(uint16 x1,uint16 y1,uint16 x2,uint16 y2,uint8 *title)
{
	Fcolor=WHITE;
	Bcolor=NAVY;	
	SetColor(Fcolor,Bcolor);
	DrawRect(0,25,800,455,2);	//填充标题栏
	
	TextOut(0,30,title,1);

	/*
	SetColor(WHITE,BLUE);
	DrawRect(x1,y1,x2,y1+30,2);	//标题栏
	TextOut((x1+7),(y1+7),title,0);  //标题栏文字

	Fcolor=WHITE;
	Bcolor=DGRAY;
	SetColor(Fcolor,Bcolor);
	DrawRect(x1,y1+30,x2,y2,2);	//显示区
	//窗口边界
	DrawRect((x1+1),(y1+1),(x2-1),(y1+1),3);
	DrawRect((x1+1),(y1+1),(x1+1),(y2-1),3);
	DrawRect((x1+1),(y2-1),(x2-1),(y2-1),3);
	DrawRect((x2-1),(y1+1),(x2-1),(y2-1),3);
	*/
}

//文本框
void Edit(uint8 h,uint8 l,uint8 length)
{
	uint16 x1,y1,x2,y2;
	x1=l*12-20;
	y1=h*24-5;
	x2=(l+length)*12+20;
	y2=y1+34;	
	Fcolor=BLACK;
	Bcolor=WHITE;
	SetColor(Fcolor,Bcolor);
	//DrawRect(x1,y1,x2,y2,2);	//显示区

	//窗口边界
	//DrawRect((x1+1),(y1+1),(x2-1),(y1+1),3);
	//DrawRect((x1+1),(y1+1),(x1+1),(y2-1),3);
	//DrawRect((x1+1),(y2-1),(x2-1),(y2-1),3);
	//DrawRect((x2-1),(y1+1),(x2-1),(y2-1),3);

	//DrawRect(x1-1,y1-1,x2+1,y2+1,2);	//显示区
	DrawRect(x1,y1,x2,y2,2);	//显示区
	DrawRect(x1+1,y1+1,x2-1,y2-1,0);	//前景色画矩形
}

void OutChars(uint8 h,uint8 l,uint8 *pString,uint8 type)
{
	uint8 i,char1;
	uint16 x1,y1;
	if(type==0)
	{
		x1=l*12;
		y1=h*24;
	}
	else
	{
		x1=l*16;
		y1=h*32;		
	}
	SendFrame3[0]=0xAA;
	if(type==0)
		SendFrame3[1]=0x6F;/////////////  24*24字体大小
	else
		SendFrame3[1]=0x55;/////////////  32*32字体大小
	SendFrame3[2]=x1 / 0x100;
	SendFrame3[3]=x1 & 0xFF;
	SendFrame3[4]=y1 / 0x100;
	SendFrame3[5]=y1 & 0xFF;
	i=6;
	while(1)
	{
		char1=*pString; pString++;
		if(char1==0x00 || char1==0xFF)
			break;
		else
		{
			SendFrame3[i]=char1;
			i++;
		}	
	}
	SendFrame3[i]=0xCC;
	SendFrame3[i+1]=0x33;
	SendFrame3[i+2]=0xC3;
	SendFrame3[i+3]=0x3C;
	TotalSendCount3=i+4;
	uart3Send();	

}

//显示三位数字，h,l为行和列 
void OutThreeChar(uint8 h,uint8 l,uint16 num,uint8 type)
{
	uint16 x1,y1;
	if(type==0)
	{
		x1=l*12;
		y1=h*24;
	}
	else
	{
		x1=l*16;
		y1=h*32;		
	}
	SendFrame3[0]=0xAA;
	if(type==0)
		SendFrame3[1]=0x6F;
	else
		SendFrame3[1]=0x55;
	SendFrame3[2]=x1 / 0x100;
	SendFrame3[3]=x1 & 0xFF;
	SendFrame3[4]=y1 / 0x100;
	SendFrame3[5]=y1 & 0xFF;
	SendFrame3[6]=num /100+0x30;
	SendFrame3[7]=(num %100)/10+0x30;
	SendFrame3[8]=num % 10+0x30;
	SendFrame3[9]=0xCC;
	SendFrame3[10]=0x33;
	SendFrame3[11]=0xC3;
	SendFrame3[12]=0x3C;
	TotalSendCount3=13;
	uart3Send();	
}

//显示一个字符，h,l为行和列
void OutOneChar(uint8 h,uint8 l,uint8 char1,uint8 type)
{
	uint16 x1,y1;
	if(type==0)
	{
		x1=l*12;
		y1=h*24;
	}
	else
	{
		x1=l*16;
		y1=h*32;		
	}
	SendFrame3[0]=0xAA;
	if(type==0)
		SendFrame3[1]=0x6F;
	else
		SendFrame3[1]=0x55;
	SendFrame3[2]=x1 / 0x100;
	SendFrame3[3]=x1 & 0xFF;
	SendFrame3[4]=y1 / 0x100;
	SendFrame3[5]=y1 & 0xFF;
	SendFrame3[6]=char1;
	SendFrame3[7]=0xCC;
	SendFrame3[8]=0x33;
	SendFrame3[9]=0xC3;
	SendFrame3[10]=0x3C;
	TotalSendCount3=11;
	uart3Send();	
}

//显示一位数字，x,y为坐标 
void OutOneChar1(uint16 x,uint16 y,uint8 num,uint8 type)
{
	SendFrame3[0]=0xAA;
	if(type==0)
		SendFrame3[1]=0x6F;
	else if(type==2)
		SendFrame3[1]=0x54;	
	else
		SendFrame3[1]=0x55;
	SendFrame3[2]=x / 0x100;
	SendFrame3[3]=x & 0xFF;
	SendFrame3[4]=y / 0x100;
	SendFrame3[5]=y & 0xFF;
	SendFrame3[6]=num;
	SendFrame3[7]=0xCC;
	SendFrame3[8]=0x33;
	SendFrame3[9]=0xC3;
	SendFrame3[10]=0x3C;
	TotalSendCount3=11;
	uart3Send();	
}


//用前景色连线
void DrawLine(uint16 x1,uint16 y1,uint16 x2,uint16 y2)
{
	SendFrame3[0]=0xAA;
	SendFrame3[1]=0x56;
	SendFrame3[2]=x1 >> 8;
 	SendFrame3[3]=x1 & 0xFF;
	SendFrame3[4]=y1 >> 8;
 	SendFrame3[5]=y1 & 0xFF;
	SendFrame3[6]=x2 >> 8;
 	SendFrame3[7]=x2 & 0xFF;
	SendFrame3[8]=y2 >> 8;
 	SendFrame3[9]=y2 & 0xFF;

	SendFrame3[10]=0xCC;
	SendFrame3[11]=0x33;
	SendFrame3[12]=0xC3;
	SendFrame3[13]=0x3C;
	TotalSendCount3=14;
	uart3Send();	

}

/****************************************************************************
* 名称：GUI_WindowsDraw()
* 功能：显示窗口。根据提供的窗口参数进行画窗口。
* 入口参数：win		窗口句柄
* 出口参数：返回0表示操作失败，返回1表示操作成功
****************************************************************************/
uint8_t  GUI_WindowsDraw(WINDOWS *win)
{  
   //uint8_t  *str;
   //uint32_t  bak, bak1, bak2;
   	
   /* 开始画窗口 */
   DrawRect(win->x, win->y, win->x + win->with - 1, win->y + win->hight - 1, 2);

   DrawRect(win->x, win->y, win->x + win->with - 1, win->y + 22, 3);
   DrawRect(win->x, win->y + win->hight - 11, win->x + win->with - 1, win->y + win->hight - 1, 3);
   
   DrawRect(win->x, win->y, win->x + win->with - 1, win->y + win->hight - 1, 0);	// 画窗口

   DrawLine(win->x, win->y + 12, win->x + win->with - 1, win->y + 12);							// 画标题目栏
   DrawLine(win->x + 12, win->y, win->x + 12, win->y + 12);										// 画关闭窗号按钮
   DrawLine(win->x, win->y, win->x + 12, win->y + 12);
   DrawLine(win->x + 12, win->y, win->x, win->y + 12);
   
   
   return(1);

}
