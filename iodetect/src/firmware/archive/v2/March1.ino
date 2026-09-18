#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SDA 13
#define SCL 14

#define BTN_NEXT 4
#define BTN_ENTER 5

//Bools to choose what iodine scale is used
bool pregnant = 0; // uses iodineLactating
bool over8 = 0; //uses void iodine 
bool lactatingOrUnder2 = 0; //uses pregnancyIodine
bool under6Months = 0; //uses iodine6month
bool age7to12month = 0; // uses iodine7to12month
bool age1to8year = 0; // uses iodine1to8year

LiquidCrystal_I2C lcd(0x27,16,2);

//SYSTEM STATES
int stage = 0;
int selection = 0;

String gender = "";
String pregnant = "";

//BUTTON EDGE DETECTION
bool buttonPressed(int pin)
{
  static bool lastNext = HIGH;
  static bool lastEnter = HIGH;

  bool current = digitalRead(pin);

  if(pin == BTN_NEXT)
  {
    if(lastNext == HIGH && current == LOW)
    {
      delay(25);
      lastNext = current;
      return true;
    }
    lastNext = current;
  }

  if(pin == BTN_ENTER)
  {
    if(lastEnter == HIGH && current == LOW)
    {
      delay(25);
      lastEnter = current;
      return true;
    }
    lastEnter = current;
  }

  return false;
}

//SETUP
void setup()
{
  Wire.begin(SDA, SCL);

  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_ENTER, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Iodetect");
  delay(2000);
  lcd.clear();
}

//MAIN LOOP
void loop()
{
  switch(stage)
  {
     case 0:
      under6Months();
      break;

    case 1:
      ageRange2(); //asks if user is between 7 to 12 months  
      break;

    case 2:
      ageRange3(); // asks if user is between 1 and 8 years 
      break;

    case 3:
      ageRange4(); //asks if user is over age 8
      break;

    case 4:
      lactatingMenu();
      break;

    case 5:
      pregnantMenu(); 
      break;

    case 6:
      analyzingScreen();
      break;
  }

  //Call the iodine scale to be used 
  if (pregnant = 1)
  {
    pregnancyIodine();
  }

  else if (lactatingOrUnder2 = 1)
  {
    iodineLactating();
  }

  else if (under6Months = 1)
  {
    iodine6month();
  }

  else if (age7to12months = 1)
  {
    iodine7to12month();
  }

  else if (age1to8years = 1)
  {
      iodine1to8year();
  }

  else if (over8 = 1)
  {
      iodine();
  }
}

// OVER 8 YEARS OLD
void ageRange4()
{
  lcd.setCursor(0,0);
  lcd.print("Over age 8?");

  while(true)
  {
    lcd.setCursor(0,1);

    if(selection == 0)
      lcd.print(">Yes   No ");
      over8 = 1;
    else
      lcd.print(" Yes  >No");

    if(buttonPressed(BTN_NEXT))
      selection = !selection;

    if(buttonPressed(BTN_ENTER))
    {
      gender = (selection==0) ? "Yes":"No";
      selection = 0;
      stage++;
      lcd.clear();
      return;
    }
  }
}

//LACTATING MENU
void lactatingMenu()
{
  lcd.setCursor(0,0);
  lcd.print("Lactating?");

 while(true)
  {
    lcd.setCursor(0,1);

    if(selection == 0)
      lcd.print(">Yes   No ");
      lactating = 1;
    else
      lcd.print(" Yes  >No");

    if(buttonPressed(BTN_NEXT))
      selection = !selection;

    if(buttonPressed(BTN_ENTER))
    {
      gender = (selection==0) ? "Yes":"No";
      selection = 0;
      stage++;
      lcd.clear();
      return;
    }
  }
}

//Under 6 months menu
void under6Months()
{
  lcd.setCursor(0,0);
  lcd.print("Under 6 months?");

  while(true)
  {
    lcd.setCursor(0,1);
    
    if(selection == 0)
      lcd.print(">Yes     No   ");
    under6Month = 1;
    pregnant = 1;
    else
      lcd.print(" Yes    >No   ");

    if(buttonPressed(BTN_NEXT))
      selection = !selection;

    if(buttonPressed(BTN_ENTER))
    {
      pregnant = (selection==0) ? "Yes":"No";
      selection = 0;
      stage++;
      lcd.clear();
      return;
    }  
  }
}

//Between 7 and 12 months menu
void ageRange2()
{
  lcd.setCursor(0,0);
  lcd.print("7 to 12 months?");

  while(true)
  {
    lcd.setCursor(0,1);
    
    if(selection == 0)
      lcd.print(">Yes     No   ");
      age7to12month = 1;
    under6Month = 1;
    pregnant = 1;
    else
      lcd.print(" Yes    >No   ");

    if(buttonPressed(BTN_NEXT))
      selection = !selection;

    if(buttonPressed(BTN_ENTER))
    {
      pregnant = (selection==0) ? "Yes":"No";
      selection = 0;
      stage++;
      lcd.clear();
      return;
    }
  }
}

//Between 1 and 8 years menu
void ageRange3()
{
  lcd.setCursor(0,0);
  lcd.print("1 to 8 years?");

  while(true)
  {
    lcd.setCursor(0,1);
    
    if(selection == 0)
      lcd.print(">Yes     No   ");
      age1to8year = 1;
    under6Month = 1;
    pregnant = 1;
    else
      lcd.print(" Yes    >No   ");

    if(buttonPressed(BTN_NEXT))
      selection = !selection;

    if(buttonPressed(BTN_ENTER))
    {
      pregnant = (selection==0) ? "Yes":"No";
      selection = 0;
      stage++;
      lcd.clear();
      return;
    }
  }
}

//PREGNANT MENU 
void pregnantMenu()
{
  lcd.setCursor(0,0);
  lcd.print("Pregnant?");

  while(true)
  {
    lcd.setCursor(0,1);

    if(selection == 0)
      lcd.print(">Yes     No   ");
    pregnant = 1;
    else
      lcd.print(" Yes    >No   ");

    if(buttonPressed(BTN_NEXT))
      selection = !selection;

    if(buttonPressed(BTN_ENTER))
    {
      pregnant = (selection==0) ? "Yes":"No";
      selection = 0;
      stage++;
      lcd.clear();
      return;
    }
  }
}

//ANALYSIS SCREEN 
void analyzingScreen()
{
  lcd.setCursor(0,0);
  lcd.print("Analyzing...");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Iodine Level:");
  lcd.setCursor(0,1);
  lcd.print("Normal");

  delay(4000);

  stage = 0;
  lcd.clear();
}

void iodineLactating() //Iodine levels for lactating women
{
  if(iodineLevel < 100)
  {
    lcd.setCursor(0, 0);
    lcd.print("Insufficient");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  if(iodineLevel >= 100)
  {
    lcd.setCursor(0,0);
    lcd.print("Adquete");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Iodine Level: ");
    lcd.setCursor(0,1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }
}

void iodine() //Iodine levels for kids over 6
{
  if(iodineLevel < 20)
  {
    lcd.setCursor(0, 0);
    lcd.print("Severe Def");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if(iodineLevel >=20 && iodineLevel < 50)
  {
    lcd.setCursor(0, 0);
    lcd.print("Moderate Def");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if(iodineLevel>=50 && iodineLevel < 100)
  {
    lcd.setCursor(0, 0);
    lcd.print("Mild Deficiency");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if(iodineLevel>=100 && iodineLevel < 200)
  {
    lcd.setCursor(0, 0);
    lcd.print("Adequate");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if(iodineLevel>=200 && iodineLevel<300)
  {
    lcd.setCursor(0, 0);
    lcd.print("Over Adequate");
    lcd.setCursor(0, 1);
    lcd.print("Slight Risk");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if(iodineLevel>=300)
  {
    lcd.setCursor(0, 0);
    lcd.print("Excessive");
    lcd.setCursor(0, 1);
    lcd.print("Health Risk");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }
}

void pregnancyIodine() //Iodine levels for pregnant women
{
  if(iodineLevel < 150)
  {
    lcd.setCursor(0, 0);
    lcd.print("Insufficent");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if(iodineLevel >= 150 && iodineLevel < 250)
  {
    lcd.setCursor(0, 0);
    lcd.print("Adequate");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if(iodineLevel >= 250 && iodineLevel < 500)
  {
    lcd.setCursor(0, 0);
    lcd.print("Above Req");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  else if (iodineLevel >= 500)
  {
    lcd.setCursor(0, 0);
    lcd.print("Excessive");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }
}

void iodine6month()
{
  if(iodineLevel < 110)
  {
    lcd.setCursor(0, 0);
    lcd.print("Insufficient");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  if(iodineLevel >= 110)
  {
    lcd.setCursor(0,0);
    lcd.print("Adquete");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Iodine Level: ");
    lcd.setCursor(0,1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }
}

void iodine7to12month()
{
   if(iodineLevel < 130)
  {
    lcd.setCursor(0, 0);
    lcd.print("Insufficient");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  if(iodineLevel >= 130)
  {
    lcd.setCursor(0,0);
    lcd.print("Adquete");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Iodine Level: ");
    lcd.setCursor(0,1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }
}

void iodine1to8year()
{
  if(iodineLevel < 90)
  {
    lcd.setCursor(0, 0);
    lcd.print("Insufficient");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iodine Level:");
    lcd.setCursor(0, 1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }

  if(iodineLevel >= 90)
  {
    lcd.setCursor(0,0);
    lcd.print("Adquete");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Iodine Level: ");
    lcd.setCursor(0,1);
    lcd.print(iodineLevel);
    lcd.print(" ug/L");
  }
}

