  int StationPin = 12;
  int liftPin = 13;
  int preBrakePin = 10;
  int brakePin = 11;

  int station_motor = 4;
  int lift_motor = 3;
  int brake_run_motor = 2;

  int buttonPin = 5;

  int switchLeft = 33;
  int switchRight = 34;
  int autoLED = 35;
   int showLED = 36;
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

  pinMode(7, OUTPUT);  //layoutOC led  
  pinMode(8, OUTPUT);  //layoutOC led        
  pinMode(9, OUTPUT);  //layoutOC led        

  pinMode(station_motor, OUTPUT);         
  pinMode(lift_motor, OUTPUT);         
  pinMode(brake_run_motor, OUTPUT);         


  digitalWrite(station_motor, LOW);
  digitalWrite(lift_motor, LOW);
  digitalWrite(brake_run_motor, LOW);

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
  dispatchButton = digitalRead(buttonPin);
  //digitalWrite(3, HIGH);

  
    digitalWrite(7, station_OC); //print out layout OC
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
//Serial.println(liftSensor);

  if((StationSensor == HIGH) && (layout_OC == 0) && (dispatchButton == 1)) {
     //digitalWrite(station_motor, LOW); // stop station motor
       Serial.println("button pressed here");
       digitalWrite(station_motor, HIGH); //start station motor
       digitalWrite(lift_motor, HIGH); //start lift motor
   // }
  }

  if (liftSensor == HIGH) {
    if (brakeRun_OC == true) {
      digitalWrite(lift_motor, LOW); //stop lift motor
    }
    else {
      digitalWrite(lift_motor, HIGH);//start lift motor
    }
  }  
  
  if(breakSensor == HIGH) {   
    if(station_OC == false) {
       digitalWrite(station_motor, HIGH);//start station motor
       digitalWrite(brake_run_motor, HIGH);//start station motor
      //start break motor
    }
    else {
      digitalWrite(brake_run_motor, LOW);// stop brake motor
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
            digitalWrite(station_motor, LOW); //stop station motor
            station_OC = true;
         } 
        if (sensorState == 0 && lastStationState == 1) {
             delay(100);
            Serial.println("leaving Station");
            station_OC = false;
            layout_OC = true;
            digitalWrite(station_motor, LOW); //stop station motor

          }
      lastStationState = sensorState;
}

void ISRPreBrake() //entering brakerun
{
      int sensorState = not digitalRead(preBrakePin);
        if (sensorState == 1 && lastPrebrakeState == 0) {
            delay(100);
            Serial.println("entering pre-break-run");
            digitalWrite(brake_run_motor, HIGH); //start break motor
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
            //digitalWrite(brake_run_motor, LOW);//stop break motor

         } 
        if (sensorState == 0 && lastBreakState == 1) {
            delay(100);
             Serial.println("left breakrun");
             station_OC = true;
             brakeRun_OC = false;
             digitalWrite(brake_run_motor, LOW);//stop break motor
    
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
            digitalWrite(lift_motor, LOW);//stop lift motor
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
