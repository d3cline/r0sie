// Variables will change:
int heatState = LOW;             // heatState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 2000;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:
  pinMode(D1, OUTPUT); // Cut Motor
  pinMode(D2, OUTPUT); // Feed Motor 
  pinMode(D3, OUTPUT); // LED 3 FIXME
  pinMode(D5, OUTPUT); // Seal Heater
  pinMode(D8, OUTPUT); // Cut Heater
  pinMode(D0, INPUT); // Cut 12'Oclock (one mag) (cut sensor)
  pinMode(3, INPUT); // Feed Pi 3.14 (three mag) (feed sensor)
  pinMode(D7, INPUT); // PF in ( pulls high)
  pinMode(D6, OUTPUT); // PF square wave out (ready)
  //Serial.begin(115200);

  // init the cut motor, correcting bad shutdown. Advance until ready state. 
  while ( digitalRead(D0) == HIGH ){
    digitalWrite(D1, HIGH);
  }
    digitalWrite(D1, LOW); // end init
}

void heaters(){
  unsigned long currentMillis = millis();  
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (heatState == LOW) {
      heatState = HIGH;
    } else {
      heatState = LOW;
    }
    // set the LED with the heatState of the variable:
    digitalWrite(D5, heatState);
    digitalWrite(D8, heatState);
    digitalWrite(D3, heatState);
  }
}

bool CUTTER_COMPLETE(){ // is cutter closed? 
  int cutState = digitalRead(D0);
  delay(10); // this delay matters so it can do the read
  if(cutState==LOW){ // low is 12 O'clock
    return true;
  } else {
    return false;
  }
}
bool STARTED = false;

bool PF_START(){ // Signal from PF machines
  int cutState = digitalRead(D7);
  if(cutState==LOW){ // Pulled low is 'on' 
    return true;
  } else {
    return false;
  }
}


void cut(){
  do{
  digitalWrite(D1, HIGH); // start the motor
  } while(CUTTER_COMPLETE() == true);
  
  while(CUTTER_COMPLETE()==false){ // wait for the rotation to complete
  digitalWrite(D1, HIGH); // keep it up while its waiting
    }
  digitalWrite(D1, LOW); // when its done shut the motor down
  }

int FEED_TICK = 3;
int feedState; // current state
int lastFeedState; // previous state
int ticks = 0; // counter
void feed(){
  digitalWrite(D2, HIGH);
  do{
    feedState = digitalRead(3);
    delay(20); // this dleay is a bit longer for stability 
    if(feedState != lastFeedState){
      if(feedState==LOW){ticks++;}
      }
    lastFeedState = feedState;
  }
  while(ticks < FEED_TICK);
  digitalWrite(D2, LOW);  
}

bool PF_READY = false;

/*
1. on PF_START
2. Wait for candy to drop
3. Complete 1 cutter rotation 
4. Feed the new plastic. 
5 .Signal PF_READY
*/
void loop() {
  heaters();
  delay(10);
  if(PF_START()){STARTED=true;}
  if(STARTED){
  delay(1000); // FOR THE CANDY TO FALL
   cut();
   feed();
    Serial.println('Y');
    STARTED = false;
    }
}
