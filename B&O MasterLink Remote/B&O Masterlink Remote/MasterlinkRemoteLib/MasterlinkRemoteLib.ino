/*
2017 created by Eric van der Knaap
 
This program listens for serial masterlink commands from the BEO stereo which are then translated to Sony TV commands
sent over the network. 

The program listen for masterlink commands on pin 9. If data is available, the data is read and stored in an char array. 
The tv_cmd_off is then send to a method that creates a HTTP body of the command. 
*/
 
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
#include "SonyTVMote.h"

#define beo_cmd_standby 17

SoftwareSerial mySerial(9, 8); // Software serial to receive masterlink commands
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Assign Mac address to the Etherner Shield
IPAddress board(192,168,178,10);   // Assign static IP to the ethernet shield
IPAddress tvIP(192,168,178,31);  // IP of the TV
EthernetClient client; // Initialize the Ethernet client library with the IP address and port of the server
SonyTvMote mote;

byte command [100]; // holds the masterlink command data
int counter = 0;

void setup() {
  // Open serial communications and wait for port to open
  //Serial.begin(9600);
  mote.begin("0000");
  
  // Initialize the network settings 
  Ethernet.begin(mac, board); // Settings of the board
  
  // give the Ethernet shield a second to initialize
  delay(1000);
  
  // Start the softwareSerial
  mySerial.begin(19200);
  
  Serial.println("setup complete");  
}

void loop()
{ 
  // Beo bits received
  while (mySerial.available()) {  
    delay(10); 
    int c =  mySerial.read();
    command[counter] = c;
    Serial.print(c,HEX); // For debugging
    counter++;
  }

  // New command received
  if(counter > 0)
  {
    Serial.println();
    counter = 0;
    String cmd = getBeoCommandFromBytes(command);
    if(cmd.length() > 3) sendBeoCommandToTv(cmd);      // Else something went wrong
  }
  
  // Read TV HTTP response to clear the client buffer
  while(client.connected() && client.available() > 0)
      client.read(); 
}

String getBeoCommandFromBytes(byte cmd [])
{  
  Serial.print(" The 8th bit: ");
  Serial.println(cmd[7]);
  
  if(cmd[7] == beo_cmd_standby)
  { 
    Serial.println("Turn off");
    return tv_off;
  }
  else if((cmd[8] == 171) && (cmd[9] == 221))
  {
    Serial.println("HDMI1");
    return tv_HDMI1;
  }
  else if((cmd[8] == 172) && (cmd[9] == 222))
  {
    Serial.println("HDMI2");
    return tv_HDMI2;
  }
  else return "err";  
}

void sendBeoCommandToTv(String cmd)
{
  Serial.println("connecting to send to tv");
  
  if(client.connected())
  {
    Serial.println("connected, sending command to tv");
    mote.sendPostRequest(client,cmd);
  }
  else 
  {
    client.stop();
    
    if(client.connect(tvIP,80))
    {
      Serial.println("connected, sending command to tv");
      mote.sendPostRequest(client,cmd);
    }
    else {
      Serial.println("Connection failed");
    }
    
  }
}







