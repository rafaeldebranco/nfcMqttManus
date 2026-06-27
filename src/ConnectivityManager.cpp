#include "ConnectivityManager.h"

ConnectivityManager::ConnectivityManager(ConfigManager& configMgr) 
    : _configMgr(configMgr), _serialGsm(2), _modem(_serialGsm), _gsmClient(_modem) {}

void ConnectivityManager::begin() {
    _serialGsm.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17 (Padrão SIM800L no ESP32)
    delay(3000);
    Serial.println("Iniciando modem GSM...");
    if (!_modem.restart()) {
        Serial.println("Falha ao reiniciar modem GSM");
    }
}

bool ConnectivityManager::maintain() {
    if (WiFi.status() == WL_CONNECTED) {
        _wifiConnected = true;
        return true;
    }

    if (connectWiFi()) {
        _wifiConnected = true;
        return true;
    }

    _wifiConnected = false;
    
    if (_modem.isGprsConnected()) {
        _gsmConnected = true;
        return true;
    }

    if (connectGSM()) {
        _gsmConnected = true;
        return true;
    }

    _gsmConnected = false;
    return false;
}

bool ConnectivityManager::connectWiFi() {
    Config& cfg = _configMgr.getConfig();
    if (cfg.wifi_ssid == "") return false;

    Serial.print("Conectando ao WiFi: ");
    Serial.println(cfg.wifi_ssid);
    
    WiFi.begin(cfg.wifi_ssid.c_str(), cfg.wifi_pass.c_str());
    
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        delay(500);
        Serial.print(".");
        attempt++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Conectado!");
        return true;
    }
    
    Serial.println("\nFalha no WiFi.");
    return false;
}

bool ConnectivityManager::connectGSM() {
    Config& cfg = _configMgr.getConfig();
    Serial.println("Tentando conexão GSM/GPRS...");
    
    if (!_modem.waitForNetwork()) {
        Serial.println("Falha ao registrar na rede celular");
        return false;
    }

    if (!_modem.gprsConnect(cfg.gsm_apn.c_str(), cfg.gsm_user.c_str(), cfg.gsm_pass.c_str())) {
        Serial.println("Falha ao conectar GPRS");
        return false;
    }

    Serial.println("GSM Conectado!");
    return true;
}

Client* ConnectivityManager::getClient() {
    if (WiFi.status() == WL_CONNECTED) return &_wifiClient;
    return &_gsmClient;
}

bool ConnectivityManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED) || _modem.isGprsConnected();
}

String ConnectivityManager::getActiveInterface() {
    if (WiFi.status() == WL_CONNECTED) return "WiFi";
    if (_modem.isGprsConnected()) return "GSM";
    return "Nenhuma";
}
