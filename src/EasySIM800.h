#ifndef AzSIM800_h
#define AzSIM800_h

#include <Arduino.h>

#define AZ_BUF_SIZE 128          // размер буфера под общение с модемом

enum AzNetStatus {               // статусы сети
    AZ_NOT_REGISTERED = 0,
    AZ_REGISTERED_HOME = 1,
    AZ_SEARCHING = 2,
    AZ_DENIED = 3,
    AZ_UNKNOWN = 4,
    AZ_ROAMING = 5
};

class AzSIM800 {
public:
    AzSIM800(Stream* serial);

    bool begin();
    
    void tick();                        // тикер для "ассинхроных функций"

    void setResetPin(int8_t pin);
    void hardReset();                   // жесткая перезагрузка с помощью RST pin

    int8_t getSignal();                 // получить уровень сети
    AzNetStatus getNetwork();           // статус подключения

private:
    Stream* _serial;
    int8_t _rstPin = -1;
    char _buffer[AZ_BUF_SIZE];
    
    bool waitResponse(const char* expected, uint32_t timeout);
    void clearBuffer();
};

#endif