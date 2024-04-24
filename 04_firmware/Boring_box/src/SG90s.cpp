#include "SG90s.h"

uint8_t LIFT_OPEN_ANGLE = 60;   // 掀盖打开角度
uint8_t LIFT_CLOSE_ANGLE = 123; // 掀盖关闭角度

uint8_t PUSH_OPEN_ANGLE = 180;  // 推杆打开角度
uint8_t PUSH_CLOSE_ANGLE = 100; // 推杆关闭角度

Servo_Base pushservo;
Servo_Base liftservo;

/**
 * @brief 等待舵机到位
 *
 * @param targetAngle 目标角度
 * @return uint8_t 到位返回1，超时返回0
 */
uint8_t Servo_Base::wait_inplace(uint8_t targetAngle)
{
    uint32_t startTime = xTaskGetTickCount() * portTICK_PERIOD_MS; // 获取当前时间
    while (abs(servo.read() - targetAngle) > ANGLE_LIMIT)          // 当前角度与目标角度的差值大于5度
    {
        if ((xTaskGetTickCount() * portTICK_PERIOD_MS) - startTime > TIME_OUT) // 超时2s未到位则退出
        {
            Debug_println("Error: Servo failed to reach the target angle within timeout period.");
            return 0; // 超时退出
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return 1; // 到位
}

/**
 * @brief Lift舵机随机下一个角度
 *
 * @param currentAngle 当前角度
 * @return uint8_t Lift舵机随机角度
 */
uint8_t Servo_Base::lift_random_nextangle(uint8_t currentAngle)
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t angles[] = {LIFT_HIGH_ANGLE, LIFT_LOOK_ANGLE, LIFT_LOW_ANGLE};
    uint8_t newAngle;
    do
    {
        uint8_t index = random(0, 3); // 随机选择一个索引（0到3）
        newAngle = angles[index];
    } while (newAngle == currentAngle); // 确保新角度与当前角度不同

    return newAngle; // 返回新的角度
}

/**
 * @brief Push舵机随机下一个角度
 *
 * @param currentAngle 当前角度
 * @return uint8_t Push舵机随机角度
 */
uint8_t Servo_Base::push_random_nextangle(uint8_t currentAngle)
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t angles[] = {PUSH_LOW_ANGLE, PUSH_HIGH_ANGLE, PUSH_TOUCH_ANGLE};
    uint8_t newAngle;
    do
    {
        uint8_t index = random(0, 3); // 随机选择一个索引（0到2）
        newAngle = angles[index];
    } while (newAngle == currentAngle); // 确保新角度与当前角度不同

    return newAngle; // 返回新的角度
}

/**
 * @brief 随机速度到达下一个角度
 *
 * @param nextAngle 下一个角度
 */
void Servo_Base::random_velocity_Open(int nextAngle)
{
    randomSeed(micros());         // 使用当前时间的微秒部分更新随机种子
    bool useQuick = random(0, 2); // 随机决定使用快速或慢速方法
    if (useQuick)
    {
        quick_range(nextAngle);
    }
    else
    {
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        const Velocity validVelocities[] = {SUPER_SLOW, NORMAL, SUPER_FAST};
        Velocity randomVelocity = validVelocities[rand() % 3]; // 随机选取速度
        Debug_print("Random velocity: ");
        Debug_println(randomVelocity);
        slow_range(currentAngle, nextAngle, CHANGE_RANGE, randomVelocity);
    }
}

/****************************************************************************************************************/
/****************************************************************************************************************/
/****************************************************PUBLIC******************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/

/**
 * @brief 舵机初始化
 *
 * @param pin 指定PWM引脚
 * @param pos 指定初始角度
 */
void Servo_Base::init(int pin, int pos)
{
    ESP32PWM::allocateTimer(0);
    servo.setPeriodHertz(50);
    servo.attach(pin, minUs, maxUs);
    servo.write(pos);
    currentAngle = pos;
}

uint8_t Servo_Base::get_current_angle()
{
    return currentAngle;
}

void Servo_Base::slow_range(uint8_t begin, uint8_t final, uint8_t change, int velocity, uint16_t windup, uint16_t recovery)
{
    const uint8_t minChange = 1;      // 最小步进
    const uint8_t maxChange = change; // 最大步进，使用传入的change作为最大步进
    uint8_t currentChange = minChange;
    const Velocity maxVelocity = SUPER_FAST; // 最大速度
    const Velocity minVelocity = SUPER_SLOW; // 最小速度，示例中减半
    uint16_t currentVelocity = maxVelocity;
    const uint8_t accelerationSteps = (final - begin) / 4; // 加速/减速阶段的步长

    vTaskDelay(pdMS_TO_TICKS(windup));

    if (begin < final)
    {
        for (uint8_t i = begin; i < final; i += currentChange)
        {
            servo.write(i);
            vTaskDelay(pdMS_TO_TICKS(currentVelocity));

            // 动态调整步进和速度
            if (i - begin < accelerationSteps)
            {
                if (currentChange < maxChange)
                    currentChange++;
                if (currentVelocity > minVelocity)
                    currentVelocity--;
            }
            else if (final - i < accelerationSteps)
            {
                if (currentChange > minChange)
                    currentChange--;
                if (currentVelocity < maxVelocity)
                    currentVelocity++;
            }
        }
    }
    else
    {
        for (uint8_t i = begin; i > final; i -= currentChange)
        {
            servo.write(i);
            vTaskDelay(pdMS_TO_TICKS(currentVelocity));

            // 动态调整步进和速度
            if (begin - i < accelerationSteps)
            {
                if (currentChange < maxChange)
                    currentChange++;
                if (currentVelocity > minVelocity)
                    currentVelocity--;
            }
            else if (i - final < accelerationSteps)
            {
                if (currentChange > minChange)
                    currentChange--;
                if (currentVelocity < maxVelocity)
                    currentVelocity++;
            }
        }
    }

    vTaskDelay(pdMS_TO_TICKS(recovery));
}

void Servo_Base::quick_range(uint8_t angle, uint16_t windup, uint16_t recovery)
{
    vTaskDelay(pdMS_TO_TICKS(windup));
    servo.write(angle);
    vTaskDelay(pdMS_TO_TICKS(recovery));
    vTaskDelay(pdMS_TO_TICKS(50)); // 引入50毫秒的延时, 保证舵机能够到位
}

/**
 * @brief 规定角度左右鬼畜晃动
 *
 * @param angle 指定角度
 * @param range 晃动幅度
 * @param times 晃动次数
 */
void Servo_Base::shake(uint8_t angle, uint8_t range, uint8_t times)
{
    uint8_t minAngle = angle - range; // 计算摆动的最小角度
    uint8_t maxAngle = angle + range; // 计算摆动的最大角度

    for (uint8_t i = 0; i < times; i++)
    {
        // 移动到最小角度
        quick_range(minAngle);
        wait_inplace(minAngle); // 等待伺服电机到达最小角度
        //  移动到最大角度
        quick_range(maxAngle);
        wait_inplace(maxAngle); // 等待伺服电机到达最大角度
    }

    quick_range(angle);
    wait_inplace(angle);  // 等待伺服电机返回到指定的角度
    currentAngle = angle; // 更新当前角度
}

void Servo_Base::swing(uint8_t times, u_int8_t begin, uint8_t final)
{
    for (uint8_t count = 0; count < times; count++)
    {
        quick_range(final);
        wait_inplace(final);
        quick_range(begin);
        wait_inplace(begin);
    }
    currentAngle = begin;
}

/**
 * @brief 规定角度内慢速摆动
 * @param times 来回摆动次数
 * @param begin 起始角度
 * @param final 终止角度
 * @param change 变化幅度
 * @param velocity 变化速度
 */
void Servo_Base::swing(uint8_t times, uint8_t begin, uint8_t final, uint8_t change, Velocity velocity)
{
    for (int count = 0; count < times; count++)
    {
        slow_range(begin, final, change, velocity);
        wait_inplace(final);
        slow_range(final, begin, change, velocity);
        wait_inplace(begin);
    }
    currentAngle = begin;
}

/*lift舵机模式*/
void Servo_Base::lift_quick_open()
{
    quick_range(LIFT_OPEN_ANGLE);
    if (wait_inplace(LIFT_OPEN_ANGLE))
        currentAngle = LIFT_OPEN_ANGLE;
}

void Servo_Base::lift_slow_open()
{
    uint8_t begin = get_current_angle();
    slow_range(begin, LIFT_OPEN_ANGLE, CHANGE_RANGE, SUPER_SLOW);
    if (wait_inplace(LIFT_OPEN_ANGLE))
        currentAngle = LIFT_OPEN_ANGLE;
}

void Servo_Base::lift_quick_close()
{
    quick_range(LIFT_CLOSE_ANGLE);
    if (wait_inplace(LIFT_CLOSE_ANGLE))
        currentAngle = LIFT_CLOSE_ANGLE;
}

void Servo_Base::lift_slow_close()
{
    uint8_t begin = get_current_angle();
    slow_range(begin, LIFT_CLOSE_ANGLE, CHANGE_RANGE, SUPER_SLOW);
    if (wait_inplace(LIFT_CLOSE_ANGLE))
        currentAngle = LIFT_CLOSE_ANGLE;
}

void Servo_Base::lift_heavyhit()
{
    quick_range(LIFT_LOW_ANGLE); // 快速到达重击点
    if (wait_inplace(LIFT_LOW_ANGLE))
    {
        swing(HIT_TIMES, LIFT_LOW_ANGLE, LIFT_HIGH_ANGLE); // 猛砸
    }
    quick_range(LIFT_OPEN_ANGLE);
    currentAngle = LIFT_OPEN_ANGLE;
}

void Servo_Base::lift_caress()
{
    quick_range(LIFT_LOW_ANGLE); // 快速到达重击点
    if (wait_inplace(LIFT_LOW_ANGLE))
    {
        swing(HIT_TIMES, LIFT_LOW_ANGLE, LIFT_HIGH_ANGLE, CHANGE_RANGE, SUPER_SLOW); // 轻抚
    }
    quick_range(LIFT_OPEN_ANGLE);
    currentAngle = LIFT_OPEN_ANGLE;
}

/**
 * @brief Lift舵机随机模式打开
 *
 * @param maxTries 随机动作次数
 */
void Servo_Base::lift_random_open(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = lift_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Lift next angle: ");
        Debug_println(nextAngle);
        random_velocity_Open(nextAngle); // 随机速度到达下一个角度
        wait_inplace(nextAngle);         // 等待到位

        currentAngle = nextAngle; // 更新当前角度
        count++;
    }
    quick_range(LIFT_OPEN_ANGLE);
    currentAngle = LIFT_OPEN_ANGLE;
    Debug_println("Lift Force open");
}

void Servo_Base::lift_random_close(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = lift_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Lift next angle: ");
        Debug_println(nextAngle);
        quick_range(nextAngle);  // 随机速度到达下一角度
        wait_inplace(nextAngle); // 等待到位

        currentAngle = nextAngle; // 更新当前角度
        count++;
    }
    quick_range(LIFT_CLOSE_ANGLE);
    currentAngle = LIFT_CLOSE_ANGLE;
}

void Servo_Base::lift_random_shake(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = lift_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Lift next angle: ");
        Debug_println(nextAngle);
        quick_range(nextAngle);  // 随机速度到达下一角度
        wait_inplace(nextAngle); // 等待到位

        shake(nextAngle, SHAKE_RANGE, SHAKE_TIMES); // 抖动

        currentAngle = nextAngle; // 更新当前角度
        count++;
    }
    quick_range(LIFT_OPEN_ANGLE);
    currentAngle = LIFT_OPEN_ANGLE;
}

void Servo_Base::lift_random_swing(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = lift_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Lift next angle: ");
        Debug_println(nextAngle);
        quick_range(nextAngle);  // 随机速度到达下一角度
        wait_inplace(nextAngle); // 等待到位

        randomSeed(micros());
        bool QUICK_FLAG = random(0, 2); // 随机选择快速或慢速摆动
        if (QUICK_FLAG)
        {
            swing(SWING_TIMES, nextAngle, LIFT_OPEN_ANGLE); // 快速摇摆到触摸处
        }
        else
        {
            swing(SWING_TIMES, nextAngle, LIFT_OPEN_ANGLE, CHANGE_RANGE, SUPER_SLOW); // 慢速摇摆到触摸处
        }

        currentAngle = nextAngle; // 更新当前角度
        count++;
    }
    quick_range(LIFT_OPEN_ANGLE);
    currentAngle = LIFT_OPEN_ANGLE;
}

/*********************************************push舵机控制**********************************************************/
void Servo_Base::push_quick_open()
{
    quick_range(PUSH_OPEN_ANGLE);
    if (wait_inplace(PUSH_OPEN_ANGLE))
        currentAngle = PUSH_OPEN_ANGLE;
}

void Servo_Base::push_quick_touch()
{
    quick_range(PUSH_TOUCH_ANGLE);
    if (wait_inplace(PUSH_TOUCH_ANGLE))
        currentAngle = PUSH_TOUCH_ANGLE;
}

void Servo_Base::push_poweropen()
{
    if (!(get_current_angle() == PUSH_LOW_ANGLE))
    {
        quick_range(PUSH_LOW_ANGLE);
        wait_inplace(PUSH_LOW_ANGLE);
    }
    vTaskDelay(pdMS_TO_TICKS(100)); // 引入50毫秒的延时, 保证舵机能够到位
    push_quick_open();
}

void Servo_Base::push_slow_open()
{
    push_slow_touch();
    push_poweropen();
}

void Servo_Base::push_slow_touch()
{
    uint8_t begin = get_current_angle();
    slow_range(begin, PUSH_TOUCH_ANGLE, CHANGE_RANGE, SUPER_SLOW);
    if (wait_inplace(PUSH_TOUCH_ANGLE))
        currentAngle = PUSH_TOUCH_ANGLE;
}

void Servo_Base::push_quick_close()
{
    quick_range(PUSH_CLOSE_ANGLE);
    if (wait_inplace(PUSH_CLOSE_ANGLE))
        currentAngle = PUSH_CLOSE_ANGLE;
}

void Servo_Base::push_slow_close()
{
    uint8_t begin = get_current_angle();
    slow_range(begin, PUSH_CLOSE_ANGLE, CHANGE_RANGE, SUPER_SLOW);
    if (wait_inplace(PUSH_CLOSE_ANGLE))
        currentAngle = PUSH_CLOSE_ANGLE;
}

void Servo_Base::push_random_open(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = push_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Push next angle: ");
        Debug_println(nextAngle);
        random_velocity_Open(nextAngle); // 随机速度到达下一个角度
        wait_inplace(nextAngle);         // 等待到位
        currentAngle = nextAngle;        // 更新当前角度
        count++;
    }
    push_poweropen(); // 蓄力打开
}

void Servo_Base::push_random_close(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = push_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Push next angle: ");
        Debug_println(nextAngle);
        quick_range(nextAngle);  // 快速到达下个角度
        wait_inplace(nextAngle); // 等待到位

        currentAngle = nextAngle; // 更新当前角度
        count++;
    }
    quick_range(PUSH_CLOSE_ANGLE);
    currentAngle = PUSH_CLOSE_ANGLE;
}

void Servo_Base::push_random_shake(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = push_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Push next angle: ");
        Debug_println(nextAngle);
        quick_range(nextAngle);  // 随机速度到达下一角度
        wait_inplace(nextAngle); // 等待到位

        shake(nextAngle, SHAKE_RANGE, SHAKE_TIMES); // 抖动

        currentAngle = nextAngle; // 更新当前角度
        count++;
    }
    push_poweropen(); // 蓄力打开
}

void Servo_Base::push_random_swing(uint8_t maxTries)
{
    int count = 0;
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t now_angle = get_current_angle();
    while (count < maxTries) // 当前角度不是开启角度且尝试次数小于最大值时循环
    {
        int nextAngle = push_random_nextangle(now_angle); // 获取一个随机的下一个角度
        Debug_print("Push next angle: ");
        Debug_println(nextAngle);
        quick_range(nextAngle);  // 随机速度到达下一角度
        wait_inplace(nextAngle); // 等待到位

        randomSeed(micros());
        bool QUICK_FLAG = random(0, 2); // 随机选择快速或慢速摆动
        if (QUICK_FLAG)
        {
            if (nextAngle != PUSH_TOUCH_ANGLE)
                swing(SWING_TIMES, nextAngle, PUSH_TOUCH_ANGLE); // 快速摇摆到触摸处
        }
        else
        {
            if (nextAngle != PUSH_TOUCH_ANGLE)
                swing(SWING_TIMES, nextAngle, PUSH_TOUCH_ANGLE, CHANGE_RANGE, SUPER_SLOW); // 慢速摇摆到触摸处
        }

        currentAngle = nextAngle; // 更新当前角度
        count++;
    }
    push_poweropen(); // 蓄力打开
}
