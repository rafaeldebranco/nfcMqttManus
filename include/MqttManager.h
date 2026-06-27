#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "ConfigManager.h"
#include "ConnectivityManager.h"

class MqttManager {
public:
    MqttManager(ConfigManager& configMgr, ConnectivityManager& connMgr);
    void begin();
    void loop();
    bool publish(const char* payload);
    bool isConnected();

private:
    ConfigManager& _configMgr;
    ConnectivityManager& _connMgr;
    PubSubClient _mqttClient;
    unsigned long _lastReconnectAttempt = 0;

    bool reconnect();
};

#endif
