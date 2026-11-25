#include <AccelStepper.h>

constexpr int STEP_PIN = 3;
constexpr int DIR_PIN = 2;
constexpr int MS1_PIN = 7;
constexpr int MS2_PIN = 6;
constexpr long STEPS_PER_REV = 200 * 8;
constexpr long HALF_TURN_STEPS = STEPS_PER_REV / 2;
constexpr float TARGET_RPM = 250.0f;
constexpr float STEP_RATE = TARGET_RPM * STEPS_PER_REV / 60.0f;
constexpr unsigned long ENDPOINT_PAUSE_MS = 250;
constexpr unsigned long PLOT_INTERVAL_MS = 50;

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
long targetPosition = HALF_TURN_STEPS;
unsigned long arrivedAtMs = 0;
unsigned long lastPlotMs = 0;
bool paused = false;

void setup() {
  Serial.begin(115200);
  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  digitalWrite(MS1_PIN, LOW);
  digitalWrite(MS2_PIN, LOW);
  stepper.setMinPulseWidth(2);
  stepper.setMaxSpeed(STEP_RATE);
  stepper.setCurrentPosition(0);
  stepper.moveTo(targetPosition);
  stepper.setSpeed(STEP_RATE);
}

void loop() {
  const unsigned long now = millis();
  if (paused) {
    if (now - arrivedAtMs >= ENDPOINT_PAUSE_MS) {
      targetPosition = HALF_TURN_STEPS - targetPosition;
      stepper.moveTo(targetPosition);
      stepper.setSpeed(targetPosition == 0 ? -STEP_RATE : STEP_RATE);
      paused = false;
    }
  } else {
    stepper.runSpeedToPosition();
    if (stepper.distanceToGo() == 0) {
      arrivedAtMs = now;
      paused = true;
    }
  }

  if (now - lastPlotMs >= PLOT_INTERVAL_MS) {
    lastPlotMs = now;
    Serial.print("Commanded_RPM:");
    Serial.println(paused ? 0.0f : stepper.speed() * 60.0f / STEPS_PER_REV, 1);
  }
}
