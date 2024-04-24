#ifndef __DEBUG_H
#define __DEBUG_H

// 调试开关

#define DEBUG_TEST

#ifdef DEBUG_TEST
#define Debug_print(str) Serial.print(str)
#define Debug_println(str) Serial.println(str)
#else
#define Debug_print(str)
#define Debug_println(str)
#endif

#endif