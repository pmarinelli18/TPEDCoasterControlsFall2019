  int StationPin = 12;
  int liftPin = 13;
  int preBreakPin = 10;
  int breakPin = 11;

  int station_motor = 4;
  int lift_motor = 3;
  int break_run_motor = 2;

  int buttonPin = 5;
  
  // put your setup code here, to run once:
  bool station_OC = true;
  bool breakRun_OC = true;
  bool layout_OC = false;
  int lastStationState = 0;
  int lastLiftState = 1;
  int lastPreBreakState = 1;
  int lastBreakState = 1;
 int dispatchButton = 0;
void setup() 
{
  //sets up LEDs as temp for the motors

  //attachInterrupt(digitalPinToInterrupt(StationPin)/*station sensor*/, ISRStation, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(preBreakPin)/*preBreak sensor*/, ISRPreBreak, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(breakPin)/*break sensor*/, ISRBreak, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(liftPin)/*lift sensor*/, ISRLift, CHANGE);
  pinMode(StationPin, INPUT);
  pinMode(liftPin, INPUT);
  pinMode(preBreakPin, INPUT);
  pinMode(breakPin, INPUT);
  pinMode(buttonPin, INPUT);

  pinMode(7, OUTPUT);  //layoutOC led        
  pinMode(station_motor, OUTPUT);         
  pinMode(lift_motor, OUTPUT);         
  pinMode(break_run_motor, OUTPUT);         


  
  digitalWrite(station_motor, LOW);
  digitalWrite(lift_motor, LOW);
  digitalWrite(break_run_motor, LOW);
    Serial.begin(9600);

}

void loop() 
{
  dispatchButton = digitalRead(buttonPin);
  
    digitalWrite(7, station_OC); //print out layout OC
  ISRLift();
  ISRBreak();
  ISRPreBreak();
  ISRStation();
//Serial.println(dispatchButton);

  if (dispatchButton == 1) {
   // Serial.println("button pressed");
  }
  int StationSensor = not digitalRead(StationPin);
  int liftSensor = not digitalRead(liftPin);
  int breakSensor = not digitalRead(breakPin);
//Serial.println("Printing");
//Serial.println(liftSensor);

  if((StationSensor == HIGH) && (layout_OC == 0) && (dispatchButton == 1)) {
     //digitalWrite(station_motor, LOW); // stop station motor
      //if((layout_OC == 0) && (dispatchButton == 1)){ //(layout_OC == false) && (dispatchButton == 1)
       Serial.println("button pressed here");
       digitalWrite(station_motor, HIGH); //start station motor
       digitalWrite(lift_motor, HIGH); //start lift motor
   // }
  }
  
  if(liftSensor == HIGH) {
    if(breakRun_OC == true) {
       digitalWrite(lift_motor, LOW); //stop lift motor      
    }
    else {
       digitalWrite(lift_motor, HIGH);//start lift motor      
    }
  }
  
  
  if(breakSensor == HIGH) {   
    if(station_OC == false) {
       digitalWrite(station_motor, HIGH);//start station motor
      //start break motor
    }
    else {
      digitalWrite(break_run_motor, LOW);// stop break motor
    }
  }

}
/*void ISRLift()  //gets triggered on falling edge
{
  lift_OC = false;
  layout_OC = true;
}*/
void ISRStation() // station
{
   delay(100);
      int sensorState = digitalRead(StationPin);
        if (sensorState == 1 && lastStationState == 0) {
            Serial.println("leaving Station");
            station_OC = false;
            layout_OC = true;
            digitalWrite(station_motor, LOW); //stop station motor
         } 
        if (sensorState == 0 && lastStationState == 1) {
             Serial.println("entering Station");
            digitalWrite(station_motor, LOW); //stop station motor

          }
      lastStationState = sensorState;

}

void ISRPreBreak() //entering breakrun
{
   delay(100);
      int sensorState = digitalRead(preBreakPin);
        if (sensorState == 1 && lastPreBreakState == 0) {
            Serial.println("leaving pre-breakrun...off layout");
            layout_OC =false;
            breakRun_OC = true;
         } 
        if (sensorState == 0 && lastPreBreakState == 1) {
            Serial.println("entering pre-break-run");
            digitalWrite(break_run_motor, HIGH); //start break motor
          }
      lastPreBreakState = sensorState;
}

void ISRBreak() //leaving breakrun
{
  delay(100);
      int sensorState = digitalRead(breakPin);
        if (sensorState == 1 && lastBreakState == 0) {
             Serial.println("left breakrun");
             station_OC = true;
             digitalWrite(break_run_motor, LOW);//stop break motor

         } 
        if (sensorState == 0 && lastBreakState == 1) {
            Serial.println("at Breakrun");
            breakRun_OC = false;
            station_OC = false;
            digitalWrite(break_run_motor, LOW);//stop break motor
          }
      lastBreakState = sensorState;
}

void ISRLift() // leaving station
{
      int sensorState = digitalRead(liftPin);
        if (sensorState == 1 && lastLiftState == 0) {
            Serial.println("leaving lift hill");
            digitalWrite(lift_motor, LOW);//stop lift motor
            } 
        if (sensorState == 0 && lastLiftState == 1) {

            Serial.println("entering top of Lift");
          }
      lastLiftState = sensorState;
}
