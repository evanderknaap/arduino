
#include "TimerOne.h"
#include "SPI.h"
#include "Ethernet.h"
#include "SonyTVMote.h"

// Common divisors for 3125 = 1, 5, 25, 125, 625, 3125

// This code creates a timer at 40kHz on pin 13. It checks if the PIN is high and low. 
// On Pin 9 an IR receiver is attached, to listen to B&O commands using IR signals 
// Digital pin 9, is B5 register

// Defining the Bang & Olufsen commands
#define TICK 125 // 125 micro seconds  

#define BEO_ZERO    3125
#define BEO_SAME    6250
#define BEO_ONE     9375
#define BEO_STOP    12500 
#define BEO_START   15625

int lastPinState = 0;
int index;
int ticks  = 0;
int message [20]; //holds the messagebody
int dibs [20]; //holds the messagebody
int command = 0;
int address = 0;
bool received = false;

long freq =  1000000/TICK;
int prescale = 16000000/(8*freq)-1;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Assign Mac address to the Ethernet Shield
IPAddress board(192,168,178,10);                     // Assign static IP to the ethernet shield
IPAddress tvIP(192,168,178,31);                      // Assign the IP of the TV. To make 
EthernetClient client;                               // Initialize the Ethernet client library with the IP address and port of the server
SonyTvMote mote;

void setup(){
 
  Serial.begin(9600);
  Ethernet.begin(mac, board);
  mote.begin("0000");
  
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  DDRB = DDRB & B11111101;   // Set Pin 10 as input, leave the rest of the pins as - is
  PORTB = DDRB & B11111101; // Set Pin 10 to LOW, leave the rest of the registers untouched

  Timer1.initialize(TICK); // set a timer to count pulses
  Timer1.attachInterrupt(processTick); // attach the service routine here
 
  cli();//stop interrupts
  
    //set timer1 interrupt at 1Hz
    TCCR0A = 0;// set entire TCCR1A register to 0
    TCCR0B = 0;// same for TCCR1B
    TCNT0  = 0;//initialize counter value to 0
    // set compare match register for 40Khz increments
    OCR0A = prescale ;// = (16*10^6) / (8*40000) - 1 (must be <65536)
    // turn on CTC mode
    TCCR0A |= (1 << WGM01);
    // Set 8 bit prescaler
    TCCR0B |= (1 << CS01);  
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A);
   
  sei();//allow interrupts
  
  delay(1000);
  Serial.println("setup done");

  if (client.connect(tvIP,80)) Serial.println("connected");
  else Serial.println("connection failed");

}

ISR(TIMER0_COMPA_vect)
{
  
}

boolean didReceivePulse()
{  
  // Check if we changed from HIGH to LOW
  int currentPinState = PINB;                     // Read the states of pin 8 - 13
  currentPinState = currentPinState & B00000010;  // Read out the state of PIN 10 only
  int change = currentPinState - lastPinState;
  lastPinState = currentPinState;
  
  if (change < 0) return true;
  else return false;
}

void processTick()
{ 
  if (received) return; // If we just received a message, process before moving on to the next

  ticks++;
  
  // Check if we received a pulse in this tick
  if(!didReceivePulse()) return;                      

  int pulse = ticks * TICK;
  index++;
  ticks = 0;
  message[index] = pulse;

  if (index > 19) index =  -1;
  if ((pulse == BEO_STOP))
  {
    //Timer1.detachInterrupt();
    received = true;
    return;
  }
  
  if (pulse == BEO_START)
  {
    reset();
    return;
  }
}


void reset() 
{ 
  index = -1;
  received = false;
}

void readBits()
{ 
  // Read the pulse, and save it into 1 - 0 values
  if (!received) return;
  
  for (int ind = 1; ind < 17; ind ++)
  {
    int pulse = message[ind];
    if (pulse >= (BEO_ZERO-TICK) && pulse <= (BEO_ZERO+TICK)) dibs[ind] = 0;
    else if (pulse >= (BEO_ONE-TICK) && pulse <= (BEO_ONE+TICK)) dibs[ind] = 1;
    else if (pulse >= (BEO_SAME-TICK) && pulse <= (BEO_SAME+TICK)) dibs[ind] = dibs[ind-1];
    else if (pulse >= (BEO_START-TICK) && pulse <= (BEO_START+TICK));
    else if (pulse >= (BEO_STOP-TICK) && pulse <= (BEO_STOP+TICK));
    else; // failed to recognize
  } 

  // Get the int value of the Bits
  int base = 1; 
  int back_count = 16;
  command = dibs[back_count];
  address = dibs[back_count-8];
  while (back_count > 8)
  {
    back_count--;
    base *= 2;
    command += base*dibs[back_count];
    address += base*dibs[back_count-8];
  }

  Serial.println(command);
    
  reset();
  //Timer1.attachInterrupt(processTick);
}

void loop(){

  if(received)
  {
    readBits();  
    if(command == 12)
    {
      if(!client.connected()) client.connect(tvIP,80);
      mote.sendPostRequest(client,tv_off);
    }
  }

  while (client.available())
      client.read();
}

