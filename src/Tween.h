#ifndef TWEEN_h
#define TWEEN_h

class Tween {
  public:
    Tween();
    long update();
    void restart();
    void init(long initial, long change, long duration);
    bool hasFinished();
  private:
    long _current;
    long _start;
    long _initial;
    long _change;
    long _duration;
    bool _finished;
};

#endif
