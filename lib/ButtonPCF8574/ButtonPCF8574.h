#pragma once
#include <Arduino.h>
#include <Wire.h>

class ButtonPCF8574
{
private:
    bool pins[8] = {false, false, false, false, false, false, false, false};
    bool leds[8] = {false, false, false, false, false, false, false, false};

    int cont = 0;
    int addressBot;
    int addressLed;
    byte ledbuffer = B00000000;

    /* data */
public:
    bool enabledLeds = false;
    bool enabledBots = false;
    ButtonPCF8574();
    ButtonPCF8574(int address, int id);
    ButtonPCF8574(int addressBot, int id, int addressLed, int cont);
    int read();
    void lightup(int id);
    void turnoff(int id);

    ~ButtonPCF8574();
};
