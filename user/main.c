#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "manipulator.h"
#include "control.h"

int main(void)
{
    uart_init(115200);
    delay_init(168);
    
    /*
    MANIPULATOR_Init();
    delay_ms(500);
    
    while(1)
    {
        MANIPULATOR_Release();
        delay_ms(200);
        MANIPULATOR_Grab();
        delay_ms(200);
    }
    */
    
    printf("control init:\r\n");
    delay_ms(1000);
    CONTROL_Init();
    delay_ms(8000);
    CONTROL_GoStraight(360);
    while(1)
    {
//        CONTROL_Turn(10.0f, 360, 10);
//        CONTROL_Turn(-10.0f, 360, -10);
        CONTROL_GoStraight(360);
        delay_ms(2000);
        CONTROL_Turn(10.0f, 360, 180);
        CONTROL_GoStraight(360);
        delay_ms(2000);
        CONTROL_Turn(10.0f, 360, 180);
        
    }
    
    
}
