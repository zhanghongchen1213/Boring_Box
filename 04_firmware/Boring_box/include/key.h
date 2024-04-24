#ifndef __KEY_H
#define __KEY_H

#include <Arduino.h>

#define KEY_PIN 13

class Key
{
public:
    void init();
    bool read();
};


extern Key key;

#endif