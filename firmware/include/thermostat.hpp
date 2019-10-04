#include <Arduino.h>
#include <cmath>

#include "relay.hpp"
#include "dht.hpp"

#ifndef THERMOSTAT_HPP
#define THERMOSTAT_HPP

enum struct thermostat_fan_mode {
    automatic,
    on
};

enum struct thermostat_mode {
    automatic,
    off
};

enum struct thermostat_output {
    idle,
    heat,
    cool
};

struct thermostat_state {

    thermostat_mode mode;
    thermostat_fan_mode fan_mode;
    thermostat_output output;
    float temperature_target;
    float temperature;
    float humidity;

    explicit thermostat_state(): 
         mode(thermostat_mode::automatic),
         fan_mode(thermostat_fan_mode::automatic),
         output(thermostat_output::idle),
         temperature_target(22.77778),   // 73F
         temperature(0.0),
         humidity(0.0) {}

    thermostat_state(thermostat_state const& rhs): 
        mode(rhs.mode),
        output(rhs.output),
        temperature_target(rhs.temperature_target),
        temperature(rhs.temperature),
        humidity(rhs.humidity) {}

};

struct thermostat_config {

    float poll_rate;
    float temperature_threshold;

    explicit thermostat_config():
        poll_rate(5000 /* 10 * 60 * 1000*/),            // 10 minutes
        temperature_threshold(0.5556f / 2.0f) {}        // 0.5 deg C

};

struct thermostat_controller {
    
    const int RELAY_FAN = 16;
    const int RELAY_HEAT = 4;
    const int RELAY_COOL = 5;

    thermostat_config config;
    thermostat_state state;
    timer command_timer;
    dht sensor;
    relay fan;
    relay heat;
    relay cool;

    thermostat_controller(): 
        config(),
        state(),
        command_timer(),
        sensor(),
        fan(RELAY_FAN),
        heat(RELAY_HEAT),
        cool(RELAY_COOL) {
            heat.close();
            cool.close();
            fan.close();
        }

    void
    set_mode(thermostat_mode mode) {
        state.mode = mode;
    }

    void 
    set_fan_mode(thermostat_fan_mode mode) {
        state.fan_mode = mode;
    }

    void 
    set_temperature_target(float target) {
        target = max(target, 15.555555556f);
        target = min(target, 29.444444444f);
        state.temperature_target = target;
    }

    void
    set_fan_output() {
        
    }

    void 
    set_output(thermostat_output output) {
        if (output != state.output) {
            switch (output) {
                case thermostat_output::idle:
                    heat.close();
                    cool.close();
                    fan.close();
                    break;
                case thermostat_output::cool:
                    cool.close();
                    fan.open();
                    heat.open();
                    break;
                case thermostat_output::heat:
                    heat.close();
                    cool.open();
                    fan.open();
                    break;
            }
            state.output = output;
        }
        if (state.fan_mode == thermostat_fan_mode::on) {
            fan.open();
        }
    }

    void
    update() noexcept {

        // update temperature/humidity readings
        auto report = sensor.report();
        state.temperature = report.temperature;
        state.humidity = report.humidity;

        // react if threshold reached
        float current = state.temperature;
        float target = state.temperature_target;
        float delta = abs(state.temperature_target - state.temperature);

        // Serial.printf("mode %d, temperature %f, humidity %f, delta %f, threshold %f\n",
        //     state.mode, state.temperature, state.humidity, 
        //     delta, config.temperature_threshold);

        if (command_timer.tick(config.poll_rate)) {

            if (state.mode == thermostat_mode::off) {
                set_output(thermostat_output::idle);
                return;
            }

            if (current > target && delta >= config.temperature_threshold) {
                set_output(thermostat_output::cool);
            }
            else if (current < target && delta >= config.temperature_threshold) {
                set_output(thermostat_output::heat);
            }
            else {
                set_output(thermostat_output::idle);
            }

        }

    }

};

#endif