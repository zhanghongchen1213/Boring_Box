#ifndef _NETWORK_H
#define _NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <functional>
#include "debug.h"
#include "SG90s.h"

class Network
{
public:
    void init(String ssid, String password); // 初始化网络
};

class MQTT
{
private:
    String rootJson();
    void parseInfo(String message);
    void subscribe();
    void callback(char *topic, byte *payload, unsigned int length);

public:
    void connect(); // 连接EMQX服务器
    void publish(); // 向MQTT平台发布主题
    void loop();
    void attemptConnection();
};

extern Network wifi;
extern MQTT mqtt;

#endif
