int StationPin = 12;
int liftPin = 13;
int preBrakePin = 10;
int brakePin = 11;

int station_motor = 4;
int lift_motor = 3;
int brake_run_motor = 2;

int buttonPin = 5;

// put your setup code here, to run once:
bool station_OC = true;
bool brakeRun_OC = true;
bool layout_OC = false;
int lastStationState = 0;
int lastLiftState = 1;
int lastPreBrakeState = 1;
int lastBrakeState = 1;
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
  pinMode(station_motor, OUTPUT);
  pinMode(lift_motor, OUTPUT);
  pinMode(brake_run_motor, OUTPUT);



  digitalWrite(station_motor, LOW);
  digitalWrite(lift_motor, LOW);
  digitalWrite(brake_run_motor, LOW);
  Serial.begin(9600);

}

void loop()
{
  dispatchButton = digitalRead(buttonPin);

  digitalWrite(7, station_OC); //print out layout OC
  ISRLift();
  ISRBrake();
  ISRPreBrake();
  ISRStation();
  //Serial.println(dispatchButton);

  if (dispatchButton == 1) {
    // Serial.println("button pressed");
  }
  int StationSensor = not digitalRead(StationPin);
  int liftSensor = not digitalRead(liftPin);
  int brakeSensor = not digitalRead(brakePin);
  //Serial.println("Printing");
  //Serial.println(liftSensor);

//Vehicle is in station, layout is not occupied, and dispatch button is pressed. 
//Send vehicle to lift!

  if ((StationSensor == HIGH) && (layout_OC == 0) && (dispatchButton == 1)) {
    //digitalWrite(station_motor, LOW); // stop station motor
    //if((layout_OC == 0) && (dispatchButton == 1)){ //(layout_OC == false) && (dispatchButton == 1)
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


  if (brakeSensor == HIGH) {
    if (station_OC == false) {
      digitalWrite(station_motor, HIGH);//start station motor
      //start brake motor
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

void ISRPreBrake() //entering brakerun
{
  delay(100);
  int sensorState = digitalRead(preBrakePin);
  if (sensorState == 1 && lastPreBrakeState == 0) {
    Serial.println("leaving pre-brakerun...off layout");
    layout_OC = false;
    brakeRun_OC = true;
  }
  if (sensorState == 0 && lastPreBrakeState == 1) {
    Serial.println("entering pre-brake-run");
    digitalWrite(brake_run_motor, HIGH); //start brake motor
  }
  lastPreBrakeState = sensorState;
}

void ISRBrake() //leaving brakerun
{
  delay(100);
  int sensorState = digitalRead(brakePin);
  if (sensorState == 1 && lastBrakeState == 0) {
    Serial.println("left brakerun");
    station_OC = true;
    digitalWrite(brake_run_motor, LOW);//stop brake motor

  }
  if (sensorState == 0 && lastBrakeState == 1) {
    Serial.println("at Brakerun");
    brakeRun_OC = false;
    station_OC = false;
    digitalWrite(brake_run_motor, LOW);//stop brake motor
  }
  lastBrakeState = sensorState;
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
