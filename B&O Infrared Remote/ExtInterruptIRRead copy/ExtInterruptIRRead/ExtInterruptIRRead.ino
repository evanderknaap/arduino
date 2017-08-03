
// This code creates a hardware interrupt on pin 2 of the arduino UNO / Ethernet shield, to listen to infrared signals using a TSOP7000 using a 455khz frequency.
// it measures the time in between pulses, to decode the IR message following the B&O protocol:

  // Coding: pulse distance (manchester biphase code)
  // Frame:  4 start bits + 16 data bits + 1 trailer bit + 1 stopbit
  // Data: 8 address bits + 8 command bits
  // Start-Bit 1:  200µs puls, 3125µs pause
  // Start-Bit 2:  200µs puls, 3125µs pause
  // Start-Bit 3:  200µs puls, 15625µs pause
  // Start-Bit 4:  200µs puls, 3125µs pause
  // 0-Bit:  200µs puls, 3125µs pause
  // 1-Bit:  200µs puls, 9375µs pause
  // R-Bit:  200µs puls, 6250µs pause, repetition of last bit
  // Trailer bit:  200µs puls, 12500µs pause
  // Stop bit: 200µs puls
  // Repetition: none
  // Pushbutton repetition:  N-times repetition of original-frames within 100ms
  // Bit order:  MSB first

// Defining the Bang & Olufsen commands
#define TICK 625 

#define BEO_ZERO    3125
#define BEO_SAME    6250
#define BEO_ONE     9375
#define BEO_STOP    12500 
#define BEO_START   15625.0

int pulse_counter = 0;
volatile int ticks  = 0;
volatile unsigned long pulse_up_time = 0;
volatile unsigned long pulse_down_time = 0;
unsigned long timeout_time = BEO_START * 2.0;


int lastPinState = 0;
int index;

int message [20]; //holds the messagebody
int dibs [20]; //holds the messagebody
int command = 0;
int address = 0;
bool received = false;
bool received_pulse =  false;

long freq =  1000000/TICK;
int prescale = 16000000/(8*freq)-1;

void setup(){
 
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  // attach external interrupt service which calls "processExternalTrigger" when there is a state change (RISE or FALL)
  // Only works on pin 2 or 3 for UNO 
  attachInterrupt(digitalPinToInterrupt(2), processExternalTrigger, CHANGE);
  
  delay(5);
  Serial.println("setup done");
}

void processExternalTrigger()
{
  ticks++;
  if((ticks % 2) == 0) pulse_down_time = micros(); // get time_stamp of FALL (even tick)
  else pulse_up_time = micros();                   // get time_stamp of RISE (uneven tick)
}


void loop(){
   receivedMessage(); // returns true if a IR message was received in full, ready to be processed 
}

boolean receivedMessage()
{
  
  if (index < 2) return false; // do nothing if no pulse is received (index 0), or the first rise occured (index 1)
  else 
  { 
    if((ticks % 2) != 0) // if ticks is uneven, we have the time between pulses. The pulse is constantl (200us) 
    {
       // Compute the time between the end of the last pulse, and start of the new pulse
       unsigned long time_passed = pulse_up_time - pulse_down_time;
       
       // If nothing happened for a while since the last fall, we assume we received the end of the message
       if((micros() - pulse_down_time) > timeout_time)
       {
          // process the message 
          reset(); 
          return true;
       }
       
       else
       {
          message[pulse_counter] = time_passed; 
          Serial.print("Pulse length: ");
          Serial.println(time_passed);
          return false;
       }
    }
  }
}

void reset()
{
  index = 0;            // count of external interrupt triggers (both rising and falling)
  pulse_up_time = 0;    // the timestamp of the last rising trigger
  pulse_down_time = 0;  // the timestamp of the last rising trigger
  pulse_counter = 0;    // index to the message array, stores time between each pulse 
}

void readBits()
{ 
  // Read the pulse, and save it into 1 - 0 values
  
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

  reset();
}


//boolean didReceivePulse()
//{  
//  // Check if we changed from HIGH to LOW
//  int currentPinState = PINB;                     // Read the states of pin 8 - 13
//  currentPinState = currentPinState & B00000010;  // Read out the state of PIN 10 only
//  int change = currentPinState - lastPinState;
//  lastPinState = currentPinState;
//  
//  if (change < 0) {
//    received_pulse = true;
//    return true;
//  }
//  else return false;
//}

//void processTick()
//{ 
//  if (received) return; // If we just received a message, process before moving on to the next
//
//  ticks++;
//  
//  // Check if we received a pulse in this tick
//  if(!didReceivePulse()) return;                      
//
//}


//void reset() 
//{ 
//  index = -1;
//  received = false;
//}



//boolean available()
//{
//  return received;
//}

