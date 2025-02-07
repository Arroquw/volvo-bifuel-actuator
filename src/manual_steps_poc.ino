#include <TMCStepper.h>
#include <SoftwareSerial.h>

#define EN_PIN 10
#define DIR_PIN 3
#define STEP_PIN 4
#define SW_TX 6              // SoftwareSerial transmit pin - YELLOW
#define SW_RX 7              // SoftwareSerial receive pin - BROWN
#define PWM_INPUT_PIN 11      // PWM signal input pin
#define READY_OUT_PIN 12
#define DRIVER_ADDRESS 0b00  // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.075f       // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)
#define STEP_DELAY 65       // Delay between steps in microseconds (max speed (mechanically) is a delay of 65)
#define STEPS 240            // Steps for 8.5mm total travel
#define PITCH 0.042          // mm per step
#define MICROSTEPS 16
#define MAX_STEPS (STEPS * MICROSTEPS)
SoftwareSerial SoftSerial(SW_RX, SW_TX);                         // Be sure to connect RX to TX and TX to RX between both devices
TMC2209Stepper TMCdriver(&SoftSerial, R_SENSE, DRIVER_ADDRESS);  // Create TMC driver

enum {
  RETRACT = false,
  EXTEND = true,
};

long currentPosition = 0;  // Current position in steps

void setup() {
  // Initialize pins
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PWM_INPUT_PIN, INPUT);

  // Initialize TMC2209 driver
  SoftSerial.begin(115200);
  TMCdriver.begin();
  TMCdriver.toff(5);                 // Enable driver
  TMCdriver.rms_current(380);        // Set motor current (mA)
  TMCdriver.microsteps(MICROSTEPS);  // Set microsteps (1/16 steps)

  digitalWrite(EN_PIN, LOW);   // Enable motor driver
  TMCdriver.TCOOLTHRS(0xFFFF); // Enable coolStep and stallGuard
  TMCdriver.index_step();
  resetPos();
  digitalWrite(READY_OUT_PIN, HIGH);
}

void step() {
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(STEP_DELAY);
}

void setDir(bool dir) {
  digitalWrite(DIR_PIN, dir);
}

void resetPos() {
  TMCdriver.rms_current(100);
  setDir(RETRACT);
  for(int i = 0; i < MAX_STEPS; i++) {
    step();
  }
  delay(1000);
  currentPosition = 0;
  setDir(EXTEND);
  TMCdriver.rms_current(380);
}

void loop() {
  bool dir = false;
  while (1) {
    step();
    currentPosition+= dir ? 1 : -1;
    if (currentPosition > MAX_STEPS || currentPosition < 0) {
      setDir(dir);
      dir = !dir;
    }
  }
}