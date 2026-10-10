/* ***************************
 Website:  http://wch.cn
 Email:    tech@wch.cn
 Author:   W.ch 2006.4
 CH450 接口子程序
****************************
 CH450的2线接口，只需要2个I/O引脚，兼容I2C/IIC时序
 为了节约传输时间，可以适当减少SCL/SDA之间的延时
 如果希望SCL与其它电路共用，那么只要保持SDA不变化，SCL就可以用于任何用途
 如果希望SCL和SDA都与其它电路共用，那么关键是确保SDA只在SCL为低电平期间发生变化，所以SCL和SDA适合作为输出引脚使用
     1、对于SCL引脚，直接输出
     2、对于SDA引脚，输出时：
          如果SCL为低电平，那么直接输出，
          如果SCL为高电平，那么先将SCL输出低电平，再将SDA输出，最后将SCL恢复为高电平
*************************************************************************** */
#include "CH450IF.H"		// 修改该文件以适应硬件环境/单片机型号等


void CH453_SDA_SET()
{
	gpio_bit_set(GPIOC, GPIO_PIN_9);
}

void CH453_SDA_OUT()
{
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_9);   // GPIO 模式设置
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  // 输出参数设置
}

void CH453_SCL_SET()
{
	gpio_bit_set(GPIOA, GPIO_PIN_8);
}

void CH453_SDA_CLR()
{
	gpio_bit_reset(GPIOC, GPIO_PIN_9);
}

void CH453_SCL_CLR()
{
	gpio_bit_reset(GPIOA, GPIO_PIN_8);
}

void CH453_SCL_OUT()
{
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);   // GPIO 模式设置
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);  // 输出参数设置
}

void CH453_DI_IN()
{
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_9);        // 引脚配置
    gpio_output_options_set(GPIOC, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, GPIO_PIN_9);   // 参数设置
}

void DELAY_0_1US(void)
{
	volatile uint32_t i;
   	for (i=0; i<10; i++);
}


void CH453_I2c_Start(void)  // 操作起始
{
	CH453_SDA_SET();   /*发送起始条件的数据信号*/
	CH453_SDA_OUT();   /* 设置SDA为输出方向 */
	CH453_SCL_SET();
	CH453_SCL_OUT();   /* 设置SCL为输出方向 */
	DELAY_0_1US();
	CH453_SDA_CLR();   /*发送起始信号*/
	DELAY_0_1US();      
	CH453_SCL_CLR();   /*钳住I2C总线，准备发送或接收数据 */
}

void CH453_I2c_Stop(void)  // 操作结束
{
	CH453_SDA_CLR();
	DELAY_0_1US();
	CH453_SCL_SET();
	DELAY_0_1US();
	CH453_SDA_SET();  /*发送I2C总线结束信号*/
	DELAY_0_1US();
}

void CH453_I2c_WrByte(unsigned char dat)	//写一个字节数据
{
	unsigned char i;
	for(i=0;i!=8;i++)  // 输出8位数据
	{
		if(dat&0x80) {CH453_SDA_SET();}
		else {CH453_SDA_CLR();}
		DELAY_0_1US();
		CH453_SCL_SET();
		dat<<=1;
		DELAY_0_1US();  // 可选延时
		CH453_SCL_CLR();
	}
	CH453_SDA_SET();
	DELAY_0_1US();
	CH453_SCL_SET();  // 接收应答
	DELAY_0_1US();
	CH453_SCL_CLR();
}


unsigned char  CH453_I2c_RdByte(void)		//读一个字节数据
{
    unsigned char dat,i;
    CH453_SDA_SET();
    CH453_DI_IN();    // 设置SDA为输入方向 
    dat=0;
    for( i = 0; i != 8; i++ )    // 输入8位数据
    {
        DELAY_0_1US();    // 可选延时
        CH453_SCL_SET();
        DELAY_0_1US();    // 可选延时
        dat<<=1;
        if(gpio_input_bit_get(GPIOC, GPIO_PIN_9)==SET) dat++;    // 输入1位
        CH453_SCL_CLR();
    }
    CH453_SDA_SET();
    DELAY_0_1US();
    CH453_SCL_SET();    // 发出无效应答
    DELAY_0_1US();
    CH453_SCL_CLR();
    return(dat);
}

void CH453_Write(unsigned char data1, unsigned char data2)	//写命令
{
	CH453_I2c_Start();               /*启动总线*/
   	CH453_I2c_WrByte(data1);
   	CH453_I2c_WrByte(data2);         /*发送数据*/
  	CH453_I2c_Stop();                /*结束总线*/ 
}


unsigned char CH453_ReadByte( void )    // 读取CH453数据
{
	unsigned char din;
    CH453_I2c_Start();    // 启动总线
    CH453_I2c_WrByte( 0x4F );    
    din=CH453_I2c_RdByte();    // 读取数据
    CH453_I2c_Stop();    // 结束总线
    return( din );
}



