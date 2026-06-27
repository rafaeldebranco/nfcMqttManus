#ifndef NFC_MANAGER_H
#define NFC_MANAGER_H

#include <Arduino.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (19)
#define PN532_RESET (18)

class NfcManager {
public:
    NfcManager();
    bool begin();
    bool scan(String& uidStr);

private:
    Adafruit_PN532 _nfc;
};

#endif
