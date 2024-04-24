#ifndef __MP3_H
#define __MP3_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "debug.h"

#define MP3_EQ_NORMAL 0
#define MP3_EQ_POP 1
#define MP3_EQ_ROCK 2
#define MP3_EQ_JAZZ 3
#define MP3_EQ_CLASSIC 4
#define MP3_EQ_BASS 5

// Looping options, ALL, FOLDER, ONE and ONE_STOP are the
// only ones that appear to do much interesting
//  ALL plays all the tracks in a repeating loop
//  FOLDER plays all the tracks in the same folder in a repeating loop
//  ONE plays the same track repeating
//  ONE_STOP does not loop, plays the track and stops
//  RAM seems to play one track and someties disables the ability to
//  move to next/previous track, really weird.

#define MP3_LOOP_ALL 0
#define MP3_LOOP_FOLDER 1
#define MP3_LOOP_ONE 2
#define MP3_LOOP_RAM 3

#define MP3_STATUS_STOPPED 0
#define MP3_STATUS_PLAYING 1
#define MP3_STATUS_PAUSED 2
#define MP3_STATUS_FF 3
#define MP3_STATUS_FR 4
// The response from a status query we get is  for some reason
// a bit... iffy, most of the time it is reliable, but sometimes
// instead of a playing (1) response, we get a paused (2) response
// even though it is playing.  Stopped responses seem reliable.
// So to work around this when getStatus() is called we actually
// request the status this many times and only if one of them is STOPPED
// or they are all in agreement that it is playing or paused then
// we return that status.  If some of them differ, we do another set
// of tests etc...
#define MP3_STATUS_CHECKS_IN_AGREEMENT 4

#define MP3_DEBUG 1

class Mp3
{
private:
    unsigned int count = 0;
    /*1.发送一个命令到MP3模块并期待返回一个十六进制数字的响应，然后将这个响应转换成无符号整数*/
    unsigned int sendCommandWithUnsignedIntResponse(uint8_t command);
    /*2.等待直到串口有数据可读或者超时。它不断检查串口是否有数据，如果在指定的超时时间内没有数据可读，则停止等待。*/
    int waitUntilAvailable(unsigned long maxWaitTime = 1000);
    /*3.发送一个命令到MP3模块并带两个参数，然后将响应存储到一个缓冲区中*/
    void sendCommand(uint8_t command, uint8_t arg1, uint8_t arg2, char *responseBuffer, unsigned int bufferLength);

    void sendCommand(uint8_t command);                             // 发送一个命令到MP3模块
    void sendCommand(uint8_t command, uint8_t arg1);               // 发送一个命令到MP3模块并带一个参数
    void sendCommand(uint8_t command, uint8_t arg1, uint8_t arg2); // 发送一个命令到MP3模块并带两个参数

public:
    void init();
    void play();                                           // 播放
    void restart();                                        // 重新播放
    void pause();                                          // 暂停
    void next();                                           // 下一首
    void prev();                                           // 上一首
    void fastReverse();                                    // 快退
    void fastForward();                                    // 快进
    void playFileByIndexNumber(unsigned int fileNumber);   // 播放指定编号的文件
    void volumeUp();                                       // 音量+
    void volumeDn();                                       // 音量-
    void setVolume(uint8_t volumeFrom0To30);               // 设置音量 0-30
    void setEqualizer(uint8_t equalizerMode);              // 设置均衡器 0-5
    void setLoopMode(uint8_t loopMode);                    // 设置循环模式 00全盘循环  01文件夹循环  02单曲循环  03随机  04单曲播放
    void playFileNumber(uint8_t file, uint8_t fileNumber); // 播放指定文件夹中文件编号

    uint8_t getStatus();                   // 查询播放状态
    uint8_t getVolume();                   // 查询音量
    uint8_t getEqualizer();                // 查询均衡器
    uint8_t getLoopMode();                 // 查询循环模式
    unsigned int countFiles();             // 查询文件数量
    unsigned int currentFileIndexNumber(); // 查询当前文件编号

    void joke_play();   // 欺诈音效
    void random_play(); // 随机播放搞怪音乐

    void random_joke();
};

extern Mp3 mp3;
#endif