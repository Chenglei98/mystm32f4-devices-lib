#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "manipulator.h"
#include "control.h"
#include "drv8825.h"
#include "oled.h"

OLED_HandleTypedef oledHandle = 
{
    .stringX = 0,
    .stringY = 0,
    .stringClear = DISABLE,
    .stringContinuous = ENABLE,
    .__string = {0},
};

int main(void)
{
    delay_init(168);
    OLED_Init(&oledHandle);
    OLED_DisplayLog(&oledHandle, "oled\t\t\t\tok\r\n");
    OLED_DisplayLog(&oledHandle, "uart\t\t\t\t");
    uart_init(115200);
    OLED_DisplayLog(&oledHandle, "ok\r\ndelay\t\t\t\t");
    OLED_DisplayLog(&oledHandle, "ok\r\niniting control...\r\n");
    CONTROL_Init();

    while(1)
    {
        //OLED_DisplayFormat(&oledHandle, "abcdefghijklmnopqrstuvwxy\r\n\r\nzabcde\ti\r\njklmnopqrstuvwxyzabcpqrstuvwxyzabc!");
    }
    
    
}
