//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : Evk123
//  ��������   : 2014-0101
//  ����޸�   : 
//  ��������   : 0.69��OLED �ӿ���ʾ����(STM32F103ZEϵ��IIC)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PD6��SCL��
//              SDA   ��PD7��SDA��            
//              ----------------------------------------------------------------
#ifndef __OLED_H
#define __OLED_H

#include "stm32f4xx.h" 
#include "stdlib.h"
#include "bsp_iic.h"

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED IIC�˿ڶ���----------------  					   

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_6)//SDA IIC�ӿڵ�ʱ���ź�
#define OLED_SCLK_Set() GPIO_SetBits(GPIOD,GPIO_Pin_6)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_7)//SCL IIC�ӿڵ������ź�
#define OLED_SDIN_Set() GPIO_SetBits(GPIOD,GPIO_Pin_7)

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//OLED�����ú���
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[]);
void Delay_50ms(uint32_t Del_50ms);
void Delay_1ms(uint32_t Del_1ms);
void fill_picture(uint8_t fill_Data);
void Picture();
void IIC_Start();
void IIC_Stop();
uint8_t OLED_WriteCommand(uint8_t command);
uint8_t OLED_WriteData(uint8_t data);
uint8_t OLED_Write(uint8_t byte, uint8_t isData);
void IIC_Wait_Ack();
#endif 
