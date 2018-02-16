#include <SPI.h>
#include <SPIEEP.h>       //from: https://bitbucket.org/spirilis/spieep/


#include <Wire.h> // for accelerometer
#include <ADXL345.h> // for accelerometer

#include "rv8523_clock_alarm.h" //RTC library

//Sound sensor variables
int sound_din=11;
int sound_ain=23;
int ad_value;
int counterOne = 1500;
int counterTwo = 0;
long sumOne = 0;
long sumTwo = 0;
int avgOne = 0;
int avgTwo = 0;
int minLimit = 0;
int maxLimit = 1023;
int trigPrint = 0;
 
ADXL345 adxl; //variable adxl is an instance of the ADXL345 library // for accelerometer

double saX = 0.0; // for accelerometer
double saY = 0.0; // for accelerometer
double saZ = 0.0; // for accelerometer
int counter = 0; // for accelerometer

//RTC variables
uint8_t   sec, minutes, hours, days, weekdays, months;
uint8_t years; //to store time data

// Create object of RTC class
RV8523_RTC rtc;

SPIEEP eep(16,128,65536);          //for 512kBit model, 
                                  //library author says: eep(16,128,65536)
#define eep_CSPIN     9         //eep CS
/** the current address in the eep (i.e. which byte we're going to write to next) **/
uint32_t addr = 0 ;   //Starting memory address to store data
int value;
 
void setup() {
  Serial.begin(115200);
 
  adxl.powerOn(); //power on accelerometer

  rtc.setTimeMode(RV8523_RTC::TM_24HOUR);
  //rtc.setClockTime(0,25,10, 14,2,18);
  rtc.startRTC();
  //rtc.batterySwitchOver(1);

  eep.begin_spi(eep_CSPIN);
  eep.wakeup();                    //Wake up eep for writing data

}

void loop() {


//Real Time Clock block
    
    rtc.getClockTime(&sec, &minutes, &hours, &days, &months, &years);


//Sound Sensor block
    
    ad_value=analogRead(sound_ain);

    if (counterOne > 0) {
      Serial.println(ad_value);
    }
    if (counterOne < 501) {
      sumOne = sumOne + ad_value; 
    }

    if (counterOne == 1) {
      //Serial.println(sumOne);
      //Serial.print("Average is: ");
      avgOne = sumOne / 500 ;
      //Serial.println(avgOne);
      minLimit = avgOne - 10;
      //Serial.print("Min: ");
      //Serial.println(minLimit);
      maxLimit = avgOne + 10;
      //Serial.print("Max: ");
      //Serial.println(maxLimit);
      sumOne = 0;
    }
    if(counterOne > 0) {
      counterOne--;
    }
    
  

    if (ad_value < minLimit || ad_value > maxLimit) {
      trigPrint = 1;
      //Serial.println(trigPrint);
      //Serial.println(ad_value);
      counterTwo = 0;
      sumTwo = 0;
      avgTwo = 0;
    }

    if (counterTwo < 10) {
      sumTwo = sumTwo + ad_value;
      counterTwo++;
    }
    if (counterTwo == 10) {
      avgTwo = sumTwo / 10;
      counterTwo = 0;
      sumTwo = 0;
      //Serial.print("Average 2: ");
      //Serial.println(avgTwo);
    }
    if (avgTwo > minLimit && ad_value < maxLimit) {
      trigPrint = 0;
      //Serial.println(trigPrint);
      //Serial.println(ad_value);
    }
    if (trigPrint == 1) {
      Serial.print("Time is : ");
      Serial.print(hours);
      Serial.print(":");
      Serial.print(minutes);
      Serial.print(":");
      Serial.print(sec);
      Serial.print(" ");
      eep.write_int(addr, hours);
      eep.write_int(addr+2, minutes);
      eep.write_int(addr+4, sec);
      value = eep.read_int(addr);
      Serial.print(value);
      Serial.print(":");
      value = eep.read_int(addr+2);
      Serial.print(value);
      Serial.print(":");
      value = eep.read_int(addr+4);
      Serial.print(value);
      Serial.print(" ");
      Serial.print(ad_value);
      Serial.print(" ");
      eep.write_int(addr+6, ad_value);
      int soundLevel = eep.read_int(addr+6);
      Serial.print(soundLevel);
      Serial.print(" ");
    
    }

  
//Accelerometer block

    double xyz[3];
    double ax,ay,az;
    adxl.getAcceleration(xyz);
    ax = xyz[0];
    ay = xyz[1];
    az = xyz[2];
    saX = ax;
    saY = ay;
    saZ = az;
    counter = 1 ;
    if (trigPrint == 1) {
      saX = saX/counter ;
      saY = saY/counter ;
      saZ = saZ/counter ;
      Serial.print(saX);
      Serial.print(" ");
      int X= saX * 100;
      if (X < 0) {
        char sign = '-';
        X = -1 * X;
        eep.write(addr+14, sign);
        eep.write_int(addr+15, X);
      }
      else {
        char sign = '+';
        eep.write(addr+14, sign);
        eep.write_int(addr+15, X);
      }
      
      Serial.print(saY);
      Serial.print(" ");
      int Y= saY * 100;
      if (Y < 0) {
        char sign = '-';
        Y = -1 * Y;
        eep.write(addr+19, sign);
        eep.write_int(addr+20, Y);
      }
      else {
        char sign = '+';
        eep.write(addr+19, sign);
        eep.write_int(addr+20, Y);
      }
      
      Serial.print(saZ);
      Serial.print(" ");
      int Z= saZ * 100;
      if (Z < 0) {
        char sign = '-';
        Z = -1 * Z;
        eep.write(addr+24, sign);
        eep.write_int(addr+25, Z);
      }
      else {
        char sign = '+';
        eep.write(addr+24, sign);
        eep.write_int(addr+25, Z);
      }
      
      char byteXsign = eep.read(addr+14);
      Serial.print(byteXsign);
      int byteXinteger = eep.read_int(addr+15);
      Serial.print(byteXinteger);
      Serial.print(" ");

      char byteYsign = eep.read(addr+19);
      Serial.print(byteYsign);
      int byteYinteger = eep.read_int(addr+20);
      Serial.print(byteYinteger);
      Serial.print(" ");

      char byteZsign = eep.read(addr+24);
      Serial.print(byteZsign);
      int byteZinteger = eep.read_int(addr+25);
      Serial.print(byteZinteger);
      Serial.print(" ");
      //Serial.print(addr);
      Serial.println(" ");

      saX = 0;
      saY = 0;
      saZ = 0;
      counter = 0;
      addr = addr+29;
    }

    
    delay(100);

}
