
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// store data using wifi capabilities, send to cloud
#define SDA 13                    //Define SDA pins (Arduino to ESP)
#define SCL 14                    //Define SCL pins (Arduino to ESP)
const uint8_t buttonPins[] = {4, 5}; //Two buttons: one for NO, one for YES

// Timer running throughout (for reaction)
// After button is pressed, timer starts, 
// use delay function? Dsiplay timer

LiquidCrystal_I2C lcd(0x27,16,2); 

enum State {
  WELCOME,
  Q_PREGNANT,
  Q_LACTATING,
  Q_SUPPLEMENT,
  INSERT_CHIP,
  MEASURING,
  CALCULATING,
  RESULT
};

State state = WELCOME;
bool stateJustChanged = true

// Answers

bool isPregnant = false;
bool isLactating = false;
bool takingSupp = false;

bool i2CAddrTest(uint8_t addr);
int readButtonsOnce();   // returns -1, 0, or 1


void setup() {
  Wire.begin(SDA, SCL);           // attach the IIC pin
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight


  // Setup buttons
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);

  // Static text (first line)
  lcd.setCursor(0, 0);             // Move the cursor to row 0, column 0
  lcd.print("Iodetect Ready!");     // The print content is displayed on the LCD
  lcd.setCursor(0, 1);
  lcd.print("Press any button")
}

void loop() {
  // Clear only line 2, keep title static
  lcd.setCursor(0,1);             // Move the cursor to row 1, column 0
  lcd.print("Time (s):");          // The count is displayed every second
  lcd.print(millis() / 1000);     // Display seconds since boot
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press button to start");
  delay(5000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pregnant?");
  lcd.setCursor(1, 0);
  lcd.print("Yes/No");
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Age < 6?");
  lcd.setCursor(1, 0);
  lcd.print("Age < 6?");
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Supplemental Iodine?");
  lcd.setCursor(1, 0);
  lcd.print("Yes/No");
  delay(5000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Insert chip");
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Load sample");
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Processing...");
  lcd.setCursor(1, 0);
  lcd.print("Age < 6?");
  delay(5000);
}

bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

byte readButtons() {
  while (true) {
    if (digitalRead(buttonPins[1]) == LOW) {
      return 1;
    }
    if (digitalRead(buttonPins[0]) == LOW) {
      return 0;
    }
    delay(1);

  }
}
