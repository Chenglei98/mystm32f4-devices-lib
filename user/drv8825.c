/**
 * @file    drv8825.c
 * @author  Miaow, JYL
 * @version 0.1.0
 * @date    2018/09/24
 * @brief   
 *          This file provides bsp functions to manage the following 
 *          functionalities of DRV8825:
 *              1. Initialization and Configuration
 *              2. Motor operation
 * @note
 *          Minimum version of header file:
 *              0.1.0
 *          Recommanded pin connection:
 *             ©°©¤©¤©¤©¤©¤©¤©¤©¤©´     ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
 *             ©¦     STP©À©¤©¤©¤©¤©¤©ÈPA2        ©¦
 *             ©¦     DIR©À©¤©¤©¤©¤©¤©ÈPC5        ©¦
 *             ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¼     ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼
 *              DRV8825        STM32F407  
 *  
 *          The source code repository is available on GitHub:
 *              https://github.com/3703781/mystm32f4-devices-lib
 *          Your pull requests will be welcome.
 *          Here are the guidelines for your pull requests:
 *              1. Respect my coding style.
 *              2. Avoid to commit several features in one commit.
 *              3. Make your modification compact - don't reformat source code in your request.
 */
#include "drv8825.h"

/** @addtogroup DRV8825
 * @{
 */
 
/**
 * @brief Initialize DRV8825 module.
 */
void DRV8825_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    //general use
    //GPIO
    RCC_AHB1PeriphClockCmd(DRV8825_GPIO_ALL_CLK, ENABLE);
    //DIR
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStructure.GPIO_Pin = DRV8825_DIR_PIN;
    GPIO_Init(DRV8825_DIR_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(DRV8825_DIR_PORT, DRV8825_DIR_PIN);//DIR = L
    //STP
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = DRV8825_STP_PIN;
    GPIO_Init(DRV8825_STP_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DRV8825_STP_PORT, DRV8825_STP_GPIO_PINSOURCE, DRV8825_STP_GPIO_AF);
    
    
    //TIM
    DRV8825_STP_TIM_CLK_FUNC(DRV8825_STP_TIM_CLK, ENABLE);
    TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseInitStructure.TIM_Prescaler = 35 - 1;
    TIM_BaseInitStructure.TIM_Period = 125 - 1;//38400Hz 1080¡ã/s
    TIM_TimeBaseInit(DRV8825_STP_TIM, &TIM_BaseInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//OCxREF is high when TIMx_CNT < TIMx_CCRx else low
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;//0% dutyrate
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//output high when OCxREF is high
    TIM_ARRPreloadConfig(DRV8825_STP_TIM, DISABLE);//do not buffer ARR
    DRV8825_STP_TIM_OC_FUNC(DRV8825_STP_TIM, &TIM_OCInitStructure);
    DRV8825_STP_TIM_OC_PRELOAD_FUNC(DRV8825_STP_TIM, TIM_OCPreload_Disable);//do not buffer CCR1
    
    
    TIM_SelectMasterSlaveMode(DRV8825_STP_TIM, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(DRV8825_STP_TIM, TIM_TRGOSource_Update);
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1);
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR2);
}

/**
 * @brief Stop and hold the stepper motor.
 */
inline void DRV8825_Stop()
{
    TIM_Cmd(DRV8825_STP_TIM, DISABLE);
}

/**
 * @brief Set speed of specific motor
 * @param motorX            TB6612FNG_MOTOR_X where X can be (A...D) to select motor.
 *                          This parameter can be any combination of @ref TB6612FNG_motor_select.
 * @param pwmPulse          The absolute value of this parameter configures the pulse time of pwm period.
 *                          This parameter should be within the range of [-4200, 4200], value less than zero reverses rotation of motor(s).
 *                          The dutyrate is computed using the following formula:
 *                           - dutyrate = abs(pwmPulse) / period
 *                          Where period is the value in the ARR register.
 */
void DRV8825_Run(float speed, uint16_t angle)
{
    TIM_Cmd(DRV8825_STP_TIM, DISABLE);
    DRV8825_STP_TIM_ARR = (135000 / speed) - 1;//(uint32_t)(4800000.0f/(speed/0.9f*32.0f)) - 1;
    DRV8825_STP_TIM_CCR = (DRV8825_STP_TIM_ARR - 1) >> 1;//50% dutyrate
    TIM_Cmd(DRV8825_STP_TIM, ENABLE);
}
