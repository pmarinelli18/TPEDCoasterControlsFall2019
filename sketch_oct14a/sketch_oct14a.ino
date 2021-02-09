//Location of pin input and output
#include <EEPROM.h>

#include "LCDController.h"
#include "MotorController.h"

//Motors
//*** pin locations should be changed to where the wires end up being**
struct MotorController station_motor;

struct MotorController lift_motor;

struct MotorController brake_motor;


int stationSpeed = 1;
int liftSpeed = 1; //liftMotor
int brakeSpeed = 1;
int maintenanceSpeed = 5;

//Sensors
int stationSensorPin = A5;
int liftSensorPin = A4;
int preBrakeSensorPin = A2;
int brakeSensorPin = A3;

//Indicator LEDs
int eStopLedPin = 6;
int stationOCLedPin = 0;
int liftOCLedPin = 0;
int breakRunOCLedPin = 0;
int layoutOCLedPin = 0;

//Buttons
int dispatchButtonPin = 11;
int keyPin = 10;
int eStopClearButtonPin = 9;
int eStopButtonPin = 8;

//Switches
int switchShow = 13;
int switchMaintenance = 12;
int switchStationForward = 1; //NEED TO add correct pins
int switchStationBackward = 1;
int switchBrakeForward = 1;
int switchBrakeBackward = 1;
int switchLiftForward = 1;
int switchLiftBackward = 1;

//Block occupiled bools
// Initializes variables at what they should be at the beginning
bool stationOC = false;
bool liftOC = false;
bool layoutOC = false;
bool brakeRunOC = false;

int lastStationState = 0;
int lastLiftState = 0;
int lastPrebrakeState = 0;
int lastBrakeState = 0;

//Show mode vars
int time = -1;
bool run = true;

enum mode
{
  maintenance,
  dispatch,
  show,
  eStop
};

struct LCDController lcdControllerInstance;
mode curMode = show;

void setup()
{
  Serial.begin(9600);

  //initlize lcd
  lcdControllerInstance.initLCDController("Welcome", "");
  
  station_motor.initMotor(2, 1);
  lift_motor.initMotor(4, 1);
  brake_motor.initMotor(5, 1);
  //sets up LEDs as temp for the motors
  //]attachInterrupt(digitalPinToInterrupt(stationSensorPin)/*station sensor*/, ISRStation, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(preBrakeSensorPin)/*preBrake sensor*/, ISRPreBrake, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(brakeSensorPin)/*brake sensor*/, ISRBrake, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(liftSensorPin)/*lift sensor*/, ISRLift, CHANGE);

  pinMode(stationSensorPin, INPUT);
  pinMode(liftSensorPin, INPUT);
  pinMode(preBrakeSensorPin, INPUT);
  pinMode(brakeSensorPin, INPUT);
  pinMode(dispatchButtonPin, INPUT);
  pinMode(eStopClearButtonPin, INPUT);
  pinMode(eStopLedPin, OUTPUT);
  pinMode(eStopButtonPin, INPUT);

  pinMode(stationOCLedPin, OUTPUT);  //stationOC led
  pinMode(layoutOCLedPin, OUTPUT);   //layoutOC led
  pinMode(breakRunOCLedPin, OUTPUT); //brakeRunOC led
  pinMode(liftOCLedPin, OUTPUT);     // led

  // digitalWrite(station_motor, LOW);

  pinMode(switchShow, INPUT_PULLUP);
  pinMode(switchMaintenance, INPUT_PULLUP);

  pinMode(switchStationForward, INPUT);
  pinMode(switchStationBackward, INPUT);

  pinMode(switchBrakeForward, INPUT);
  pinMode(switchBrakeBackward, INPUT);

  pinMode(switchLiftForward, INPUT);
  pinMode(switchLiftBackward, INPUT);

  pinMode(keyPin, INPUT_PULLUP);

  scanTrackForInitialValues();
}
void loop()
{
  int keyValue = digitalRead(keyPin);

  if (digitalRead(eStopButtonPin) == LOW)
  {
    activateEStop(0);
  }

  if (curMode == eStop)
  {
    eStopMode();
  }
  //If the key is inserted
  else if (keyValue == 1)
  {
    lcdControllerInstance.setUpDispatchString();
    curMode = show;
    if (digitalRead(switchShow) == LOW)
    {
      showMode();
    }
    //maintenance mode
    else if (digitalRead(switchMaintenance) == LOW)
    {
      curMode = maintenance;
      maintenanceMode();
    }
    //Dispatch mode
    else
    {
      curMode = dispatch;
      dispatchMode();
    }
  }

  //If the key is not inserted
  else
  {
    if (curMode == show)
    {
      showMode();
    }
    else
    {
      dispatchMode();
    }

    if (curMode == show && digitalRead(switchShow) != LOW)
    {
      lcdControllerInstance.updateLowerString("Insert Key");
    }
    else if (curMode == dispatch && (digitalRead(switchShow) == LOW || digitalRead(switchMaintenance) == LOW))
    {
      lcdControllerInstance.updateLowerString("Insert Key");
    }
    else if (curMode == maintenance && digitalRead(switchMaintenance) == LOW)
    {
      lcdControllerInstance.updateLowerString("Insert Key");
    }
    else if (curMode != eStop)
    {
      lcdControllerInstance.setUpDispatchString();
    }
  }
}

void ISRStation()
{ // station
  int sensorState = not digitalRead(stationSensorPin);
  if (curMode != eStop)
  {

    if (sensorState == 1 && lastStationState == 0)
    {
      delay(100);
      Serial.println("entering Station");
      station_motor.updateMotorSpeed(0);
    }
    if (sensorState == 0 && lastStationState == 1)
    {
      delay(100);
      Serial.println("leaving Station");
      stationOC = false;
      station_motor.updateMotorSpeed(0);
    }
    lastStationState = sensorState;
  }
}

void ISRPreBrake() //entering brakerun
{
  int sensorState = not digitalRead(preBrakeSensorPin);
  if (curMode != eStop)
  {
    if (sensorState == 1 && lastPrebrakeState == 0)
    {
      delay(100);
      Serial.println("entering pre-brake-run");
      
      brake_motor.updateMotorSpeed(brakeSpeed);
      brakeRunOC = true;
    }
    if (sensorState == 0 && lastPrebrakeState == 1)
    {
      delay(100);
      Serial.println("leaving pre-brakerun...off layout");
      layoutOC = false;
    }
    lastPrebrakeState = sensorState;
  }
}

void ISRBrake() //leaving brakerun
{
  int sensorState = not digitalRead(brakeSensorPin);
  if (curMode != eStop)
  {
    if (sensorState == 1 && lastBrakeState == 0)
    {
      delay(100);
      Serial.println("at Brakerun");
    }
    if (sensorState == 0 && lastBrakeState == 1)
    {
      delay(100);
      Serial.println("left brakerun");
      stationOC = true;
      brakeRunOC = false;
      brake_motor.updateMotorSpeed(0);
    }
    lastBrakeState = sensorState;
  }
}

void ISRLift() // leaving lift
{
  int sensorState = not digitalRead(liftSensorPin);
  if (curMode != eStop)
  {
    if (sensorState == 1 && lastLiftState == 0)
    {
      delay(100);
      Serial.println("entering top of Lift");
    }
    if (sensorState == 0 && lastLiftState == 1)
    {
      delay(100);
      liftOC = false;
      layoutOC = true;
      Serial.println("leaving lift hill");
      lift_motor.updateMotorSpeed(0);
    }
    lastLiftState = sensorState;
  }
}

void eStopMode()
{
  //Serial.println("estopped");

  if (digitalRead(keyPin) == HIGH && digitalRead(eStopClearButtonPin) == HIGH)
  {
    //Serial.println("button pressed!");
    digitalWrite(eStopLedPin, 0);
    curMode = show;
    scanTrackForInitialValues();
  }
}

void activateEStop(int faultKey)
{ ///int faultKey
  lcdControllerInstance.updateUpperString("Fault:");
  switch (faultKey)
  {
  case 0:
    lcdControllerInstance.updateLowerString("EStop bnt active");
    break;
  case 1:
    lcdControllerInstance.updateLowerString("stn trig unexp");
    break;
  case 2:
    lcdControllerInstance.updateLowerString("lift trig unexp");
    break;
  case 3:
    lcdControllerInstance.updateLowerString("brk trig unexp");
    break;
  case 4:
    lcdControllerInstance.updateLowerString("prBrk trig unexp");
    break;
  default:
    lcdControllerInstance.updateLowerString("undoc error");
    break;
  }

  // Serial.println("GOING INTO ESTOP");
  station_motor.updateMotorSpeed(0);
  lift_motor.updateMotorSpeed(0);
  brake_motor.updateMotorSpeed(0);
  digitalWrite(eStopLedPin, 1);
  curMode = eStop;
}

//Will only allow the next coaster to go when an input is given
void dispatchMode()
{
  lcdControllerInstance.updateUpperString("Dispatch Mode");

  station_motor.updateDirection(1); //HIGH
  lift_motor.updateDirection(1);
  brake_motor.updateDirection(1);

  int dispatchButton = digitalRead(dispatchButtonPin);

  digitalWrite(stationOCLedPin, stationOC);
  digitalWrite(layoutOCLedPin, layoutOC);
  digitalWrite(breakRunOCLedPin, brakeRunOC);
  digitalWrite(liftOCLedPin, liftOC);

  ISRLift();
  ISRBrake();
  ISRPreBrake();
  ISRStation();
  //Serial.println(layout_OC);

  int StationSensor = not digitalRead(stationSensorPin);
  int liftSensor = not digitalRead(liftSensorPin);
  int brakeSensor = not digitalRead(brakeSensorPin);
  int preBrakeSensor = not digitalRead(preBrakeSensorPin);

  if (stationOC == false && StationSensor == HIGH)
  { //Stn trig unexp -> staton triggered unexpectidly
    activateEStop(1);
  }
  else if ((StationSensor == HIGH) && (liftOC == 0) && (dispatchButton == 1))
  {
    while (digitalRead(dispatchButtonPin) == 1)
      ;

    lcdControllerInstance.updateDispatch();
    Serial.println("button pressed here");        //FOR REAL APPLICATION use analogWrite()
    station_motor.updateMotorSpeed(stationSpeed); //FOR REAL APPLICATION use analogWrite()
    lift_motor.updateMotorSpeed(liftSpeed);
    liftOC = true;
  }
  if (liftSensor == HIGH)
  {
    if (liftOC == false)
    {                   //If lift sensor is triggered but lift is not activated estop will activate
      activateEStop(2); //lft trig unexp
    }
    else if (brakeRunOC == true || layoutOC == true)
    {

      lift_motor.updateMotorSpeed(0);
    }
    else
    {
      lift_motor.updateMotorSpeed(liftSpeed);
    }
  }

  if (brakeSensor == HIGH)
  {
    if (brakeRunOC == false)
    { //If the brake sensor is triggered when brake is not occupied estop will trigger
      Serial.println("break run not occupied");
      activateEStop(3); // brk trig unex
    }
    else if (stationOC == false)
    { //FOR REAL APPLICATION use analogWrite()
      station_motor.updateMotorSpeed(stationSpeed);
      brake_motor.updateMotorSpeed(brakeSpeed);
    }
    else
    {
      brake_motor.updateMotorSpeed(0);
    }
  }
  if (preBrakeSensor == HIGH && layoutOC == false)
  {
    activateEStop(4);
  }
}

//Will continue to run for a given amount of time with no action needed
void showMode()
{
  lcdControllerInstance.updateUpperString("Show Mode");
  station_motor.updateDirection(1);
  lift_motor.updateDirection(1);
  brake_motor.updateDirection(1);

  //if the the button pin is pressed the trains will continue to run for 1 minute
  int dispatchButton = digitalRead(dispatchButtonPin);
  if (dispatchButton == 1)
  {
    time = millis(); //when pressed the time is set to the current time
  }
  if (time != -1 && millis() < time + 5000) //60000
  {
    run = true; //as long as the recorder time + a minute is greater than the current time the train will run
  }
  else
  {
    run = false;
  }

  digitalWrite(stationOCLedPin, stationOC);
  digitalWrite(layoutOCLedPin, layoutOC);
  digitalWrite(breakRunOCLedPin, brakeRunOC);
  digitalWrite(liftOCLedPin, liftOC);

  ISRLift();
  ISRBrake();
  ISRPreBrake();
  ISRStation();

  int StationSensor = not digitalRead(stationSensorPin);
  int liftSensor = not digitalRead(liftSensorPin);
  int brakeSensor = not digitalRead(brakeSensorPin);
  int preBrakeSensor = not digitalRead(preBrakeSensorPin);

  //Serial.println("Printing");

  if (stationOC == false && StationSensor == HIGH)
  { //if station sensor is triggered, lift not occupied, and run is true but station is not activated estop wil activate
    activateEStop(1);
  }
  else if ((StationSensor == HIGH) && liftOC == 0 && run)
  {
    Serial.println("dispatch triggered");
    station_motor.updateMotorSpeed(stationSpeed);
    lift_motor.updateMotorSpeed(liftSpeed);
    liftOC = true;
  }

  if (liftSensor == HIGH)
  {
    if (liftOC == false)
    { //If lift sensor is triggered but lift is not activated estop will activate
      activateEStop(2);
    }
    else if (brakeRunOC == true || layoutOC == true)
    {
      lift_motor.updateMotorSpeed(0);
    }
    else
    {
      lift_motor.updateMotorSpeed(liftSpeed);
    }
  }

  if (brakeSensor == HIGH)
  {
    if (brakeRunOC == false)
    { //If brake sensor is triggered but brake is not occupied eStop will activte
      activateEStop(3);
    }
    else if (stationOC == false)
    {

      station_motor.updateMotorSpeed(stationSpeed);
      station_motor.updateMotorSpeed(brakeSpeed);
    }
    else
    {
      brake_motor.updateMotorSpeed(0);
    }
  }
  if (preBrakeSensor == HIGH && layoutOC == false)
  {
    activateEStop(4);
  }
}

//Will turn on specific motors on comand
void maintenanceMode()
{

  lcdControllerInstance.updateUpperString("Maintenance Mode");

  if (digitalRead(switchStationForward) == HIGH)
  {
    station_motor.updateMotorSpeed(maintenanceSpeed);
    station_motor.updateDirection(1);
  }
  else if (digitalRead(switchStationBackward) == HIGH)
  {
    station_motor.updateMotorSpeed(maintenanceSpeed);
    station_motor.updateDirection(0);
  }
  if (digitalRead(switchBrakeForward) == HIGH)
  {

    brake_motor.updateMotorSpeed(maintenanceSpeed);
    brake_motor.updateDirection(1);
  }
  else if (digitalRead(switchBrakeBackward) == HIGH)
  {
    brake_motor.updateMotorSpeed(maintenanceSpeed);
    brake_motor.updateDirection(0);
  }
  if (digitalRead(switchLiftForward) == HIGH)
  {
    lift_motor.updateMotorSpeed(maintenanceSpeed);
    lift_motor.updateDirection(1);
  }
  else if (digitalRead(switchLiftBackward) == HIGH)
  {
    lift_motor.updateMotorSpeed(maintenanceSpeed);
    lift_motor.updateDirection(0);
  }
}

//Config initial occupy values based on sensor input
void scanTrackForInitialValues()
{
  int stationSensor = not digitalRead(stationSensorPin);
  int liftSensor = not digitalRead(liftSensorPin);
  int brakeSensor = not digitalRead(brakeSensorPin);

  stationOC = false;
  brakeRunOC = false;
  liftOC = false;
  layoutOC = false;

  if (stationSensor == HIGH)
  {
    stationOC = true;
    Serial.println("At station");
  }

  if (brakeSensor == HIGH)
  {
    brakeRunOC = true;
    Serial.println("At brake");
  }

  if (liftSensor == HIGH)
  {
    liftOC = true;
    Serial.println("At lift");
  }
  digitalWrite(stationOCLedPin, stationOC);
  digitalWrite(layoutOCLedPin, layoutOC);
  digitalWrite(breakRunOCLedPin, brakeRunOC);
  digitalWrite(liftOCLedPin, liftOC);

  lcdControllerInstance.setUpDispatchString();
}
