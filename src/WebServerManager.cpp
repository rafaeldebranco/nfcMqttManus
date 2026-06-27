#include "WebServerManager.h"
#include <LittleFS.h>

// Variável global para acessar o WebServerManager no callback do WebSocket
WebServerManager* globalWebServerManager = nullptr;

WebServerManager::WebServerManager(ConfigManager& configManager, const char* username, const char* password)
    : _configManager(configManager),
      _server(80),
      _webSocket(81),
      _http_username(username),
      _http_password(password)
{
    globalWebServerManager = this;
}

void WebServerManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Falha ao montar LittleFS para WebServer");
        return;
    }

    setupRoutes();
    _webSocket.begin();
    _webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        this->onWebSocketEvent(num, type, payload, length);
    });
    _server.begin();
    Serial.println("Servidor Web iniciado na porta 80");
    Serial.println("WebSocket iniciado na porta 81");
}

void WebServerManager::handleClient() {
    _webSocket.loop();
}

void WebServerManager::sendSerialData(const String& data) {
    _webSocket.broadcastTXT(data);
}

bool WebServerManager::authenticate(AsyncWebServerRequest *request) {
    if (request->authenticate(_http_username, _http_password)) {
        return true;
    }
    request->requestAuthentication();
    return false;
}

void WebServerManager::setupRoutes() {
    // Rota para a página de login
    _server.on("/login", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/login.html", "text/html");
    });

    // Rota para autenticação (POST)
    _server.on("/login", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (request->hasParam("username", true) && request->hasParam("password", true)) {
            if (request->arg("username") == _http_username && request->arg("password") == _http_password) {
                request->send(200, "text/plain", "Login successful"); // Em um cenário real, você definiria um cookie de sessão
                return;
            }
        }
        request->send(401, "text/plain", "Unauthorized");
    });

    // Rota raiz - redireciona para login se não autenticado
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!authenticate(request)) return;
        request->send(LittleFS, "/index.html", "text/html");
    });

    // Rota para a página de status/terminal
    _server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!authenticate(request)) return;
        request->send(LittleFS, "/status.html", "text/html");
    });

    // Rota para a página de configuração
    _server.on("/config", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!authenticate(request)) return;
        request->send(LittleFS, "/config.html", "text/html");
    });

    // Rota para obter as configurações atuais (JSON)
    _server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!authenticate(request)) return;
        JsonDocument doc;
        Config& cfg = _configManager.getConfig();
        doc["wifi_ssid"] = cfg.wifi_ssid;
        doc["wifi_pass"] = cfg.wifi_pass;
        doc["gsm_apn"] = cfg.gsm_apn;
        doc["gsm_user"] = cfg.gsm_user;
        doc["gsm_pass"] = cfg.gsm_pass;
        doc["mqtt_server"] = cfg.mqtt_server;
        doc["mqtt_port"] = cfg.mqtt_port;
        doc["mqtt_user"] = cfg.mqtt_user;
        doc["mqtt_pass"] = cfg.mqtt_pass;
        doc["mqtt_topic"] = cfg.mqtt_topic;
        doc["mqtt_status_topic"] = cfg.mqtt_status_topic;
        doc["mqtt_status_online_msg"] = cfg.mqtt_status_online_msg;
        doc["mqtt_status_offline_msg"] = cfg.mqtt_status_offline_msg;

        String jsonResponse;
        serializeJson(doc, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    });

    // Rota para salvar as configurações (POST)
    _server.on("/api/saveConfig", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (!authenticate(request)) return;
        if (request->hasParam("plain", true)) {
            String jsonBody = request->arg("plain");
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, jsonBody);

            if (error) {
                request->send(400, "text/plain", "JSON inválido");
                return;
            }

            Config newConfig = _configManager.getConfig(); // Começa com a config atual
            newConfig.wifi_ssid = doc["wifi_ssid"] | newConfig.wifi_ssid;
            newConfig.wifi_pass = doc["wifi_pass"] | newConfig.wifi_pass;
            newConfig.gsm_apn = doc["gsm_apn"] | newConfig.gsm_apn;
            newConfig.gsm_user = doc["gsm_user"] | newConfig.gsm_user;
            newConfig.gsm_pass = doc["gsm_pass"] | newConfig.gsm_pass;
            newConfig.mqtt_server = doc["mqtt_server"] | newConfig.mqtt_server;
            newConfig.mqtt_port = doc["mqtt_port"] | newConfig.mqtt_port;
            newConfig.mqtt_user = doc["mqtt_user"] | newConfig.mqtt_user;
            newConfig.mqtt_pass = doc["mqtt_pass"] | newConfig.mqtt_pass;
            newConfig.mqtt_topic = doc["mqtt_topic"] | newConfig.mqtt_topic;
            newConfig.mqtt_status_topic = doc["mqtt_status_topic"] | newConfig.mqtt_status_topic;
            newConfig.mqtt_status_online_msg = doc["mqtt_status_online_msg"] | newConfig.mqtt_status_online_msg;
            newConfig.mqtt_status_offline_msg = doc["mqtt_status_offline_msg"] | newConfig.mqtt_status_offline_msg;

            if (_configManager.saveConfig(newConfig)) {
                request->send(200, "text/plain", "Configurações salvas com sucesso! Reinicie o ESP32 para aplicar.");
            } else {
                request->send(500, "text/plain", "Falha ao salvar configurações.");
            }
        } else {
            request->send(400, "text/plain", "Corpo da requisição inválido.");
        }
    });

    // Servir arquivos estáticos do LittleFS
    _server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
}

void WebServerManager::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Desconectado!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = _webSocket.remoteIP(num);
                Serial.printf("[%u] Conectado de %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                // Enviar algumas mensagens de log recentes se houver
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] Recebido: %s\n", num, payload);
            // Aqui você pode adicionar lógica para processar comandos recebidos via WebSocket
            break;
        case WStype_BIN:
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}
