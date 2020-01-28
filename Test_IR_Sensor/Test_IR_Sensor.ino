#define LEDPIN 13
  // Pin 13: Arduino has an LED connected on pin 13
  // Pin 11: Teensy 2.0 has the LED on pin 11
  // Pin  6: Teensy++ 2.0 has the LED on pin 6
  // Pin 13: Teensy 3.0 has the LED on pin 13
 
#define SENSORPIN 4
 
// variables will change:
int sensorState = 0, lastState=0;         // variable for reading the pushbutton status
 
void setup() {
  pinMode(13, OUTPUT);      
  // initialize the sensor pin as an input:
  //pinMode(4, INPUT);  
  pinMode (8, OUTPUT);   
  digitalWrite(13, HIGH); // turn on the pullup
  
  Serial.begin(9600);
}
 
void loop(){
  // read the state of the pushbutton value:
  sensorState = analogRead(A0);
 
  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState > 100) {     
    // turn LED on:
    digitalWrite(13, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(13, LOW); 
  }
  
  if (sensorState > 100 && !lastState) {
    Serial.println("Unbroken");
  } 
  if (sensorState <100 && lastState) {
    Serial.println("Broken");
  }
  lastState = sensorState;
}
