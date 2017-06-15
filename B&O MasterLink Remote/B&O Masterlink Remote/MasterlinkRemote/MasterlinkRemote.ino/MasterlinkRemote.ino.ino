#include <SPI.h>         
#include <Ethernet.h>
#include <EthernetUdp.h>  
#include <SoftwareSerial.h>       

#define TV_ON 1
#define TV_OFF 0
#define TV_OFFLINE 2
#define TV_TCP_PORT 20060
#define LAST_BYTE 23
#define TIMEOUT_TIME 4000 // If we did not receive a reponse after 3[sec] from the tv, we concluded it failed & we close the connection
#define RX_PORT 9
#define TX_PORT 10
#define beo_cmd_standby 0x11
#define beo_master_audio 0xC1
#define beo_HMDI1_1 0xAB
#define beo_HMDI1_2 0xDD
#define beo_HMDI2_1 0xAC
#define beo_HMDI2_2 0xDE
#define beo_HMDI3_1 0xAD
#define beo_HMDI3_2 0xDF

IPAddress arduino_ip(192, 168, 178, 10); 
IPAddress tv_ip(192,168,178,31);  
byte arduino_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte tvMAC[] = {0xC0,0x38,0x96,0x67,0x1D,0xA9};  
byte abyTargetIPAddress[] = { 192, 168, 178, 255 };                                                          // Broadcast IP for Wake On Lan       
EthernetClient client;                          
EthernetUDP Udp;
SoftwareSerial mySerial(RX_PORT, TX_PORT); 

//byte snd_read[24] = {42,83,69,80,79,87,82,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,10};                     // read tv power state command
//byte snd_on[24] = {42,83,67,80,79,87,82,'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1',10};       // switch power off command
//byte snd_off[24] = {42,83,67,80,79,87,82,'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',10};      // switch power off command
const unsigned char snd_off[25] = "*SCPOWR0000000000000000\n";
const unsigned char snd_on[25] = "*SCPOWR0000000000000001\n";
const unsigned char snd_read[25] = "*SEPOWR################\n";
const unsigned char snd_HDMI1[25] = "*SCINPT0000000100000001\n";
const unsigned char snd_HDMI2[25] = "*SCINPT0000000100000002\n";
const unsigned char snd_HDMI3[25] = "*SCINPT0000000100000003\n";
const unsigned char snd_error[3] = "-1";

byte response[24];
byte beo_command[100];
int counter = 0;

void setup() {
  
  Ethernet.begin(arduino_mac, arduino_ip);  // Initalize the Ethernet board 
 // Serial.begin(9600);       // Uncomment for debugging
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
    Serial.println(c,HEX); // For debugging
    counter++;
    if(mySerial.overflow()) Serial.println("overflow");
  }

  // New command received
  if(counter > 0)
  {
    Serial.println();
    counter = 0;

   const unsigned char * cmd = didReceiveStandbyCommand(beo_command);
   Serial.print("size off ");
   Serial.println(sizeof(cmd));
   Serial.println(cmd[0]);
   if(cmd[0] == '-') return; // do nothing if we didn't receive a standby command

    Serial.println("Check connection"); 
    if(client.connect(tv_ip,TV_TCP_PORT)) // Does not matter the key, if the tv is offline - WakeOnLan 
    {
      Serial.println("Connected");
      int state = getTVPowerState();
      if(client.connect(tv_ip, TV_TCP_PORT) && state == TV_ON) sendMessage(cmd);
      else Serial.println("connection failed");
    }
    else
    {
      Serial.println("TV not online, do nothing");
    }
  }
  
}

void sendMessage( const unsigned char message [])
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

const unsigned char * didReceiveStandbyCommand(byte cmd [])
{
  if(cmd[7] == beo_cmd_standby && cmd[1] == beo_master_audio)
  {
    Serial.println("its mater audio telling to shut down");
    return snd_off;
  }
  else if (cmd[1] == beo_master_audio && cmd[11] == beo_HMDI1_1 && cmd[12] == beo_HMDI1_2)
  {
    Serial.println("Its master audio telling HDMI 1");
    return snd_HDMI1;
  }
  else if (cmd[1] == beo_master_audio && cmd[11] == beo_HMDI2_1 && cmd[12] == beo_HMDI2_2)
  {
    Serial.println("Its master audio telling HDMI2");
    return snd_HDMI2;
  }
   else if (cmd[1] == beo_master_audio && cmd[11] == beo_HMDI3_1 && cmd[12] == beo_HMDI3_2)
  {
     Serial.println("Its master audio telling HDMI3");
    return snd_HDMI3;
  }
  else
  {
    return snd_error;
  }
}


void sendBeoCommandFromBytesAndTvState(const unsigned char * cmd, int state)
{  
  Serial.print(" The 8th bit: ");
  Serial.println(cmd[7]);
  
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
}





