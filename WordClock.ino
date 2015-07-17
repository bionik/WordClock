#include <Wire.h>
#define DS1337_I2C_ADDRESS 0b1101000

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
}

void getDate(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  
  // Reset the register pointer
  Wire.beginTransmission(DS1337_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  
  Wire.requestFrom(DS1337_I2C_ADDRESS, 7);
  
  if (Wire.available() == 7){
    // A few of these need masks because certain bits are control bits
    *second     = bcdToDec(Wire.read() & 0x7f);
    *minute     = bcdToDec(Wire.read());
    *hour       = bcdToDec(Wire.read() & 0x3f); 
    *dayOfWeek  = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month      = bcdToDec(Wire.read());
    *year       = bcdToDec(Wire.read());
  } else {
    Serial.println("ERROR: Can't read shit, captain");
  }
  
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) {
    ; //Waiting serial to start
  }
  
  setDate(1, 1, 1, 1, 1, 1, 1);
  
  Serial.println("WordClock stared!");
}

void loop() {
  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  
  Serial.println("Loop started");
  
  getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  
  Serial.print("Seconds: ");
  Serial.println(second, DEC);
    
  delay(1000);
  
}

