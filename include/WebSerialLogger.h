#ifndef WEBSERIAL_LOGGER_H
#define WEBSERIAL_LOGGER_H

#include <Arduino.h>
#include "WebServerManager.h"

class WebSerialLogger : public Print {
public:
    WebSerialLogger(WebServerManager* webServerManager) : _webServerManager(webServerManager) {}

    size_t write(uint8_t c) override {
        _buffer += (char)c;
        if (c == '\n') {
            Serial.print(_buffer);
            if (_webServerManager) {
                _webServerManager->sendSerialData(_buffer);
            }
            _buffer = "";
        }
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) override {
        for (size_t i = 0; i < size; i++) {
            write(buffer[i]);
        }
        return size;
    }

private:
    WebServerManager* _webServerManager;
    String _buffer;
};

#endif
