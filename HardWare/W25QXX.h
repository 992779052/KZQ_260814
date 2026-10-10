/************************************************************
 * 版权：严禁商业用途，仅供学习使用。 
 * 文件：W25QXX.h
 * 作者: 凌智君
 * 平台: 凌跃智能雷神开发板
 * 微信: YunXiang_TechShare  
 * Q Q: 2228398717
 * 公众号：凌智君
************************************************************/
#ifndef __W25QXX_H
#define __W25QXX_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

/************************* 宏定义 *************************/
#define  SPI_FLASH_PAGE_SIZE           0x100

#define  SPI_FLASH_CS_LOW()            gpio_bit_reset(GPIOD, GPIO_PIN_4)
#define  SPI_FLASH_CS_HIGH()           gpio_bit_set(GPIOD, GPIO_PIN_4)


#define WRITE            0x02     /* write to memory instruction */
#define QUADWRITE        0x32     /* quad write to memory instruction */
#define WRSR             0x01     /* write status register instruction */
#define WREN             0x06     /* write enable instruction */

#define READ             0x03     /* read from memory instruction */
#define QUADREAD         0x6B     /* read from memory instruction */
#define RDSR             0x05     /* read status register instruction */
#define RDID             0x9F     /* read identification */

#define SE               0x20     /* sector erase instruction */
#define BE               0xC7     /* bulk erase instruction */

#define WIP_FLAG         0x01     /* write in progress(wip) flag */

#define DUMMY_BYTE       0xA5

/************************ 变量定义 ************************/


/************************ 函数定义 ************************/

void SPI_Flash_Init(void);   // SPI 引脚初始化

/* erase the specified flash sector */
void spi_flash_sector_erase(uint32_t sector_addr);   // 擦除指定地址数据
/* erase the entire flash */
void spi_flash_bulk_erase(void);   // 擦除所有数据
/* write more than one byte to the flash */
void spi_flash_page_write(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write);   // 写入超过一个字节的数据
/* write block of data to the flash */
void spi_flash_buffer_write(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write);  // 写整个扇区数据
/* read a block of data from the flash */
void spi_flash_buffer_read(uint8_t* pbuffer, uint32_t read_addr, uint16_t num_byte_to_read);  // 读取整个扇区的数据
/* read flash identification */
uint32_t spi_flash_read_id(void);  // 获取Flash的ID
/* initiate a read data byte (read) sequence from the flash */
void spi_flash_start_read_sequence(uint32_t read_addr);  // 读取数据
/* read a byte from the SPI flash */
uint8_t spi_flash_read_byte(void);  // 读取一个字节
/* send a byte through the SPI interface and return the byte received from the SPI bus */
uint8_t spi_flash_send_byte(uint8_t byte);   // 发送一个字节
/* send a half word through the SPI interface and return the half word received from the SPI bus */
uint16_t spi_flash_send_halfword(uint16_t half_word);
/* enable the write access to the flash */
void spi_flash_write_enable(void);   // 使能写
/* poll the status of the write in progress (wip) flag in the flash's status register */
void spi_flash_wait_for_write_end(void);  

void Flash_Sector_Erase(uint32_t Dst1,uint32_t Dst2);


void SPI_FLASH_HighSpeedReadCont(uint32_t ReadAddr, uint16_t NumByteToRead,uint8_t* pBuffer);

void Auto_Add_Write(uint32_t Dst,uint8_t *pBuffer, uint32_t bytes);

void SPI_FLASH_WriteCont(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToRead);

				    
#endif


/****************************End*****************************/

