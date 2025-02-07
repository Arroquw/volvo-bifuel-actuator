// Constants for Timer1
const int pwmPin = 9;   // Pin 9 is controlled by Timer1
float frequency = 175;  // Initial PWM frequency in Hz

void setup() {
  // Set pin 9 as an output
  pinMode(pwmPin, OUTPUT);

  // Start serial communication for printing
  Serial.begin(115200);

  // Enable the PWM signal
  enablePWM(frequency);
}

void loop() {
  // Loop to iterate between 20% and 83% duty cycle
  //float duty = 15.8;
  for (float duty = 80; duty >= 20; duty -= 5) {
    enablePWM(frequency);


    // Calculate OCR1A value based on duty cycle percentage
    OCR1A = (ICR1 * duty) / 100;
    delay(70);
    stopPWM();
    // Calculate the high time in milliseconds (to verify the duty cycle is correct)
    float period = 1000.0 / frequency;  // PWM period in milliseconds
    float highTime = (duty / 100.0) * period;  // High time in milliseconds (time pin is HIGH)

    // Print the duty cycle, high time, and frequency for verification
    Serial.print("Duty Cycle: ");
    Serial.print(duty);
    Serial.print("%, Frequency: ");
    Serial.print(frequency);
    Serial.print(" Hz, High Time: ");
    Serial.print(highTime);
    Serial.println(" ms");

    // Wait for some time before changing the duty cycle again
    delay();  // Adjust the delay as needed

    //while(1);
  }

  // After the loop, stop the PWM signal
 
}

// Function to enable the PWM signal
void enablePWM(float freq) {
  // Clear Timer1 control registers
  TCCR1A = 0;
  TCCR1B = 0;

  // Set PWM mode to Phase Correct PWM with ICR1 as TOP
  TCCR1A |= (1 << COM1A1);   // Non-inverting mode for OC1A (pin 9)
  TCCR1A |= (1 << WGM11);    // Set WGM11 to 1 for Phase Correct PWM mode
  TCCR1B |= (1 << WGM13);    // Set WGM13 to 1 for Phase Correct PWM with ICR1 as TOP
  TCCR1B |= (1 << CS11) | (1 << CS10);    // Prescaler set to 64

  // Set the PWM frequency dynamically based on the 'frequency' variable
  setPWMFrequency(freq);

  // Print a message to confirm PWM has started
  Serial.println("PWM Enabled");
}

// Function to set the PWM frequency by updating ICR1 based on the 'frequency' variable
void setPWMFrequency(float freq) {
  // Calculate ICR1 value based on the given frequency for Phase Correct PWM
  ICR1 = (16000000 / (2 * 64 * freq)) - 1;  // Note the factor of 2 for phase correct mode

  // Print the updated frequency and ICR1 value for debugging purposes
  Serial.print("Updated Frequency: ");
  Serial.print(freq);
  Serial.print(" Hz, ICR1: ");
  Serial.println(ICR1);
}

// Function to stop PWM signal generation
void stopPWM() {
  // Clear the COM1A1 bit in TCCR1A to stop PWM output on pin 9
  TCCR1A &= ~(1 << COM1A1);

  // Optionally stop the timer by clearing the prescaler bits in TCCR1B
  TCCR1B &= ~(1 << CS11);  // Stop the timer by clearing the prescaler bits
  TCCR1B &= ~(1 << CS10);

  // Set the pin to LOW to ensure the signal is off
  digitalWrite(pwmPin, LOW);

  // Print a message to confirm PWM has stopped
  Serial.println("PWM Stopped");
}