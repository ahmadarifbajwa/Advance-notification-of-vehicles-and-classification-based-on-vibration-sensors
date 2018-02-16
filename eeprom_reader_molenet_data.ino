#include <SPI.h>
#include <SPIEEP.h>       //from: https://bitbucket.org/spirilis/spieep/

SPIEEP eep(16,128,65536);          //for 512kBit model, 
                                  //library author says: eep(16,128,65536)
#define eep_CSPIN     9         //eep CS
/** the current addr in the eep (i.e. which byte we're going to write to next) **/
uint32_t addr = 0 ;   //Starting memory address to store data

int value;
 
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  eep.begin_spi(eep_CSPIN);
  eep.wakeup();                    //Wake up eep for writing data
 
}

void loop() {
    Serial.println(addr);
    Serial.print("Time is : ");
    value = eep.read_int(addr);
    Serial.print(value);
    Serial.print(":");
    value = eep.read_int(addr+2);
    Serial.print(value);
    Serial.print(":");
    value = eep.read_int(addr+4);
    Serial.print(value);
    Serial.print(" ");
    
    int bytePeak = eep.read_int(addr+6);
    Serial.print(bytePeak);
    //bytePeak = eep.read_int(addr+10);
    //Serial.print(".");
    //Serial.print(bytePeak);
    Serial.print(" ");

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
    Serial.println(" ");

    addr = addr + 29;

    delay(100);

}
