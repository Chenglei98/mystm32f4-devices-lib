#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "manipulator.h"
#include "control.h"
#include "drv8825.h"
int main(void)
{
    uart_init(115200);
    delay_init(168);
    printf("CONTROL Init...\r\n");
    CONTROL_Init();
    printf("OK\r\n");
    while(1)
    {
    }
    
    
}
