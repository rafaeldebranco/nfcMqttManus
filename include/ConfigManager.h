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
    String mqtt_status_topic; // Novo campo para o tópico de status
    String mqtt_status_online_msg; // Novo campo para mensagem online
    String mqtt_status_offline_msg; // Novo campo para mensagem offline
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
