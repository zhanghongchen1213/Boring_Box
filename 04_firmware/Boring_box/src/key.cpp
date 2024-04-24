#include "key.h"

Key key;


void Key::init()
{
    pinMode(KEY_PIN, INPUT);
}

bool Key::read()
{
    return digitalRead(KEY_PIN);
}