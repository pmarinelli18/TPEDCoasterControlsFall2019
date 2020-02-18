  int StationPin = 3;
  int station_motor = 4;
  int stationHIGH = 5;
  int stationLOW = 6;

  int liftPin = 2;
  int lift_motor = 26;
  int liftHIGH = 27;
  int liftHOW = 28;
  
  int preBrakePin = 22;
  int brakePin = 24;
  int brake_run_motor = 13;
  int breakHIGH = 14;
  int breakLOW = 15;

  int buttonPin = 12;

  int switchLeft = 33;
  int switchRight = 34;
  int autoLED = 36;
   int showLED = 35;
  int maintenceLED = 37;
  
  // put your setup code here, to run once:
  bool station_OC = false;
  bool brakeRun_OC = false;
  bool layout_OC = false;
  int lastStationState = 0;
  int lastLiftState = 0;
  int lastPrebrakeState = 0;
  int lastBreakState = 0;
 int dispatchButton = 0;

 int stationSpeed = 200;
  int liftSpeed = 200; //liftMotor
  int breakSpeed = 200;

void setup() 
{
  //sets up LEDs as temp for the motors

  //attachInterrupt(digitalPinToInterrupt(StationPin)/*station sensor*/, ISRStation, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(preBrakePin)/*preBrake sensor*/, ISRPreBrake, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(brakePin)/*brake sensor*/, ISRBrake, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(liftPin)/*lift sensor*/, ISRLift, CHANGE);
  pinMode(StationPin, INPUT);
  pinMode(liftPin, INPUT);
  pinMode(preBrakePin, INPUT);
  pinMode(brakePin, INPUT);
  pinMode(buttonPin, INPUT);

//  pinMode(7, OUTPUT);  //layoutOC led  
//  pinMode(8, OUTPUT);  //layoutOC led        
//  pinMode(9, OUTPUT);  //layoutOC led        

  pinMode(station_motor, OUTPUT);         
  pinMode(lift_motor, OUTPUT);         
  pinMode(brake_run_motor, OUTPUT);         


 // digitalWrite(station_motor, LOW);
  analogWrite(station_motor, 0);
  
  analogWrite(lift_motor, 0);
  analogWrite(lift_motor, 0);

  pinMode(switchLeft, INPUT);
  pinMode(switchRight, INPUT);

  pinMode(autoLED, OUTPUT);
  pinMode(showLED, OUTPUT);
  pinMode(maintenceLED, OUTPUT);
  
  Serial.begin(9600);
  scanTrackForInitialValues();

}

void loop(){
  digitalWrite(showLED, LOW);
  digitalWrite(maintenceLED, LOW);
  digitalWrite(autoLED, LOW);
  //Show mode
  if (digitalRead(switchLeft) == HIGH ){
    digitalWrite(showLED, HIGH);
  }
  //Maintence mode
  else if  (digitalRead(switchRight) == HIGH){
    digitalWrite(maintenceLED, HIGH);  
  }
  //Auto mode
  else {
    autoMode();
    digitalWrite(autoLED, HIGH);
   
  }
  
}


void autoMode()
{
  digitalWrite(stationHIGH, HIGH); //sets direction of station motor
  digitalWrite(stationLOW, LOW);

  digitalWrite(liftHIGH, HIGH); //sets direction of station motor
  digitalWrite(liftLOW, LOW);

  digitalWrite(breakHIGH, HIGH); //sets direction of station motor
  digitalWrite(breakLOW, LOW);

  dispatchButton = digitalRead(buttonPin);
  
    digitalWrite(7, station_OC); //print out layout OC to LED
    digitalWrite(8, layout_OC); //layout_OC
    digitalWrite(9, brakeRun_OC); 
      
 ISRLift();
  ISRBrake();
  ISRPreBrake();
  ISRStation();
  //Serial.println(dispatchButton);

  int StationSensor = not digitalRead(StationPin);
  int liftSensor = not digitalRead(liftPin);
  int breakSensor = not digitalRead(brakePin);
//Serial.println("Printing");

  if((StationSensor == HIGH) && (layout_OC == 0) && (dispatchButton == 1)) {
       Serial.println("button pressed here");
       analogWrite(station_motor, stationSpeed);
       analogWrite(lift_motor, liftSpeed);
   // }
  }

  if (liftSensor == HIGH) {
    if (brakeRun_OC == true) {
      analogWrite(lift_motor, 0);
    }
    else {
      analogWrite(lift_motor, liftSpeed);
    }
  }  
  
  if(breakSensor == HIGH) {   
    if(station_OC == false) {
       analogWrite(station_motor, stationSpeed);
       analogWrite(brake_run_motor, breakSpeed);
      //start break motor
    }
    else {
      analogWrite(brake_run_motor, 0);
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
      int sensorState = not digitalRead(StationPin);
        if (sensorState == 1 && lastStationState == 0) {
             delay(100);
             Serial.println("entering Station");
             analogWrite(station_motor, 0);
            station_OC = true;
         } 
        if (sensorState == 0 && lastStationState == 1) {
             delay(100);
            Serial.println("leaving Station");
            station_OC = false;
            layout_OC = true;
            analogWrite(station_motor, 0);

          }
      lastStationState = sensorState;
}

void ISRPreBrake() //entering brakerun
{
      int sensorState = not digitalRead(preBrakePin);
        if (sensorState == 1 && lastPrebrakeState == 0) {
            delay(100);
            Serial.println("entering pre-break-run");
            analogWrite(brake_run_motor, breakSpeed);
            brakeRun_OC = true;
         } 
        if (sensorState == 0 && lastPrebrakeState == 1) {
            delay(100);
            Serial.println("leaving pre-breakrun...off layout");
            layout_OC =false;
       //     brakeRun_OC = true;
          }
      lastPrebrakeState = sensorState;
}

void ISRBrake() //leaving brakerun
{
      int sensorState = not digitalRead(brakePin);
        if (sensorState == 1 && lastBreakState == 0) {
            delay(100);
            Serial.println("at Breakrun");
           // station_OC = false;
            brakeRun_OC = true; //sets up initial state

         } 
        if (sensorState == 0 && lastBreakState == 1) {
            delay(100);
             Serial.println("left breakrun");
             station_OC = true;
             brakeRun_OC = false;
             analogWrite(brake_run_motor, 0);
    
          }
      lastBreakState = sensorState;
}

void ISRLift() // leaving lift
{
      int sensorState = not digitalRead(liftPin);
        if (sensorState == 1 && lastLiftState == 0) {
            delay(100);
              Serial.println("entering top of Lift");
            } 
        if (sensorState == 0 && lastLiftState == 1) {
            delay(100);
            Serial.println("leaving lift hill");
            analogWrite(lift_motor, 0);
          }
      lastLiftState = sensorState;
}
//Config initial occupy values based on sensor input
void scanTrackForInitialValues(){
  int stationSensor = not digitalRead(StationPin);
  int liftSensor = not digitalRead(liftPin);
  int brakeSensor = not digitalRead(brakePin);

  if (stationSensor == HIGH){
    station_OC = true;
  }

  if (brakeSensor == HIGH){
    brakeRun_OC = true;
  }
  
  if (liftSensor == HIGH){
    layout_OC = true;
  }
}
