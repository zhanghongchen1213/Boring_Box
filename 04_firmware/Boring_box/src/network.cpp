#include "network.h"

// MQTT服务器
#define mqtt_server "www.zhcmqtt.top" // EMQX服务器的域名
#define publish_topic "ESP32_Box/pub" // 发布消息主题
// #define subscribe_topic_a "Wechat/sub" // 订阅消息主题
#define subscribe_topic_b "HA_Box/pub" // 订阅消息主题
String User_ID = "ESP32_Box";          // 用户ID，必须唯一

WiFiClient ESP32_Mqtt_Client;
PubSubClient ESP32_MQTT(ESP32_Mqtt_Client);

Network wifi;
MQTT mqtt;

void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        Debug_println("WiFi connected");
        Debug_println("IP address: ");
        Debug_println(WiFi.localIP());
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Debug_println("WiFi lost connection");
        break;
    default:
        break;
    }
}

/*连接WIFI*/
void Network::init(String ssid, String password)
{
    WiFi.onEvent(WiFiEvent);
    Debug_println("Connecting to WiFi network: " + String(ssid));
    WiFi.begin(ssid.c_str(), password.c_str());
};

/*打包数据发送*/
String MQTT::rootJson()
{
    StaticJsonDocument<192> doc;

    doc["Name"] = "ESP32_Box";
    doc["Push_close"] = PUSH_CLOSE_ANGLE;
    doc["Push_open"] = PUSH_OPEN_ANGLE;
    doc["lift_close"] = LIFT_CLOSE_ANGLE;
    doc["lift_open"] = LIFT_OPEN_ANGLE;
    String jsonCode;
    serializeJson(doc, jsonCode);
    return jsonCode;
};

void MQTT::callback(char *topic, byte *payload, unsigned int length)
{
    Debug_print("订阅主题: ");
    Debug_println(topic);
    String message;
    for (int i = 0; i < length; i++)
    {
        message = message + (char)payload[i]; // convert *byte to string
    }
    Debug_print("订阅主题消息内容: ");
    Debug_print(message);
    // 解析json数据
    parseInfo(message);
};

/*解析订阅主题信息*/
void MQTT::parseInfo(String message)
{
    // Stream& input;

    StaticJsonDocument<192> doc;

    DeserializationError error = deserializeJson(doc, message);

    if (error)
    {
        Debug_print("deserializeJson() failed: ");
        Debug_println(error.c_str());
        return;
    }
    const char *Name = doc["Name"];             // "HA_Box"
    const char *Push_close = doc["Push_close"]; // "Push_close"
    const char *Push_open = doc["Push_open"];   // "Push_open"
    const char *lift_close = doc["lift_close"]; // "lift_close"
    const char *lift_open = doc["lift_open"];   // "lift_open"

    /*更新全局变量*/
    PUSH_CLOSE_ANGLE = doc["Push_close"].as<uint8_t>();
    PUSH_OPEN_ANGLE = doc["Push_open"].as<uint8_t>();
    LIFT_CLOSE_ANGLE = doc["lift_close"].as<uint8_t>();
    LIFT_OPEN_ANGLE = doc["lift_open"].as<uint8_t>();
    Debug_println("更新全局变量: ");
    Debug_print("PUSH_CLOSE_ANGLE: ");
    Debug_println(PUSH_CLOSE_ANGLE);
    Debug_print("PUSH_OPEN_ANGLE: ");
    Debug_println(PUSH_OPEN_ANGLE);
    Debug_print("LIFT_CLOSE_ANGLE: ");
    Debug_println(LIFT_CLOSE_ANGLE);
    Debug_print("LIFT_OPEN_ANGLE: ");
    Debug_println(LIFT_OPEN_ANGLE);
    publish(); // 发布主题,更新状态
};

/*订阅主题*/
void MQTT::subscribe()
{
    // ESP32_MQTT.subscribe(subscribe_topic_a);
    ESP32_MQTT.subscribe(subscribe_topic_b);
};

/*连接EMQX服务器*/
void MQTT::connect()
{
    ESP32_MQTT.setServer(mqtt_server, 1883);
    attemptConnection();
}

/*向MQTT平台发布主题*/
void MQTT::publish()
{
    char jsonBuf[192];
    sprintf(jsonBuf, rootJson().c_str());
    ESP32_MQTT.publish(publish_topic, jsonBuf);
    Debug_println("发布主题: ");
    Debug_println(publish_topic);
    Debug_println("发布主题消息内容: ");
    Debug_println(jsonBuf);
};

void MQTT::loop()
{
    if (!ESP32_MQTT.connected())
    {
        connect();
    }
    ESP32_MQTT.loop();
};

void MQTT::attemptConnection()
{
    if (!ESP32_MQTT.connected())
    {
        Debug_println("Attempting MQTT connection...");
        if (ESP32_MQTT.connect(User_ID.c_str()))
        {
            Debug_println("MQTT connected");
            subscribe();
            ESP32_MQTT.setCallback(std::bind(&MQTT::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        }
        else
        {
            Debug_print("failed, rc=");
            Debug_print(ESP32_MQTT.state());
            Debug_println(" try again in 2 seconds");
            // Schedule the reconnection attempt using a FreeRTOS timer instead of delay
            const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;
            vTaskDelay(xDelay);  // This delay is acceptable since it yields to other tasks
            attemptConnection(); // Recursive call to try again
        }
    }
}
