/**
 * @file    control.c
 * @author  Miaow
 * @version 0.2.2
 * @date    2018/09/08
 * @brief   
 *          This file provides application functions to manage the following 
 *          functionalities of the car:
 *              1. Initialization
 *              2. Steering, Shift and Operating direction with pid control
 * @note
 *          Minimum version of header file:
 *              0.2.2
 */
 
#include "stm32f4xx.h"
#include "hallencoder.h"
#include "tb6612fng.h"
#include "mpu9250.h"
#include "control.h"
#include "delay.h"
#include "stdio.h"/////////////
#ifdef CONTROL_DEBUG
#include "stdio.h"
#endif
/** @addtogroup CONTROL
 * @{
 */
 
/**
 * @brief Target speed of left and right motors in бу/s.
 * @note targetSpeed[0] - left speed.
 *       targetSpeed[1] - right speed.
 */
static int32_t targetSpeed[3] = {0};

/**
 * @brief Actual speed of left and right motors in бу/s.
 * @note actualSpeed[0] - left speed.
 *       actualSpeed[1] - right speed.
 */
static int32_t actualSpeed[2] = {0};

/**
 * @brief Pwm dutyrate for left and right motors.
 * @note actualSpeed[0] - left speed.
 *       actualSpeed[1] - right speed.
 */
static int32_t outputSpeed[2] = {0};

static int32_t targetAngle, actualAngle;

/**
 * @brief States of the car.
 */
static CONTROL_StateTypedef CONTROL_State = CONTROL_StateStop;


int32_t CONTROL_IncrementalPi(uint8_t motorX, int32_t actualSpeed, int32_t targetSpeed);
void CONTROL_Refresh(void);

/**
 * @brief Initialize the contorller.
 */
void CONTROL_Init()
{
    /*
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    */
    #ifdef CONTROL_DEBUG
    printf("tb6612fng init...");
    #endif

    TB6612FNG_Init();
    #ifdef CONTROL_DEBUG
    printf("ok\r\nhallencode init...");
    #endif
    HALLENCODER_Init();
    #ifdef CONTROL_DEBUG
    printf("ok\r\nmpu init...");
    #endif
    int32_t code = (int32_t)MPU9250_InitWithDmp(CONTROL_Refresh);
    #ifdef CONTROL_DEBUG
    printf("%d\r\n", code);
    #endif
    /*
    //Sample rate T = 50ms
    RCC_APB1PeriphClockCmd(CONTROL_SAMPLE_TIM_CLK, ENABLE);
    TIM_TimeBaseStructure.TIM_Prescaler = 420;
    TIM_TimeBaseStructure.TIM_Period = 10000;
    TIM_TimeBaseInit(CONTROL_SAMPLE_TIM, &TIM_TimeBaseStructure);
    NVIC_InitStructure.NVIC_IRQChannel = CONTROL_SAMPLE_TIM_IRQ_CHANNEL;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(CONTROL_SAMPLE_TIM, TIM_EventSource_Update, ENABLE);

    TIM_Cmd(CONTROL_SAMPLE_TIM, ENABLE);
    */
}

/**
 * @brief Callback for sampling, calculation and action and so on.
 */
void CONTROL_Refresh()
{
    static uint32_t i = 0;
    float pitch, roll, yaw;//Euler angle of the car
    if(!MPU9250_GetDmpData(&pitch, &roll, &yaw))
        actualAngle = (int32_t)yaw;

    if((++i) == 10)
    {
        i = 0;
        actualSpeed[0] = -HALLENCODER_ReadDeltaValue(HALLENCODER_B) * 7 / 2;//actual speed of left
        actualSpeed[1] = HALLENCODER_ReadDeltaValue(HALLENCODER_A) * 7 / 2;//actual speed of right
        outputSpeed[0] = -CONTROL_IncrementalPi(CONTROL_MOTOR_LEFT, actualSpeed[0], targetSpeed[0]);//calculate left pwm
        outputSpeed[1] = CONTROL_IncrementalPi(CONTROL_MOTOR_RIGHT, actualSpeed[1], targetSpeed[1]);//calculate right pwm
        TB6612FNG_Run(CONTROL_MOTOR_LEFT, outputSpeed[0]);//motor A, B --> left motors
        TB6612FNG_Run(CONTROL_MOTOR_RIGHT, outputSpeed[1]);//motor C, D --> right motors
        #ifdef CONTROL_DEBUG
        //printf("t=%d,%d,o=%d,%d,a=%d,%d\r\n", targetSpeed[0], targetSpeed[1], outputSpeed[0], outputSpeed[1], actualSpeed[0], actualSpeed[1]);
        printf("%d,%d,%d,%d\r\n", targetSpeed[0], targetSpeed[1], actualSpeed[0], actualSpeed[1]);
        //printf("%d,%f\r\n", targetAngle, yaw);
        #endif
        switch(CONTROL_State)
        {
            case CONTROL_StateStop:
                break;
            case CONTROL_StateTurning:
                if(targetAngle - actualAngle <= CONTROL_TURNING_ANGLE_THRESHOLD && targetAngle - actualAngle >= -CONTROL_TURNING_ANGLE_THRESHOLD)
                    CONTROL_State = CONTROL_StateTurnComplete;
                break;
            case CONTROL_StateGoStraight:
                break;
            default:
                break;
        }
    }      
}

/**
 * @brief IRQ for sampling and motor action.
 */
/*
void CONTROL_SAMPLE_TIM_IRQ_HANDLER(void)
{
    if(CONTROL_SAMPLE_TIM->SR & TIM_FLAG_Update)
    {
        actualSpeed[0] = HALLENCODER_ReadDeltaValue(HALLENCODER_A) * 7;//actual speed of left
        actualSpeed[1] = HALLENCODER_ReadDeltaValue(HALLENCODER_B) * 7;//actual speed of right
        outputSpeed[0] = CONTROL_IncrementalPi(TB6612FNG_MOTOR_A, actualSpeed[0], targetSpeed[0]);//TB6612FNG_MOTOR_A | TB6612FNG_MOTOR_B, actualSpeed[0], targetSpeed[0]);//calculate left pwm
        outputSpeed[1] = CONTROL_IncrementalPi(TB6612FNG_MOTOR_B, actualSpeed[1], targetSpeed[1]);//TB6612FNG_MOTOR_C | TB6612FNG_MOTOR_D, actualSpeed[1], targetSpeed[1]);//calculate right pwm
        TB6612FNG_Run(TB6612FNG_MOTOR_A, outputSpeed[0]);//TB6612FNG_MOTOR_A | TB6612FNG_MOTOR_B, outputSpeed[0]);//motor A, B --> motors in left
        TB6612FNG_Run(TB6612FNG_MOTOR_B, outputSpeed[1]);//TB6612FNG_MOTOR_C | TB6612FNG_MOTOR_D, outputSpeed[1]);//motor C, D --> motors in right
        //printf("%d,%d,%d,%d\r\n", targetSpeed[0], targetSpeed[1], actualSpeed[0], actualSpeed[1]);
        CONTROL_SAMPLE_TIM->SR &= ~TIM_FLAG_Update;
    }
}
*/

inline CONTROL_StateTypedef CONTROL_GetState()
{
    return CONTROL_State;
}


/**
 * @brief Get speed of left or right motors.
 * @param motorLeftRight    This parameter can be a value of @ref CONTROL_motor_select.
 *                          @arg CONTROL_MOTOR_LEFT    Select left motors.
 *                          @arg CONTROL_MOTOR_RIGHT   Select right motors.
 *                          DO NOT pass in any combination of @ref CONTROL_motor_select.
 * @return Return actual speed in бу/s of specified motors.
 */
float CONTROL_GetSpeed(uint8_t motorLeftRight)//бу/s
{
    switch(motorLeftRight)
    {
        case CONTROL_MOTOR_LEFT:
            return actualSpeed[0];
        case CONTROL_MOTOR_RIGHT:
            return actualSpeed[1];
        default:
            return 0.0f;
    }
}

/**
 * @brief Set speed of left or right motors.
 * @param motorLeftRight    This parameter can be a value of @ref CONTROL_motor_select.
 *                          @arg CONTROL_MOTOR_LEFT    Select left motors.
 *                          @arg CONTROL_MOTOR_RIGHT   Select right motors.
 *                          DO NOT pass in any combination of @ref CONTROL_motor_select.
 * @param speed             Target speed in бу/s of specified motors.
 */
void CONTROL_SetSpeed(uint8_t motorLeftRight, int32_t speed)//бу/s
{
    if((motorLeftRight & CONTROL_MOTOR_LEFT) == CONTROL_MOTOR_LEFT)
        targetSpeed[0] = speed;
    if((motorLeftRight & CONTROL_MOTOR_RIGHT) == CONTROL_MOTOR_RIGHT)
        targetSpeed[1] = speed;
}

/**
 * @brief Go staright.
 * @param speed             Target speed in бу/s of the car.
 */
void CONTROL_GoStraight(int32_t speed)
{
    targetSpeed[0] = targetSpeed[1] = speed;
    targetSpeed[2] = speed;
    CONTROL_State = CONTROL_StateGoStraight;
}

/**
 * @brief Awsome turn.
 * @param turningRadius     Turning radius in centimeter of the car at central axis, negative to turn another direction.
 * @param speed             Target speed in бу/s of the car, negative to go back.
 * @param angle             Delta angle in бу between the yaw after turning and that before turning.
 * @note                    If turningRadius don't equals to zero the speed is measured at central axis,
 *                          otherwise the speed is target speed of motors.
 */
void CONTROL_Turn(float turningRadius, int32_t speed, int32_t angle)//cm, бу/s, бу
{

//    outerSpeed / innerSpeed = outerRadius/innerRadius
//    outerRadius = innerRadius + CONTROL_WHEELBASE
//    turningRadius = (outerRadius + innerRadius) / 2
//    speed = (outerSpeed + innerSpeed) / 2
//    innerSpeed / speed = innerRadius / turningRadius
//    outerSpeed / speed = outerRadius / turningRadius
//==> 
//    innerSpeed = (turningRadius - CONTROL_WHEELBASE / 2) / turningRadius * speed
//    outerSpeed = (turningRadius + CONTROL_WHEELBASE / 2) / turningRadius * speed
    
    int16_t tmpTargetAngle;
    
    #ifdef CONTROL_TURN_METHOD1
    static uint32_t lastAngle;
    static uint8_t isFirstTime = 1;
    if(isFirstTime)
    {
        lastAngle = actualAngle;
        isFirstTime = 0;
    }
    tmpTargetAngle = (lastAngle + angle) % 360;
    #elif defined (CONTROL_TURN_METHOD2)
    tmpTargetAngle = (actualAngle + angle) % 360;
    #endif
    
    if(turningRadius == 0)
    {
        targetSpeed[0] = speed;
        targetSpeed[1] = -speed;
    }else{
        targetSpeed[0] = (int32_t)(turningRadius + CONTROL_WHEELBASE / 2) / turningRadius * speed;
        targetSpeed[1] = (int32_t)(turningRadius - CONTROL_WHEELBASE / 2) / turningRadius * speed;
    }
    targetSpeed[2] = speed;
    
    if(tmpTargetAngle > 180)
        targetAngle = tmpTargetAngle - 360;
    else if(tmpTargetAngle < -180)
        targetAngle = tmpTargetAngle + 360;
    else
        targetAngle = tmpTargetAngle;
    
    #ifdef CONTROL_TURN_METHOD1
    lastAngle = targetAngle;
    #endif
    
    CONTROL_State = CONTROL_StateTurning;
    delay_ms(10);//wait for at least one CONTROL_Refresh()
    while(CONTROL_State == CONTROL_StateTurning);//wait until complete
}

/**
 * @brief Control the motor speed with PI algorithm.
 * @param motorX            TB6612FNG_MOTOR_X where X can be (A...D) to select motor.
 *                          This parameter can be any combination of @ref TB6612FNG_motor_select.
 * @param actualSpeed   Actual speed of the motor in бу/s.
 * @param targetSpeed   Target speed of the motor in бу/s.
 * @return Pwm dutyrate for the motor.
 */
int32_t CONTROL_IncrementalPi(uint8_t motorX, int32_t actualSpeed, int32_t targetSpeed)
{
    static float bias[2] = {0.0f, 0.0f};
    static float outputValue[2] = {0.0f, 0.0f};
    static float lastBias[2] = {0.0f, 0.0f};
    if((motorX & CONTROL_MOTOR_LEFT) == CONTROL_MOTOR_LEFT)//(motorX & TB6612FNG_MOTOR_A) || (motorX & TB6612FNG_MOTOR_B))
    {
        bias[0] = actualSpeed - targetSpeed;
        outputValue[0] += (CONTROL_VELOCITY_KP * (bias[0] - lastBias[0]) + CONTROL_VELOCITY_KI * bias[0]);
        lastBias[0] = bias[0];
        return (int32_t)outputValue[0];
    }
    if((motorX & CONTROL_MOTOR_RIGHT) == CONTROL_MOTOR_RIGHT)//(motorX & TB6612FNG_MOTOR_C) || (motorX & TB6612FNG_MOTOR_D))
    {
        bias[1] = actualSpeed - targetSpeed;
        outputValue[1] += (CONTROL_VELOCITY_KP * (bias[1] - lastBias[1]) + CONTROL_VELOCITY_KI * bias[1]);
        lastBias[1] = bias[1];
        return (int32_t)outputValue[1];
    }
    return 0;
}

/**
 * @}
 */ 
