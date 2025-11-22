#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define NUM_SERVOS 6

// Servo shield object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Angle limits (change if your servos need tuning)
#define SERVO_MIN 150   // ~500µs pulse
#define SERVO_MAX 600   // ~2500µs pulse

int servoChannels[NUM_SERVOS] = {0, 1, 2, 3, 4, 5};  // PCA9685 channels
int currentPos[NUM_SERVOS] = {90, 90, 90, 90, 90, 90};
int targetPos[NUM_SERVOS]  = {90, 90, 90, 90, 90, 90};

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit 16ch Servo Shield – 6DOF Arm Ready");

  pwm.begin();
  pwm.setPWMFreq(50);  // Standard analog servo frequency
  delay(10);

  // Initialize all servos at starting position
  for (int i = 0; i < NUM_SERVOS; i++) {
    writeServo(i, currentPos[i]);
    delay(1000)
  }
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    parseCommand(input);
    moveSmooth();
    reportPositions();
  }
}

void parseCommand(String input) {
  input.replace(',', ' ');
  int idx = 0;

  while (input.length() > 0 && idx < NUM_SERVOS) {
    int spaceIndex = input.indexOf(' ');
    String val = (spaceIndex == -1) ? input : input.substring(0, spaceIndex);
    val.trim();

    if (val.length() > 0) {
      targetPos[idx] = constrain(val.toInt(), 0, 180);
      idx++;
    }

    if (spaceIndex == -1) break;
    input = input.substring(spaceIndex + 1);
  }
}

void moveSmooth() {
  bool moving = true;

  while (moving) {
    moving = false;

    for (int i = 0; i < NUM_SERVOS; i++) {
      if (currentPos[i] != targetPos[i]) {
        moving = true;

        if (currentPos[i] < targetPos[i]) currentPos[i]++;
        else currentPos[i]--;

        writeServo(i, currentPos[i]);
      }
    }

    delay(15);  // smoothness speed
  }
}

// Convert angle to PCA9685 pulse and send
void writeServo(int index, int angle) {
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(servoChannels[index], 0, pulse);
}

void reportPositions() {
  Serial.print("Current positions: ");
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print(currentPos[i]);
    if (i < NUM_SERVOS - 1) Serial.print(" ");
  }
  
  Serial.println();
  Serial.print("OK");
  Serial.println();
}
