# nfcMqttManus

Projeto profissional para ESP32 utilizando **PlatformIO** e framework **Arduino**, seguindo padrões de **Programação Orientada a Objetos (POO)**.

## Funcionalidades
- **Gerenciamento de Configurações**: Armazenamento de credenciais em JSON no sistema de arquivos **LittleFS**.
- **Conectividade Híbrida**: Alternância automática entre **WiFi** e **GSM (GPRS)** usando a biblioteca TinyGSM.
- **Protocolo MQTT**: Publicação de leituras via PubSubClient, incluindo **Last Will and Testament (LWT)** para monitoramento de status online/offline.
- **Leitura NFC**: Integração com o sensor **Adafruit PN532**.

## Estrutura de Classes
- `ConfigManager`: Gerencia a montagem do LittleFS e a carga/escrita do arquivo `config.json`.
- `ConnectivityManager`: Lida com a lógica de conexão WiFi e failover para GSM (SIM800L).
- `MqttManager`: Gerencia a conexão com o broker, publicação de mensagens e o status LWT.
- `NfcManager`: Encapsula a lógica de detecção e leitura de tags NFC.

## Pinagem Recomendada (ESP32)
| Componente | Pino ESP32 | Função |
|------------|------------|--------|
| **SIM800L** | GPIO 16 | RX |
| **SIM800L** | GPIO 17 | TX |
| **PN532**   | GPIO 19 | IRQ |
| **PN532**   | GPIO 18 | RESET |
| **PN532**   | I2C SDA/SCL| Padrão (GPIO 21/22) |

## Como usar
1. Edite o arquivo `data/config.json` com suas credenciais e os tópicos de status MQTT.
2. Use o comando `Upload Filesystem Image` no PlatformIO para gravar as configurações no LittleFS.
3. Faça o `Upload` do código para o ESP32.

---
Desenvolvido por Manus.
