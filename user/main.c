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
    DRV8825_Init();
    while(1)
    {
        
        DRV8825_Run(200.0f, 0.9f);
        delay_ms(50);
    }
    
    
}
