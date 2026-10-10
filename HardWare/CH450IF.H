
/* ********************************************************************************************* */
#ifndef __CH450IF_H
#define __CH450IF_H

#include "HeaderFiles.h"


// 对外子程序
void CH453_Write(uint8_t data1, uint8_t data2);	// 向CH453发出操作命令

uint8_t CH453_ReadByte( void );    // 读取CH453数据

#endif

