  int StationPin = number;
  int liftPin = number;
  int preBreakPin = number;
  int breakPin = number;

  int station_motor = number;
  int lift_motor = number;
  int break_run_motor = number;

  int buttonPin = number;
  
  // put your setup code here, to run once:
  bool station_OC;
  bool breakRun_OC;
  bool layout_OC;
  int lastStationState = 0;
  int lastLiftState = 0;
  int lastPreBreakState = 0;
  int lastBreakState = 0;
  
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

  pinMode(station_motor, OUTPUT);         
  pinMode(lift_motor, OUTPUT);         
  pinMode(break_run_motor, OUTPUT);         


  
  digitalWrite(station_motor, LOW);
  digitalWrite(lift_motor, LOW);
  digitalWrite(break_run_motor, LOW);
  
}

void loop() 
{
  int dispatchButton = digitalRead(buttonPin);
  ISRLift();
  ISRBreak();
  ISRPreBreak();
  ISRStation();
  
  int StationSensor = digitalRead(StationPin);
  int liftSensor = digitalRead(liftPin);
  int breakSensor = digitalRead(breakPin);

  if(StationSensor =HIGH) {
     //digitalWrite(station_motor, LOW); // stop station motor
      if(layout_OC = false) && dispatchButton{
       digitalWrite(station_motor, HIGH); //start station motor
       digitalWrite(lift_motor, HIGH); //start lift motor
    }
  }
  
  if(liftSensor =HIGH) {
    if(breakRun_OC = true) {
       digitalWrite(lift_motor, LOW); //stop lift motor      
    }
    else {
       digitalWrite(lift_motor, HIGH);//start lift motor      
    }
  }
  
  
  if(breakSensor =HIGH) {
    digitalWrite(break_run_motor, LOW);// stop break motor
   
    if(station_OC == false) {
       digitalWrite(station_motor, HIGH);//start station motor
      //start break motor
    }
  }
  /*
  if(lift_OC == true && station_OC == true)
  {
    function 
    //do something;
  }
  if(station_OC == true && layot_OC == true)
  {
    //do something;
  }
  if(layout_OC == true && lift_OC == true)
  {
    //do something;
  }

  orrr


  When the track section is occupied and the next one is not

  would that be better?
  */

}
/*void ISRLift()  //gets triggered on falling edge
{
  lift_OC = false;
  layout_OC = true;
}*/
void ISRStation() // station
{
      int sensorState = digitalRead(StationPin);
        if (sensorState == 1 && lastStationState == 0) {
          Serial.println("entering Station");
          digitalWrite(station_motor, LOW); //stop station motor
         } 
        if (sensorState == 0 && lastStationState == 1) {
          Serial.println("leaving Station");
            station_OC = false;
            layout_OC = true;
            digitalWrite(station_motor, LOW); //stop station motor
          }
      lastStationState = sensorState;
 
//  //if falling...
//  station_OC = false;
//  layout_OC = true;
//  digitalWrite(station_motor, LOW); //stop station motor
//
//  //if rising...
//  digitalWrite(station_motor, LOW); //stop station motor
}

void ISRPreBreak() //entering breakrun
{
      int sensorState = digitalRead(preBreakPin);
        if (sensorState == 1 && lastPreBreakState == 0) {
            Serial.println("entering pre-break-run");
            digitalWrite(break_run_motor, HIGH); //start break motor
         } 
        if (sensorState == 0 && lastPreBreakState == 1) {
            Serial.println("leaving pre-breakrun...off layout");
            layout_OC =false;
             breakRun_OC = true;
          }
      lastPreBreakState = sensorState;
  // if falling...
//  layout_OC =false;
//  breakRun_OC = true;
  //if rising...
//  digitalWrite(break_run_motor, HIGH)//start break motor
}

void ISRBreak() //leaving breakrun
{
      int sensorState = digitalRead(2);
        if (sensorState == 1 && lastBreakState == 0) {
            Serial.println("at Breakrun");
            breakRun_OC = false;
            station_OC = true;
            digitalWrite(break_run_motor, LOW);//stop break motor
         } 
        if (sensorState == 0 && lastBreakState == 1) {
          Serial.println("leaving breakrun");
          }
      lastBreakState = sensorState;
      
  //if falling...
//  breakRun_OC = false;
//  station_OC = true;
//  digitalWrite(break_run_motor, LOW);//stop break motor
}

void ISRLift() // leaving station
{
      int sensorState = digitalRead(2);
        if (sensorState == 1 && lastLiftState == 0) {
            Serial.println("entering top of Lift");
         } 
        if (sensorState == 0 && lastLiftState == 1) {
            Serial.println("leaving lift hill");
            digitalWrite(lift_motor, LOW);//stop lift motor
          }
      lastLiftState = sensorState;
  //if falling...
//   digitalWrite(lift_motor, LOW);//stop lift motor
}
