#ifndef CONNECTIVITY_MANAGER_H
#define CONNECTIVITY_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include "ConfigManager.h"

class ConnectivityManager {
public:
    ConnectivityManager(ConfigManager& configMgr);
    void begin();
    bool maintain();
    Client* getClient();
    bool isConnected();
    String getActiveInterface();

private:
    ConfigManager& _configMgr;
    HardwareSerial _serialGsm;
    TinyGsm _modem;
    TinyGsmClient _gsmClient;
    WiFiClient _wifiClient;
    
    bool _wifiConnected = false;
    bool _gsmConnected = false;
    
    bool connectWiFi();
    bool connectGSM();
};

#endif
