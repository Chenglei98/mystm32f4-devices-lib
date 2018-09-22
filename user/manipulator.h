/**
 * @file    tb6612fng.h
 * @author  Miaow
 * @version 0.1.0
 * @date    2018/09/20
 * @brief   
 *          This file provides bsp functions to manage the following 
 *          functionalities of manipulator:
 *              1. Initialization and Configuration
 *              2. maipulator operation
 * @note
 *          Minimum version of source file:
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

#ifndef __MANIPULATOR_H
#define __MANIPULATOR_H

#include "stm32f4xx.h"

/** 
 * @defgroup MANIPULATOR
 * @brief MANIPULATOR driver modules
 * @{
 */
/** 
 * @defgroup HALLENCODER_gpio_define
 * @{
 */
//GPIO
#define MANIPULATOR_PWM_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define MANIPULATOR_PWM_PORT                GPIOB
#define MANIPULATOR_PWM_PIN                 GPIO_Pin_8
#define MANIPULATOR_PWM_GPIO_PINSOURCE      GPIO_PinSource8
#define MANIPULATOR_PWM_GPIO_AF             GPIO_AF_TIM10
/**
 * @}
 */
 
 /** 
 * @defgroup HALLENCODER_timer_define
 * @{
 */
//TIM
#define MANIPULATOR_TIM_CLK_FUNC            RCC_APB2PeriphClockCmd
#define MANIPULATOR_TIM_CLK                 RCC_APB2Periph_TIM10
#define MANIPULATOR_TIM                     TIM10
#define MANIPULATOR_TIM_OC_FUNC             TIM_OC1Init
/**
 * @}
 */

/**
 * @brief Angle of servo to release
 */
#define MANIPULATOR_ANGLE_RELEASE           0
/**
 * @brief Angle of servo to grab
 */
#define MANIPULATOR_ANGLE_GRAB              -40

/**
 * @brief Grab states defination of manipulator
 */
typedef enum
{
    //grabbing state
    MANIPULATOR_State_Grab = 0,
    //releasing state
    MANIPULATOR_State_Release,
    //unknown state
    MANIPULATOR_State_Unknown,
    //num of states = 3
    MANIPULATOR_State_Num
}MANIPULATOR_GrabStateTypeDef;

/**
 * @brief States defination of manipulator
 */
typedef struct
{
    //state
    __IO MANIPULATOR_GrabStateTypeDef MANIPULATOR_GrabState;
    //angle
    __IO int32_t MANIPULATOR_ServoAngle;
}MANIPULATOR_StateTypeDef;


void MANIPULATOR_Grab(void);
void MANIPULATOR_Release(void);
void MANIPULATOR_Init(void);
extern inline MANIPULATOR_StateTypeDef MANIPULATOR_GetState(void);

/**
 * @}
 */
#endif
