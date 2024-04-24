#include "Mp3.h"

Mp3 mp3;

unsigned int Mp3::sendCommandWithUnsignedIntResponse(uint8_t command)
{
    char buffer[6];
    memset(buffer, 0, sizeof(buffer)); // 初始化buffer，防止未定义的读取
    sendCommand(command, 0, 0, buffer, sizeof(buffer));
    Debug_print("Received buffer: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    } // 打印接收到的数据

    if (buffer[0] == 0xAA && buffer[2] == command)
    {
        // 提取数据
        unsigned int value = (buffer[3] << 8) | buffer[4];
        Debug_print("Received value: ");
        Debug_println(value);
        return value;
    }
    else
    {
        Debug_println("Invalid response format");
        return 0; // 表示格式错误
    }
}

int Mp3::waitUntilAvailable(unsigned long maxWaitTime)
{
    unsigned long startTime;
    int c = 0;
    startTime = millis();
    do
    {
        c = Serial2.available();
        if (c)
            break;
    } while (millis() - startTime < maxWaitTime);
    return c;
}

void Mp3::sendCommand(uint8_t command, uint8_t arg1, uint8_t arg2, char *responseBuffer, unsigned int bufferLength)
{
    uint8_t args = (arg1 != 0) ? 1 : 0; // 判断arg1是否作为参数
    if ((command == 0x41) || (command == 0x42))
    {
        args = 2;
    }

    while (waitUntilAvailable(10))
    {
        Serial2.read();
    }

#if MP3_DEBUG
    char buf[4];
    Serial.println();
    Serial.print("7E ");
    itoa(2 + args, buf, 16);
    Serial.print(buf);
    Serial.print(" ");
    memset(buf, 0, sizeof(buf));
    itoa(command, buf, 16);
    Serial.print(buf);
    Serial.print(" ");
    memset(buf, 0, sizeof(buf));
    if (args >= 1)
        itoa(arg1, buf, 16);
    Serial.print(buf);
    Serial.print(" ");
    memset(buf, 0, sizeof(buf));
    if (args >= 2)
        itoa(arg2, buf, 16);
    Serial.print(buf);
    Serial.print(" ");
    memset(buf, 0, sizeof(buf));
    Serial.print("EF");
#endif

    Serial2.write(0x7E);
    Serial2.write(2 + args); // 计算命令长度
    Serial2.write(command);
    if (args >= 1)
        Serial2.write(arg1);
    if (args == 2)
        Serial2.write(arg2);
    Serial2.write(0xEF);
    vTaskDelay(pdMS_TO_TICKS(10));

    if (responseBuffer && bufferLength)
    {
        memset(responseBuffer, 0, bufferLength);
    }
    waitUntilAvailable(1000);
#if MP3_DEBUG
    Serial.print(" ==> [");
#endif

    unsigned int i = 0;
    char j = 0;
    while (waitUntilAvailable(150))
    {
        j = (char)Serial2.read();

#if MP3_DEBUG
        Serial.print("J:");
        Serial.print(j);
        Serial.print(" ");
#endif
        if (responseBuffer && (i < (bufferLength - 1)))
        {
            responseBuffer[i++] = j;
        }
    }

#if MP3_DEBUG
    Serial.print("]");
    Serial.println();
#endif

#if 0
    // 处理响应
    if (responseBuffer != nullptr && bufferLength > 0)
    {
        unsigned int idx = 0;
        memset(responseBuffer, 0, bufferLength); // 清空缓冲区
        unsigned long startTime = millis();
        while (millis() - startTime < 1000 && idx < bufferLength - 1)
        {
            if (Serial2.available())
            {
                responseBuffer[idx++] = Serial2.read();
            }
        }
        responseBuffer[idx] = '\0'; // 确保字符串正确终止
    }
#endif
}

void Mp3::sendCommand(uint8_t command)
{
    this->sendCommand(command, 0, 0, 0, 0);
}

void Mp3::sendCommand(uint8_t command, uint8_t arg1)
{
    this->sendCommand(command, arg1, 0, 0, 0);
}

void Mp3::sendCommand(uint8_t command, uint8_t arg1, uint8_t arg2)
{
    this->sendCommand(command, arg1, arg2, 0, 0);
}

/*************************************************************************/

void Mp3::init()
{
    // Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX: GPIO16, TX: GPIO17
    Serial2.begin(9600); // RX: GPIO16, TX: GPIO17
    setVolume(20);       // 设置音量
    uint8_t volume = getVolume();
    Debug_println("Volume: " + String(volume));
    setLoopMode(4);       // 设置单曲播放
    count = countFiles(); // 获取当前文件数量
    Debug_println("MP3 file count: " + String(count));
}

void Mp3::play()
{
    sendCommand(0x01);
}

void Mp3::restart()
{
    byte oldVolume = getVolume();
    setVolume(0);
    next();
    pause();
    setVolume(oldVolume);
    prev();
}

void Mp3::pause()
{
    sendCommand(0x02);
}

void Mp3::next()
{
    sendCommand(0x03);
}

void Mp3::prev()
{
    sendCommand(0x04);
}
void Mp3::fastReverse()
{
    sendCommand(0x0B);
}

void Mp3::fastForward()
{
    sendCommand(0x0A);
}

void Mp3::playFileByIndexNumber(unsigned int fileNumber)
{
    sendCommand(0x41, (fileNumber >> 8) & 0xFF, fileNumber & (byte)0xFF);
}

void Mp3::volumeUp()
{
    sendCommand(0x05);
}

void Mp3::volumeDn()
{
    sendCommand(0x06);
}

void Mp3::setVolume(uint8_t volumeFrom0To30)
{
    sendCommand(0x31, volumeFrom0To30);
}

void Mp3::setEqualizer(uint8_t equalizerMode)
{
    sendCommand(0x32, equalizerMode);
}

void Mp3::setLoopMode(uint8_t loopMode)
{
    sendCommand(0x33, loopMode);
}

void Mp3::playFileNumber(uint8_t file, uint8_t fileNumber)
{
    // 将file转换为16进制
    sendCommand(0x42, file, fileNumber);
}

uint8_t Mp3::getStatus()
{
    uint8_t statTotal = 0;
    uint8_t stat = 0;
    do
    {
        statTotal = 0;
        for (uint8_t x = 0; x < MP3_STATUS_CHECKS_IN_AGREEMENT; x++)
        {
            stat = this->sendCommandWithUnsignedIntResponse(0x42);
            if (stat == 0)
                return 0; // STOP is fairly reliable
            statTotal += stat;
        }

    } while (statTotal != 1 * MP3_STATUS_CHECKS_IN_AGREEMENT && statTotal != 2 * MP3_STATUS_CHECKS_IN_AGREEMENT);

    return statTotal / MP3_STATUS_CHECKS_IN_AGREEMENT;
}

uint8_t Mp3::getVolume() { return sendCommandWithUnsignedIntResponse(0x11); }
uint8_t Mp3::getEqualizer() { return sendCommandWithUnsignedIntResponse(0x44); }
uint8_t Mp3::getLoopMode() { return sendCommandWithUnsignedIntResponse(0x13); }

unsigned int Mp3::countFiles()
{
    return sendCommandWithUnsignedIntResponse(0x14);
}

unsigned int Mp3::currentFileIndexNumber()
{
    return sendCommandWithUnsignedIntResponse(0x1A);
}

void Mp3::joke_play()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    uint8_t random_count = random(1, 8);
    Debug_println("Random count: ");
    Debug_println(random_count);
    playFileNumber(2, random_count);
}

void Mp3::random_play()
{
    randomSeed(micros());             // 使用当前时间的微秒部分更新随机种子
    static int action_count = 0;      // 静态变量，用于计数特定动作的执行频率
    static int interval = 3;          // 每3次执行一次特定动作
    if (action_count % interval == 0) // 随机次数添加joke任务
    {
        joke_play();               // 特殊音效
        interval = random(10, 15); // 重新设置随机次数
    }
    else
    {
        randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
        uint8_t random_count = random(1, 83);
        Debug_println("Random count: ");
        Debug_println(random_count);
        playFileNumber(3, random_count); // 日常音效
    }
    action_count++;
}

void Mp3::random_joke()
{
    playFileNumber(2, 2);
}