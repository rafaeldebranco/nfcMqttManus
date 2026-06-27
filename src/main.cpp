#include <Arduino.h>
#include "ConfigManager.h"
#include "ConnectivityManager.h"
#include "MqttManager.h"
#include "NfcManager.h"

ConfigManager configMgr;
ConnectivityManager connMgr(configMgr);
MqttManager mqttMgr(configMgr, connMgr);
NfcManager nfcMgr;

unsigned long lastStatusUpdate = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- nfcMqttManus Iniciando ---");

    if (!configMgr.begin() || !configMgr.loadConfig()) {
        Serial.println("Erro ao carregar configurações!");
    }

    connMgr.begin();
    mqttMgr.begin();
    
    if (!nfcMgr.begin()) {
        Serial.println("Erro ao iniciar NFC!");
    }
}

void loop() {
    // Mantém conectividade (WiFi ou GSM)
    connMgr.maintain();
    
    // Mantém conexão MQTT
    mqttMgr.loop();

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
