
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

//storage variables
boolean received = 1;
int timer = 0;
int counter  = 0;
int currentState = 1;
int lastState = 0;

void setup(){
 
  Serial.begin(9600);
  DDRB = B00000000;  // Set all pins (including 13, to input) Pin 13 is BIT 6, starting from the right
  PORTB = B00000000; // Set all pins (including 13, to LOW)

//delay(5);
//Serial.println("setup done");

cli();//stop interrupts

////set timer1 interrupt at 1Hz
//  TCCR1A = 0;// set entire TCCR1A register to 0
//  TCCR1B = 0;// same for TCCR1B
//  TCNT1  = 0;//initialize counter value to 0
//  // set compare match register for 1hz increments
//  OCR1A = 399;// = (16*10^6) / (1*1024) - 1 (must be <65536)
//  // turn on CTC mode
//  TCCR1B |= (1 << WGM12);
//  // Set CS10 and CS12 bits for 1024 prescaler
//  TCCR1B |= (1 << CS12) | (1 << CS10);  
//  // enable timer compare interrupt
//  TIMSK1 |= (1 << OCIE1A);
  
//set timer2 interrupt at 40kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 49;// = (16*10^6) / (8*40000) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);  
  // Setting the timer overflow interrupt enable bit
  //TIMSK1 = _BV(TOIE1);
sei();//allow interrupts


}//end setup

// Called at 40 kHz 
ISR(TIMER2_COMPA_vect){ // it should have a timer 
  decode();
}

void decode()
{
  // Reset if nothing happens...
  if (counter > 10000) counter = 0;
  
  counter++;
  int currentState = PINB; // Read the states of pin 8 - 13
  currentState = currentState & B00100000; // Get the state of PIN 13
  
  // If it was high and wet to low, or visa versa, check how many pulsed were in between those two events
  if (lastState - currentState != 0)
  {
    Serial.print("switch ");
    Serial.println(counter * TICK);
    
    counter = 0;
    lastState =  currentState;
  }
}

//void decode()
//{
//  if(received==1) timer++it ;
//  
//  int irData = PINB;
//  if (irData == 1 && lastState == HIGH) 
//  {
//      // The pause 
//      Serial.println(timer);
//      received = 0;
//      lastState = LOW;
//      timer = 0;
//  }
//  else if(irData == 33 && lastState == LOW)
//  {
//      lastState =  HIGH;
//      received = 1;
//  }
//}

void loop(){

}
