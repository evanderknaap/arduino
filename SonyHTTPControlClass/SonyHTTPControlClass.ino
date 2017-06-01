/*
2017 created by Eric van der Knaap
 
This program listens for serial masterlink commands from the BEO stereo which are then translated to Sony TV commands
sent over the network. 

The program listen for masterlink commands on pin 9. If data is available, the data is read and stored in an char array. 
The tv_cmd_off is then send to a method that creates a HTTP body of the command. 
*/
 
#include <SPI.h>
#include <Ethernet.h>

#define tv_cmd_off "AAAAAQAAAAEAAAAvAw=="  
#define tv_cmd_HDMI2 "AAAAAgAAABoAAABbAw=="
#define tv_cmd_HDMI3 "AAAAAgAAABoAAABcAw=="

#define SUCCESS 1
#define TIMED_OUT -1
#define INVALID_SERVER -2
#define TRUNCATED -3
#define INVALID_RESPONSE -4

#define DEFAULT_PORT 80

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Assign Mac address to the Ethernet Shield
IPAddress board(192,168,178,10);   // Assign static IP to the ethernet shield
IPAddress tvIP(192,168,178,31);  // IP of the TV
EthernetClient client; // Initialize the Ethernet client library with the IP address and port of the server

void setup() {
  // Open serial communications and wait for port to open
  Serial.begin(9600);
  
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  // Initialize the network settings 
  Ethernet.begin(mac, board); // Settings of the board
  
  // give the Ethernet shield a second to initialize
  delay(1000);

  Serial.println("setup complete");  
  
  sendTvCommand(tv_cmd_off);
}


void loop()
{ 
  // Read TV HTTP response to clear the client buffer
  while(client.available() > 0)
      Serial.println(client.read()); 
}

void sendPostRequest(EthernetClient client, String package)
{
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

void sendTvCommand(EthernetClient client, String cmd)
{
  // Create a message body 
  String package = getCommandPackage(cmd);
  
  // Serial.println("connecting...");
  if(client.connected()) sendPostRequest(package);
  else{ 
      client.stop(); // close the connection, in order to make a new one
      if(client.connect(tvIP, DEFAULT_PORT)) sendPostRequest(client, package);
      else Serial.println("connection failed");
  }
}

String getCommandPackage(String command)
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

//// Underlying Ethernet Functionality that we want to expose to the user
//
//// returns the number of bytes are availble for reading
//int available()
//{
//  return client.available();
//}
//
//// checks if we are connected to our TV, return true or false
//boolean connected()
//{
//  return client.connected();
//}
//
///* 
//Connects to the TV, returns one of the following responses to the user
//  SUCCESS 1
//  TIMED_OUT -1
//  INVALID_SERVER -2
//  TRUNCATED -3
//  INVALID_RESPONSE -4
//*/
//int connect()
//{
//  return client.connect(tvIP, DEFAULT_PORT);
//  return 1;
//}
//
//// Disconnect from the tv
//void stop()
//{
//  client.stop();
//}
//
//// read the next tv byte
//byte read()
//{
//  return client.read();
//}






