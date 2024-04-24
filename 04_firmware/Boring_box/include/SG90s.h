#ifndef __SG90S_H
#define __SG90S_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "debug.h"

#define minUs 500
#define maxUs 2500

#define LIFT_PIN 14
#define PUSH_PIN 12

#if 1

#define LIFT_HIGH_ANGLE 75 // 掀盖最高角度
#define LIFT_LOOK_ANGLE 90 // 掀盖高角度
#define LIFT_LOW_ANGLE 100 // 掀盖低角度，只能打开，push杆不会出来

#define PUSH_TOUCH_ANGLE 160 // 推杆触碰角度
#define PUSH_HIGH_ANGLE 150  // 推杆高角度
#define PUSH_LOW_ANGLE 135   // 推杆低角度

#endif

#define CHANGE_RANGE 1 // 舵机慢速步距

#define SHAKE_RANGE 5  // 抖动幅度
#define SHAKE_TIMES 10 // 抖动次数

#define SWING_TIMES 5 // 摆动次数

#define ANGLE_LIMIT 5 // 角度限制
#define TIME_OUT 2000 // 舵机未到位超时时间

#define JOKE_TIME 1000 // 欺诈时间

#define HIT_TIMES 5 // 暴击次数

#define WINDUP_TIME 500   // 前摇时间
#define RECOVERY_TIME 500 // 后摇时间

extern uint8_t LIFT_OPEN_ANGLE;  // 掀盖打开角度
extern uint8_t LIFT_CLOSE_ANGLE; // 掀盖关闭角度

extern uint8_t PUSH_OPEN_ANGLE;  // 推杆打开角度
extern uint8_t PUSH_CLOSE_ANGLE; // 推杆关闭角度

enum Velocity
{
    SUPER_SLOW = 25, // 超慢速
    NORMAL = 15,     // 正常速度
    SUPER_FAST = 5   // 超快速
};

class Servo_Base
{
private:
    uint8_t wait_inplace(uint8_t targetAngle);
    Servo servo;
    int currentAngle = 0;
    uint8_t lift_random_nextangle(uint8_t currentAngle);
    uint8_t push_random_nextangle(uint8_t currentAngle);
    void random_velocity_Open(int nextAngle);

public:
    void init(int pin, int pos);
    uint8_t get_current_angle();
    void slow_range(uint8_t begin, uint8_t final, uint8_t change, int velocity, uint16_t windup = 0, uint16_t recovery = 0);
    void quick_range(uint8_t angle, uint16_t windup = 0, uint16_t recovery = 0);
    void shake(uint8_t angle, uint8_t range, uint8_t times);
    void swing(uint8_t times, u_int8_t begin, uint8_t final);
    void swing(uint8_t times, u_int8_t begin, uint8_t final, uint8_t change, Velocity velocity);

    /*lift模式*/
    void lift_quick_open();                   // 快速打开（不限位置）
    void lift_slow_open();                    // 慢速打开（不限位置）
    void lift_quick_close();                  // 快速关闭（不限位置）
    void lift_slow_close();                   // 慢速关闭（不限位置）
    void lift_heavyhit();                     // 重砸推杆，之后打开（不限位置）
    void lift_caress();                       // 轻抚推杆，之后打开（不限位置）
    void lift_random_open(uint8_t maxTries);  // 随机到位中间位置，随机选择快速/随机速度慢速切换，之后打开（不限位置）
    void lift_random_close(uint8_t maxTries); // 随机到位中间位置，随机选择快速/随机速度慢速切换，之后关闭（不限位置）
    void lift_random_shake(uint8_t maxTries); // 随机到位中间位置，之后抖动（不限位置）
    void lift_random_swing(uint8_t maxTries); // 随机到位中间位置，之后摇摆（不限位置）

    /*push模式*/
    void push_quick_open();                   // 快速打开（不限位置）
    void push_quick_touch();                  // 快速触摸（不限位置）
    void push_poweropen();                    // 快速蓄力打开
    void push_slow_open();                    // 慢速到触摸，然后后仰至高位然后快速打开（不限位置）
    void push_slow_touch();                   // 慢速触摸（不限位置）
    void push_quick_close();                  // 快速关闭（不限位置）
    void push_slow_close();                   // 慢速关闭（不限位置）
    void push_random_open(uint8_t maxTries);  // 随机到中间位置，随机选择快速/随机速度慢速切换，之后打开（不限位置）
    void push_random_close(uint8_t maxTries); // 随机到中间位置，随机选择快速/随机速度慢速切换，之后关闭（不限位置）
    void push_random_shake(uint8_t maxTries); // 随机到中间位置，之后抖动（不限位置）
    void push_random_swing(uint8_t maxTries); // 随机到中间位置，之后摇摆（不限位置）
};

extern Servo_Base pushservo;
extern Servo_Base liftservo;

#endif
