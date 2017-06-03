
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
int message [18]; //holds the messagebody
int command = 0;
int address = 0;

double freq =  1000000/TICK;
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
  
  // Check if we recieved a pulse in this tick
  if(!didReceivePulse()) return;                      
  
  // compute the pulseLength and check if we recognized a beoBit from the pulse, returns -1 if failed
  int beoBit = getBeoBitFromPulseAndStore(ticks * TICK);
  //Serial.println(beoBit);
  ticks = 0;
  
  // If we reached the end of the BEO message, get the BEO integer command if succesfull (16 BIT)
  if(beoBit!= BEO_STOP) return;
  else
  {  
    Serial.println(index);
    getBeoCommandFromMessage();
  }
}

// set the address and command ints. First 8 bits are the address, other 8 the command
void getBeoCommandFromMessage()
{
  command = 0;
  address = 0;
  for (int i = 0; i < 8; i++)
  {
    command *= 2;
    command += message[i+1] ; // First bit is the start bit
    address *= 2;
    address += message[i+9] ;
  }
  Serial.println(command);
  Serial.println(address);
  
  return;
}

void reset() 
{
  command = 0;
  address = 0;
  memset(message, 0, sizeof(message));
  index = -1;
}

int getBeoBitFromPulseAndStore(int pulse)
{ 
  // returns the BEO_command corresponding to the pulseLength
  // if a BEO_command is recognized, the corresponding value is stored in the message body
  
  if (pulse >= (BEO_ZERO-TICK) && pulse <= (BEO_ZERO+TICK))
  {     
    index++;
    message[index] = 0;
    return BEO_ZERO;
  }
  else if (pulse >= (BEO_ONE-TICK) && pulse <= (BEO_ONE+TICK)) 
  {
    index++;
    message[index] = 1;
    return BEO_ONE;  
  }
  else if (pulse >= (BEO_SAME-TICK) && pulse <= (BEO_SAME+TICK))
  { 
    index++;
    message[index] = message[index-1];
    return BEO_SAME;  
  }
  else if (pulse >= (BEO_START-TICK) && pulse <= (BEO_START+TICK))
  { 
    reset();
    return BEO_START;
  }
  else if (pulse >= (BEO_STOP-TICK) && pulse <= (BEO_STOP+TICK)) return BEO_STOP;
  else return -1; // failed to recognize
 
}

void loop(){

}

