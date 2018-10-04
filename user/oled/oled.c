/**
 * @file    oled.c
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
 *          Minimum version of header file:
 *              0.1.0
 *          Recommanded pin connection:
 *          ┌────────┐     ┌────────┐
 *          │     PB8├─────┤SCL     │
 *          │     PB9├─────┤SDA     │
 *          └────────┘     └────────┘
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
 
#include "oled.h"
#include "oled_font.h"       
#include "delay.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

static const uint8_t tabLookUpTable[22] = {3,3,3,7,7,7,7,11,11,11,11,15,15,15,15,19,19,19,19,23,23,23};
static __IO uint8_t gRam[8][128] = {0};

static uint8_t OLED_WriteCommand(uint8_t command);
static uint8_t OLED_WriteData(uint8_t data);
static inline void OLED_ScrollUpOneLine(void);

/**
 * @brief 初始化OLED
 */
void OLED_Init()
{   
    IIC_Init();
    delay_ms(200);
    OLED_WriteCommand(0xAE);//--display off
    OLED_WriteCommand(0x00);//---set low column address
    OLED_WriteCommand(0x10);//---set high column address
    OLED_WriteCommand(0x40);//--set start line address  
    OLED_WriteCommand(0xB0);//--set page address
    OLED_WriteCommand(0x81);//contract control
    OLED_WriteCommand(0xFF);//--128   
    OLED_WriteCommand(0xA1);//set segment remap 
    OLED_WriteCommand(0xA6);//--normal / reverse
    OLED_WriteCommand(0xA8);//--set multiplex ratio(1 to 64)
    OLED_WriteCommand(0x3F);//--1/32 duty
    OLED_WriteCommand(0xC8);//Com scan direction
    OLED_WriteCommand(0xD3);//-set display offset
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0xD5);//set osc division
    OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xD8);//set area color mode off
    OLED_WriteCommand(0x05);
    OLED_WriteCommand(0xD9);//Set Pre-Charge Period
    OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDA);//set com pin configuartion
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0xDB);//set Vcomh
    OLED_WriteCommand(0x30);
    OLED_WriteCommand(0x8D);//set charge pump enable
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0xAF);//--turn on oled panel
    OLED_Clear();
}
/**
 * @brief 写命令
 * @param command 命令
 * @return 0-正常; 1-出错
 */
static uint8_t OLED_WriteCommand(uint8_t command)
{
    IIC_Start();
    IIC_WriteByte(0x78);            //Slave address,SA0=0
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(0x00);            //write command
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(command); 
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_Stop();
    return 0;
}

/**
 * @brief 写数据
 * @param data 数据
 * @return 0-正常; 1-出错
 */
static uint8_t OLED_WriteData(uint8_t data)
{
    IIC_Start();
    IIC_WriteByte(0x78);            //D/C#=0; R/W#=0
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(0x40);            //write data
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(data);
    IIC_IF_NOT_ACK_RETURN_1;  
    IIC_Stop();
    return 0;
}


/**
 * @brief 全屏填充
 * @param fillData 要填充的数据
 */
void OLED_FillScreen(uint8_t fillData)
{
    uint8_t m, n;
    for(m = 0; m < 8; m++)
    {
        OLED_WriteCommand(0xb0 + m);        //page0-page1
        OLED_WriteCommand(0x00);        //low column start address
        OLED_WriteCommand(0x10);        //high column start address
        for(n = 0; n < 128; n++)
        {
            OLED_WriteData(fillData);
            gRam[m][n] = fillData;
        }
    }
}



/**
 * @brief 设置坐标
 * @param x 横坐标(0~127)
 * @param y 页坐标(0~7)
 */
void OLED_SetPosition(uint8_t x, uint8_t y)
{     
    OLED_WriteCommand(0xb0 + y);
    OLED_WriteCommand(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCommand((x & 0x0f));
}

/**
 * @brief 开启OLED
 */
void OLED_TurnOn()
{
    OLED_WriteCommand(0X8D);  //SET DCDC命令
    OLED_WriteCommand(0X14);  //DCDC ON
    OLED_WriteCommand(0XAF);  //DISPLAY ON
}

/**
 * @brief 关闭OLED
 */  
void OLED_TurnOff()
{
    OLED_WriteCommand(0X8D);  //SET DCDC命令
    OLED_WriteCommand(0X10);  //DCDC OFF
    OLED_WriteCommand(0XAE);  //DISPLAY OFF
}

/**
 * @brief 清屏
 * @note 清屏后, 整个屏幕是黑色的, 和没点亮一样
 */
void OLED_Clear()
{  
    uint8_t m, n;            
    for(m = 0; m < 8; m++)  
    {  
        OLED_WriteCommand(0xb0 + m);    //设置页地址（0~7）
        OLED_WriteCommand(0x00);      //设置显示位置—列低地址
        OLED_WriteCommand(0x10);      //设置显示位置—列高地址   
        for(n = 0; n < 128; n++)
        {
            OLED_WriteData(0);
            gRam[m][n] = 0;
        }
    } //更新显示
}

/**
 * @brief 全屏点亮
 * @note 全屏点亮后, 每个像素是全部点亮的
 */
void OLED_Blank()
{  
    uint8_t m,n;            
    for(m = 0; m < 8; m++)  
    {  
        OLED_WriteCommand(0xb0 + m);    //设置页地址（0~7）
        OLED_WriteCommand(0x00);      //设置显示位置—列低地址
        OLED_WriteCommand(0x10);      //设置显示位置—列高地址   
        for(n = 0; n < 128; n++)
        {
            OLED_WriteData(1);
            gRam[m][n] = 1;
        }
    } //更新显示
}

/**
 * @brief 在指定位置显示一个字符, 包括部分字符
 * @param positionX 横坐标(0~127)
 * @param positionY 页坐标(0~7)
 * @param character 要显示的字符
 * @param characterSize 字体尺寸 16/8
 */
void OLED_DisplayCharacter(uint8_t positionX, uint8_t positionY, uint8_t character, uint8_t characterSize)
{          
    uint32_t characterOffset = (uint32_t)(character - ' ');//得到偏移后的值
    uint8_t i;
    OLED_SetPosition(positionX, positionY);
    for(i = 0; i < 6; i++)
    {
        OLED_WriteData(F6x8[characterOffset][i]);
        gRam[positionY][positionX + i] = F6x8[characterOffset][i];
    }
    
}

/**
 * @brief 从指定位置显示字符串
 * @param positionX 横坐标(0~127)
 * @param positionY 页坐标(0~7)
 * @param format 格式字符串, 不支持控制字符
 * @param ... 你懂的
 */
void OLED_DisplayFormat(uint8_t positionX, uint8_t positionY, const char *format, ...)
{
    uint8_t j = 0;
    char string[21] = {0};
    va_list aptr;
    va_start(aptr, format);
    vsprintf(string, format, aptr);
    va_end(aptr);
    while (string[j] != '\0')
    {        
        OLED_DisplayCharacter(positionX, positionY, string[j], 8);
        positionX += 8;
        if(positionX > 120)
        {
            positionX = 0;
            positionY += 2;
        }
        j++;
    }
}

/**
 * @brief 清空一行
 * @param lineIndex 从0开始的行索引
 * @note 一行即一页高度, 8个像素
 */
void OLED_ClearLine(uint8_t lineIndex)
{
    uint8_t n;
    OLED_WriteCommand(0xb0 + lineIndex);//page0-page1
    OLED_WriteCommand(0x00);//low column start address
    OLED_WriteCommand(0x10);//high column start address
    for(n = 0; n < 128; n++)
    {
        OLED_WriteData(0);
        gRam[lineIndex][n] = 0;
    }
}

/**
 * @brief 屏幕内容上移一行
 * @note 一行即一页高度, 8个像素
 */
static inline void OLED_ScrollUpOneLine()
{
    uint8_t m, n;
    for(m = 0; m < 7; m++)  
    {  
        OLED_WriteCommand(0xb0 + m);//设置页地址（0~6）
        OLED_WriteCommand(0x00);//设置显示位置—列低地址
        OLED_WriteCommand(0x10);//设置显示位置—列高地址   
        for(n = 0; n < 128; n++)
        {
            gRam[m][n] = gRam[m + 1][n];
            OLED_WriteData(gRam[m][n]);
        }
    } //更新显示
    OLED_ClearLine(7);
}

/**
 * @brief 显示信息
 * @param format 格式字符串, 支持控制字符
 * @param ... 你懂的
 * @note 新信息显示不下时原显示内容上移一行, 支持自动换行, CPU占用高且运行慢
 */
void OLED_DisplayLog(const char *format, ...)
{
    static uint8_t x = 0;
    static uint16_t y = 0;
    static uint8_t needScrollUp = 0, needYPlus = 1;
    char string[100] = {0};
    uint16_t i = 0;
    va_list aptr;
    
    va_start(aptr, format);
    vsprintf(string, format, aptr);
    va_end(aptr);

    while (string[i] != '\0')
    { 
        switch(string[i])
        {
            case '\r':
                x = 0;
                break;
            case '\n':
                if(needYPlus && (++y > 7))
                {
                    needYPlus = 1;
                    needScrollUp++;
                }
                break;
            case '\t':
                x = tabLookUpTable[x];
                needYPlus = 1;
                if((x > 20) && (x = 0, ++y > 7))
                    needScrollUp++;
                break;
            default:
                needYPlus = 1;
                while(needScrollUp)
                {
                    OLED_ScrollUpOneLine();
                    needScrollUp--;
                }
                
                OLED_DisplayCharacter((x << 1) + (x << 2), y > 7 ? 7 : y, string[i], 8);
                if((x++ == 21) && (x = needYPlus = 0, ++y > 7))//忍住, 别骂。。。
                    needScrollUp++;
                break;
        }
        i++;
    }
    
    
}

/**
 * @brief 从指定位置显示汉字
 * @param positionX 横坐标(0~127)
 * @param positionY 页坐标(0~7)
 * @param number 汉字序号
 */
void OLED_DisplayChinese(uint8_t positionX, uint8_t positionY, uint8_t number)
{                      
    uint8_t i, address=0;
    OLED_SetPosition(positionX, positionY);    
    for(i = 0; i < 22; i++)
    {
        OLED_WriteData(Hzk[number << 1][i]);
        gRam[positionY][positionX + i] = Hzk[(number << 1) + 1][i];
        address += 1;
    }    
    OLED_SetPosition(positionX, positionY + 1);    
    for(i = 0; i < 16; i++)
    {    
        OLED_WriteData(Hzk[(number << 1) + 1][i]);
        gRam[positionY + 1][positionX + i] = Hzk[(number << 1) + 1][i];
        address += 1;
    }                    
}

/**
 * @brief 从指定位置显示图片
 * @param positionX 横坐标(0~127)
 * @param positionY 页坐标(0~7)
 * @param width 图片宽度(像素数)
 * @param width 图片高度(像素数)
 * @param picture 要显示的图片
 */
void OLED_DisplayPicture(uint8_t positionX, uint8_t positionY, uint8_t width, uint8_t height, uint8_t picture[])
{     
    uint32_t j = 0;
    uint8_t x, y;
    height += (positionY >> 3);
    uint8_t endY = !(height % 8) ?  (height >> 3) : ((height >> 3) + 1);
    uint8_t endX = positionX + width;
    for(y = positionY; y <= endY; y++)
    {
        OLED_SetPosition(positionX, y);
        for(x = positionX; x <= endX; x++)
        {
            OLED_WriteData(picture[j]);
            gRam[y][x] = picture[j];
            j++;
        }
    }
} 


