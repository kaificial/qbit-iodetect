

/*Firebase*/
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define Web_API_KEY "YOUR_API_KEY"
#define DATABASE_URL "https://YOUR_PROJECT_ID.firebaseio.com/"
#define USER_EMAIL "user@example.com"
#define USER_PASS "YOUR_PASSWORD"
/*Firebase*/


#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

#define SDA 13                    //Define SDA pins
#define SCL 14                    //Define SCL pins
const uint8_t buttonPins[] = {4, 5};
int stage = 1;
int initialTime = 0;
bool buttonOption = true;
bool send1Done = false;


/*Firebase*/
// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;

// Timer variables for sending data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10 seconds in milliseconds

// Variables to send to the database
int intValue = 0;
float floatValue = 0.01;
String stringValue = "";
/*Firebase*/


LiquidCrystal_I2C lcd(0x27,16,2);
void setup() {

  /*Firebase*/
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)    {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  // Configure SSL client
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);
  
  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  /*Firebase*/

 Wire.begin(SDA, SCL);           // attach the IIC pin
 if (!i2CAddrTest(0x27)) {
   lcd = LiquidCrystal_I2C(0x3F, 16, 2);
 }
 lcd.init();                     // LCD driver initialization
 lcd.backlight();                // Open the backlight
 lcd.setCursor(0,0);             // Move the cursor to row 0, column 0
 lcd.print("IODetect");     // The print content is displayed on the LCD
 pinMode(buttonPins[0], INPUT_PULLUP);
 pinMode(buttonPins[1], INPUT_PULLUP);
 }


void loop() {
  /*Firebase*/
  // Maintain authentication and async tasks
  app.loop();
  /*Firebase*/

  if(stage!=6){
  byte buttonPressed = readButtons();
  if(buttonPressed == 0){
       if(buttonOption == true){
         buttonOption= false;
       }else{
         buttonOption=true;
       }
       delay(1000);
  }else if(buttonPressed == 1){
     stage = stage +1;
     delay(1000);
     if(stage == 5){
      initialTime = millis();
     }
  }
 }
 lcd.setCursor(0,0);             // Move the cursor to row 1, column 0
 if(stage == 1){
   lcd.print("Are you a woman?");
   lcd.setCursor(0,1);
   if(buttonOption == true){
     lcd.print("Yes(X)   No()");
   }else{
     lcd.print("Yes()   No(X)");
   }
 }else if(stage == 2){
   lcd.print("Are you pregnant?");
   lcd.setCursor(0,1);
   if(buttonOption == true){
     lcd.print("Yes(X)   No()");
   }else{
     lcd.print("Yes()   No(X)");
   }
 }else if(stage == 3){
   lcd.print("Are you lactating?");
   lcd.setCursor(0,1);
   if(buttonOption == true){
     lcd.print("Yes(X)   No()");
   }else{
     lcd.print("Yes()   No(X)");
   }
 }else if(stage == 4){
   lcd.print("Age above 6?      ");
   lcd.setCursor(0,1);
   if(buttonOption == true){
     lcd.print("Yes(X)   No()");
   }else{
     lcd.print("Yes()   No(X)");
   }
 }else if(stage == 5){
   lcd.print("Chip Entered?      ");

   lcd.setCursor(0,1);
   lcd.print("Enter to cont");
 }
 else if(stage == 6){
  /*firebase*/
   /*If successful, this part should send woman to firebase, but it is not successful*/
   String testPath;
   if (app.ready() && send1Done == false){
      //stringValue = "woman";
      //Database.push<String>(aClient, "/test/sex", stringValue, processData, "RTDB_Send_String");
      testPath = Database.push(aClient, "/tests");
      /*Database.set<String>(aClient, testPath + "/sex", sex, processData, "sex");
      Database.set<bool>(aClient, testPath + "/pregnant", pregnant, processData, "pregnant");
      Database.set<bool>(aClient, testPath + "/lactating", lactating, processData, "lactating");
      Database.set<int>(aClient, testPath + "/iodineConcentration", iodineConcentration, processData, "iodineConc");
      Database.set<String>(aClient, testPath + "/iodineLevel", iodineLevel, processData, "iodineLevel");*/
      Database.set<String>(aClient, testPath + "/sex", "Female", processData, "sex");
      Database.set<bool>(aClient, testPath + "/pregnant", False, processData, "pregnant");
      Database.set<bool>(aClient, testPath + "/lactating", False, processData, "lactating");
      Database.set<int>(aClient, testPath + "/iodineConcentration", 37.1, processData, "iodineConc");
      Database.set<String>(aClient, testPath + "/iodineLevel", High, processData, "iodineLevel");
      send1Done = true;
   }
   /*firebase*/
   lcd.print("Time left:          ");
   lcd.setCursor(0,1);
   lcd.print("                  ");
   lcd.setCursor(0,1);
   lcd.print(35 - (millis()-initialTime)/60000 );
   lcd.setCursor(13,1);
   lcd.print("min");
   delay(1000);
 }


 /*lcd.print("Counter:");          // The count is displayed every second
 lcd.print(millis() / 1000);
 delay(1000);*/
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
  if(stage != 6){}{
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
}

/*Firebase*/
// User function

void processData(AsyncResult &aResult) {
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}

