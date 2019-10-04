#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "thermostat.hpp"

const int port = 8888;

const char* ssid = THERMOSTAT_WIFI_SSID;
const char* password = THERMOSTAT_WIFI_PASSWORD;

WiFiUDP udp;
thermostat_controller thermostat;

void
setup() {

    pinMode(0, OUTPUT);

    Serial.begin(115200);
    Serial.println();
    Serial.print("[wifi] connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(25);
        Serial.print(".");
    }

    Serial.println();
    Serial.printf("[wifi] connected to %s\n", 
        WiFi.localIP().toString().c_str());

    udp.begin(port);

}

void
network_read(thermostat_controller& thermostat) {

    char buffer[1024];

    auto packet_size = udp.parsePacket();

    if (packet_size > 0) {

        Serial.printf("[wifi] received %d bytes from %s",
            packet_size, udp.remoteIP().toString().c_str());
        Serial.println();

        auto len = udp.read(buffer, packet_size);
        buffer[len] = '\0';
        
        Serial.printf("received <%s>\n", buffer);

        DynamicJsonDocument request(1024);
        DynamicJsonDocument response(1024);

        auto error = deserializeJson(request, buffer);

        if (error != error.Ok) {
            Serial.println("[wifi] error deserializing request");
            return;
        }

        const char* cmd = request["cmd"];

        response["cmd"] = cmd;
        response["ts"] = millis();

        if (strcmp("report", cmd) == 0) {

            auto data = response.createNestedObject("data");

            switch (thermostat.state.output) {
                case thermostat_output::idle:
                    data["output"] = "idle";
                    break;
                case thermostat_output::cool:
                    data["output"] = "cool";
                    break;
                case thermostat_output::heat:
                    data["output"] = "heat";
                    break;
            }

            switch (thermostat.state.mode) {
                case thermostat_mode::automatic:
                    data["mode"] = "automatic";
                    break;
                case thermostat_mode::off:
                    data["mode"] = "off";
                    break;
            }

            switch (thermostat.state.fan_mode) {
                case thermostat_fan_mode::automatic:
                    data["fan_mode"] = "automatic";
                    break;
                case thermostat_fan_mode::on:
                    data["fan_mode"] = "on";
                    break;
            }
            
            data["temperatureTarget"] = thermostat.state.temperature_target;
            data["temperature"] = thermostat.state.temperature;
            data["humidity"] = thermostat.state.humidity;

        }
        else if (strcmp("set_temperature_target", cmd) == 0) {
            auto target = request["target"].as<float>();
            thermostat.set_temperature_target(target);
            // do nothing
        }
        else if (strcmp("set_fan_mode", cmd) == 0) {
            const char* mode = request["fan_mode"];
            if (strcmp(mode, "automatic") == 0) {
                thermostat.set_fan_mode(thermostat_fan_mode::automatic);
            }
            else if (strcmp(mode, "on") == 0) {
                thermostat.set_fan_mode(thermostat_fan_mode::on);
            }
        }
        else if (strcmp("set_mode", cmd) == 0) {
            const char* mode = request["mode"];
            if (strcmp(mode, "automatic") == 0) {
                thermostat.set_mode(thermostat_mode::automatic);
            }
            if (strcmp(mode, "off") == 0) {
                thermostat.set_mode(thermostat_mode::off);
            }
        }
        else {
            response["error"] = "unrecognized command";
        }

        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        serializeJson(response, udp);
        udp.endPacket();
    }

}

void
blink() {
    digitalWrite(0, HIGH);
    delay(1000);
    digitalWrite(0, LOW);
    delay(100);
}

void
loop() {
    // blink();
    network_read(thermostat);
    thermostat.update();
}