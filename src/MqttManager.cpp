#include "MqttManager.h"

MqttManager::MqttManager(ConfigManager& configMgr, ConnectivityManager& connMgr)
    : _configMgr(configMgr), _connMgr(connMgr) {}

void MqttManager::begin() {
    // O cliente será configurado dinamicamente no loop conforme a interface ativa
}

void MqttManager::loop() {
    if (!_connMgr.isConnected()) return;

    _mqttClient.setClient(*_connMgr.getClient());
    _mqttClient.setServer(_configMgr.getConfig().mqtt_server.c_str(), _configMgr.getConfig().mqtt_port);

    if (!_mqttClient.connected()) {
        unsigned long now = millis();
        if (now - _lastReconnectAttempt > 5000) {
            _lastReconnectAttempt = now;
            if (reconnect()) {
                _lastReconnectAttempt = 0;
            }
        }
    } else {
        _mqttClient.loop();
    }
}

bool MqttManager::reconnect() {
    Config& cfg = _configMgr.getConfig();
    String clientId = "ESP32NfcManus-" + String(random(0xffff), HEX);
    
    Serial.print("Tentando conexão MQTT...");
    
    bool connected = false;
    if (cfg.mqtt_user.length() > 0) {
        connected = _mqttClient.connect(clientId.c_str(), cfg.mqtt_user.c_str(), cfg.mqtt_pass.c_str(), 
                                        cfg.mqtt_status_topic.c_str(), 0, true, cfg.mqtt_status_offline_msg.c_str());
    } else {
        connected = _mqttClient.connect(clientId.c_str(), 
                                        cfg.mqtt_status_topic.c_str(), 0, true, cfg.mqtt_status_offline_msg.c_str());
    }

    if (connected) {
        Serial.println("conectado");
        publishStatus(cfg.mqtt_status_online_msg.c_str()); // Publica status online ao conectar
    } else {
        Serial.print("falhou, rc=");
        Serial.println(_mqttClient.state());
    }
    return connected;
}

bool MqttManager::publish(const char* payload) {
    if (!_mqttClient.connected()) return false;
    return _mqttClient.publish(_configMgr.getConfig().mqtt_topic.c_str(), payload);
}

void MqttManager::publishStatus(const char* statusMessage) {
    if (!_mqttClient.connected()) return;
    _mqttClient.publish(_configMgr.getConfig().mqtt_status_topic.c_str(), statusMessage, true);
}

bool MqttManager::isConnected() {
    return _mqttClient.connected();
}
