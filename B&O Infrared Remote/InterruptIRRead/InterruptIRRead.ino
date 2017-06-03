
// Common divisors for 3125 = 1, 5, 25, 125, 625, 3125
//#define TICK 625

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
double command = 0;
double address = 0;
bool received = false;

long freq =  1000000/TICK;
int prescale = 16000000/(8*freq)-1;

void setup(){
 
  Serial.begin(9600);
  
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  DDRB = DDRB & B11111101;   // Set Pin 10 as input, leave the rest of the pins as - is
  PORTB = DDRB & B11111101; // Set Pin 10 to LOW, leave the rest of the registers untouched
  
  cli();//stop interrupts
  
    //set timer1 interrupt at 1Hz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 40Khz increments
    OCR1A = prescale ;// = (16*10^6) / (8*40000) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set 8 bit prescaler
    TCCR1B |= (1 << CS11);  
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
   
  sei();//allow interrupts
  
  delay(5);
  Serial.println("setup done");

}

ISR(TIMER1_COMPA_vect)
{
  processTick();
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
  ticks++;

  // If we are processing a received message, return
  if (received) return;
  
  // Check if we received a pulse in this tick
  if(!didReceivePulse()) return;                      

  int pulse = ticks * TICK;
 
  index++;
  ticks = 0;
  message[index] = pulse;

  if (index > 18) index = -1;
  
  if ((pulse == BEO_STOP))
  {
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
  command = 0;
  address = 0;
  index = -1;
  received = false;
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
  
  int base = 0;
  int back_count = 16;
  while (back_count > 8)
  {
    command += pow(2,base)*dibs[back_count];
    address += pow(2,base)*dibs[back_count-8];
    base++;
    back_count--;
  }
  Serial.println(command);
}

void loop(){
  if (received)
  {
    readBits();
    reset();
  }
}

