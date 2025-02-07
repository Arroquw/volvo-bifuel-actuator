#include <TMCStepper.h>
#include <SoftwareSerial.h>

#define EN_PIN  2
#define DIR_PIN 3
#define STEP_PIN 4
#define SW_TX            6      // SoftwareSerial transmit pin - YELLOW
#define SW_RX            7      // SoftwareSerial receive pin - BROWN
#define PWM_INPUT_PIN    8      // PWM signal input pin
#define DRIVER_ADDRESS   0b00   // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE          0.075f // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)

#define MAX_STEPS 235  // Steps for 8.5mm total travel
#define PITCH 0.042    // mm per step

SoftwareSerial SoftSerial(SW_RX, SW_TX);                          // Be sure to connect RX to TX and TX to RX between both devices
TMC2209Stepper TMCdriver(&SoftSerial, R_SENSE, DRIVER_ADDRESS);   // Create TMC driver

long currentPosition = 0; // Current position in steps

void setup() {
  // Initialize pins
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PWM_INPUT_PIN, INPUT);

  // Initialize TMC2209 driver
  SoftSerial.begin(115200);
  TMCdriver.begin();
  TMCdriver.toff(5);          // Enable driver
  TMCdriver.rms_current(380); // Set motor current (mA)
  TMCdriver.microsteps(2);   // Set microsteps (1/16 steps)

  digitalWrite(EN_PIN, LOW); // Enable motor driver
}

void moveSteps(long steps) {
  bool direction = steps > 0; // True for forward, false for reverse
  steps = abs(steps);

  // Set direction based on the number of steps
  digitalWrite(DIR_PIN, direction ? HIGH : LOW);

  for (long i = 0; i < steps; i++) {
    // Prevent over-extension or under-retraction
    if ((currentPosition >= MAX_STEPS && direction) || (currentPosition <= 0 && !direction)) {
      Serial.println("Limit reached!");
      break;  // Stop if the limit is reached
    }

    // Step pulse
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(100);  // Adjust speed by changing the delay time
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(100);  // Adjust speed by changing the delay time

    // Update current position
    currentPosition += direction ? 1 : -1;

    // Debugging: Print current position to track the movement
    Serial.print("Current Position: ");
    Serial.println(currentPosition);
  }
}

void loop() {
  // Test: Extend by 238 steps (10mm), then retract by 238 steps
  Serial.println("Extending...");
  moveSteps(300);  // Extend 10mm
  delay(10);     // Wait 1 second

  Serial.println("Retracting...");
  moveSteps(-300); // Retract 10mm
  delay(10);     // Wait 1 second
}
