/**
 * @file    oled.h
 * @author  Miaow, Evk123
 * @version 0.1.0
 * @date    2018/10/05
 * @brief   
 *          This file provides functions to manage the following 
 *          functionalities of 0.96" OLED display:
 *              1. Initialization
 *              2. Display formatted strings, pictures and Chinese characters
 *              3. Turn on/off the screen
 *              4. Show logs
 * @note
 *          Minimum version of source file:
 *              0.1.0
 *          Recommanded pin connection:
 *          ©°©¤©¤©¤©¤©¤©¤©¤©¤©´     ©°©¤©¤©¤©¤©¤©¤©¤©¤©´
 *          ©¦     PB8©À©¤©¤©¤©¤©¤©ÈSCL     ©¦
 *          ©¦     PB9©À©¤©¤©¤©¤©¤©ÈSDA     ©¦
 *          ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¼     ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¼
 *          STM32F407      0.96" OLED
 *          
 *          The source code repository is not available on GitHub now:
 *              https://github.com/3703781
 *          Your pull requests will be welcome.
 *          Here are the guidelines for your pull requests:
 *              1. Respect my coding style.
 *              2. Avoid to commit several features in one commit.
 *              3. Make your modification compact - don't reformat source code in your request.
 */
 
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

void OLED_Init(void);
void OLED_FillScreen(uint8_t fillData);
void OLED_SetPosition(uint8_t x, uint8_t y);
void OLED_TurnOn(void);
void OLED_TurnOff(void);
void OLED_Clear(void);
void OLED_Blank(void);
void OLED_DisplayCharacter(uint8_t positionX, uint8_t positionY, uint8_t character, uint8_t characterSize);
void OLED_DisplayFormat(uint8_t positionX, uint8_t positionY, const char *format, ...);
void OLED_ClearLine(uint8_t lineIndex);
void OLED_DisplayLog(const char *format, ...);
void OLED_DisplayChinese(uint8_t positionX, uint8_t positionY, uint8_t number);
void OLED_DisplayPicture(uint8_t positionX, uint8_t positionY, uint8_t width, uint8_t height, uint8_t picture[]);

#endif 
