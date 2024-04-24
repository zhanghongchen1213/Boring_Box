#include "mode.h"
Mode mode;
void Mode::random_nine_open()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    int lift_mode = random(0, 3);
    switch (lift_mode)
    {
    case 0:
        liftservo.lift_quick_open();
        Debug_println("Mode: lift_quick_open");
        break;
    case 1:
        liftservo.lift_slow_open();
        Debug_println("Mode: lift_slow_open");
        break;
    case 2:
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        uint8_t BORING_TIMES = random(1, 5);
        liftservo.lift_random_open(BORING_TIMES);
        Debug_println("Mode: lift_random_open");
        break;
    }
    if (liftservo.get_current_angle() == LIFT_OPEN_ANGLE)
    {
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        int push_mode = random(0, 3);
        switch (push_mode)
        {
        case 0:
            pushservo.push_quick_open();
            Debug_println("Mode: push_quick_open");
            break;
        case 1:
            pushservo.push_slow_open();
            Debug_println("Mode: push_quick_open");
            break;
        case 2:
            randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
            uint8_t BORING_TIMES = random(1, 5);
            pushservo.push_random_open(BORING_TIMES);
            Debug_println("Mode: push_random_open");
            break;
        }
    }
}

void Mode::random_four_close()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    int push_mode = random(0, 2);
    switch (push_mode)
    {
    case 0:
        pushservo.push_quick_close();
        Debug_println("Mode: push_quick_close");
        break;
    case 1:
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        uint8_t BORING_TIMES = random(1, 4);
        pushservo.push_random_close(BORING_TIMES);
        Debug_println("Mode: push_random_close");
        break;
    }
    if (pushservo.get_current_angle() == PUSH_CLOSE_ANGLE)
    {
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        int lift_mode = random(0, 2);
        switch (lift_mode)
        {
        case 0:
            liftservo.lift_quick_close();
            Debug_println("lift_quick_close");
            break;
        case 1:
            randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
            uint8_t BORING_TIMES = random(1, 3);
            liftservo.lift_random_close(BORING_TIMES);
            Debug_println("lift_random_close");
            break;
        }
    }
}

void Mode::joke_open_close()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    int lift_mode = random(0, 2);
    switch (lift_mode)
    {
    case 0:
        liftservo.lift_quick_open();
        break;
    case 1:
        liftservo.lift_slow_open();
        break;
    }
    if (liftservo.get_current_angle() == LIFT_OPEN_ANGLE)
    {
        vTaskDelay(pdMS_TO_TICKS(JOKE_TIME));
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        int lift_mode = random(0, 2);
        switch (lift_mode)
        {
        case 0:
            liftservo.lift_quick_close();
            break;
        case 1:
            liftservo.lift_slow_close();
            break;
        }
    }
}

void Mode::lickdog_open_close()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    int lift_mode = random(0, 2);
    switch (lift_mode)
    {
    case 0:
        liftservo.lift_quick_open();
        break;
    case 1:
        liftservo.lift_slow_open();
        break;
    }
    if (liftservo.get_current_angle() == LIFT_OPEN_ANGLE)
    {
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        int push_mode = random(0, 2);
        switch (push_mode)
        {
        case 0:
            pushservo.push_quick_touch();

            break;
        case 1:
            pushservo.push_slow_touch();
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        random_four_close();
    }
}

void Mode::hit_open()
{
    liftservo.lift_quick_open();  // 开盖
    pushservo.push_quick_touch(); // 触摸
    liftservo.lift_heavyhit();    // 重砸
    pushservo.push_poweropen();   // 蓄力打开
}

void Mode::caress_open()
{
    liftservo.lift_quick_open();  // 开盖
    pushservo.push_quick_touch(); // 触摸
    liftservo.lift_caress();      // 重砸
    pushservo.push_poweropen();   // 蓄力打开
}

void Mode::grumpy_open()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t BORING_SWING_TIMES = random(1, 3);
    liftservo.lift_random_swing(BORING_SWING_TIMES);
    if (liftservo.get_current_angle() == LIFT_OPEN_ANGLE)
    {
        pushservo.push_random_swing(BORING_SWING_TIMES);
    }
}

void Mode::Grausens_open()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t BORING_SHAKE_TIMES = random(1, 3);
    liftservo.lift_random_shake(BORING_SHAKE_TIMES);
    if (liftservo.get_current_angle() == LIFT_OPEN_ANGLE)
    {
        pushservo.push_random_shake(BORING_SHAKE_TIMES);
    }
}

void Mode::random_open()
{
    randomSeed(micros());             // 使用当前时间的微秒部分更新随机种子
    static int action_count = 0;      // 静态变量，用于计数特定动作的执行频率
    static int interval = 3;          // 每3次执行一次特定动作
    if (action_count % interval == 0) // 随机次数添加joke任务
    {
        joke_open_close();
        vTaskDelay(pdMS_TO_TICKS(100));
        interval = random(4, 9);
    }
    // 概率模式A、B、C
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t taskDecision = random(0, 100);
    if (taskDecision < COMMON_PROB)
    {
        random_nine_open();
    }
    else if (taskDecision < OCCASIONAL_PROB)
    {
        grumpy_open();
    }
    else if (taskDecision < UNCOMMON_PROB)
    {
        Grausens_open();
    }
    else if (taskDecision < RARE_PROB)
    {
        hit_open();
    }
    else
    {
        caress_open();
    }
    action_count++;
}
