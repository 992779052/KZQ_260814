
//SPIOption.c

#include "SPIOption.h"

uint32_t 	pFireHistory; 	//火警历史记录指针
uint32_t	pRunHistory;	//联动历史记录指针
uint32_t	pOtherHistory;	//其它历史记录指针
uint32_t 	pKFireHistory; 	//燃气报警历史记录指针


extern void WriteToHxzZone(void);
 
//历史记录初始化
void InitHistory(void)
{
	SPI_FLASH_HighSpeedReadCont(E_HistoryInitAddr,32,HistoryBuf);
	if(HistoryBuf[0]!=0xA3 || HistoryBuf[1]!=0x53 || HistoryBuf[2]!=0x73)
	{
		//未初始化
		Flash_Sector_Erase(31,31);	//清除指针块
		Flash_Sector_Erase(47,47);	//清除火警历史最后一块
		Flash_Sector_Erase(63,63);	//清除联动历史最后一块
		Flash_Sector_Erase(127,127);	//清除其它历史最后一块
		Flash_Sector_Erase(143,143);	//清除可燃报警历史最后一块
		
		pFireHistory=E_FireBegin;
		pRunHistory=E_RunBegin;
		pOtherHistory=E_OtherBegin;
		pKFireHistory=E_KFireBegin;


		//写入初始化标志
		HistoryBuf[0]=0xA3;
		HistoryBuf[1]=0x53;
		HistoryBuf[2]=0x73;
		HistoryBuf[3]=0;
		//写入指针
		HistoryBuf[4]=pFireHistory  >> 16;
		HistoryBuf[5]=(pFireHistory & 0xFFFF) >> 8;
		HistoryBuf[6]=pFireHistory & 0xFF;
		HistoryBuf[7]=0;

		HistoryBuf[8]=pRunHistory  >> 16;
		HistoryBuf[9]=(pRunHistory & 0xFFFF) >> 8;
		HistoryBuf[10]=pRunHistory & 0xFF;
		HistoryBuf[11]=0;

		HistoryBuf[12]=pOtherHistory >> 16;
		HistoryBuf[13]=(pOtherHistory & 0xFFFF) >> 8;
		HistoryBuf[14]=pOtherHistory & 0xFF;
		HistoryBuf[15]=0;
		
		HistoryBuf[16]=pKFireHistory >> 16;
		HistoryBuf[17]=(pKFireHistory & 0xFFFF) >> 8;
		HistoryBuf[18]=pKFireHistory & 0xFF;
		HistoryBuf[19]=0;

		Auto_Add_Write(E_HistoryInitAddr,HistoryBuf,32);
	}
	else
	{
		//从SPI中得到写入指针，因为是16个字节，所以低4位为0
		pFireHistory=(HistoryBuf[4]<<16)+ (HistoryBuf[5]<<8)+ (HistoryBuf[6] & 0xF0);
		pRunHistory=(HistoryBuf[8]<<16)+ (HistoryBuf[9]<<8)+ (HistoryBuf[10] & 0xF0);
		pOtherHistory=(HistoryBuf[12]<<16)+ (HistoryBuf[13]<<8)+ (HistoryBuf[14] & 0xF0);
		pKFireHistory=(HistoryBuf[16]<<16)+ (HistoryBuf[17]<<8)+ (HistoryBuf[18] & 0xF0);
	}

}

//写入一条历史记录	   0:其它 1：火警 2：联动  3:燃气报警
void WriteHisRecord(uint8_t type)
{
    uint8_t sector;
	uint32_t pAddr;
	
	//return;
	switch(type)
	{
	case 1:	//火警记录
		if((pFireHistory % 0x1000)==0)
		{
			//是一块的起始，需要擦除块
			sector=pFireHistory >>12;
			Flash_Sector_Erase(sector,sector);
		}
		pAddr=pFireHistory;
		//修改指针
		pFireHistory +=16;
		if(pFireHistory>E_FireEnd)
			pFireHistory=E_FireBegin;	
		pFireHistory &= 0xFFFFF0;
		break;
	case 2:	//联动记录
		if((pRunHistory % 0x1000)==0)
		{
			//是一块的起始，需要擦除块
			sector=pRunHistory >>12;
			Flash_Sector_Erase(sector,sector);
		}
		pAddr=pRunHistory;
		//修改指针
		pRunHistory +=16;
		if(pRunHistory>E_RunEnd)
			pRunHistory=E_RunBegin;	
		pRunHistory &= 0xFFFFF0;
		break;
	case 3:	//燃气报警记录
		if((pKFireHistory % 0x1000)==0)
		{
			//是一块的起始，需要擦除块
			sector=pKFireHistory >>12;
			Flash_Sector_Erase(sector,sector);
		}
		pAddr=pKFireHistory;
		//修改指针
		pKFireHistory +=16;
		if(pKFireHistory>E_KFireEnd)
			pKFireHistory=E_KFireBegin;	
		pKFireHistory &= 0xFFFFF0;
		break;		
	default:	//其它记录
		if((pOtherHistory % 0x1000)==0)
		{
			//是一块的起始，需要擦除块
			sector=pOtherHistory >>12;
			Flash_Sector_Erase(sector,sector);
		}
		pAddr=pOtherHistory;
		//修改指针
		pOtherHistory +=16;
		if(pOtherHistory>E_OtherEnd)
			pOtherHistory=E_OtherBegin;	
		pOtherHistory &= 0xFFFFF0;
		break;
	}
    //写入记录
    HistoryBuf[0]=m_Record.sign;
    HistoryBuf[1]=m_Record.panel;
    HistoryBuf[2]=m_Record.loop;
    HistoryBuf[3]=m_Record.addr;
    HistoryBuf[4]=m_Record.type;
	HistoryBuf[11]=m_Record.type1;
	HistoryBuf[12]=m_Record.type2;
	if(m_Record.sign==0x04)
	{
		//关机记录
	    HistoryBuf[5]=m_Record.year;
	    HistoryBuf[6]=m_Record.month;
	    HistoryBuf[7]=m_Record.day;
	    HistoryBuf[8]=m_Record.hour;
	    HistoryBuf[9]=m_Record.minute;
	    HistoryBuf[10]=m_Record.second;
	}
	else
	{
	    HistoryBuf[5]=D_Year;
	    HistoryBuf[6]=D_Month;
	    HistoryBuf[7]=D_Day;
	    HistoryBuf[8]=D_Hour;
	    HistoryBuf[9]=D_Minute;
	    HistoryBuf[10]=D_Second;
    }
	Auto_Add_Write(pAddr,HistoryBuf,14);

    
	//修改写入位置指针
	Flash_Sector_Erase(31,31);	//清除指针块
	//写入初始化标志
	HistoryBuf[0]=0xA3;
	HistoryBuf[1]=0x53;
	HistoryBuf[2]=0x73;
	HistoryBuf[3]=0;
	//写入指针
	HistoryBuf[4]=pFireHistory  >> 16;
	HistoryBuf[5]=(pFireHistory & 0xFFFF) >> 8;
	HistoryBuf[6]=pFireHistory & 0xFF;
	HistoryBuf[7]=0;
	
	HistoryBuf[8]=pRunHistory  >> 16;
	HistoryBuf[9]=(pRunHistory & 0xFFFF) >> 8;
	HistoryBuf[10]=pRunHistory & 0xFF;
	HistoryBuf[11]=0;
	
	HistoryBuf[12]=pOtherHistory >> 16;
	HistoryBuf[13]=(pOtherHistory & 0xFFFF) >> 8;
	HistoryBuf[14]=pOtherHistory & 0xFF;
	HistoryBuf[15]=0;

	HistoryBuf[16]=pKFireHistory >> 16;
	HistoryBuf[17]=(pKFireHistory & 0xFFFF) >> 8;
	HistoryBuf[18]=pKFireHistory & 0xFF;
	HistoryBuf[19]=0;
	
	Auto_Add_Write(E_HistoryInitAddr,HistoryBuf,20);
	
	if(m_Record.sign !=0x05)	//手自动切换已经另传
		WriteToHxzZone();	//写黑匣子发送区
            
}


//屏蔽初始化
void InitDisable(void)
{
	SPI_FLASH_HighSpeedReadCont(E_DisableAddr,4,HistoryBuf);
    if(HistoryBuf[0]!=0xAA || HistoryBuf[1]!=0x55)
    {
        //未初始化
		Flash_Sector_Erase(30,30);	//清除屏蔽块
		
		//写入初始化标志
		HistoryBuf[0]=0xAA;
		HistoryBuf[1]=0x55;
		HistoryBuf[2]=0;	//屏蔽总数
		HistoryBuf[3]=0;
		Auto_Add_Write(E_DisableAddr,HistoryBuf,4);
		Total_Isolate=0;


    }
    else
	{
		//得到屏蔽总数
		Total_Isolate=HistoryBuf[2];
		if(Total_Isolate>0)
		{
			m_deng[0] |=0x80;	//屏蔽灯
		}	
	}

}


