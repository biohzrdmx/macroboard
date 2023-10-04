#ifndef RING_h
#define RING_h

#include <Adafruit_NeoPixel.h>
#include "Tween.h"

enum RingMode {
  DISABLED,
  SOLID,
  SEGMENT,
  GLOW,
  BREATHE,
  SPIN_CW,
  SPIN_CCW,
  FLASH
};

class Ring {
  public:
    Ring();
    void begin(uint16_t pin, uint16_t leds);
    void setMode(RingMode mode);
    void setValue(byte value);
    void setColor(uint8_t red, uint8_t green, uint8_t blue);
    void setBrightness(uint8_t brightness);
    void update();
  private:
    Adafruit_NeoPixel _pixels;
    RingMode _mode;
    Tween _tween;
    byte _direction;
    byte _value;
    uint16_t _leds;
    uint8_t _red;
    uint8_t _green;
    uint8_t _blue;
    uint8_t _brightness;
};

#endif
