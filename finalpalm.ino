#include "LUFAConfig.h"
#include <LUFA.h>
#include "Joystick.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <Adafruit_NeoPixel.h>

#define MILLIDEBOUNCE 1 //Debounce time in milliseconds

bool buttonStartBefore;
bool buttonSelectBefore;
byte buttonStatus[16];

#define DPAD_UP_MASK_ON 0x00
#define DPAD_UPRIGHT_MASK_ON 0x01
#define DPAD_RIGHT_MASK_ON 0x02
#define DPAD_DOWNRIGHT_MASK_ON 0x03
#define DPAD_DOWN_MASK_ON 0x04
#define DPAD_DOWNLEFT_MASK_ON 0x05
#define DPAD_LEFT_MASK_ON 0x06
#define DPAD_UPLEFT_MASK_ON 0x07
#define DPAD_NOTHING_MASK_ON 0x08
#define A_MASK_ON 0x04
#define B_MASK_ON 0x02
#define X_MASK_ON 0x08
#define Y_MASK_ON 0x01
#define LB_MASK_ON 0x10
#define RB_MASK_ON 0x20
#define ZL_MASK_ON 0x40
#define ZR_MASK_ON 0x80
#define START_MASK_ON 0x200
#define SELECT_MASK_ON 0x100
#define HOME_MASK_ON 0x1000

#define BUTTONUP 0
#define BUTTONDOWN 1
#define BUTTONLEFT 2
#define BUTTONRIGHT 3

#define BUTTONRUP 4
#define BUTTONRDOWN 5
#define BUTTONRLEFT 6
#define BUTTONRRIGHT 7

#define BUTTONA 8
#define BUTTONB 9
#define BUTTONX 10
#define BUTTONY 11
#define BUTTONLB 12
#define BUTTONRB 13
#define BUTTONSTART 14
#define BUTTONHOME 15

// Pin Mapping
#define LED_RING1_PIN 0
#define LED_RING2_PIN 1

#define PIN_LUP 6
#define PIN_LDOWN 7
#define PIN_LLEFT 9
#define PIN_LRIGHT 8

#define PIN_RUP 14
#define PIN_RDOWN 15
#define PIN_RLEFT 16
#define PIN_RRIGHT 10

#define PIN_A 18
#define PIN_B 19
#define PIN_X 4
#define PIN_Y 5

#define PIN_L 2
#define PIN_R 20

#define PIN_PLUS 3
#define PIN_HOME 21

// NeoPixel Setup
#define NUM_LEDS 16
Adafruit_NeoPixel ring1(NUM_LEDS, LED_RING1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring2(NUM_LEDS, LED_RING2_PIN, NEO_GRB + NEO_KHZ800);

Bounce joystickUP = Bounce();
Bounce joystickDOWN = Bounce();
Bounce joystickLEFT = Bounce();
Bounce joystickRIGHT = Bounce();

Bounce joystickRUP = Bounce();
Bounce joystickRDOWN = Bounce();
Bounce joystickRLEFT = Bounce();
Bounce joystickRRIGHT = Bounce();

Bounce buttonA = Bounce();
Bounce buttonB = Bounce();
Bounce buttonX = Bounce();
Bounce buttonY = Bounce();

Bounce buttonLB = Bounce();
Bounce buttonRB = Bounce();

Bounce buttonSTART = Bounce();
Bounce buttonHOME = Bounce();

float breathPhase = 0.0;
const float breathSpeed = 0.002;

uint32_t colorDim(Adafruit_NeoPixel &strip, uint32_t color, float factor) {
  uint8_t r = (uint8_t)(((color >> 16) & 0xFF) * factor);
  uint8_t g = (uint8_t)(((color >> 8) & 0xFF) * factor);
  uint8_t b = (uint8_t)((color & 0xFF) * factor);
  return strip.Color(r, g, b);
}

void updateLEDs() {
  uint32_t baseColor = ring1.Color(80, 30, 100); // softer purple
  float brightness = (sin(breathPhase) + 1.0) / 4.0 + 0.1; // breath cycle 0.1 to 0.6
  breathPhase += breathSpeed;

  // Reset rings with breathing effect
  for (int i = 0; i < NUM_LEDS; i++) {
    ring1.setPixelColor(i, colorDim(ring1, baseColor, brightness));
    ring2.setPixelColor(i, colorDim(ring2, baseColor, brightness));
  }

  // Threshold to consider a direction pressed
  const int threshold = 40; // adjust sensitivity as needed

  // Define directions for left and right sticks with corresponding LEDs and axis checks
  struct Direction {
    const char* name;
    int ledIndex;
    bool leftRing;
    bool isXAxis; // true if direction depends on X axis, false for Y axis
    bool positive; // true for positive direction, false for negative
  }
  
  directions[] = {
    {"left_up", 0, true, false, false},    // LY < center - threshold
    {"left_down", 8, true, false, true},  // LY > center + threshold
    {"left_left", 12, true, true, false}, // LX < center - threshold
    {"left_right", 4, true, true, true},  // LX > center + threshold

    {"right_up", 0, false, false, false},    // RY < center - threshold
    {"right_down", 8, false, false, true},  // RY > center + threshold
    {"right_left", 12, false, true, false},  // RX < center - threshold
    {"right_right", 4, false, true, true}, // RX > center + threshold
  };

  for (auto& dir : directions) {
    bool pressed = false;
    if (dir.leftRing) {
      // Left stick
      if (dir.isXAxis) {
        if (dir.positive) pressed = (ReportData.LX > 128 + threshold);
        else pressed = (ReportData.LX < 128 - threshold);
      } else {
        if (dir.positive) pressed = (ReportData.LY > 128 + threshold);
        else pressed = (ReportData.LY < 128 - threshold);
      }
    } else {
      // Right stick
      if (dir.isXAxis) {
        if (dir.positive) pressed = (ReportData.RX > 128 + threshold);
        else pressed = (ReportData.RX < 128 - threshold);
      } else {
        if (dir.positive) pressed = (ReportData.RY > 128 + threshold);
        else pressed = (ReportData.RY < 128 - threshold);
      }
    }

    if (pressed) {
      for (int j = -2; j <= 2; j++) {
        int idx = (dir.ledIndex + j + NUM_LEDS) % NUM_LEDS;
        float factor = 1.0 - abs(j) * 0.25;
        uint32_t color = colorDim(dir.leftRing ? ring1 : ring2, baseColor, factor + brightness);
        if (dir.leftRing) ring1.setPixelColor(idx, color);
        else ring2.setPixelColor(idx, color);
      }
    }
  }

  ring1.show();
  ring2.show();
}

void setupPins(){
    joystickUP.attach(PIN_LUP,INPUT_PULLUP);
    joystickDOWN.attach(PIN_LDOWN,INPUT_PULLUP);
    joystickLEFT.attach(PIN_LLEFT,INPUT_PULLUP);
    joystickRIGHT.attach(PIN_LRIGHT,INPUT_PULLUP);

    joystickRUP.attach(PIN_RUP,INPUT_PULLUP);
    joystickRDOWN.attach(PIN_RDOWN,INPUT_PULLUP);
    joystickRLEFT.attach(PIN_RLEFT,INPUT_PULLUP);
    joystickRRIGHT.attach(PIN_RRIGHT,INPUT_PULLUP);

    buttonA.attach(PIN_A,INPUT_PULLUP);
    buttonB.attach(PIN_B,INPUT_PULLUP);;
    buttonX.attach(PIN_X,INPUT_PULLUP);
    buttonY.attach(PIN_Y,INPUT_PULLUP);

    buttonLB.attach(PIN_L,INPUT_PULLUP);
    buttonRB.attach(PIN_R,INPUT_PULLUP);

    buttonSTART.attach(PIN_PLUS,INPUT_PULLUP);
    buttonHOME.attach(PIN_HOME,INPUT_PULLUP);

    joystickUP.interval(MILLIDEBOUNCE);
    joystickDOWN.interval(MILLIDEBOUNCE);
    joystickLEFT.interval(MILLIDEBOUNCE);
    joystickRIGHT.interval(MILLIDEBOUNCE);

    joystickRUP.interval(MILLIDEBOUNCE);
    joystickRDOWN.interval(MILLIDEBOUNCE);
    joystickRLEFT.interval(MILLIDEBOUNCE);
    joystickRRIGHT.interval(MILLIDEBOUNCE);

    buttonA.interval(MILLIDEBOUNCE);
    buttonB.interval(MILLIDEBOUNCE);
    buttonX.interval(MILLIDEBOUNCE);
    buttonY.interval(MILLIDEBOUNCE);

    buttonLB.interval(MILLIDEBOUNCE);
    buttonRB.interval(MILLIDEBOUNCE);

    buttonSTART.interval(MILLIDEBOUNCE);
    buttonHOME.interval(MILLIDEBOUNCE);
    
}
void setup() {
  setupPins();

  ring1.begin();
  ring1.show();
  ring2.begin();
  ring2.show();

  SetupHardware();
  GlobalInterruptEnable();
}


void loop() {
    buttonRead();
    processButtons();
    updateLEDs();
    HID_Task();
    // We also need to run the main USB management task.
    USB_USBTask();
}

void buttonRead()
{
  if (joystickUP.update()) {buttonStatus[BUTTONUP] = joystickUP.fell();}
  if (joystickDOWN.update()) {buttonStatus[BUTTONDOWN] = joystickDOWN.fell();}
  if (joystickLEFT.update()) {buttonStatus[BUTTONLEFT] = joystickLEFT.fell();}
  if (joystickRIGHT.update()) {buttonStatus[BUTTONRIGHT] = joystickRIGHT.fell();}

  if (joystickRUP.update()) {buttonStatus[BUTTONRUP] = joystickRUP.fell();}
  if (joystickRDOWN.update()) {buttonStatus[BUTTONRDOWN] = joystickRDOWN.fell();}
  if (joystickRLEFT.update()) {buttonStatus[BUTTONRLEFT] = joystickRLEFT.fell();}
  if (joystickRRIGHT.update()) {buttonStatus[BUTTONRRIGHT] = joystickRRIGHT.fell();}

  if (buttonA.update()) {buttonStatus[BUTTONA] = buttonA.fell();}
  if (buttonB.update()) {buttonStatus[BUTTONB] = buttonB.fell();}
  if (buttonX.update()) {buttonStatus[BUTTONX] = buttonX.fell();}
  if (buttonY.update()) {buttonStatus[BUTTONY] = buttonY.fell();}

  if (buttonLB.update()) {buttonStatus[BUTTONLB] = buttonLB.fell();}
  if (buttonRB.update()) {buttonStatus[BUTTONRB] = buttonRB.fell();}

  if (buttonSTART.update()) {buttonStatus[BUTTONSTART] = buttonSTART.fell();}
  if (buttonHOME.update()) {buttonStatus[BUTTONHOME] = buttonHOME.fell();}
}


void processLANALOG(){
    ReportData.HAT = DPAD_NOTHING_MASK_ON;

    if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])){ReportData.LY = 0;ReportData.LX = 255;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {ReportData.LY = 255;ReportData.LX = 255;}
    else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {ReportData.LY = 255;ReportData.LX = 0;}
    else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])){ReportData.LY = 0;ReportData.LX = 0;}
    else if (buttonStatus[BUTTONUP]) {ReportData.LY = 0;ReportData.LX = 128;}
    else if (buttonStatus[BUTTONDOWN]) {ReportData.LY = 255;ReportData.LX = 128;}
    else if (buttonStatus[BUTTONLEFT]) {ReportData.LX = 0;ReportData.LY = 128;}
    else if (buttonStatus[BUTTONRIGHT]) {ReportData.LX = 255;ReportData.LY = 128;}
    else {ReportData.LX = 128;ReportData.LY = 128;}
}

void processRANALOG(){
    ReportData.HAT = DPAD_NOTHING_MASK_ON;
    
    if ((buttonStatus[BUTTONRUP]) && (buttonStatus[BUTTONRRIGHT])){ReportData.RY = 0;ReportData.RX = 255;}
    else if ((buttonStatus[BUTTONRUP]) && (buttonStatus[BUTTONRLEFT])){ReportData.RY = 0;ReportData.RX = 0;}
    else if ((buttonStatus[BUTTONRDOWN]) && (buttonStatus[BUTTONRRIGHT])) {ReportData.RY = 255;ReportData.RX = 255;}
    else if ((buttonStatus[BUTTONRDOWN]) && (buttonStatus[BUTTONRLEFT])) {ReportData.RY = 255;ReportData.RX = 0;}
    else if (buttonStatus[BUTTONRUP]) {ReportData.RY = 0;ReportData.RX = 128;}
    else if (buttonStatus[BUTTONRDOWN]) {ReportData.RY = 255;ReportData.RX = 128;}
    else if (buttonStatus[BUTTONRLEFT]) {ReportData.RX = 0;ReportData.RY = 128;}
    else if (buttonStatus[BUTTONRRIGHT]) {ReportData.RX = 255;ReportData.RY = 128;}
    else {ReportData.RX = 128;ReportData.RY = 128;}
}
void processButtons(){
    processLANALOG();
    processRANALOG();
    buttonProcessing();
}

void buttonProcessing(){
  if (buttonStatus[BUTTONA]) {ReportData.Button |= A_MASK_ON;}
  if (buttonStatus[BUTTONB]) {ReportData.Button |= B_MASK_ON;}
  if (buttonStatus[BUTTONX]) {ReportData.Button |= X_MASK_ON;}
  if (buttonStatus[BUTTONY]) {ReportData.Button |= Y_MASK_ON;}
  if (buttonStatus[BUTTONLB]) {ReportData.Button |= LB_MASK_ON;}
  if (buttonStatus[BUTTONRB]) {ReportData.Button |= RB_MASK_ON;}
  if (buttonStatus[BUTTONSTART]){ReportData.Button |= START_MASK_ON;}
  if (buttonStatus[BUTTONHOME]){ReportData.Button |= HOME_MASK_ON;}
}
