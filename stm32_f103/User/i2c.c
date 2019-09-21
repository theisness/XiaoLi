#include "i2c.h"
#include "lib_common.h"

////////////////////////////////////////// ��ֲ��֪��//////////////////////////////////////
//
// ��ֲ��ģ��IICʱ����Ҫע�� IIC_WriteByteToSlave() д�������ĵ�ַʱ�����ַ�ʽ�ĵ�ַ���Ѿ�
// ���ƹ�1λ�ģ�ע���� ( I2C_Addr <<1 | 0 )�ķ�ʽ�������֣��������������ͨ��,������ֲ!
//
///////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��IIC
void IIC_Init(void)
{					     

	RCC->APB2ENR|=1<<3;//��ʹ������IO PORTBʱ�� 	
	GPIOB->CRL&=0XFF00FFFF;//PB4,5 �������
	GPIOB->CRL|=0X00330000;	   
	GPIOB->ODR|=3<<4;     //PB4,5 �����
	
}//����IIC��ʼ�ź�
void IIC_Start(void)
{
    SDA_OUT();     //sda�����
    IIC_SDA(1);
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
void IIC_Stop(void)
{
    SDA_OUT();//sda�����
    IIC_SCL(0);
    IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL(1);
    IIC_SDA(1);//����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();//SDA����Ϊ����
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
    IIC_SCL(0);//ʱ�����0
    return 0;
}
//����ACKӦ��
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
//������ACKӦ��
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {
        IIC_SDA((txd&0x80)>>7);
        txd<<=1;
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL(1);
        delay_us(2);
        IIC_SCL(0);
        delay_us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}


/******************************************************************************
* ��  ����uint8_t IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t addr)
* �����ܣ���ȡָ���豸 ָ���Ĵ�����һ��ֵ
* ��  ����I2C_Addr  Ŀ���豸��ַ
          reg       �Ĵ�����ַ
          *buf      ��ȡ����Ҫ�洢�ĵ�ַ
* ����ֵ������ 1ʧ�� 0�ɹ�
* ��  ע����
*******************************************************************************/
uint8_t IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t *buf)
{
    IIC_Start();
    IIC_Send_Byte(I2C_Addr);     //���ʹӻ���ַ
    if(IIC_Wait_Ack()) //����ӻ�δӦ�������ݷ���ʧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg); //���ͼĴ�����ַ
    IIC_Wait_Ack();

    IIC_Start();
    IIC_Send_Byte(I2C_Addr+1); //�������ģʽ
    IIC_Wait_Ack();
    *buf=IIC_Read_Byte(0);
    IIC_Stop(); //����һ��ֹͣ����
    return 0;
}

/*************************************************************************************
* ��  ����uint8_t IIC_WriteByteFromSlave(uint8_t I2C_Addr,uint8_t addr��uint8_t buf))
* �����ܣ�д��ָ���豸 ָ���Ĵ�����һ��ֵ
* ��  ����I2C_Addr  Ŀ���豸��ַ
          reg       �Ĵ�����ַ
          buf       Ҫд�������
* ����ֵ��1 ʧ�� 0�ɹ�
* ��  ע����
**************************************************************************************/
uint8_t IIC_WriteByteToSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t data)
{
    IIC_Start();
    IIC_Send_Byte(I2C_Addr); //���ʹӻ���ַ
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //�ӻ���ַд��ʧ��
    }
    IIC_Send_Byte(reg); //���ͼĴ�����ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(data);
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //����д��ʧ��
    }
    IIC_Stop(); //����һ��ֹͣ����
    return 0;
}

/***************************************************************************************
* ��  ����uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
* �����ܣ���ȡָ���豸 ָ���Ĵ����� length��ֵ
* ��  ����dev     Ŀ���豸��ַ
          reg     �Ĵ�����ַ
          length  Ҫ�����ֽ���
          *data   ���������ݽ�Ҫ��ŵ�ָ��
* ����ֵ��1�ɹ� 0ʧ��
* ��  ע����
***************************************************************************************/
uint8_t IIC_ReadMultByteFromSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
{
    uint8_t count = 0;
    uint8_t temp;
    IIC_Start();
    IIC_Send_Byte(dev); //���ʹӻ���ַ
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //�ӻ���ַд��ʧ��
    }
    IIC_Send_Byte(reg); //���ͼĴ�����ַ
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(dev+1); //�������ģʽ
    IIC_Wait_Ack();
    for(count=0;count<length;count++)
    {
        if(count!=(length-1))
            temp = IIC_Read_Byte(1); //��ACK�Ķ�����
        else
            temp = IIC_Read_Byte(0); //���һ���ֽ�NACK

        data[count] = temp;
    }
    IIC_Stop(); //����һ��ֹͣ����
    return 0;
}

/****************************************************************************************
* ��  ����uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
* �����ܣ�������ֽ�д��ָ���豸 ָ���Ĵ���
* ��  ����dev     Ŀ���豸��ַ
          reg     �Ĵ�����ַ
          length  Ҫд���ֽ���
          *data   Ҫд������ݽ�Ҫ��ŵ�ָ��
* ����ֵ��1�ɹ� 0ʧ��
* ��  ע����
****************************************************************************************/
uint8_t IIC_WriteMultByteToSlave(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{
    uint8_t count = 0;
    IIC_Start();
    IIC_Send_Byte(dev); //���ʹӻ���ַ
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1; //�ӻ���ַд��ʧ��
    }
    IIC_Send_Byte(reg); //���ͼĴ�����ַ
    IIC_Wait_Ack();
    for(count=0;count<length;count++)
    {
        IIC_Send_Byte(data[count]);
        if(IIC_Wait_Ack()) //ÿһ���ֽڶ�Ҫ�ȴӻ�Ӧ��
        {
            IIC_Stop();
            return 1; //����д��ʧ��
        }
    }
    IIC_Stop(); //����һ��ֹͣ����
    return 0;
}









