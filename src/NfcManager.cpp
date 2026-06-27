#include "NfcManager.h"

NfcManager::NfcManager() : _nfc(PN532_IRQ, PN532_RESET) {}

bool NfcManager::begin() {
    _nfc.begin();
    uint32_t versiondata = _nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("Não encontrei a placa PN53x");
        return false;
    }
    
    _nfc.SAMConfig();
    Serial.println("PN532 pronto para leitura.");
    return true;
}

bool NfcManager::scan(String& uidStr) {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    // Leitura não bloqueante (timeout curto)
    success = _nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

    if (success) {
        uidStr = "";
        for (uint8_t i = 0; i < uidLength; i++) {
            if (uid[i] < 0x10) uidStr += "0";
            uidStr += String(uid[i], HEX);
            if (i < uidLength - 1) uidStr += ":";
        }
        uidStr.toUpperCase();
        return true;
    }
    return false;
}
