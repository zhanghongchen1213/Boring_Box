#include "System_Interaction.h"
#include "debug.h"

TaskHandle_t Servos_Open_Handle;
TaskHandle_t Servos_Close_Handle;
TaskHandle_t OLED_Handle;
TaskHandle_t Mp3_Handle;
TaskHandle_t Key_Handle;
TaskHandle_t LickDog_Handle;

void Servos_Open_Task(void *pvParameters);
void Servos_Close_Task(void *pvParameters);
void OLED_Task(void *pvParameters);
void Mp3_Task(void *pvParameters);
void Key_Task(void *pvParameters);
void LickDog_Task(void *pvParameters);

void Service_Init(void)
{
    /*核心0任务*/
    xTaskCreatePinnedToCore(OLED_Task, "OLED_Task", 1024 * 10, NULL, 2, &OLED_Handle, 0);

    xTaskCreatePinnedToCore(Mp3_Task, "Mp3_Task", 1024, NULL, 1, &Mp3_Handle, 0);

    xTaskCreatePinnedToCore(Key_Task, "Key_Task", 1024, NULL, 1, &Key_Handle, 0);

    /*核心1任务*/
    xTaskCreatePinnedToCore(Servos_Open_Task, "Servos_Open_Task", 1024 * 5, NULL, 1, &Servos_Open_Handle, 1);

    xTaskCreatePinnedToCore(Servos_Close_Task, "Servos_Close_Task", 1024 * 5, NULL, 1, &Servos_Close_Handle, 1);

    xTaskCreatePinnedToCore(LickDog_Task, "LickDog_Task", 1024 * 10, NULL, 1, &LickDog_Handle, 1);
}

void OLED_Task(void *pvParameters)
{
    OLED_Handle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        Debug_println("OLED_Task");
        oled.random_gif();
        vTaskDelay(pdMS_TO_TICKS(10)); // 轻微延迟可以给I2C通讯留出处理时间
    }
}

void Mp3_Task(void *pvParameters)
{
    Mp3_Handle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        Debug_println("Mp3_Task");
        // mp3.random_play();
        mp3.random_play();
    }
}

void Key_Task(void *pvParameters)
{
    TickType_t _xTicksToWait = pdMS_TO_TICKS(50); // 50ms
    bool lastButtonState = LOW;                   // 默认没有按下
    uint32_t idleTime = 0;
    while (1)
    {
        bool currentButtonState = key.read();
        // 检测从高电平到低电平的变化（按钮按下）
        if (lastButtonState == LOW && currentButtonState == HIGH)
        {
            idleTime = 0;
            xTaskNotifyGive(Mp3_Handle);
            xTaskNotifyGive(Servos_Open_Handle);
        }
        else if (lastButtonState == HIGH && currentButtonState == LOW)
        {
            xTaskNotifyGive(OLED_Handle);
            xTaskNotifyGive(Servos_Close_Handle);
        }
        else
        {
            idleTime += _xTicksToWait;
        }
        if (idleTime >= pdMS_TO_TICKS(IDLE_TIMEOUT_MS)) // 定时超过10s执行舔狗任务
        {
            xTaskNotifyGive(LickDog_Handle); // 通知舔狗任务
            Debug_println("Idle timeout, notifying tasks.");
            idleTime = 0;
        }
        // 更新按钮状态
        lastButtonState = currentButtonState;
        vTaskDelay(_xTicksToWait);
    }
}

void Servos_Open_Task(void *pvParameters)
{
    Servos_Open_Handle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        Debug_println("Servos_Open_Task");
        mode.random_open(); // 随机打开任务
    }
}

void Servos_Close_Task(void *pvParameters)
{
    Servos_Close_Handle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        Debug_println("Servos_Close_Task");
        mode.random_four_close(); // 随机关闭任务
    }
}

void LickDog_Task(void *pvParameters)
{
    LickDog_Handle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        Debug_println("LickDog_Task");
        // mp3播放指定音频
        oled.random_emoji();
        mode.lickdog_open_close();
    }
}