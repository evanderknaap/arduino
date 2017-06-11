#include <SPI.h>         
#include <Ethernet.h>
#include <EthernetUdp.h>  
#include <SoftwareSerial.h>       

#define TV_POWER 1
#define TV_ON 1
#define TV_OFF 0
#define TV_OFFLINE 2
#define TV_ERROR -1
#define TV_TCP_PORT 20060
#define LAST_BYTE 23
#define TIMEOUT_TIME 3000 // If we did not receive a reponse after 3[sec] from the tv, we concluded it failed & we close the connection
#define RX_PORT 9
#define TX_PORT 10
#define beo_cmd_standby 17

IPAddress arduino_ip(192, 168, 178, 10); 
IPAddress tv_ip(192,168,178,31);  
byte arduino_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte tvMAC[] = {0xC0,0x38,0x96,0x67,0x1D,0xA9};  
byte abyTargetIPAddress[] = { 192, 168, 178, 255 };                                                          // Broadcast IP for Wake On Lan       
EthernetClient client;                          
EthernetUDP Udp;
SoftwareSerial mySerial(9, 8); 

byte snd_read[24] = {42,83,69,80,79,87,82,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,10};                     // read tv power state command
byte snd_on[24] = {42,83,67,80,79,87,82,'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1',10};       // switch power off command
byte snd_off[24] = {42,83,67,80,79,87,82,'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',10};      // switch power off command

byte response[24];
byte beo_command[100];
int counter = 0;

void setup() {
  
  Ethernet.begin(arduino_mac, arduino_ip);  // Initalize the Ethernet board 
  //Serial.begin(9600);       // Uncomment for debugging
  mySerial.begin(19200);
  delay(1000);
  Serial.println("setup complete");  
}

void loop() {

  // Beo bits received
  while (mySerial.available()) {  
    delay(10); 
    int c =  mySerial.read();
    beo_command[counter] = c;
    Serial.print(c,HEX); // For debugging
    counter++;
  }

  // New command received
  if(counter > 0)
  {
    Serial.println();
    counter = 0;

    Serial.println("Connecting"); 
    if(!client.connect(tv_ip,TV_TCP_PORT)) // Does not matter the key, if the tv is offline - WakeOnLan 
    {
      Serial.println("TV not online, waking on lan");
      wakeTVOnLan();
    }
    else
    {
      Serial.println("Connected");
      int state = getTVPowerState();
      if(client.connect(tv_ip, TV_TCP_PORT)) sendBeoCommandFromBytesAndTvState(beo_command, state);
      else Serial.println("connection failed");
    }
  }
  
}

void sendMessage( byte message [])
{
  if (!client.connected()) return;

  float starttime = millis();
  float timepassed = 0;
  
  Serial.println("Sending message");
  
  for(int i=0; i <= LAST_BYTE ;i++)
  {
    Serial.print((char)message[i]);
    client.write(message[i]);
  }

   // Stay in this loop untill we received a respone
   while (client.connected()){
      while (client.available() > 0)
      {
        response[counter] = client.read();
        Serial.print((char)response[counter]);
        counter++;
        
        if(client.available() == 0) 
        {
          counter = 0;
          Serial.println("Message received, Disconnecting");
          client.stop();
        }
      }

      timepassed = millis() - starttime;
      if(timepassed > TIMEOUT_TIME && client.available() == 0)
      {
        Serial.println("Timed out");
        client.stop();
      }
   }

   if(!client.connected()) Serial.println("Disconnected");
}

int getTVPowerState()
{
    if(!client.connected()) return TV_OFFLINE;
    sendMessage(snd_read); // get the power state of the tv 
      
    // check the response, 1 is on, 2 is off F is error 
    if (response[22] == '1') return TV_ON;
    else if (response[22] == '0') return TV_OFF;
    else return TV_OFFLINE;
}

void wakeTVOnLan()
{
  const int nMagicPacketLength = 102;
  byte abyMagicPacket[nMagicPacketLength] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  byte abyTargetIPAddress[] = { 192, 168, 178, 255 }; // broadcast
    
  // Compose magic packet to wake remote machine. 
  for (int ix = 6; ix < 102; ix++) {
    abyMagicPacket[ix] = tvMAC[ix % 6];
  }
  
  Udp.begin(7);
  if (Udp.beginPacket(abyTargetIPAddress, 7)) {
    Serial.println("sending package");
    Udp.write(abyMagicPacket, nMagicPacketLength);
    Udp.endPacket();
  }

  Udp.stop();
}

void sendBeoCommandFromBytesAndTvState(byte cmd [], int state)
{  
  Serial.print(" The 8th bit: ");
  Serial.println(cmd[7]);
  
  if(cmd[7] == beo_cmd_standby)
  { 
    if(state == TV_ON) 
    {
      Serial.println("Switch tv off");
      sendMessage(snd_off);
    }
    else if(state == TV_OFF)
    {
      Serial.println("Switch tv on");
      sendMessage(snd_on);
    }
    else;
  }  
}



