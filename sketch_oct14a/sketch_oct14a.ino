//Location of pin input and output  
  int StationPin = 3;
  int station_motor = 44;
  int stationHIGH = 5;
  int stationLOW = 6;

  int liftPin = 2;
  int lift_motor = 46;
  int liftHIGH = 27;
  int liftLOW = 28;
  
  int preBrakePin = 22;
  int brakePin = 24;
  int brake_run_motor = 45;
  int brakeHIGH = 14;
  int brakeLOW = 15;

  int buttonPin = 12;

  int switchShow = 33;
  int switchMaintenance = 34;
  int autoLED = 36;
   int showLED = 35;
  int maintenanceLED = 37;

  int switchStationForward =   1; //NEED TO add correct pins
  int switchStationBackward =  1;

  int switchBrakeForward =   1;
  int switchBrakeBackward =  1;

  int switchLiftForward =   1;
  int switchLiftBackward =  1;
  
  // Initializes variables at what they should be at the beginning
  bool station_OC = false;
  bool brakeRun_OC = false;
  bool layout_OC = false;
  int lastStationState = 0;
  int lastLiftState = 0;
  int lastPrebrakeState = 0;
  int lastBrakeState = 0;
 int dispatchButton = 0;

 int stationSpeed = 20;
  int liftSpeed = 20; //liftMotor
  int brakeSpeed = 20;

  int maintenanceSpeedForward = 5;
  int maintenanceSpeedBackward = -5;
  
  int time = -1;
  bool run = true;
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

  pinMode(switchShow, INPUT);
  pinMode(switchMaintenance, INPUT);

  pinMode(switchStationForward, INPUT);
  pinMode(switchStationBackward, INPUT);

  pinMode(switchBrakeForward, INPUT);
  pinMode(switchBrakeBackward, INPUT);

  pinMode(switchLiftForward, INPUT);
  pinMode(switchLiftBackward, INPUT);

  pinMode(autoLED, OUTPUT);
  pinMode(showLED, OUTPUT);
  pinMode(maintenanceLED, OUTPUT);
  
  Serial.begin(9600);
  scanTrackForInitialValues();

}

void loop(){
  digitalWrite(showLED, LOW);
  digitalWrite(maintenanceLED, LOW);
  digitalWrite(autoLED, LOW);
  //Show mode
  if (digitalRead(switchShow) == HIGH ){
    showMode();
    digitalWrite(showLED, HIGH);
  }
  //maintenance mode
  else if  (digitalRead(switchMaintenance) == HIGH){
    maintenanceMode();
    digitalWrite(maintenanceLED, HIGH);  
  }
  //Auto mode
  else {
    autoMode();
    digitalWrite(autoLED, HIGH);
   
  }
  
}



//Will only allow the next coaster to go when an input is given
void autoMode()
{
  digitalWrite(stationHIGH, HIGH); //sets direction of station motor
  digitalWrite(stationLOW, LOW);

  digitalWrite(liftHIGH, HIGH); //sets direction of station motor
  digitalWrite(liftLOW, LOW);

  digitalWrite(brakeHIGH, HIGH); //sets direction of station motor
  digitalWrite(brakeLOW, LOW);

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
  int brakeSensor = not digitalRead(brakePin);
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
  
  if(brakeSensor == HIGH) {   
    if(station_OC == false) {
       analogWrite(station_motor, stationSpeed);
       analogWrite(brake_run_motor, brakeSpeed);
      //start brake motor
    }
    else {
      analogWrite(brake_run_motor, 0);
    }
  }

}




//Will continue to run for a given amount of time with no action needed
void showMode()
{
  digitalWrite(stationHIGH, HIGH); //sets direction of station motor
  digitalWrite(stationLOW, LOW);

  digitalWrite(liftHIGH, HIGH); //sets direction of station motor
  digitalWrite(liftLOW, LOW);

  digitalWrite(brakeHIGH, HIGH); //sets direction of station motor
  digitalWrite(brakeLOW, LOW);


  //if the the button pin is pressed the trains will continue to run for 1 minute
  dispatchButton = digitalRead(buttonPin); 
  if(dispatchButton ==1) 
  {
    time = millis(); //when pressed the time is set to the current time
  }
  if(time != -1 && millis() < time + 60000) 
  {
    run = true; //as long as the recorder time + a minute is greater than the current time the train will run
  }
  else
  {
    run = false;
  }
  
    digitalWrite(7, station_OC); //print out layout OC to LED
    digitalWrite(8, layout_OC); //layout_OC
    digitalWrite(9, brakeRun_OC); 
      
 ISRLift();
  ISRBrake();
  ISRPreBrake();
  ISRStation();

  int StationSensor = not digitalRead(StationPin);
  int liftSensor = not digitalRead(liftPin);
  int brakeSensor = not digitalRead(brakePin);
//Serial.println("Printing");

  if((StationSensor == HIGH) && (layout_OC == 0)&& run) {
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
  
  if(brakeSensor == HIGH) {   
    if(station_OC == false) {
       analogWrite(station_motor, stationSpeed);
       analogWrite(brake_run_motor, brakeSpeed);
      //start brake motor
    }
    else {
      analogWrite(brake_run_motor, 0);
    }
  }

}


//Will turn on specific motors on comand
void maintenanceMode()
{

  if (digitalRead(switchStationForward) == HIGH ){
    analogWrite(station_motor, maintenanceSpeedForward);
  }
  else if  (digitalRead(switchStationBackward) == HIGH){
    analogWrite(station_motor, maintenanceSpeedBackward);
  }
  if (digitalRead(switchBrakeForward) == HIGH ){
    analogWrite(brake_run_motor, maintenanceSpeedForward);
  }
  else if  (digitalRead(switchBrakeBackward) == HIGH){
    analogWrite(brake_run_motor, maintenanceSpeedBackward);
  }
  if (digitalRead(switchLiftForward) == HIGH ){
    analogWrite(lift_motor, maintenanceSpeedForward);
  }
  else if  (digitalRead(switchLiftBackward) == HIGH){
    analogWrite(lift_motor, maintenanceSpeedBackward);
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
            Serial.println("entering pre-brake-run");
            analogWrite(brake_run_motor, brakeSpeed);
            brakeRun_OC = true;
         } 
        if (sensorState == 0 && lastPrebrakeState == 1) {
            delay(100);
            Serial.println("leaving pre-brakerun...off layout");
            layout_OC =false;
       //     brakeRun_OC = true;
          }
      lastPrebrakeState = sensorState;
}

void ISRBrake() //leaving brakerun
{
      int sensorState = not digitalRead(brakePin);
        if (sensorState == 1 && lastBrakeState == 0) {
            delay(100);
            Serial.println("at Brakerun");
           // station_OC = false;
            brakeRun_OC = true; //sets up initial state

         } 
        if (sensorState == 0 && lastBrakeState == 1) {
            delay(100);
             Serial.println("left brakerun");
             station_OC = true;
             brakeRun_OC = false;
             analogWrite(brake_run_motor, 0);
    
          }
      lastBrakeState = sensorState;
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
