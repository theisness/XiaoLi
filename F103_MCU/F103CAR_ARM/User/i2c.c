#include "i2c.h"
#include "lib_common.h"

////////////////////////////////////////// 移植须知！//////////////////////////////////////
//
// 移植此模拟IIC时，需要注意 IIC_WriteByteToSlave() 写从器件的地址时，这种方式的地址是已经
// 左移过1位的，注意与 ( I2C_Addr <<1 | 0 )的方式进行区分，否则不能与从器件通信,谨慎移植!
//
///////////////////////////////////////////////////////////////////////////////////////////

//初始化IIC
void IIC_Init(void)
{					     

	RCC->APB2ENR|=1<<3;//先使能外设IO PORTB时钟 	
	GPIOB->CRL&=0XFF00FFFF;//PB4,5 推挽输出
	GPIOB->CRL|=0X00330000;	   
	GPIOB->ODR|=3<<4;     //PB4,5 输出高
	
}//产生IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
    IIC_SDA(1);
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL(0);//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL(0);
    IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL(1);
    IIC_SDA(1);//发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();//SDA设置为输入
    IIC_SDA(1);delay_us(1);
    IIC_SCL(1);delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(0);//时钟输出0
    return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}
//不产生ACK应答
void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        IIC_SDA((txd&0x80)>>7);
        txd<<=1;
        delay_us(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL(1);
        delay_us(2);
        IIC_SCL(0);
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
    {
        IIC_SCL(0);
        delay_us(2);
        IIC_SCL(1);
        receive<<=1;
        if(READ_SDA)receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}


/******************************************************************************
* 函  数：uint8_t IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t addr)
* 功　能：读取指定设备 指定寄存器的一个值
* 参  数：I2C_Addr  目标设备地址
          reg       寄存器地址
          *buf      读取数据要存储的地址
* 返回值：返回 1失败 0成功
* 备  注：无
*******************************************************************************/
uint8_t IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t *buf)
{
    IIC_Start();
    IIC_Send_Byte(I2C_Addr);     //发送从机地址
    if(IIC_Wait_Ack()) //如果从机未应答则数据发送失败
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg); //发送寄存器地址
    IIC_Wait_Ack();

    IIC_Start();
    IIC_Send_Byte(I2C_Addr+1); //进入接收模式
    IIC_Wait_Ack();
    *buf=IIC_Read_Byte(0);
    IIC_Stop(); //产生一个停止条件
    return 0;
}

/*************************************************************************************
* 函  数：uint8_t IIC_WriteByteFromSlave(uint8_t I2C_Addr,uint8_t addr，uint8_t buf))
* 功　能：写入指定设备 指定寄存器的一个值
* 参  数：I2C_Addr  目标设备地址
          reg       寄存器地址
          buf       要写入的数据
* 返回值：1 失败 0成功
* 备  注：无
**************************************************************************************/
uint8_t IIC_WriteByteToSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t data)
{
    IIC_Start();
    IIC_Send_Byte(I2C_Addr); //发送从机地址
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //从机地址写入失败
    }
    IIC_Send_Byte(reg); //发送寄存器地址
    IIC_Wait_Ack();
    IIC_Send_Byte(data);
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //数据写入失败
    }
    IIC_Stop(); //产生一个停止条件
    return 0;
}

/***************************************************************************************
* 函  数：uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
* 功　能：读取指定设备 指定寄存器的 length个值
* 参  数：dev     目标设备地址
          reg     寄存器地址
          length  要读的字节数
          *data   读出的数据将要存放的指针
* 返回值：1成功 0失败
* 备  注：无
***************************************************************************************/
uint8_t IIC_ReadMultByteFromSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
{
    uint8_t count = 0;
    uint8_t temp;
    IIC_Start();
    IIC_Send_Byte(dev); //发送从机地址
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //从机地址写入失败
    }
    IIC_Send_Byte(reg); //发送寄存器地址
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(dev+1); //进入接收模式
    IIC_Wait_Ack();
    for(count=0;count<length;count++)
    {
        if(count!=(length-1))
            temp = IIC_Read_Byte(1); //带ACK的读数据
        else
            temp = IIC_Read_Byte(0); //最后一个字节NACK

        data[count] = temp;
    }
    IIC_Stop(); //产生一个停止条件
    return 0;
}

/****************************************************************************************
* 函  数：uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
* 功　能：将多个字节写入指定设备 指定寄存器
* 参  数：dev     目标设备地址
          reg     寄存器地址
          length  要写的字节数
          *data   要写入的数据将要存放的指针
* 返回值：1成功 0失败
* 备  注：无
****************************************************************************************/
uint8_t IIC_WriteMultByteToSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{
    uint8_t count = 0;
    IIC_Start();
    IIC_Send_Byte(dev); //发送从机地址
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //从机地址写入失败
    }
    IIC_Send_Byte(reg); //发送寄存器地址
    IIC_Wait_Ack();
    for(count=0;count<length;count++)
    {
        IIC_Send_Byte(data[count]);
        if(IIC_Wait_Ack()) //每一个字节都要等从机应答
        {
            IIC_Stop();
            return 1; //数据写入失败
        }
    }
    IIC_Stop(); //产生一个停止条件
    return 0;
}









