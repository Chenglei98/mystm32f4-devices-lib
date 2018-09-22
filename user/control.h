/**
 * @file    control.h
 * @author  Miaow
 * @version 0.2.2
 * @date    2018/09/11
 * @brief   
 *          This file provides application functions to manage the following 
 *          functionalities of the car:
 *              1. Initialization
 *              2. Steering, Shift and Operating direction with pid control
 * @note
 *          Minimum version of source file:
 *              0.1.12
 */

#ifndef __CONTROL_H
#define __CONTROL_H

/** 
 * @defgroup CONTROL
 * @brief Control Modules
 * @{
 */

//#define CONTROL_DEBUG

/** 
 * @defgroup CONTROL_timer_define
 * @brief timer for sampling
 * @{
 */
//#define CONTROL_SAMPLE_TIM              TIM6
//#define CONTROL_SAMPLE_TIM_CLK          RCC_APB1Periph_TIM6
//#define CONTROL_SAMPLE_TIM_IRQ_CHANNEL  TIM6_DAC_IRQn
//#define CONTROL_SAMPLE_TIM_IRQ_HANDLER  TIM6_DAC_IRQHandler
/**
 * @}
 */ 
 
/** 
 * @defgroup CONTROL_motor_select
 * @{
 */
#define CONTROL_MOTOR_LEFT              (TB6612FNG_MOTOR_A | TB6612FNG_MOTOR_B)//motor A, B --> left motors
#define CONTROL_MOTOR_RIGHT             (TB6612FNG_MOTOR_C | TB6612FNG_MOTOR_D)//motor C, D --> right motors
#define CONTROL_MOTOR_ALL               (CONTROL_MOTOR_LEFT | CONTROL_MOTOR_RIGHT)
/**
 * @}
 */
 
/** 
 * @defgroup CONTROL_pid_parameter
 * @{
 */
#define CONTROL_VELOCITY_KP             0.9f
#define CONTROL_VELOCITY_KI             1.07f
/**
 * @}
 */

/** 
 * @defgroup CONTROL_car_parameter
 * @{
 */
 
 /**
 * @brief States of the car.
 */
typedef enum
{
    CONTROL_StateStop,
    CONTROL_StateTurning,
    CONTROL_StateTurnComplete,
    CONTROL_StateGoStraight
}CONTROL_StateTypedef;

/**
 * @brief Wheel-base of the car in centimeter
 */
#define CONTROL_WHEELBASE   15.4f

/**
 * @brief Threshold of angle in degree when turning
 */
#define CONTROL_TURNING_ANGLE_THRESHOLD     9.5f
/**
 * @}
 */

#define CONTROL_TURN_METHOD2

void CONTROL_Init(void);
float CONTROL_GetSpeed(uint8_t motorLeftRight);
void CONTROL_SetSpeed(uint8_t motorLeftRight, int32_t speed);
void CONTROL_GoStraight(int32_t speed);
void CONTROL_Turn(float turningRadius, int32_t speed, int32_t angle);
extern inline CONTROL_StateTypedef CONTROL_GetState(void);
/**
 * @}
 */ 
  
#endif
