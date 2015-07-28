#include <Wire.h>

#include <ShiftOutX.h>
#include <ShiftPinNo.h>

#define DS1337_I2C_ADDRESS 0b1101000

//For setting time
#define minutePin 12
#define hourPin 11

//Shift register stuff
#define dataPin 4
#define clockPin 5
#define latchPin 6

//Init shift register library as a global variable
shiftOutX leds(latchPin, dataPin, clockPin, MSBFIRST, 3);

//Button pressed state
boolean buttonPressed = false;
boolean timePrinted = false;

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void setDate(byte second, byte minute, byte hour,  byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  Wire.beginTransmission(DS1337_I2C_ADDRESS);
  Wire.write(0);
   
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
   
  Wire.endTransmission();

  //Enable clock
  /*Wire.beginTransmission(DS1337_I2C_ADDRESS);
  Wire.write(0x0e);
  Wire.write(0b00011000);
  Wire.endTransmission(); */
}

void getDate(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  
  // Reset the register pointer
  Wire.beginTransmission(DS1337_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  
  Wire.requestFrom(DS1337_I2C_ADDRESS, 7);
  
  if (Wire.available() == 7){
    // A few of these need masks because certain bits are control bits
    *second     = bcdToDec(Wire.read());
    *minute     = bcdToDec(Wire.read());
    *hour       = bcdToDec(Wire.read() & 0b00111111); 
    *dayOfWeek  = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month      = bcdToDec(Wire.read() & 0b01111111);
    *year       = bcdToDec(Wire.read());
  } else {
    Serial.println("ERROR: Can't read shit, captain");
  }
  
}

void incrementMinute(){
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  byte newMinute = minute + 1;
  newMinute = newMinute % 60;

  second = 0;

  setDate(second, newMinute, hour, dayOfWeek, dayOfMonth, month, year);  
}

void incrementHour(){
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  byte newHour = hour + 1;
  newHour = newHour % 24;

  setDate(second, minute, newHour, dayOfWeek, dayOfMonth, month, year);  
}

void setup() {

  //Init I2C
  Wire.begin();

  //Init serial
  Serial.begin(9600);
  /*while (!Serial) {
    ; //Waiting serial to start
  }*/

  //Setup pins
  pinMode(minutePin, INPUT);
  digitalWrite(minutePin, HIGH);

  pinMode(hourPin, INPUT);
  digitalWrite(hourPin, HIGH);
  
  //Init RTC
  setDate(0, 0, 0, 1, 1, 1, 0);

  leds.allOff();
  
  Serial.println("WordClock stared!");

}

void setupLeds(byte hours, byte minutes){
  leds.allOff();

  //it's
  leds.pinOn(shPin1);

  byte minuteBlock = minutes / 5;
  switch (minuteBlock){
    case 0:
      //o'clock
      leds.pinOn(shPin22);
      break;
    case 1:
      //five
      leds.pinOn(shPin6);
      //past
      leds.pinOn(shPin8);
      break;
    case 2:
      //ten
      leds.pinOn(shPin2);
      //past
      leds.pinOn(shPin8);
      break;
    case 3:
      //quarter
      leds.pinOn(shPin5);
      //past
      leds.pinOn(shPin8);
      break;
    case 4:
      //twenty
      leds.pinOn(shPin4);
      //minutes
      leds.pinOn(shPin7);
      //past
      leds.pinOn(shPin8);
      break;
    case 5:
      //twenty
      leds.pinOn(shPin4);
      //five
      leds.pinOn(shPin6);
      //minutes
      leds.pinOn(shPin7);
      //past
      leds.pinOn(shPin8);
      break;
    case 6:
      //half
      leds.pinOn(shPin3);
      //past
      leds.pinOn(shPin8);
      break;
    case 7:
      hours++;
      //twenty
      leds.pinOn(shPin4);
      //five
      leds.pinOn(shPin6);
      //minutes
      leds.pinOn(shPin7);
      //to
      leds.pinOn(shPin9);
      break;
    case 8:
      hours++;
      //twenty
      leds.pinOn(shPin4);
      //minutes
      leds.pinOn(shPin7);
      //to
      leds.pinOn(shPin9);
      break;
    case 9:
      hours++;
      //quarter
      leds.pinOn(shPin5);
      //to
      leds.pinOn(shPin9);
      break;
    case 10:
      hours++;
      //ten
      leds.pinOn(shPin2);
      //to
      leds.pinOn(shPin9);
      break;
    case 11:
      hours++;
      //five
      leds.pinOn(shPin6);
      //to
      leds.pinOn(shPin9);
      break;  
  }

  byte hourBlock = hours % 12;
  switch (hourBlock){
    case 0:
      //twelve
      leds.pinOn(shPin21);
      break;
    case 1:
      //one
      leds.pinOn(shPin10);
      break;
    case 2:
      //two
      leds.pinOn(shPin11);
      break;
    case 3:
      //three
      leds.pinOn(shPin12);
      break;
    case 4:
      //four
      leds.pinOn(shPin13);
      break;
    case 5:
      //five
      leds.pinOn(shPin14);
      break;
    case 6:
      //six
      leds.pinOn(shPin15);
      break;
    case 7:
      //seven
      leds.pinOn(shPin16);
      break;
    case 8:
      //eight
      leds.pinOn(shPin17);
      break;
    case 9:
      //nine
      leds.pinOn(shPin18);
      break;
    case 10:
      //ten
      leds.pinOn(shPin19);
      break;
    case 11:
      //eleven
      leds.pinOn(shPin20);
      break;  
  }
  
}

//Init runtime variables
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
int hourPressed, minutePressed;
  
void loop() {
  //Serial.println("Loop started");

  //Check if hour adjust is pressed
  hourPressed = digitalRead(hourPin);
  if(hourPressed == 0 && !buttonPressed){
    buttonPressed = true;
    Serial.println("Hour adjust pressed!");
    incrementHour();
    timePrinted = false;
  }

  //Check if minute adjust is pressed
  minutePressed = digitalRead(minutePin);
  if(minutePressed == 0 && !buttonPressed){
    buttonPressed = true;
    Serial.println("Minute adjust pressed!");
    incrementMinute();
    timePrinted = false;
  }

  if(hourPressed != 0 && minutePressed != 0){
    buttonPressed = false; 
  }

  //Read date from RTC
  getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  setupLeds(hour, minute);

  if(buttonPressed && !timePrinted){
    //Print date
    Serial.print(hour, DEC);
    
    Serial.print(":");
    Serial.print(minute, DEC);
  
    Serial.print(":");
    Serial.println(second, DEC);

    timePrinted = true;
  }

  //Delay loop
  delay(2);

}

