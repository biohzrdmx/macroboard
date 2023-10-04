#ifndef MAIN_h
#define MAIN_h

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <HID-Project.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <YetAnotherPcInt.h>
#include <RotaryEncoder.h>
#include "Timer.h"
#include "Ring.h"

struct Button {
    byte number;
    bool state;
    unsigned long last;
};

struct Action {
    String name;
    bool shift = false;
    bool alt = false;
    bool ctrl = false;
    bool consumer = false;
    uint8_t key;
};

struct Color {
    byte red = 0;
    byte green = 0;
    byte blue = 0;
};

struct Mode {
    String name;
    Color color;
    Action actions[6];
};

void wakeUp();
void sendButtonEvent(byte button);
void onSwitch1();
void onSwitch(Button* button);
void onRotate();

#endif