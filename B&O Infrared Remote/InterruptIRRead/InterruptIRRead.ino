
// Common divisors for 3125 = 1, 5, 25, 125, 625, 3125
//#define TICK 625

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
int counter  = 1;
int lastState = LOW;

void setup(){
 
  Serial.begin(9600);
  DDRB = B00000000;
  PORTB = B00000000; // set to low first

//delay(5);
//Serial.println("setup done");

cli();//stop interrupts

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 399;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  // Setting the timer overflow interrupt enable bit
  //TIMSK1 = _BV(TOIE1);


sei();//allow interrupts


}//end setup


ISR(TIMER1_COMPA_vect){ // it should have a timer 
  decode();
}

void decode()
{
  if(received==1) timer++;
  
  int irData = PINB;
  if (irData == 1 && lastState == HIGH) 
  {
      // The pause 
      Serial.println(timer);
      received = 0;
      lastState = LOW;
      timer = 0;
  }
  else if(irData == 33 && lastState == LOW)
  {
      lastState =  HIGH;
      received = 1;
  }
}

void loop(){

}
