#include <TMCStepper.h>
#include <SoftwareSerial.h>

#define EN_PIN 3
#define DIR_PIN 4
#define STEP_PIN 5
#define SW_TX 7          // SoftwareSerial transmit pin - YELLOW
#define SW_RX 8          // SoftwareSerial receive pin - BROWN
#define PWM_INPUT_PIN 9  // PWM signal input pin
#define READY_OUT_PIN 10
#define INDEX_PIN 2
#define DRIVER_ADDRESS 0b00  // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.075f       // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)
#define STEP_DELAY 65        // Delay between steps in microseconds (max speed (mechanically) is a delay of 65)
#define STEPS 240            // Steps for 8.5mm total travel
#define PITCH 0.042          // mm per step
#define MICROSTEPS 16
#define MAX_STEPS (STEPS * MICROSTEPS)
#define MIN_STEPS 0
SoftwareSerial SoftSerial(SW_RX, SW_TX);                         // Be sure to connect RX to TX and TX to RX between both devices
TMC2209Stepper TMCdriver(&SoftSerial, R_SENSE, DRIVER_ADDRESS);  // Create TMC driver

enum {
  RETRACT = false,
  EXTEND = true,
};

volatile long currentPosition = 0;  // Current position in steps
long targetPosition = 0;
int modifier = 0;

void countRisingEdge() {
}

void setup() {
  // Initialize pins
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PWM_INPUT_PIN, INPUT);
  pinMode(INDEX_PIN, INPUT);  // Set the pin to input mode
  attachInterrupt(digitalPinToInterrupt(INDEX_PIN), countRisingEdge, RISING);
  Serial.begin(115200);

  // Initialize TMC2209 driver
  SoftSerial.begin(115200);
  TMCdriver.begin();
  TMCdriver.toff(5);                 // Enable driver
  TMCdriver.rms_current(380);        // Set motor current (mA)
  TMCdriver.microsteps(MICROSTEPS);  // Set microsteps (1/16 steps)
  TMCdriver.en_spreadCycle(false);
  TMCdriver.pwm_autoscale(true);     // Needed for stealthChop

  digitalWrite(EN_PIN, LOW);    // Enable motor driver
  TMCdriver.TCOOLTHRS(0xFFFF);  // Enable coolStep and stallGuard
  resetPos();
  digitalWrite(READY_OUT_PIN, HIGH);
  TMCdriver.index_step();
}

void step() {
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(STEP_DELAY);
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(STEP_DELAY);
}
 
void step2() {
  TMCdriver.VACTUAL(MAX_STEPS);
}

void setDir(bool dir) {
  digitalWrite(DIR_PIN, dir);
  //TMCdriver.shaft(dir);
}

void resetPos() {
  TMCdriver.rms_current(100);
  setDir(RETRACT);
  for (int i = 0; i < MAX_STEPS; i++) {
    step();
  }
Serial.print("A current");
Serial.println(TMCdriver.cur_a());
Serial.print("B current");
Serial.println(TMCdriver.cur_b());
  currentPosition = 0;
  delay(1000);
  setDir(EXTEND);
  TMCdriver.rms_current(380);
}

// Reads PWM duty cycle from the input pin
float readPWM() {
  int highTime = pulseIn(PWM_INPUT_PIN, HIGH);
  int lowTime = pulseIn(PWM_INPUT_PIN, LOW);
  int period = highTime + lowTime;

  if (period == 0) return 0;                       // Prevent division by zero
  return (float)highTime / (float)period * 100.0;  // Return duty cycle
}

// Maps PWM duty cycle to step count
long mapPWMToSteps(float dutyCycle) {
  return map(dutyCycle, 20, 83, MIN_STEPS, MAX_STEPS * 0.85);
}

void loop() {
  float dutyCycle = readPWM();
  Serial.print("duty cycle: ");
  Serial.println(dutyCycle);
  targetPosition = mapPWMToSteps(round(constrain(dutyCycle, 20, 83)));
  Serial.print("target position: ");
  Serial.println(targetPosition);
  if (currentPosition > targetPosition) {
      modifier = -1;
      setDir(RETRACT);
  } else if (currentPosition < targetPosition) {
    modifier = 1;
    setDir(EXTEND);
  } else {
    modifier = 0;
  }
  Serial.print("current position: ");
  Serial.println(currentPosition);
  while(currentPosition < targetPosition || currentPosition > targetPosition) {

  step();
  currentPosition += modifier;
  }
  if (currentPosition > MAX_STEPS || currentPosition < 0) {
    resetPos();
  }
}