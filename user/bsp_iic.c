/**
 * @file    bsp_iic.c
 * @author  Miaow
 * @version 0.1.0
 * @date    2018/08/30
 * @brief   
 *          This file provides functions to manage the following 
 *          functionalities of IIC:
 *              1. Initialization
 *              2. Write and read one byte
 *              3. Write and read registers of slave
 * @note
 *          Minimum version of header file:
 *              0.1.0
 *          Recommanded pin connection:
 *          ��������������������     ��������������������
 *          ��     PE4��������������SCL     ��
 *          ��     PE5��������������SDA     ��
 *          ��������������������     ��������������������
 *          STM32F407        slave
 *          
 *          The source code repository is not available on GitHub now:
 *              https://github.com/3703781
 *          Your pull requests will be welcome.
 *          Here are the guidelines for your pull requests:
 *              1. Respect my coding style.
 *              2. Avoid to commit several features in one commit.
 *              3. Make your modification compact - don't reformat source code in your request.
 */

#include "bsp_iic.h"

static void IIC_DelayUs(uint32_t us);
static void IIC_Start(void);
static void IIC_Stop(void);
static uint8_t IIC_WaitAck(void);
static void IIC_Ack(void);
static void IIC_NAck(void);

/**
 * @brief ��׼��΢�뼶��ʱ����
 */
static inline void IIC_DelayUs(uint32_t us)
{
    us <<= 2;
    while(us--);
}
/**
 * @brief ��ʼ��IIC
 */
void IIC_Init()
{            
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(IIC_SCL_GPIO_CLK | IIC_SDA_GPIO_CLK, ENABLE);//ʹ��GPIOʱ��
    //SCL, SDA��ʼ������
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;//50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);//��ʼ��
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
    GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);//��ʼ��
    IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1
    IIC_SDA_PORT->BSRRL = IIC_SDA_PIN;//IIC_SDA=1
}
/**
 * @brief ����IIC��ʼ�ź�
 */
static void IIC_Start()
{
    IIC_Out();//sda�����
    IIC_SDA_PORT->BSRRL = IIC_SDA_PIN;//IIC_SDA=1      
    IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1
    IIC_DelayUs(4);
    IIC_SDA_PORT->BSRRH = IIC_SDA_PIN;//IIC_SDA=0 START:when CLK is high,DATA change form high to low 
    IIC_DelayUs(4);
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//ǯסI2C���ߣ�׼�����ͻ�������� 
}      
/**
 * @brief ����IICֹͣ�ź�
 */
static void IIC_Stop()
{
    IIC_Out();//sda�����
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0
    IIC_SDA_PORT->BSRRH = IIC_SDA_PIN;//IIC_SDA=0 STOP:when CLK is high DATA change form low to high
    IIC_DelayUs(4);
    IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1;
    IIC_SDA_PORT->BSRRL = IIC_SDA_PIN;//IIC_SDA=1IIC_SDA=1 ����I2C���߽����ź�
    IIC_DelayUs(4);                                   
}
/**
 * @brief �ȴ�Ӧ���ź�
 * @return 1-����Ӧ��ʧ��; 0-����Ӧ��ɹ�
 */
static uint8_t IIC_WaitAck()
{
    uint8_t ucErrTime=0;
    IIC_In();//SDA����Ϊ����  
    IIC_SDA_PORT->BSRRL = IIC_SDA_PIN;//IIC_SDA=1
    IIC_DelayUs(1);       
    IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1
    IIC_DelayUs(1);     
    while(IIC_SDA_PORT->IDR & IIC_SDA_PIN)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0 ʱ�����0        
    return 0;  
} 
/**
 * @brief ����ACKӦ��
 */
static void IIC_Ack()
{
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0
    IIC_Out();
    IIC_SDA_PORT->BSRRH = IIC_SDA_PIN;//IIC_SDA=0
    IIC_DelayUs(2);
    IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1
    IIC_DelayUs(2);
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0
}
/**
 * @brief ������Ӧ���ź�
 */           
static void IIC_NAck()
{
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0
    IIC_Out();
    IIC_SDA_PORT->BSRRL = IIC_SDA_PIN;//IIC_SDA=1
    IIC_DelayUs(2);
    IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1
    IIC_DelayUs(2);
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0
}                                          
/**
 * @brief IIC����һ���ֽ�
 * @return 1-��Ack; 0-��Ack
 */      
void IIC_WriteByte(uint8_t data)
{                        
    uint8_t i;   
    IIC_Out();         
    IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0 ����ʱ�ӿ�ʼ���ݴ���
    for(i = 0; i<8;i++)
    {            
        if((data & 0x80) >> 7)
            IIC_SDA_PORT->BSRRL = IIC_SDA_PIN;//IIC_SDA=1
        else
            IIC_SDA_PORT->BSRRH = IIC_SDA_PIN;//IIC_SDA=0
        data <<= 1;       
        IIC_DelayUs(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1
        IIC_DelayUs(2); 
        IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0
        IIC_DelayUs(2);
    }     
}         
/**
 * @brief ��1�ֽ�
 * @param ack
 *          1-����Ӧ��
 *          0-���ͷ�Ӧ��
 * @return ���ض�ȡ��һ���ֽ�
 */ 
uint8_t IIC_ReadByte(uint8_t ack)
{
    uint8_t i, receive = 0;
    IIC_In();//SDA����Ϊ����
    for(i = 0; i < 8; i++)
    {
        IIC_SCL_PORT->BSRRH = IIC_SCL_PIN;//IIC_SCL=0 
        IIC_DelayUs(2);
        IIC_SCL_PORT->BSRRL = IIC_SCL_PIN;//IIC_SCL=1
        receive <<= 1;
        if(IIC_SDA_PORT->IDR & IIC_SDA_PIN)
            receive++;   
        IIC_DelayUs(1); 
    }                     
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
/**
 * @brief IICд1�ֽڵ��Ĵ���
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @param data Ҫд�������
 * @return 0-����; 1-����
 */
uint8_t IIC_WriteRegByte(uint8_t addr, uint8_t reg, uint8_t data)
{ 
    IIC_Start(); 
	IIC_WriteByte(addr << 1);//����������ַ+д����	
	if(IIC_WaitAck())	//�ȴ�Ӧ��
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_WriteByte(reg);	//д�Ĵ�����ַ
    IIC_WaitAck();		//�ȴ�Ӧ�� 
	IIC_WriteByte(data);//��������
	if(IIC_WaitAck())	//�ȴ�ACK
	{
		IIC_Stop();	 
		return 1;		 
	}		 
    IIC_Stop();	 
	return 0;
}
/**
 * @brief IIC��1�ֽڵ��Ĵ���
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @return Ҫ��ȡ������
 */
uint8_t IIC_ReadRegByte(uint8_t addr, uint8_t reg)
{
	uint8_t res;
    IIC_Start(); 
	IIC_WriteByte(addr << 1);//����������ַ+д����	
	IIC_WaitAck();//�ȴ�Ӧ�� 
    IIC_WriteByte(reg);//д�Ĵ�����ַ
    IIC_WaitAck();//�ȴ�Ӧ��
    IIC_Start();
	IIC_WriteByte((addr << 1) | 1);//����������ַ+������	
    IIC_WaitAck();//�ȴ�Ӧ�� 
	res = IIC_ReadByte(0);//��ȡ����,����nACK 
    IIC_Stop();//����һ��ֹͣ���� 
	return res;		
}

/**
 * @brief IIC����д
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @param len д���ֽ���
 * @param data Ҫд�������
 * @return 0-����; 1-����
 */
uint8_t IIC_WriteRegBytes(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    uint8_t i; 
    IIC_Start(); 
    IIC_WriteByte(addr << 1);//����������ַ+д����    
    if(IIC_WaitAck())//�ȴ�Ӧ��
    {
        IIC_Stop();         
        return 1;        
    }
    IIC_WriteByte(reg);//д�Ĵ�����ַ
    IIC_WaitAck();//�ȴ�Ӧ��
    for(i = 0; i < len; i++)
    {
        IIC_WriteByte(data[i]);//��������
        if(IIC_WaitAck())//�ȴ�ACK
        {
            IIC_Stop();     
            return 1;         
        }        
    }    
    IIC_Stop();     
    return 0;    
} 
/**
 * @brief IIC������
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @param len ��ȡ�ֽ���
 * @param data Ҫ��ȡ������
 * @return 0-����; 1-����
 */
uint8_t IIC_ReadRegBytes(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{ 
    IIC_Start(); 
    IIC_WriteByte(addr<<1);//����������ַ+д����    
    if(IIC_WaitAck())//�ȴ�Ӧ��
    {
        IIC_Stop();         
        return 1;        
    }
    IIC_WriteByte(reg);//д�Ĵ�����ַ
    IIC_WaitAck();//�ȴ�Ӧ��
    IIC_Start();
    IIC_WriteByte((addr << 1) | 1);//����������ַ+������    
    IIC_WaitAck();//�ȴ�Ӧ�� 
    while(len)
    {
        if(len == 1)
            *data=IIC_ReadByte(0);//������,����nACK 
        else
            *data=IIC_ReadByte(1);//������,����ACK  
        len--;
        data++; 
    }    
    IIC_Stop();//����һ��ֹͣ���� 
    return 0;    
}