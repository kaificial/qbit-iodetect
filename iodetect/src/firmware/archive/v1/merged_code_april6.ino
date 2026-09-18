// merged code here 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// -------------------- MOTOR PINS --------------------
#define STEP_PIN   5
#define DIR_PIN    17
#define DIR_PIN2   19
#define STEP_PIN2  18

// -------------------- SENSOR / LCD / BUTTON PINS --------------------
const int sensorPin = 34;
const int ledPin = 2;

#define SDA 21 // was 13
#define SCL 22 // was 14

const uint8_t buttonPins[] = {33, 32};
// buttonPins[0] = toggle
// buttonPins[1] = enter

// -------------------- ADC SETTINGS --------------------
const float vRef = 3.3;
const int adcMax = 4095;

const unsigned long measureTime = 10000;
const int sampleInterval = 10;
const unsigned long lcdUpdateInterval = 250;

// -------------------- REACTION TIMER --------------------
const unsigned long reactionDuration = 30000;   // 60 seconds
unsigned long reactionStartTime = 0;

// -------------------- USER DATA --------------------
String sex = "Female";
bool pregnant = false;
bool lactating = false;
bool ageAbove6 = true;

// -------------------- MEASUREMENT DATA --------------------
float blankAverage = -1.0;
float sampleAverage = -1.0;
float absorbance = -1.0;
float transmittance = -1.0;

// -------------------- SCREEN FLOW --------------------
// 1 = sex
// 2 = pregnant
// 3 = lactating
// 4 = age
// 5 = insert blank
// 6 = measuring blank
// 7 = insert sample / reaction timer
// 8 = measuring sample
// 9 = results
int stage = 1;

bool buttonOption = true;
int lastRenderedStage = -1;

// result screen alternation
unsigned long resultScreenTimer = 0;
bool showAbsorbanceScreen = false;

// timer display tracking
long lastReactionSecondsShown = -1;

// -------------------- LCD --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------------------- MOTOR CONTROL --------------------
bool motorsEnabled = false;

bool motor1Dir = true;
bool motor2Dir = true;

unsigned long motor1StepDelayUs = 100;
unsigned long motor2StepDelayUs = 500;

unsigned long lastStepTime1 = 0;
unsigned long lastStepTime2 = 0;

bool stepState1 = LOW;
bool stepState2 = LOW;

// -------------------- FUNCTION DECLARATIONS --------------------
bool i2CAddrTest(uint8_t addr);
byte readButtons();
void renderQuestionScreen(const char* line1, bool option);
void renderStaticScreen(const char* line1, const char* line2);
void showStageIfChanged();
void refreshCurrentQuestion();
float measureAverageVoltage(unsigned long durationMs, const char* topLine);
void saveAnswerAndAdvance();
void doBlankMeasurement();
void startReactionTimer();
void handleReactionTimer();
void stopMotors();
void doSampleMeasurement();
void showResultsScreen();
void resetSystem();

void updateMotors();
void setMotorDirections(bool dir1, bool dir2);

// -------------------- HELPERS --------------------
bool i2CAddrTest(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

byte readButtons() {
  while (true) {
    updateMotors();

    if (digitalRead(buttonPins[1]) == LOW) {
      delay(200);
      return 1; // enter
    }
    if (digitalRead(buttonPins[0]) == LOW) {
      delay(200);
      return 0; // toggle
    }
    delay(1);
  }
}

void renderQuestionScreen(const char* line1, bool option) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  if (option) lcd.print("Yes(X) No()    ");
  else lcd.print("Yes() No(X)    ");
}

void renderStaticScreen(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void showStageIfChanged() {
  if (stage == lastRenderedStage) return;

  lastRenderedStage = stage;

  if (stage == 1) {
    renderQuestionScreen("Are you woman?", buttonOption);
  }
  else if (stage == 2) {
    renderQuestionScreen("Pregnant?", buttonOption);
  }
  else if (stage == 3) {
    renderQuestionScreen("Lactating?", buttonOption);
  }
  else if (stage == 4) {
    renderQuestionScreen("Age above 6?", buttonOption);
  }
  else if (stage == 5) {
    renderStaticScreen("Insert blank", "Press enter");
  }
  else if (stage == 7) {
    renderStaticScreen("Insert sample?", "Press enter");
  }
}

void refreshCurrentQuestion() {
  lastRenderedStage = -1;
  showStageIfChanged();
}

// -------------------- MOTOR FUNCTIONS --------------------
void setMotorDirections(bool dir1, bool dir2) {
  motor1Dir = dir1;
  motor2Dir = dir2;

  digitalWrite(DIR_PIN, motor1Dir ? HIGH : LOW);
  digitalWrite(DIR_PIN2, motor2Dir ? HIGH : LOW);
}

void updateMotors() {
  if (!motorsEnabled) return;

  unsigned long now = micros();

  if (now - lastStepTime1 >= motor1StepDelayUs) {
    lastStepTime1 = now;
    stepState1 = !stepState1;
    digitalWrite(STEP_PIN, stepState1);
  }

  if (now - lastStepTime2 >= motor2StepDelayUs) {
    lastStepTime2 = now;
    stepState2 = !stepState2;
    digitalWrite(STEP_PIN2, stepState2);
  }
}

void stopMotors() {
  motorsEnabled = false;
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(STEP_PIN2, LOW);
  stepState1 = LOW;
  stepState2 = LOW;
}

// -------------------- MEASUREMENT --------------------
float measureAverageVoltage(unsigned long durationMs, const char* topLine) {
  unsigned long startTime = millis();
  unsigned long lastLcdUpdateTime = 0;

  unsigned long count = 0;
  double sumRaw = 0.0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(topLine);

  for (int i = 0; i < 10; i++) {
    analogRead(sensorPin);
    delay(2);
    updateMotors();
  }

  while (millis() - startTime < durationMs) {
    updateMotors();

    int raw = analogRead(sensorPin);
    sumRaw += raw;
    count++;

    unsigned long nowMs = millis();
    if (nowMs - lastLcdUpdateTime >= lcdUpdateInterval) {
      lastLcdUpdateTime = nowMs;

      long secondsLeft = (durationMs - (nowMs - startTime)) / 1000;
      if (secondsLeft < 0) secondsLeft = 0;

      lcd.setCursor(0, 1);
      lcd.print("Time left: ");
      lcd.print(secondsLeft);
      lcd.print("   ");
    }

    delay(sampleInterval);
  }

  if (count == 0) return -1.0;

  double avgRaw = sumRaw / count;
  float avgVoltage = (avgRaw * vRef) / adcMax;

  Serial.print("Average raw ADC = ");
  Serial.println(avgRaw, 3);

  return avgVoltage;
}

// -------------------- FLOW --------------------
void saveAnswerAndAdvance() {
  if (stage == 1) {
    if (buttonOption) {
      sex = "Female";
      stage = 2;
    } else {
      sex = "Male";
      pregnant = false;
      lactating = false;
      stage = 4;
    }
  }
  else if (stage == 2) {
    pregnant = buttonOption;
    stage = 3;
  }
  else if (stage == 3) {
    lactating = buttonOption;
    stage = 4;
  }
  else if (stage == 4) {
    ageAbove6 = buttonOption;
    stage = 5;
  }

  buttonOption = true;
  lastRenderedStage = -1;
}

void doBlankMeasurement() {
  Serial.println("Starting blank measurement...");
  blankAverage = measureAverageVoltage(measureTime, "Measuring blank");

  Serial.print("Blank average intensity (I0) = ");
  Serial.print(blankAverage, 6);
  Serial.println(" V");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Blank avg:");
  lcd.setCursor(0, 1);
  lcd.print(blankAverage, 6);

  delay(2500);

  stage = 7;
  lastRenderedStage = -1;
}

void startReactionTimer() {
  reactionStartTime = millis();
  lastReactionSecondsShown = -1;

  setMotorDirections(HIGH, HIGH);
  motorsEnabled = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reaction going");
}

void handleReactionTimer() {
  updateMotors();

  unsigned long elapsed = millis() - reactionStartTime;
  long secondsLeft = (reactionDuration - elapsed) / 1000;
  if (secondsLeft < 0) secondsLeft = 0;

  if (secondsLeft != lastReactionSecondsShown) {
    lastReactionSecondsShown = secondsLeft;
    lcd.setCursor(0, 1);
    lcd.print("Time left: ");
    lcd.print(secondsLeft);
    lcd.print("   ");
  }

  if (elapsed >= reactionDuration) {
    stopMotors();
    stage = 8;
    doSampleMeasurement();
  }
}

void doSampleMeasurement() {
  Serial.println("Starting sample measurement...");
  sampleAverage = measureAverageVoltage(measureTime, "Measuring samp");

  Serial.print("Blank average (I0) = ");
  Serial.print(blankAverage, 6);
  Serial.println(" V");

  Serial.print("Sample average (I) = ");
  Serial.print(sampleAverage, 6);
  Serial.println(" V");

  if (sampleAverage <= 0.0 || blankAverage <= 0.0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calc error");
    lcd.setCursor(0, 1);
    lcd.print("Bad reading");
    delay(2000);

    stage = 7;
    lastRenderedStage = -1;
    return;
  }

  transmittance = sampleAverage / blankAverage;
  absorbance = log10(blankAverage / sampleAverage);

  Serial.print("Transmittance (I/I0) = ");
  Serial.println(transmittance, 6);

  Serial.print("Absorbance = ");
  Serial.println(absorbance, 6);

  stage = 9;
  resultScreenTimer = millis();
  showAbsorbanceScreen = false;
  showResultsScreen();
}

void showResultsScreen() {
  lcd.clear();

  if (!showAbsorbanceScreen) {
    lcd.setCursor(0, 0);
    lcd.print("Transmittance");
    lcd.setCursor(0, 1);
    lcd.print(transmittance, 6);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Absorbance");
    lcd.setCursor(0, 1);
    lcd.print(absorbance, 6);
  }
}

void resetSystem() {
  sex = "Female";
  pregnant = false;
  lactating = false;
  ageAbove6 = true;

  blankAverage = -1.0;
  sampleAverage = -1.0;
  absorbance = -1.0;
  transmittance = -1.0;

  buttonOption = true;
  stage = 1;
  lastRenderedStage = -1;

  stopMotors();
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN2, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);

  digitalWrite(STEP_PIN, LOW);
  digitalWrite(STEP_PIN2, LOW);

  stopMotors();

  Wire.begin(SDA, SCL);

  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IODetect");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);

  lastRenderedStage = -1;
  showStageIfChanged();
}

// -------------------- LOOP --------------------
void loop() {
  updateMotors();

  if (stage >= 1 && stage <= 4) {
    showStageIfChanged();
    byte buttonPressed = readButtons();

    if (buttonPressed == 0) {
      buttonOption = !buttonOption;
      refreshCurrentQuestion();
    }
    else if (buttonPressed == 1) {
      saveAnswerAndAdvance();
    }
  }
  else if (stage == 5) {
    showStageIfChanged();
    byte buttonPressed = readButtons();

    if (buttonPressed == 1) {
      stage = 6;
      doBlankMeasurement();
    }
  }
  else if (stage == 7) {
    showStageIfChanged();
    byte buttonPressed = readButtons();

    if (buttonPressed == 1) {
      startReactionTimer();

      while (stage == 7) {
        handleReactionTimer();
      }
    }
  }
  else if (stage == 9) {
    if (millis() - resultScreenTimer >= 2500) {
      resultScreenTimer = millis();
      showAbsorbanceScreen = !showAbsorbanceScreen;
      showResultsScreen();
    }

    if (digitalRead(buttonPins[1]) == LOW) {
      delay(300);
      resetSystem();
    }
  }
}
