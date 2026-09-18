
/* Firebase */
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define Web_API_KEY "YOUR_API_KEY"
#define DATABASE_URL "https://YOUR_PROJECT_ID.firebaseio.com/"
#define USER_EMAIL "[EMAIL_ADDRESS]"
#define USER_PASS "YOUR_PASSWORD"

#define SDA 13
#define SCL 14

const uint8_t buttonPins[] = {4, 5};

int stage = 1;
unsigned long initialTime = 0;
bool buttonOption = true;
bool send1Done = false;
bool connected = false;
bool noDataMessageShown = false;
unsigned long noDataMessageStart = 0;

//variables
bool sex = "Female";
bool pregnant = false;
bool lactating = false;

// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Function declarations
bool i2CAddrTest(uint8_t addr);
byte readButtons();
void processData(AsyncResult &aResult);

void setup() {
  Serial.begin(115200);

  /* WiFi */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  for (int i = 0; i < 60; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      break;
    }
    Serial.print(".");
    delay(1000);
  }

  if (connected) {
    Serial.println("\nWiFi connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi within 1 minute.");
  }

  /* Firebase */
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);

  if (connected) {
    initializeApp(aClient, app, getAuth(user_auth), processData, "authTask");
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
  }

  /* LCD */
  Wire.begin(SDA, SCL);

  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("IODetect");

  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
}

void loop() {
  if (connected) {
    app.loop();
  }

  if (stage != 6) {
    byte buttonPressed = readButtons();

    if (buttonPressed == 0) {
      buttonOption = !buttonOption;
      delay(300);
    } 
    else if (buttonPressed == 1) {
      stage = stage + 1;
      delay(300);

      if (stage == 6) {
        initialTime = millis();
        noDataMessageShown = false;
        send1Done = false;
      }
    }
  }

  lcd.setCursor(0, 0);

  if (stage == 1) {
    lcd.print("Are you a woman?");
    lcd.setCursor(0, 1);
    if (buttonOption) {
      lcd.print("Yes(X)   No() ");
    } else {
      lcd.print("Yes()   No(X) ");
      sex = "Male";
    }
  } 
  else if (stage == 2) {
    lcd.print("Are you pregnant");
    lcd.setCursor(0, 1);
    if (buttonOption) {
      lcd.print("Yes(X)   No() ");
      pregnant = true;
    } else {
      lcd.print("Yes()   No(X) ");
    }
  } 
  else if (stage == 3) {
    lcd.print("Are you lactating");
    lcd.setCursor(0, 1);
    if (buttonOption) {
      lcd.print("Yes(X)   No() ");
      lactating = true;
    } else {
      lcd.print("Yes()   No(X) ");
    }
  } 
  else if (stage == 4) {
    lcd.print("Age above 6?    ");
    lcd.setCursor(0, 1);
    if (buttonOption) {
      lcd.print("Yes(X)   No() ");
    } else {
      lcd.print("Yes()   No(X) ");
    }
  } 
  else if (stage == 5) {
    lcd.print("Chip Entered?   ");
    lcd.setCursor(0, 1);
    lcd.print("Enter to cont   ");
  } 
  else if (stage == 6) {
    String testPath = "/tests/test1";

    // Try to send data once if WiFi/Firebase are ready
    if (connected && app.ready() && !send1Done) {
      Database.set<String>(aClient, testPath + "/sex", "Female", processData, "sex");
      Database.set<bool>(aClient, testPath + "/pregnant", false, processData, "pregnant");
      Database.set<bool>(aClient, testPath + "/lactating", false, processData, "lactating");
      Database.set<float>(aClient, testPath + "/iodineConcentration", 37.1, processData, "iodineConc");
      Database.set<String>(aClient, testPath + "/iodineLevel", "High", processData, "iodineLevel");

      send1Done = true;
    }

    // If data cannot be sent, show a brief message once
    if ((!connected || !app.ready()) && !send1Done && !noDataMessageShown) {
      noDataMessageShown = true;
      noDataMessageStart = millis();
    }

    // Show "No data sent" briefly, then continue to timer screen
    if ((!connected || !app.ready()) && !send1Done && noDataMessageShown &&
        (millis() - noDataMessageStart < 2000)) {
      lcd.setCursor(0, 0);
      lcd.print("No data sent    ");
      lcd.setCursor(0, 1);
      lcd.print("Running test    ");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Time left:      ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);

      long minutesLeft = 5 - ((millis() - initialTime) / 60000);
      if (minutesLeft < 0) {
        minutesLeft = 0;
      }

      lcd.print(minutesLeft);
      lcd.setCursor(13, 1);
      lcd.print("min");
    }

    delay(200);
  }
}

bool i2CAddrTest(uint8_t addr) {
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

/* Firebase callback */
void processData(AsyncResult &aResult) {
  if (!aResult.isResult()) return;

  if (aResult.isEvent()) {
    Firebase.printf("Event task: %s, msg: %s, code: %d\n",
                    aResult.uid().c_str(),
                    aResult.eventLog().message().c_str(),
                    aResult.eventLog().code());
  }

  if (aResult.isDebug()) {
    Firebase.printf("Debug task: %s, msg: %s\n",
                    aResult.uid().c_str(),
                    aResult.debug().c_str());
  }

  if (aResult.isError()) {
    Firebase.printf("Error task: %s, msg: %s, code: %d\n",
                    aResult.uid().c_str(),
                    aResult.error().message().c_str(),
                    aResult.error().code());
  }

  if (aResult.available()) {
    Firebase.printf("Task: %s, payload: %s\n",
                    aResult.uid().c_str(),
                    aResult.c_str());
  }
}