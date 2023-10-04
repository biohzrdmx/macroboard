#include <Arduino.h>
#include "Tween.h"

Tween::Tween() {
  _finished = true;
}

void Tween::init(long initial, long change, long duration) {
    _start = millis();
    _current = initial;
    _initial = initial;
    _change = change;
    _duration = duration;
    _finished = false;
}

long Tween::update() {
  long elapsed;
  long delta;
  if (_duration) {
    elapsed = millis() - _start;
    delta = (elapsed * 100) / _duration;
    if (!_finished) {
      if (delta <= 100) {
        _current = _initial + (_change * delta / 100);
      } else {
        _finished = true;
      }
    }
  }
  return _current;
}

void Tween::restart() {
  _start = millis();
  _finished = false;
}

bool Tween::hasFinished() {
  return _finished;
}
