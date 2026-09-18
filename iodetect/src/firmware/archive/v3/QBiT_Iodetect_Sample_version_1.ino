#include <LiquidCrystal_I2C.h>
#include <wire.h>

#define SDA 13
#define SCL 14

const u8int_t buttonPush [] {4,5};
int stage = 1;
LiquidCrystal_I2C lcd(0x27, 16, 2);



void setup() {
 Wire.begin(SDA, SCL);


 if (!i2CAddrTest(0x27)){
  lcd = LiquidCrystal_I2C(0x3F, 16, 2);
 }

 Serial.begin(9600); // begin communicating with button at 9600 b/s
 pinMode(4, INPUT);
 lcd.init();                // initialize the lcd
 lcd.backlight();           // Turn on backlight
 lcd.setCursor(0,0); // Cursor in top row
 lcd.print("Iodetect");// Print a message to the LCD
 lcd.setCursor(0,1);
 lcd.print("Start (Y/N)");
} 


void loop() {
  // put your main code here, to run repeatedly:
if (readButtons()){
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Patient age > 12?");
lcd.setCursor(0,1);
lcd.print("Y/N");
delay(1000);
}

if (readButtons()) { //press button to move on to next question
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Patient pregnant");
lcd.setCursor(0,1);
lcd.print("Y/N");
delay(1000);
}

if (readButtons()) {
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Insert Sample");
delay(1000);
}

if (readButtons()) {
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Analyzing ...");
delay(1000);
}

if (readButtons()) {
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Iodine Level");
lcd.setCursor(0,1);
lcd.print("150 ug/mL");
delay(1000);
}

if (readButtons()) {
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Iodine Status");
lcd.setCursor(0,1);
lcd.print("Healthy");
delay(1000);
}
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
    if (buttonValue == 1) {
    return true;
    delay(1);
  }
}
}