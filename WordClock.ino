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
  while (!Serial) {
    ; //Waiting serial to start
  }

  //Setup pins
  pinMode(minutePin, INPUT);
  digitalWrite(minutePin, HIGH);

  pinMode(hourPin, INPUT);
  digitalWrite(hourPin, HIGH);
  
  //Init RTC
  setDate(0, 0, 0, 1, 1, 1, 0);

  //Init shift register library
  shiftOutX leds(latchPin, dataPin, clockPin, MSBFIRST, 3);

  leds.allOff();
  
  Serial.println("WordClock stared!");

}

void setupLeds(byte hours, byte minutes){
  leds.allOff();

  //it's

  byte minuteBlock = minutes / 5;
  switch (minuteBlock){
    case 0:
      //o'clock
      break;
    case 1:
      //five
      //past
      break;
    case 2:
      //ten
      //past
      break;
    case 3:
      //quarter
      //past
      break;
    case 4:
      //twenty
      //minutes
      //past
      break;
    case 5:
      //twenty
      //five
      //minutes
      //past
      break;
    case 6:
      //half
      //past
      break;
    case 7:
      hour++;
      //twenty
      //five
      //minutes
      //to
      break;
    case 8:
      hour++;
      //twenty
      //minutes
      //to
      break;
    case 9:
      hour++;
      //quarter
      //to
      break;
    case 10:
      hour++;
      //ten
      //to
      break;
    case 11:
      hour++;
      //five
      //to
      break;  
  }

  byte hourBlock = hours % 12;
  switch (hourBlock){
    case 0:
      //twelve
      break;
    case 1:
      //one
      break;
    case 2:
      //two
      break;
    case 3:
      //three
      break;
    case 4:
      //four
      break;
    case 5:
      //five
      break;
    case 6:
      //six
      break;
    case 7:
      //seven
      break;
    case 8:
      //eight
      break;
    case 9:
      //nine
      break;
    case 10:
      //ten
      break;
    case 11:
      //eleven
      break;  
  }
  
}

//Init runtime variables
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
int hourPressed, minutePressed;
  
void loop() {
  Serial.println("Loop started");

  //Check if hour adjust is pressed
  hourPressed = digitalRead(hourPin);
  if(hourPressed == 0){
    Serial.println("Hour adjust pressed!");
    incrementHour();
  }

  //Check if minute adjust is pressed
  minutePressed = digitalRead(minutePin);
  if(minutePressed == 0){
    Serial.println("Minute adjust pressed!");
    incrementMinute();
  }

  //Read date from RTC
  getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  setupLeds(hour, minute);
  
  //Print date
  Serial.print(hour, DEC);
  
  Serial.print(":");
  Serial.print(minute, DEC);

  Serial.print(":");
  Serial.println(second, DEC);

  //Delay loop
  delay(1000);

}

