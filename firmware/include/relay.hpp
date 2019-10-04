#include <Arduino.h>

struct relay {

    explicit relay(int pin): _pin(pin) {
        pinMode(pin, OUTPUT);
        close();
    }

    void
    open() {
        if (digitalRead(_pin) != LOW) {
            digitalWrite(_pin, LOW);
            delay(100);
        }
    }

    void 
    close() {
        if (digitalRead(_pin) != HIGH) {
            digitalWrite(_pin, HIGH);
            delay(100);
        }
    }

private:

    int _pin;
    
};