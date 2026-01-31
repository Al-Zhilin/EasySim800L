#include "AzSIM800.h"

AzSIM800::AzSIM800(Stream* serial) : _serial(serial) {}

void AzSIM800::setResetPin(int8_t pin) {
    _rstPin = pin;
    if (_rstPin != -1) pinMode(_rstPin, OUTPUT);
}

// Функция начала работы (проверка связь + отключаем ЭХО)
bool AzSIM800::begin() {
    // Вместо delay используем короткие циклы с ожиданием
    uint8_t retry = 5;
    while (retry--) {
        _serial->println(F("AT"));
        if (waitResponse("OK", 500)) {
            _serial->println(F("ATE0")); // Выключаем эхо для чистоты буфера
            waitResponse("OK", 500);
            return true;
        }
    }
    return false;
}

// Механизм ожидания ответа
bool AzSIM800::waitResponse(const char* expected, uint32_t timeout) {
    uint32_t timer = millis();
    uint16_t index = 0;
    memset(_buffer, 0, AZ_BUF_SIZE);

    while (millis() - timer < timeout) {
        if (_serial->available()) {
            char c = _serial->read();
            if (index < AZ_BUF_SIZE - 1) {
                _buffer[index++] = c;
            }
            if (strstr(_buffer, expected)) return true;
        }
    }
    return false;
}

// Получение сигнала: парсим "+CSQ: XX,YY"
int8_t AzSIM800::getSignal() {
    _serial->println(F("AT+CSQ"));
    if (waitResponse("+CSQ:", 500)) {
        char* ptr = strstr(_buffer, "+CSQ:");
        if (ptr) return (int8_t)atoi(ptr + 5);
    }
    return -1;
}

// Получение статуса сети: парсим "+CREG: X,Y"
AzNetStatus AzSIM800::getNetwork() {
    _serial->println(F("AT+CREG?"));
    if (waitResponse("+CREG:", 500)) {
        char* ptr = strchr(_buffer, ','); // Ищем первую запятую
        if (ptr) return (AzNetStatus)atoi(ptr + 1);
    }
    return AZ_UNKNOWN;
}