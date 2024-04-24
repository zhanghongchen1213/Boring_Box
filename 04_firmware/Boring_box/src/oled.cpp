#include "oled.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);

OLED oled;

void OLED::drawbitmap(const uint8_t *bitmap)
{
    u8g2.setFontDirection(0);
    u8g2.firstPage();
    do
    {
        // 清除图像左侧和右侧的区域
        u8g2.setDrawColor(0);        // 设置绘制颜色为黑
        u8g2.drawBox(0, 0, 32, 64);  // 清除左边32列像素
        u8g2.drawBox(96, 0, 32, 64); // 清除右边32列像素

        // 在中间64x64区域绘制和刷新图像
        u8g2.setDrawColor(1);                // 设置绘制颜色为白
        u8g2.drawXBM(32, 0, 64, 64, bitmap); // 在屏幕中心绘制图像
    } while (u8g2.nextPage());
}

void OLED::init()
{
    u8g2.begin();
    u8g2.enableUTF8Print();                     // enable UTF8 support for the Arduino print() function
    u8g2.setFont(u8g2_font_unifont_t_chinese2); // use chinese2 for all the glyphs of "你好世界"
}

void OLED::random_emoji()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    int index = random(1, 30);
    drawbitmap(emoji[index]);
}

void OLED::displayAnimation(const uint8_t *bitmaps[], unsigned int count, unsigned int delayTime)
{
    for (unsigned int i = 0; i < count; i++)
    {
        drawbitmap(bitmaps[i]);
        vTaskDelay(pdMS_TO_TICKS(delayTime));
    }
}

void OLED::power_on_gif()
{
    displayAnimation(bitmaps, 22, 5);
}

void OLED::random_gif()
{
    randomSeed(micros()); // 使用当前时间的微秒部分更新随机种子
    int index = random(0, 26);
    GIF &selectedGif = gifs[index];
    for (uint8_t i = 0; i < Play_gif_times; i++)
        displayAnimation(selectedGif.frames, selectedGif.frameCount, 30);
}