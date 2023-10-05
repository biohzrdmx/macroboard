#include <Arduino.h>
#include "Ring.h"

Ring::Ring() {
  _mode = RingMode::SOLID;
  _red = 255;
  _green = 0;
  _blue = 0;
  _brightness = 255;
  _value = 100;
}

void Ring::begin(uint16_t pin, uint16_t leds) {
  _leds = leds;
  _pixels.setPin(pin);
  _pixels.updateLength(leds);
  _pixels.begin();
}

void Ring::setMode(RingMode mode) {
  if (_mode != mode) {
    _mode = mode;
    switch (_mode) {
      case RingMode::DISABLED:
      case RingMode::GLOW:
      case RingMode::SEGMENT:
      case RingMode::SOLID:
        // Do nothing
      break;
      case RingMode::BREATHE:
        _tween.init(0, 100, 2000);
      break;
      case RingMode::SPIN_CW:
        _tween.init(1, 100, 750);
        break;
      case RingMode::SPIN_CCW:
        _tween.init(100, -100, 750);
      break;
      case RingMode::FLASH:
        _tween.init(1, 10, 1500);
      break;
    }
  }
}

void Ring::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  _red = red;
  _green = green;
  _blue = blue;
}

void Ring::setBrightness(uint8_t brightness) {
  _brightness = brightness;
}

void Ring::setValue(byte value) {
  _value = value;
}

void Ring::update() {
  int pos = 0;
  long delta = 0;
  _pixels.clear();
  switch (_mode) {
    case RingMode::DISABLED:
      // Do nothing
    break;
    case RingMode::SOLID:
      // Solid color mode
      _pixels.setBrightness(_brightness);
      for (uint16_t i = 0; i < _leds; i++) {
        _pixels.setPixelColor(i, _red, _green, _blue);
      }
    break;
    case RingMode::SEGMENT:
      // Segments mode, value determines how many segments are lit
      _pixels.setBrightness(_brightness);
      pos = (_value * _leds) / 100;
      for (uint16_t i = 0; i < pos; i++) {
        _pixels.setPixelColor(i, _red, _green, _blue);
      }
    break;
    case RingMode::GLOW:
      // Glow mode, value determines how bright the ring is
      pos = (_value * _brightness) / 100;
      _pixels.setBrightness(pos);
      for (uint16_t i = 0; i < _leds; i++) {
        _pixels.setPixelColor(i, _red, _green, _blue);
      }
    break;
    case RingMode::BREATHE:
      // Breathe mode, ring pulses for a 'breathing' effect
      delta = _tween.update();
      switch (_direction) {
        case 0:
          pos = (delta * _brightness) / 100;
        break; 
        case 1:
          pos = _brightness - ((delta * _brightness) / 100);
        break; 
      }
      _pixels.setBrightness(pos);
      for (uint16_t i = 0; i < _leds; i++) {
        _pixels.setPixelColor(i, _red, _green, _blue);
      }
      if (_tween.hasFinished()) {
        _direction = _direction == 0 ? 1 : 0;
        _tween.restart();
      }
    break;
    case RingMode::SPIN_CW:
    case RingMode::SPIN_CCW:
      // Spin modes, either clockwise or counter-clockwise
      delta = _tween.update();
      pos = (delta * _leds) / 100;
      _pixels.setBrightness(_brightness);
      _pixels.setPixelColor(pos, _red, _green, _blue);
      if (_tween.hasFinished()) {
        _tween.restart();
      }
    break;
    case RingMode::FLASH:
      // Flash mode, flash a solid ring of light
      delta = _tween.update();
      _pixels.setBrightness(_brightness);
      if (delta % 2 == 0) {
        for (uint16_t i = 0; i < _leds; i++) {
          _pixels.setPixelColor(i, _red, _green, _blue);
        }
      }
      if (_tween.hasFinished()) {
        _tween.restart();
      }
    break;
  }
  _pixels.show();
}
