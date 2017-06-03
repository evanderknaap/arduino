/*
2017 created by Eric van der Knaap
 
This example code creates a SonyTvMote object, and sends the "switch off" commands to the TV over ethernet. 
This sketch require you to use an ethernetshield to connec to your TV, and the HTTP control password.

On a Sony bravia smart tv, you can set the 4 digit password in the following menu item

To make sure you can connect to the tv, assign a static IP adress to your tv. You can find this in the menu of your tv in:
*/

#include <SonyTVMote.h> 
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Assign Mac address to the Ethernet Shield
IPAddress board(192,168,178,10);                     // Assign static IP to the ethernet shield
IPAddress tvIP(192,168,178,31);                      // Assign the IP of the TV. To make 
EthernetClient client;                               // Initialize the Ethernet client library with the IP address and port of the server
SonyTvMote mote;                                     // Create a SonyTvMote object

int counter = 0;

void setup() {
  // Open serial communications and wait for port to open
  Serial.begin(9600);
  
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  // Initialize the network settings and the SonyTvMote object
  Ethernet.begin(mac, board); // Settings of the board
  mote.begin("0000");         // Initialize the mote object, with the password of the tv - "" - if no password is needed
  
  // give the Ethernet shield a second to initialize
  delay(1000);
  
  // Connect to the TV
  client.connect(tvIP,80);
  if (client.connected())
  {
    Serial.println("Setup complete, shutting down tv");
    mote.sendPostRequest(client,mote.tv_HDMI2);
  }
  else
  {
    client.stop();
    Serial.println("connection failed");
  }
}


void loop()
{   
  // Read TV HTTP response to clear the client buffer. I
  // If the connections is closed, but bytes are availble to read -  the client will return its connected.
  while(client.available() > 0)
      Serial.print((char)client.read()); 

  counter++;
  while (counter>0);
}







