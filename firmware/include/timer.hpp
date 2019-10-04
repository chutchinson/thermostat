#include <Arduino.h>

#ifndef THERMOSTAT_TIMER_HPP
#define THERMOSTAT_TIMER_HPP

struct timer {

    explicit timer(): 
        last(millis()), 
        time(0) { }

    void
    reset() noexcept {
        last = millis();
        time = 0;
    }

    bool
    tick(int rate) noexcept {
        auto ts = millis();
        auto delta = ts - last;
        last = ts;
        time += delta;
        if (time >= rate) {
            time = 0;
            return true;
        }
        return false;
    }

private:

    int last;
    int time;

};

#endif