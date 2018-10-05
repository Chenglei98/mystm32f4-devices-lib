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
 *          ��������������������     ��������������������
 *          ��     PB8��������������SCL     ��
 *          ��     PB9��������������SDA     ��
 *          ��������������������     ��������������������
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

#define OLED_WIDTH                  128
#define OLED_HEIGHT                 64
#define OLED_PAGES                  8
#define OLED_CHARACTER_WIDTH        6
#define OLED_CHARACTER_HEIGHT       8
#define OLED_LINES                  8
#define OLED_CHARACTERS_ONE_LINE    21

/**
 * @brief �Ʊ�����λ�ò�ѯ��
 */
static const uint8_t tabLookUpTable[OLED_CHARACTERS_ONE_LINE + 1] = {3,3,3,7,7,7,7,11,11,11,11,15,15,15,15,19,19,19,19,23,23,23};
static __IO uint8_t gRam[OLED_PAGES][OLED_WIDTH] = {0};

static uint8_t OLED_WriteCommand(uint8_t command);
static uint8_t OLED_WriteData(uint8_t data);
static inline void OLED_ScrollUpOneLine(void);

/**
 * @brief ��ʼ��OLED, ����������
 * @param oledHandle oled���, �� @ref OLED_HandleTypedef
 */
void OLED_Init(OLED_HandleTypedef *oledHandle)
{
    IIC_Init();
    delay_ms(150);
    OLED_WriteCommand(0xAE);//--display off
    OLED_WriteCommand(0x00);//---set low column address
    OLED_WriteCommand(0x10);//---set high column address
    OLED_WriteCommand(0x40);//--set start line address  
    OLED_WriteCommand(0xB0);//--set page address
    OLED_WriteCommand(0x81);//contract control
    OLED_WriteCommand(OLED_BRIGHTNESS);//--256
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
    OLED_Clear(oledHandle);
}
/**
 * @brief д����
 * @param command ����
 * @return 0-����; 1-����
 */
static uint8_t OLED_WriteCommand(uint8_t command)
{
    IIC_Start();
    IIC_WriteByte(OLED_IIC_ADDRESS);//Slave address,SA0=0
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(0x00);//write command
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(command); 
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_Stop();
    return 0;
}

/**
 * @brief д����
 * @param data ����
 * @return 0-����; 1-����
 */
static uint8_t OLED_WriteData(uint8_t data)
{
    IIC_Start();
    IIC_WriteByte(OLED_IIC_ADDRESS);//D/C#=0; R/W#=0
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(0x40);//write data
    IIC_IF_NOT_ACK_RETURN_1;
    IIC_WriteByte(data);
    IIC_IF_NOT_ACK_RETURN_1;  
    IIC_Stop();
    return 0;
}


/**
 * @brief ȫ�����
 * @param fillData Ҫ��������
 */
void OLED_FillScreen(uint8_t fillData)
{
    uint8_t m, n;
    for(m = 0; m < OLED_PAGES; m++)
    {
        OLED_WriteCommand(0xb0 + m);//page0-page1
        OLED_WriteCommand(0x00);//low column start address
        OLED_WriteCommand(0x10);//high column start address
        for(n = 0; n < OLED_WIDTH; n++)
        {
            OLED_WriteData(fillData);
            gRam[m][n] = fillData;
        }
    }
}



/**
 * @brief ��������
 * @param x ������(0~127)
 * @param y ҳ����(0~7)
 */
static inline void OLED_SetPosition(uint8_t x, uint8_t y)
{     
    OLED_WriteCommand(0xb0 + y);
    OLED_WriteCommand(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCommand((x & 0x0f));
}

/**
 * @brief ����OLED
 */
void OLED_TurnOn()
{
    OLED_WriteCommand(0X8D);//SET DCDC����
    OLED_WriteCommand(0X14);//DCDC ON
    OLED_WriteCommand(0XAF);//DISPLAY ON
}

/**
 * @brief �ر�OLED
 */  
void OLED_TurnOff()
{
    OLED_WriteCommand(0X8D);//SET DCDC����
    OLED_WriteCommand(0X10);//DCDC OFF
    OLED_WriteCommand(0XAE);//DISPLAY OFF
}

/**
 * @brief ����
 * @param oledHandle oled���, �� @ref OLED_HandleTypedef
 * @note ������, ������Ļ�Ǻ�ɫ��, ��û����һ��
 */
void OLED_Clear(OLED_HandleTypedef *oledHandle)
{  
    uint8_t m, n;            
    for(m = 0; m < OLED_PAGES; m++)  
    {  
        OLED_WriteCommand(0xb0 + m);//����ҳ��ַ��0~7��
        OLED_WriteCommand(0x00);//������ʾλ�á��е͵�ַ
        OLED_WriteCommand(0x10);//������ʾλ�á��иߵ�ַ   
        for(n = 0; n < OLED_WIDTH; n++)
        {
            OLED_WriteData(0);
            gRam[m][n] = 0;
        }
    } //������ʾ
    oledHandle->stringX = 0;
    oledHandle->stringY = 0;
}

/**
 * @brief ȫ������
 * @note ȫ��������, ÿ�������ǵ�����
 */
void OLED_Blank()
{  
    uint8_t m,n;            
    for(m = 0; m < OLED_PAGES; m++)  
    {  
        OLED_WriteCommand(0xb0 + m);//����ҳ��ַ��0~7��
        OLED_WriteCommand(0x00);//������ʾλ�á��е͵�ַ
        OLED_WriteCommand(0x10);//������ʾλ�á��иߵ�ַ   
        for(n = 0; n < OLED_WIDTH; n++)
        {
            OLED_WriteData(1);
            gRam[m][n] = 1;
        }
    } //������ʾ
}

/**
 * @brief ��ָ��λ����ʾһ���ַ�, ���������ַ�
 * @param positionX ������(0~127)
 * @param positionY ҳ����(0~7)
 * @param character Ҫ��ʾ���ַ�
 * @param characterSize ����ߴ� 16/8
 */
static inline void OLED_DisplayCharacter(uint8_t positionX, uint8_t positionY, uint8_t character, uint8_t characterSize)
{
    uint32_t characterOffset = (uint32_t)(character - ' ');//�õ�ƫ�ƺ��ֵ
    uint8_t i;
    OLED_SetPosition(positionX, positionY);
    for(i = 0; i < OLED_CHARACTER_WIDTH; i++)
    {
        OLED_WriteData(F6x8[characterOffset][i]);
        gRam[positionY][positionX + i] = F6x8[characterOffset][i];
    }
    
}

static inline void OLED_ClearString(uint8_t beginX, uint8_t beginY, uint8_t endX, uint8_t endY)
{
    uint8_t i, j;
    beginX = (beginX << 1) + (beginX << 2);
    endX = (endX << 1) + (endX << 2);
    if(endY != beginY)
    {
        OLED_WriteCommand(0xb0 | beginY);//����ҳ��ַ
        OLED_WriteCommand(0x00 | (beginX & 0x0F));//������ʾλ�á��е͵�ַ
        OLED_WriteCommand(0x10 | (beginX >> 4));//������ʾλ�á��иߵ�ַ   
        for(j = beginX; j < 128; j++)
        {
            OLED_WriteData(0);
            gRam[beginY][j] = 0;
        }
        for(j = beginY + 1; j < endY; j++)
        {
            OLED_WriteCommand(0xb0 | j);//����ҳ��ַ
            OLED_WriteCommand(0x00);//������ʾλ�á��е͵�ַ
            OLED_WriteCommand(0x10);//������ʾλ�á��иߵ�ַ   
            for(i = 0; i < OLED_WIDTH; i++)
            {
                OLED_WriteData(0);
                gRam[j][i] = 0;
            }
        }
        OLED_WriteCommand(0xb0 | endY);//����ҳ��ַ
        OLED_WriteCommand(0x00);//������ʾλ�á��е͵�ַ
        OLED_WriteCommand(0x10);//������ʾλ�á��иߵ�ַ   
        for(j = 0; j <= endX; j++)
        {
            OLED_WriteData(0);
            gRam[endY][j] = 0;
        }
    }
    else
    {
        OLED_WriteCommand(0xb0 | beginY);//����ҳ��ַ
        OLED_WriteCommand(0x00 | (beginX & 0x0F));//������ʾλ�á��е͵�ַ
        OLED_WriteCommand(0x10 | (beginX >> 4));//������ʾλ�á��иߵ�ַ   
        for(j = beginX; j <= endX; j++)
        {
            OLED_WriteData(0);
            gRam[beginY][j] = 0;
        }
    }
}

/**
 * @brief ��ָ��λ����ʾ�ַ���
 * @param oledHandle oled���, �� @ref OLED_HandleTypedef
 * @param positionX ������(0~127)
 * @param positionY ҳ����(0~7)
 * @param format ��ʽ�ַ���
 * @param ... �㶮��
 */
void OLED_DisplayFormat(OLED_HandleTypedef *oledHandle, const char *format, ...)
{
    static uint8_t needYPlus = 1;//����β�Զ�������\r\n���ظ���ִֻ��һ�λ��� 0-����\r\n����
    uint8_t x = 0;//��ǰ������(0~20)
    uint8_t y = 0;//��ǰ������(0~7)
    uint8_t i = 0;
    va_list aptr;
 
    if(oledHandle == NULL)
        return;
    
    if(oledHandle->stringClear == ENABLE)
    {
        OLED_ClearString(oledHandle->__stringLastBeignX, oledHandle->__stringLastBeignY, 
                            oledHandle->__stringLastEndX, oledHandle->__stringLastEndY);
        oledHandle->stringClear = DISABLE;
    }
    
    va_start(aptr, format);
    vsprintf(oledHandle->__string, format, aptr);
    va_end(aptr);
    
    if(oledHandle->stringContinuous == ENABLE)
    {
        x = oledHandle->__stringLastEndX;
        y = oledHandle->__stringLastEndY;
    }
    else
    {
        x = oledHandle->stringX;
        y = oledHandle->stringY;
    }
    oledHandle->__stringLastBeignX = x;
    oledHandle->__stringLastBeignY = y;
    while (oledHandle->__string[i] != '\0')
    { 
        switch(oledHandle->__string[i])
        {
            case '\r':
                x = 0;//�س���\r���ǻص�����
                break;
            case '\n':
                if(needYPlus)//������ʱ++y����\n����
                    y++;
                needYPlus = 1;//���ٺ����´λ���
                break;
            case '\t':
                needYPlus = 1;//����β\t���ٺ���\n, ��Ϊ��ʱ�ѵ���һ��
                x = tabLookUpTable[x];//�ҵ�Ҫ�����λ��
                if(x >= OLED_CHARACTERS_ONE_LINE - 1)//\t�󳬹��ұ߽�����
                {
                    x = 0;
                    y++;
                }
                break;
            default://������ǿ����ַ�, ����Ҫ��ӡ���ַ���
                needYPlus = 1;//����β��ӡһ���ַ����ٺ���\n, ��Ϊ��ӡ���ѵ���һ��

                //(x << 1) + (x << 2)����x * 6, һ���ַ���6������
                OLED_DisplayCharacter((x << 1) + (x << 2), y > 7 ? 7 : y, oledHandle->__string[i], 8);
                
                if(++x == OLED_CHARACTERS_ONE_LINE)//������β���Զ����в�������һ��\n���з�
                {
                    x = 0;
                    y++;
                    needYPlus = 0;
                }
                break;
        }
        i++;
    }
    oledHandle->__stringLastEndX = x;
    oledHandle->__stringLastEndY = y;
}

/**
 * @brief ���һ��
 * @param lineIndex ��0��ʼ��������
 * @note һ�м�һҳ�߶�, 8������
 */
static inline void OLED_ClearLine(uint8_t lineIndex)
{
    uint8_t n;
    OLED_WriteCommand(0xb0 + lineIndex);//page0-page1
    OLED_WriteCommand(0x00);//low column start address
    OLED_WriteCommand(0x10);//high column start address
    for(n = 0; n < OLED_WIDTH; n++)
    {
        OLED_WriteData(0);
        gRam[lineIndex][n] = 0;
    }
}

/**
 * @brief ��Ļ��������һ��
 * @note һ�м�һҳ�߶�, 8������
 */
static inline void OLED_ScrollUpOneLine()
{
    uint8_t m, n;
    for(m = 0; m < OLED_LINES - 1; m++)  
    {  
        OLED_WriteCommand(0xb0 + m);//����ҳ��ַ��0~6��
        OLED_WriteCommand(0x00);//������ʾλ�á��е͵�ַ
        OLED_WriteCommand(0x10);//������ʾλ�á��иߵ�ַ   
        for(n = 0; n < OLED_WIDTH; n++)
        {
            gRam[m][n] = gRam[m + 1][n];
            OLED_WriteData(gRam[m][n]);
        }
    } //������ʾ
    OLED_ClearLine(7);
}

/**
 * @brief ��ʾ��Ϣ
 * @param oledHandle oled���, �� @ref OLED_HandleTypedef
 * @param format ��ʽ�ַ���, ֧�ֿ����ַ�
 * @param ... �㶮��
 * @note ���������������ʾ, ����ϸ��ַ�����������һ���׿�ʼ��ӡ; �����ָ��λ�ÿ�ʼ
 *       ����Ϣ��ʾ����ʱԭ��ʾ��������һ��,  CPUռ�ø���������
 *       ֧���Զ�����, �ֶ����з���windows��ͬ(crlf)
 */
void OLED_DisplayLog(OLED_HandleTypedef *oledHandle, const char *format, ...)
{
    uint8_t x = 0;//��ǰ������(0~20)
    uint16_t y = 0;//��ǰ������(0~7)
    static uint8_t linesScrollUp = 0;//ԭ������Ҫ���Ϲ���������
    static uint8_t needYPlus = 1;//����β�Զ�������\r\n���ظ���ִֻ��һ�λ��� 0-����\r\n����
    uint16_t i = 0;
    va_list aptr;
    
    if(oledHandle == NULL)
        return;
    
    va_start(aptr, format);
    vsprintf(oledHandle->__string, format, aptr);
    va_end(aptr);

    if(oledHandle->stringContinuous == ENABLE)
    {
        x = oledHandle->__stringLastEndX;
        y = oledHandle->__stringLastEndY;
    }
    else
    {
        x = oledHandle->stringX;
        y = oledHandle->stringY;
    }
    
    while (oledHandle->__string[i] != '\0')
    { 
        switch(oledHandle->__string[i])
        {
            case '\r':
                x = 0;//�س���\r���ǻص�����
                break;
            case '\n':
                if(needYPlus && (++y > OLED_LINES - 1))//������ʱ++y����\n����
                    linesScrollUp++;//������к󳬳���Ļ�ײ�, ���϶��һ��
                needYPlus = 1;//���ٺ����´λ���
                break;
            case '\t':
                needYPlus = 1;//����β\t���ٺ���\n, ��Ϊ��ʱ�ѵ���һ��
                x = tabLookUpTable[x];//�ҵ�Ҫ�����λ��
                if((x >= OLED_CHARACTERS_ONE_LINE - 1) && (x = 0, ++y > OLED_LINES - 1))//\t�󳬹��ұ߽�����
                    linesScrollUp++;//������к󳬳���Ļ�ײ�, ���϶��һ��
                break;
            default://������ǿ����ַ�, ����Ҫ��ӡ���ַ���
                needYPlus = 1;//����β��ӡһ���ַ����ٺ���\n, ��Ϊ��ӡ���ѵ���һ��
                while(linesScrollUp)//���Ϲ�����Ҫ������
                {
                    OLED_ScrollUpOneLine();
                    linesScrollUp--;
                }
                //(x << 1) + (x << 2)����x * 6, һ���ַ���6������
                OLED_DisplayCharacter((x << 1) + (x << 2), y > (OLED_LINES - 1) ? (OLED_LINES - 1) : y, oledHandle->__string[i], 8);
                if((++x == OLED_CHARACTERS_ONE_LINE) && (x = needYPlus = 0, ++y > (OLED_LINES - 1)))//��ס, �����������β���Զ����в�������һ��\n���з�
                    linesScrollUp++;//������к󳬳���Ļ�ײ�, ���϶��һ��
                break;
        }
        i++;
    }
    oledHandle->__stringLastEndX = x;
    oledHandle->__stringLastEndY = y;
}


///**
// * @brief ��ָ��λ����ʾ����
// * @param positionX ������(0~127)
// * @param positionY ҳ����(0~7)
// * @param number �������
// */
//void OLED_DisplayChinese(uint8_t positionX, uint8_t positionY, uint8_t number)
//{                      
//    uint8_t i, address=0;
//    OLED_SetPosition(positionX, positionY);    
//    for(i = 0; i < 22; i++)
//    {
//        OLED_WriteData(Hzk[number << 1][i]);
//        gRam[positionY][positionX + i] = Hzk[(number << 1) + 1][i];
//        address += 1;
//    }    
//    OLED_SetPosition(positionX, positionY + 1);    
//    for(i = 0; i < 16; i++)
//    {    
//        OLED_WriteData(Hzk[(number << 1) + 1][i]);
//        gRam[positionY + 1][positionX + i] = Hzk[(number << 1) + 1][i];
//        address += 1;
//    }                    
//}

///**
// * @brief ��ָ��λ����ʾͼƬ
// * @param positionX ������(0~127)
// * @param positionY ҳ����(0~7)
// * @param width ͼƬ���(������)
// * @param width ͼƬ�߶�(������)
// * @param picture Ҫ��ʾ��ͼƬ
// */
//void OLED_DisplayPicture(uint8_t positionX, uint8_t positionY, uint8_t width, uint8_t height, uint8_t picture[])
//{     
//    uint32_t j = 0;
//    uint8_t x, y;
//    height += (positionY >> 3);
//    uint8_t endY = !(height % 8) ?  (height >> 3) : ((height >> 3) + 1);
//    uint8_t endX = positionX + width;
//    for(y = positionY; y <= endY; y++)
//    {
//        OLED_SetPosition(positionX, y);
//        for(x = positionX; x <= endX; x++)
//        {
//            OLED_WriteData(picture[j]);
//            gRam[y][x] = picture[j];
//            j++;
//        }
//    }
//} 


