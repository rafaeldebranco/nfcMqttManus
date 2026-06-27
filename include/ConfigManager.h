#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

struct Config {
    String wifi_ssid;
    String wifi_pass;
    String gsm_apn;
    String gsm_user;
    String gsm_pass;
    String mqtt_server;
    int mqtt_port;
    String mqtt_user;
    String mqtt_pass;
    String mqtt_topic;
};

class ConfigManager {
public:
    ConfigManager();
    bool begin();
    bool loadConfig();
    bool saveConfig(const Config& config);
    Config& getConfig() { return _config; }

private:
    Config _config;
    const char* _filename = "/config.json";
};

#endif
