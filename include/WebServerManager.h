#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "ConfigManager.h"

class WebServerManager {
public:
    WebServerManager(ConfigManager& configManager, const char* username, const char* password);
    void begin();
    void handleClient();
    void sendSerialData(const String& data);

private:
    ConfigManager& _configManager;
    AsyncWebServer _server;
    WebSocketsServer _webSocket;
    const char* _http_username;
    const char* _http_password;

    bool authenticate(AsyncWebServerRequest *request);
    void setupRoutes();
    void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
};

#endif
