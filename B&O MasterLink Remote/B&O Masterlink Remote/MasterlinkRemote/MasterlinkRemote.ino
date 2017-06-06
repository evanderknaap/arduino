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

#define beo_cmd_standby 0x42
#define beo_src_master 0xC1
#define beo_src_all 0x83
#define tv_cmd_off "AAAAAQAAAAEAAAAvAw=="  
#define tv_cmd_HDMI2 "AAAAAgAAABoAAABbAw=="
#define tv_cmd_HDMI3 "AAAAAgAAABoAAABcAw=="

SoftwareSerial mySerial(9, 8); // Software serial to receive masterlink commands
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Assign Mac address to the Etherner Shield
IPAddress board(192,168,178,10);   // Assign static IP to the ethernet shield
IPAddress server(192,168,178,31);  // IP of the TV
EthernetClient client; // Initialize the Ethernet client library with the IP address and port of the server

byte command [100]; // holds the masterlink command data
int counter = 0;

void setup() {
  // Open serial communications and wait for port to open
  Serial.begin(9600);
  
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
  // if a BEO cmd is received, read it
  while (mySerial.available()) {  
    delay(10); 
    byte c =  mySerial.read();
    command[counter] = c;
    Serial.print(c,HEX); // For debugging
    counter++;
  }
  
  if(counter > 0)
  {
  //  Serial.println();
   // Serial.println("New command received");
    counter = 0;
    processCommand(command);
  }
  
  // Read TV HTTP response to clear the client buffer
  while(client.connected() && client.available() > 0)
      client.read(); 
}

void processCommand(byte cmd [])
{  
  if(cmd[1] == beo_src_master || cmd[1] == beo_src_all) // basically, just turn off in any case
   {
      Serial.println("Turn off thet tv");
      sendTvCommand(myCommandPackage(tv_cmd_off));
   }    
}

void sendTvCommand(String package)
{
  // Serial.println("connecting...");
  if(client.connected()) {
        // Make a HTTP request:
        client.println("POST /sony/IRCC HTTP/1.1");
        client.println("Host: 192.168.178.31");
        client.println("User-Agent: Arduino/1.0");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
        client.print("Content-Length: ");
        client.println(package.length());
        client.println("X-Auth-PSK: 0000");
        client.println(); // end of the header 
        client.println(package); }
  else{ 
      client.stop(); // close the connection, in order to make a new one
      
      if(client.connect(server, 80)){
        //Serial.println("connected");
        //Serial.println(package.length());

        // Make a HTTP request:
        client.println("POST /sony/IRCC HTTP/1.1");
        client.println("Host: 192.168.178.31");
        client.println("User-Agent: Arduino/1.0");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
        client.print("Content-Length: ");
        client.println(package.length());
        client.println("X-Auth-PSK: 0000");
        client.println(); // end of the header 
        client.println(package);
      }  
      else {
      // connecting failed
      // kf you didn't get a connection to the server:
      //Serial.println("connection failed");
      }
  } 
}

String myCommandPackage(String command)
{
   String data ="<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
   data.concat("<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">");
   data.concat("<s:Body>");
   data.concat("<u:X_SendIRCC xmlns:u=\"urn:schemas-sony-com:service:IRCC:1\">");
   data.concat("<IRCCCode>");
   data.concat(command);
   data.concat("</IRCCCode>");
   data.concat("</u:X_SendIRCC>");
   data.concat("</s:Body>");
   data.concat("</s:Envelope>");
   return data;
}




