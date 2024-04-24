#ifndef __MODE_H
#define __MODE_H

#include "SG90s.h"

#define COMMON_PROB 60     // "Common" - Most frequent events (50% probability)
#define OCCASIONAL_PROB 75 // "Occasional" - Less frequent, happens sometimes (20% probability)
#define UNCOMMON_PROB 85   // "Uncommon" - Rarely happening events (15% probability)
#define RARE_PROB 95       // "Rare" - Very infrequent events (10% probability)


class Mode
{
public:
    void random_nine_open();
    void random_four_close();
    void joke_open_close();
    void lickdog_open_close();
    void hit_open();      // 砸动作
    void caress_open();   // 抚摸
    void grumpy_open();   // 暴躁
    void Grausens_open(); // 鬼畜
    void random_open();
};

extern Mode mode;
#endif