#include "Beomote.h"

int irPin = 10;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Beo.initialize(irPin);
  
  Serial.println("Setup done");
}

void loop() {
  BeoCommand cmd;
  
  if (Beo.receive(cmd)) {  
    Serial.print(cmd.link, HEX);
    Serial.print(cmd.address, HEX);
    Serial.println(cmd.command, HEX);
    
    if(cmd.command == 0x36)
    {
      Serial.println("Succes");  
    }
  }
  
  
}
