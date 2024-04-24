#include "System_Config.h"

TaskHandle_t POWERON_Handle;

void System_Resource_Init(void)
{
    Serial.begin(9600);
    oled.init(); // 初始化OLED
    mp3.init();
    xTaskCreatePinnedToCore(POWERON_Task, "POWERON_Task", 1024 * 5, NULL, 2, &POWERON_Handle, 0);
    /*硬件资源初始化*/
    key.init();
    pushservo.init(PUSH_PIN, PUSH_CLOSE_ANGLE); // PWM引脚和初始位置
    liftservo.init(LIFT_PIN, LIFT_CLOSE_ANGLE);

    /*网络资源初始化*/
    wifi.init("ZHC_Web", "580231580231"); // 初始化网络
    mqtt.connect();                       // 连接EMQX服务器

    /*随机数种子*/
    unsigned long seed = 0;
    for (int i = 0; i < 10; i++)
    {
        seed ^= esp_random();          // XOR 所有生成的随机数来获得最终的种子
        vTaskDelay(pdMS_TO_TICKS(10)); // 短暂延时
    }
    randomSeed(seed);
    Debug_println("Random seed: " + String(seed));
    Debug_print("Resource init successful!\n");

    /*启动系统任务*/
    System_Tasks_Init();

    xTaskNotifyGive(POWERON_Handle);
}

void System_Tasks_Init(void)
{
    Service_Init(); // 任务初始化
}

void POWERON_Task(void *pvParameters)
{
    mp3.playFileNumber(1, 1);
    // 等待通知，没有通知就持续循环
    while (1)
    {
        // 在没有收到通知前持续播放GIF动画
        while (!ulTaskNotifyTake(pdFALSE, 0)) // 检查是否有通知，非阻塞调用
        {
            oled.power_on_gif();
        }
        // 如果接收到通知，则跳出外部循环
        break;
    }
    // 收到通知，显示一次随机静态画面
    oled.random_emoji();
    mp3.playFileNumber(1, 2);
    // 任务完成后删除自己
    vTaskDelete(NULL);
}
