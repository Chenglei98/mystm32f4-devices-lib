/**
 * @file    tb6612fng.c
 * @author  Miaow
 * @version 0.1.0
 * @date    2018/09/20
 * @brief   
 *          This file provides bsp functions to manage the following 
 *          functionalities of manipulator:
 *              1. Initialization and Configuration
 *              2. maipulator operation
 * @note
 *          Minimum version of header file:
 *              0.1.0
 *          Recommanded pin connection:
 *             ©°©¤©¤©¤©¤©¤©¤©¤©¤©´     ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
 *             ©¦  YELLOW©À©¤©¤©¤©¤©¤©ÈPB8        ©¦
 *             ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¼     ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼
 *                servo         STM32F407  
 *  
 *          The source code repository is available on GitHub:
 *              https://github.com/3703781/mystm32f4-devices-lib
 *          Your pull requests will be welcome.
 *          Here are the guidelines for your pull requests:
 *              1. Respect my coding style.
 *              2. Avoid to commit several features in one commit.
 *              3. Make your modification compact - don't reformat source code in your request.
 */
#include "manipulator.h"

/** @addtogroup MANIPULATOR
 * @{
 */
 
/**
 * @brief States defination of manipulator
 */
static MANIPULATOR_StateTypeDef MANIPULATOR_State = 
{
    MANIPULATOR_State_Unknown,
    MANIPULATOR_ANGLE_RELEASE
};

/**
 * @brief Initialize manipulator.
 */
void MANIPULATOR_Init()
{//1500=0.5ms
    //7500=2.5ms
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    
    RCC_AHB1PeriphClockCmd(MANIPULATOR_PWM_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = MANIPULATOR_PWM_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    
    GPIO_Init(MANIPULATOR_PWM_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(MANIPULATOR_PWM_PORT, MANIPULATOR_PWM_GPIO_PINSOURCE, MANIPULATOR_PWM_GPIO_AF);
    
    MANIPULATOR_TIM_CLK_FUNC(MANIPULATOR_TIM_CLK, ENABLE);
    TIM_TimeBaseStructure.TIM_Prescaler = 56 - 1;
    TIM_TimeBaseStructure.TIM_Period = 60000 - 1;//T=20ms
    TIM_TimeBaseInit(MANIPULATOR_TIM, &TIM_TimeBaseStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//OCxREF is high when TIMx_CNT < TIMx_CCRx else low
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = MANIPULATOR_ANGLE_RELEASE * 100 / 3 + 4500;//1.5ms pulse width = middle(0¡ã)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//output high when OCxREF is high
    MANIPULATOR_TIM_OC_FUNC(MANIPULATOR_TIM, &TIM_OCInitStructure);
    TIM_Cmd(MANIPULATOR_TIM, ENABLE);
    MANIPULATOR_State.MANIPULATOR_GrabState = MANIPULATOR_State_Release;
    MANIPULATOR_State.MANIPULATOR_ServoAngle = MANIPULATOR_ANGLE_RELEASE;
    
}

/**
 * @brief Set angle of the servo for grabbing.
 * @param angle Angle in degree of the servo.
 *        This parameter should be within the range of [-90, 90].
 */
static inline void MANIPULATOR_SetAngle(int32_t angle)
{
    MANIPULATOR_TIM->CCR1 = angle * 100 / 3 + 4500;
}

/**
 * @brief Start to grab.
 */
void MANIPULATOR_Grab()
{
    MANIPULATOR_SetAngle(MANIPULATOR_ANGLE_GRAB);
    MANIPULATOR_State.MANIPULATOR_GrabState = MANIPULATOR_State_Grab;
    MANIPULATOR_State.MANIPULATOR_ServoAngle = MANIPULATOR_ANGLE_GRAB;
}

/**
 * @brief Start to release.
 */
void MANIPULATOR_Release()
{
    MANIPULATOR_SetAngle(MANIPULATOR_ANGLE_RELEASE);
    MANIPULATOR_State.MANIPULATOR_GrabState = MANIPULATOR_State_Release;
    MANIPULATOR_State.MANIPULATOR_ServoAngle = MANIPULATOR_ANGLE_RELEASE;
}

/**
 * @brief Get state of manipulator.
 * @return state of manipulator @ref MANIPULATOR_StateTypeDef.
 */
inline MANIPULATOR_StateTypeDef MANIPULATOR_GetState()
{
    return MANIPULATOR_State;
}

/**
 * @}
 */
