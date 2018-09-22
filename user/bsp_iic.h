/**
 * @file    bsp_iic.h
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

#ifndef __BSP_IIC_H
#define __BSP_IIC_H
#include "sys.h"

//GPIO
#define IIC_SCL_PORT        GPIOE
#define IIC_SCL_PIN         GPIO_Pin_4
#define IIC_SCL_GPIO_CLK    RCC_AHB1Periph_GPIOE
#define IIC_SDA_PORT        GPIOE
#define IIC_SDA_PIN         GPIO_Pin_5
#define IIC_SDA_GPIO_CLK    RCC_AHB1Periph_GPIOE
#define IIC_In()            IIC_SDA_PORT->MODER &= ~0x00000C00;IIC_SDA_PORT->MODER |= 0x00000000//����ģʽ
#define IIC_Out()           IIC_SDA_PORT->MODER &= ~0x00000C00;IIC_SDA_PORT->MODER |= 0x00000400//���ģʽ

 /**
 * @brief ��ʼ��IIC
 */
void IIC_Init(void);
/**
 * @brief IIC����һ���ֽ�
 * @return 1-��Ack; 0-��Ack
 */      
void IIC_WriteByte(uint8_t data);
/**
 * @brief ��1�ֽ�
 * @param ack
 *          1-����Ӧ��
 *          0-���ͷ�Ӧ��
 * @return ���ض�ȡ��һ���ֽ�
 */ 
uint8_t IIC_ReadByte(uint8_t ack);
/**
 * @brief IICд1�ֽڵ��Ĵ���
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @param data Ҫд�������
 * @return 0-����; 1-����
 */
uint8_t IIC_WriteRegByte(uint8_t addr, uint8_t reg, uint8_t data);
/**
 * @brief IIC��1�ֽڵ��Ĵ���
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @return Ҫ��ȡ������
 */
uint8_t IIC_ReadRegByte(uint8_t addr, uint8_t reg);
/**
 * @brief IIC����д
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @param len д���ֽ���
 * @param data Ҫд�������
 * @return 0-����; 1-����
 */
uint8_t IIC_WriteRegBytes(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
/**
 * @brief IIC������
 * @param addr ������ַ
 * @param reg �Ĵ�����ַ
 * @param len ��ȡ�ֽ���
 * @param data Ҫ��ȡ������
 * @return 0-����; 1-����
 */
uint8_t IIC_ReadRegBytes(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);




#endif