#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "manipulator.h"
#include "control.h"
#include "drv8825.h"
#include "oled.h"
int main(void)
{
    uart_init(115200);
    delay_init(168);
    OLED_Init();
    OLED_DisplayLog("oled\t\t\tok\r\n");
    CONTROL_Init();


    while(1)
    {
        
    }
    
    
}
