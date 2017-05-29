
// Common divisors for 3125 = 1, 5, 25, 125, 625, 3125
//#define TICK 625

// This code creates a timer at 40kHz on pin 13. It checks if the PIN is high and low. 
// On Pin 13 an IR receiver is attached, to listen to B&O commands using IR signals 
// Digital pin 13, is B5 register

// Defining the Bang & Olufsen commands
#define TICK 25 // 25 micro seconds 

#define BEO_ZERO    (3125 / TICK)
#define BEO_SAME    (6250 / TICK)
#define BEO_ONE     (9375 / TICK)
#define BEO_STOP    (12500 / TICK)
#define BEO_START   (15625 / TICK)

int counter  = 0;
int lastState = 0;
int ticker  = 0;
double freq =  1000000/TICK;
int prescale = 16000000/(8*freq)-1;

void setup(){
 
  Serial.begin(9600);
  
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
  Serial.print("freq: ");
  Serial.println(freq);
  Serial.print("Prescale: ");
  Serial.println(prescale);
  
  DDRB = DDRB & B11111011;   // Set Pin 10 as input, leave the rest of the pins as - is
  PORTB = DDRB & B11111011; // Set Pin 10 to LOW, leave the rest of the registers untouched
  
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

ISR(TIMER1_COMPA_vect){ // it should have a timer 
  decode();
}

void decode()
{
  counter++;
  
  int currentState = PINB; // Read the states of pin 8 - 13
  currentState = currentState & B00000100; // Read out the state of PIN 10 only

  if (currentState != lastState)
  { 
    if(currentState - lastState < 0)
    {
      Serial.println(counter * TICK);
      counter = 0; 
    }
    lastState =  currentState;
  }
}

void loop(){

}

