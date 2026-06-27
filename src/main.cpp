#include <Arduino.h>
#include "ConfigManager.h"
#include "ConnectivityManager.h"
#include "MqttManager.h"
#include "NfcManager.h"
#include "WebServerManager.h"
#include "WebSerialLogger.h"

// Credenciais para o servidor web
const char* WEB_USERNAME = "admin";
const char* WEB_PASSWORD = "admin"; // Mude para uma senha forte em produção!

ConfigManager configMgr;
ConnectivityManager connMgr(configMgr);
MqttManager mqttMgr(configMgr, connMgr);
NfcManager nfcMgr;
WebServerManager webServerMgr(configMgr, WEB_USERNAME, WEB_PASSWORD);
WebSerialLogger webSerialLogger(&webServerMgr);

unsigned long lastStatusUpdate = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- nfcMqttManus Iniciando ---");

    // Redireciona a saída Serial para o WebSerialLogger
    Serial.setDebugOutput(false); // Desabilita a saída de debug padrão do ESP32 para não duplicar
    Serial.addPrint(webSerialLogger); // Adiciona nosso logger customizado

    if (!configMgr.begin() || !configMgr.loadConfig()) {
        Serial.println("Erro ao carregar configurações!");
    }

    connMgr.begin();
    mqttMgr.begin();
    webServerMgr.begin();
    
    if (!nfcMgr.begin()) {
        Serial.println("Erro ao iniciar NFC!");
    }
}

void loop() {
    // Mantém conectividade (WiFi ou GSM)
    connMgr.maintain();
    
    // Mantém conexão MQTT
    mqttMgr.loop();

    // Lida com requisições do servidor web e WebSockets
    webServerMgr.handleClient();

    // Scan de NFC
    String tagUid;
    if (nfcMgr.scan(tagUid)) {
        Serial.print("Tag detectada: ");
        Serial.println(tagUid);

        if (mqttMgr.isConnected()) {
            JsonDocument doc;
            doc["uid"] = tagUid;
            doc["interface"] = connMgr.getActiveInterface();
            doc["timestamp"] = millis();
            
            char buffer[128];
            serializeJson(doc, buffer);
            
            if (mqttMgr.publish(buffer)) {
                Serial.println("Dados publicados via MQTT!");
            } else {
                Serial.println("Falha ao publicar via MQTT.");
            }
        } else {
            Serial.println("MQTT não conectado. Tag ignorada.");
        }
        
        // Delay para evitar leituras duplicadas rápidas
        delay(2000);
    }

    // Log de status a cada 30 segundos
    if (millis() - lastStatusUpdate > 30000) {
        lastStatusUpdate = millis();
        Serial.print("Status -> Interface: ");
        Serial.print(connMgr.getActiveInterface());
        Serial.print(" | MQTT: ");
        Serial.println(mqttMgr.isConnected() ? "Conectado" : "Desconectado");
    }
}
