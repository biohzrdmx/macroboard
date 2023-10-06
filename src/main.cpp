#include "main.h"

#define PIN_ENC_DT   15
#define PIN_ENC_CLK  14
#define PIN_ENC_SW   16

#define PIN_NEOPIXEL 6

#define PIN_BTN_1    7
#define PIN_BTN_2    8
#define PIN_BTN_3    9
#define PIN_BTN_4    10

#define NUM_MODES    3

Button buttons[] = {
  { PIN_ENC_SW, false },
  { PIN_BTN_1,  false },
  { PIN_BTN_2,  false },
  { PIN_BTN_3,  false },
  { PIN_BTN_4,  false }
};

Mode modes[NUM_MODES];

Adafruit_SSD1306 display(128, 64, &Wire);
RotaryEncoder encoder(PIN_ENC_CLK, PIN_ENC_DT);
Timer timerSleep;
Timer timerBlackout;
Ring ring;

bool sleeping;
bool blackout;
bool redraw;
int mode;
int action;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(PIN_ENC_CLK, INPUT_PULLUP);
  pinMode(PIN_ENC_DT, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);
  pinMode(PIN_BTN_3, INPUT_PULLUP);
  pinMode(PIN_BTN_4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(7), onSwitch1, RISING);
  PcInt::attachInterrupt(PIN_BTN_2, onSwitch, &buttons[2], RISING);
  PcInt::attachInterrupt(PIN_BTN_3, onSwitch, &buttons[3], RISING);
  PcInt::attachInterrupt(PIN_BTN_4, onSwitch, &buttons[4], RISING);
  PcInt::attachInterrupt(PIN_ENC_SW, onSwitch, &buttons[0], RISING);
  PcInt::attachInterrupt(PIN_ENC_CLK, onRotate, CHANGE);
  PcInt::attachInterrupt(PIN_ENC_DT, onRotate, CHANGE);
  ring.begin(PIN_NEOPIXEL, 8);
  ring.setMode(RingMode::BREATHE);
  timerSleep.init(3000);
  timerBlackout.init(180000);
  Keyboard.begin();
  Consumer.begin();
  //
  modes[0].name = F("Multimedia");
  modes[0].color.red = 255;
  modes[0].color.green = 0;
  modes[0].color.blue = 0;
  modes[0].actions[1].name = F("Play");
  modes[0].actions[1].key = MEDIA_PLAY_PAUSE;
  modes[0].actions[1].consumer = true;
  modes[0].actions[3].name = F("Mute");
  modes[0].actions[3].key = MEDIA_VOL_MUTE;
  modes[0].actions[3].consumer = true;
  modes[0].actions[0].name = F("Prev track");
  modes[0].actions[0].key = MEDIA_PREVIOUS;
  modes[0].actions[0].consumer = true;
  modes[0].actions[2].name = F("Next track");
  modes[0].actions[2].key = MEDIA_NEXT;
  modes[0].actions[2].consumer = true;
  modes[0].actions[4].name = F("Volume down");
  modes[0].actions[4].key = MEDIA_VOL_DOWN;
  modes[0].actions[4].consumer = true;
  modes[0].actions[5].name = F("Volume up");
  modes[0].actions[5].key = MEDIA_VOL_UP;
  modes[0].actions[5].consumer = true;
  //
  modes[1].name = F("Text editor");
  modes[1].color.red = 0;
  modes[1].color.green = 0;
  modes[1].color.blue = 255;
  modes[1].actions[1].name = F("Cut");
  modes[1].actions[1].key = KEY_X;
  modes[1].actions[1].ctrl = true;
  modes[1].actions[3].name = F("Copy");
  modes[1].actions[3].key = KEY_C;
  modes[1].actions[3].ctrl = true;
  modes[1].actions[0].name = F("Paste");
  modes[1].actions[0].key = KEY_V;
  modes[1].actions[0].ctrl = true; 
  modes[1].actions[2].name = F("Save");
  modes[1].actions[2].key = KEY_S;
  modes[1].actions[2].ctrl = true;
  modes[1].actions[4].name = F("Page up");
  modes[1].actions[4].key = KEY_PAGE_UP;
  modes[1].actions[5].name = F("Page down");
  modes[1].actions[5].key = KEY_PAGE_DOWN;
  //
  modes[2].name = F("Affinity");
  modes[2].color.red = 255;
  modes[2].color.green = 0;
  modes[2].color.blue = 255;
  modes[2].actions[1].name = F("Undo");
  modes[2].actions[1].key = KEY_Z;
  modes[2].actions[1].ctrl = true;
  modes[2].actions[3].name = F("Redo");
  modes[2].actions[3].key = KEY_Y;
  modes[2].actions[3].ctrl = true;
  modes[2].actions[0].name = F("Toggle context");
  modes[2].actions[0].key = KEY_X;
  modes[2].actions[2].name = F("Swap colors");
  modes[2].actions[2].key = KEY_X;
  modes[2].actions[2].shift = true;
  modes[2].actions[4].name = F("Zoom in");
  modes[2].actions[4].key = KEYPAD_SUBTRACT;
  modes[2].actions[4].ctrl = true;
  modes[2].actions[5].name = F("Zoom out");
  modes[2].actions[5].key = KEYPAD_ADD;
  modes[2].actions[5].ctrl = true;
  //
  mode = 0;
  sleeping = false;
  blackout = false;
  redraw = false;
  //
  display.clearDisplay();
  display.display();
}

void onSwitch1() {
  unsigned long now = millis();
  if (now - buttons[1].last > 500) {
    buttons[1].last = now;
    buttons[1].state = true;
    wakeUp();
  }
}

void onSwitch(Button* button) {
  unsigned long now = millis();
  if (now - button->last > 500) {
    button->last = now;
    button->state = true;
    wakeUp();
  }
}

void onRotate() {
  encoder.tick();
  wakeUp();
}

void handleInputs() {
  for (int i = 0; i < 5; i++) {
    Button* button = &buttons[i];
    if ( button->state ) {
      if (button->number == PIN_ENC_SW) {
        mode += 1;
        if (mode >= NUM_MODES) {
          mode = 0;
        }
        action = -1;
      } else {
        sendButtonEvent((byte)i - 1);
        action = i - 1;
      }
      button->state = false;
      ring.setMode(RingMode::FLASH);
      redraw = true;
    }
  }
  RotaryEncoder::Direction dir = encoder.getDirection();
  if (dir == RotaryEncoder::Direction::CLOCKWISE) {
    sendButtonEvent(5);
    action = 5;
    redraw = true;
    ring.setMode(RingMode::SPIN_CW);
  } else if (dir == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
    sendButtonEvent(4);
    action = 4;
    redraw = true;
    ring.setMode(RingMode::SPIN_CCW);
  }
}

void updateRing() {
  ring.setBrightness(96);
  ring.setColor(modes[mode].color.red, modes[mode].color.green, modes[mode].color.blue);
  ring.update();
}

void drawCenteredString(const String &buf, int x, int y)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);
  display.setCursor((x - w) / 2, (y - h) / 2);
  display.print(buf);
}

void updateScreen() {
  if (redraw) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    if (! sleeping ) {
      display.drawRoundRect(2, 2, 126, 56, 4, SSD1306_WHITE);
      if (action == -1) {
        drawCenteredString(modes[mode].name, display.width(), display.height() - 4);
      } else {
        drawCenteredString(modes[mode].actions[action].name, display.width(), display.height() - 4);
      }
    }
    display.display();
    redraw = false;
  }
}

void wakeUp() {
  sleeping = false;
  blackout = false;
  timerSleep.restart();
  timerBlackout.restart();
  ring.setBrightness(96);
}

void sendButtonEvent(byte button) {
  if (button >= 0 && button <= 6) {
    if (modes[mode].actions[button].consumer) {
      Consumer.press((ConsumerKeycode)modes[mode].actions[button].key);
      Consumer.releaseAll();
    } else {
      if ( modes[mode].actions[button].alt ) {
        Keyboard.press(KEY_LEFT_ALT);
      }
      if ( modes[mode].actions[button].ctrl ) {
        Keyboard.press(KEY_LEFT_CTRL);
      }
      if ( modes[mode].actions[button].shift ) {
        Keyboard.press(KEY_LEFT_SHIFT);
      }
      Keyboard.press((KeyboardKeycode)modes[mode].actions[button].key );
      Keyboard.releaseAll();
    }
  }
}

void loop() {
  timerSleep.update();
  timerBlackout.update();
  handleInputs();
  updateScreen();
  updateRing();
  if ( timerSleep.hasFinished() && !sleeping ) {
    sleeping = true;
    redraw = true;
    ring.setMode(RingMode::BREATHE);
  }
  if ( timerBlackout.hasFinished() && !blackout ) {
    blackout = true;
    ring.setMode(RingMode::SOLID);
    ring.setBrightness(32);
  }
}