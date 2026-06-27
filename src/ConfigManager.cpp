#include "ConfigManager.h"

ConfigManager::ConfigManager() {}

bool ConfigManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Falha ao montar LittleFS");
        return false;
    }
    return true;
}

bool ConfigManager::loadConfig() {
    File file = LittleFS.open(_filename, "r");
    if (!file) {
        Serial.println("Falha ao abrir arquivo de config");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Falha ao ler JSON");
        return false;
    }

    _config.wifi_ssid = doc["wifi_ssid"] | "";
    _config.wifi_pass = doc["wifi_pass"] | "";
    _config.gsm_apn = doc["gsm_apn"] | "";
    _config.gsm_user = doc["gsm_user"] | "";
    _config.gsm_pass = doc["gsm_pass"] | "";
    _config.mqtt_server = doc["mqtt_server"] | "broker.hivemq.com";
    _config.mqtt_port = doc["mqtt_port"] | 1883;
    _config.mqtt_user = doc["mqtt_user"] | "";
    _config.mqtt_pass = doc["mqtt_pass"] | "";
    _config.mqtt_topic = doc["mqtt_topic"] | "manus/nfc/readings";
    _config.mqtt_status_topic = doc["mqtt_status_topic"] | "manus/nfc/status"; // Novo campo
    _config.mqtt_status_online_msg = doc["mqtt_status_online_msg"] | "online"; // Novo campo
    _config.mqtt_status_offline_msg = doc["mqtt_status_offline_msg"] | "offline"; // Novo campo

    return true;
}

bool ConfigManager::saveConfig(const Config& config) {
    File file = LittleFS.open(_filename, "w");
    if (!file) return false;

    JsonDocument doc;
    doc["wifi_ssid"] = config.wifi_ssid;
    doc["wifi_pass"] = config.wifi_pass;
    doc["gsm_apn"] = config.gsm_apn;
    doc["gsm_user"] = config.gsm_user;
    doc["gsm_pass"] = config.gsm_pass;
    doc["mqtt_server"] = config.mqtt_server;
    doc["mqtt_port"] = config.mqtt_port;
    doc["mqtt_user"] = config.mqtt_user;
    doc["mqtt_pass"] = config.mqtt_pass;
    doc["mqtt_topic"] = config.mqtt_topic;
    doc["mqtt_status_topic"] = config.mqtt_status_topic; // Novo campo
    doc["mqtt_status_online_msg"] = config.mqtt_status_online_msg; // Novo campo
    doc["mqtt_status_offline_msg"] = config.mqtt_status_offline_msg; // Novo campo

    if (serializeJson(doc, file) == 0) {
        Serial.println("Falha ao escrever no arquivo");
        return false;
    }
    file.close();
    _config = config;
    return true;
}
