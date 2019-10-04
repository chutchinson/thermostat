#include <DHT.h>
#include "timer.hpp"

#ifndef THERMOSTAT_DHT_HPP
#define THERMOSTAT_DHT_HPP

const int DHT_PIN = 13;

struct dht_report {
    float temperature;
    float humidity;
};

struct dht {

    dht() {
        sensor.setup(DHT_PIN, DHT::DHT22);
    }

    dht_report
    report() noexcept {
        auto sampling_period = sensor.getMinimumSamplingPeriod() * 2;
        if (sample_timer.tick(sampling_period)) {
            last_report.temperature = sensor.getTemperature();
            last_report.humidity = sensor.getHumidity();
            // Serial.printf("[dht] report (%f, %f)\n", last_report.temperature, last_report.humidity);
        }
        return last_report;
    }

private:

    DHT sensor;
    dht_report last_report;
    timer sample_timer;

};

#endif