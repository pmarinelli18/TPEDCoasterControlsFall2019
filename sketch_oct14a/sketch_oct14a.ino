// Location of pin input and output
#include <EEPROM.h>

#include "LCDController.h"
#include "SwitchController.h"
#include "MotorController.h"

// Motors
//*** pin locations should be changed to where the wires end up being**
struct MotorController station_motor;
struct MotorController lift_motor;
struct MotorController brake_motor;

int stationSpeed = 255;
int liftSpeed = 255; // liftMotor
int brakeSpeed = 255;
int maintenanceSpeed = 255;

// Sensors
int stationSensorPin = A15;
int liftSensorPin = A14;
int clearedPointSensorPin = A13;
int preBrakeSensorPin = A12;
int brakeSensorPin = A11;

// Indicator LEDs
int eStopLedPin = 7;
int stationOCLedPin = 6;
int liftOCLedPin = 5;
int breakRunOCLedPin = 4;
int layoutOCLedPin = 3;
int dispatchReadyLEDPin = 2;

// Buttons
int dispatchButtonPin = 28;
int keyPin = 29;
int eStopClearButtonPin = 30;
int eStopButtonPin = 31;

// Block occupiled bools
//  Initializes variables at what they should be at the beginning
bool stationOC = false;
bool liftOC = false;
bool layoutOC = false;
bool clearedPoint = true;
bool brakeRunOC = false;

bool dispatchReady = false;

int lastStationState = 0;
int lastLiftState = 0;
int lastPrebrakeState = 0;
int lastBrakeState = 0;

// Show mode vars
unsigned long time = 0;
bool run = false;

enum mode
{
  maintenance,
  dispatch,
  show,
  eStop
};

struct LCDController lcdControllerInstance;
mode curMode = show;

SwitchController modeSwitch = SwitchController(27, 26);

SwitchController brakeRunSwitch = SwitchController(36, 37);
SwitchController liftSwitch = SwitchController(34, 35);
SwitchController stationSwitch = SwitchController(32, 33);

void setup()
{
  Serial.begin(9600);

  // initlize lcd
  lcdControllerInstance.initLCDController("Welcome", "");

  station_motor.initMotor(13, 12);
  lift_motor.initMotor(11, 10);
  brake_motor.initMotor(9, 8);
  // sets up LEDs as temp for the motors
  // attachInterrupt(digitalPinToInterrupt(stationSensorPin)/*station sensor*/, ISRStation, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(preBrakeSensorPin)/*preBrake sensor*/, ISRPreBrake, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(brakeSensorPin)/*brake sensor*/, ISRBrake, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(liftSensorPin)/*lift sensor*/, ISRLift, CHANGE);

  pinMode(stationSensorPin, INPUT_PULLUP);
  pinMode(liftSensorPin, INPUT_PULLUP);
  pinMode(preBrakeSensorPin, INPUT_PULLUP);
  pinMode(brakeSensorPin, INPUT_PULLUP);

  pinMode(eStopClearButtonPin, INPUT_PULLUP);
  pinMode(eStopLedPin, OUTPUT);
  pinMode(eStopButtonPin, INPUT_PULLUP);
  pinMode(dispatchButtonPin, INPUT_PULLUP);
  pinMode(dispatchReadyLEDPin, OUTPUT);

  pinMode(stationOCLedPin, OUTPUT);  // stationOC led
  pinMode(layoutOCLedPin, OUTPUT);   // layoutOC led
  pinMode(breakRunOCLedPin, OUTPUT); // brakeRunOC led
  pinMode(liftOCLedPin, OUTPUT);     // led

  // digitalWrite(station_motor, LOW);

  pinMode(keyPin, INPUT_PULLUP);

  scanTrackForInitialValues();
}
void loop()
{
  int keyValue = digitalRead(keyPin);

  if (digitalRead(eStopButtonPin) == HIGH)
  {
    activateEStop(0);
  }

  else if (curMode == eStop)
  {
    eStopMode();
  }
  // If the key is inserted
  else if (keyValue == 1)
  {
    lcdControllerInstance.setUpDispatchString();

    switch (modeSwitch.getState())
    {
    case left:
      curMode = maintenance;
      maintenanceMode();
      break;
    case right:
      curMode = show;
      showMode();
      break;
    case center:
      curMode = dispatch;
      dispatchMode();
      break;
    }
  }

  // If the key is not inserted
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
    // Display Insert key is there is a discrepancy between the current mode in the system and the actual switch
    // Eg Display Inster Key if the system is in show mode and the switch is not to the right
    if (curMode == show && modeSwitch.getState() != right)
    {
      lcdControllerInstance.updateLowerString("Insert Key");
    }
    else if (curMode == dispatch && modeSwitch.getState() != center)
    {
      lcdControllerInstance.updateLowerString("Insert Key");
    }
    else if (curMode == maintenance)
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

void ISRPreBrake() // entering brakerun
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

void ISRBrake() // leaving brakerun
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
      clearedPoint = false;
      delay(100);
      liftOC = false;
      layoutOC = true;
      Serial.println("leaving lift hill");
      lift_motor.updateMotorSpeed(0);
    }
    lastLiftState = sensorState;
  }
}

void ISRClearedPoint() // leaving lift
{
  int sensorState = not digitalRead(clearedPointSensorPin);
  if (curMode != eStop)
  {
    if (!clearedPoint && sensorState == 1)
    {
      Serial.println("Coaster has cleared the point");

      clearedPoint = true;
    }
  }
}

void eStopMode()
{
  // Serial.println("estopped");
  station_motor.updateMotorSpeed(0);
  lift_motor.updateMotorSpeed(0);
  brake_motor.updateMotorSpeed(0);
  if (digitalRead(keyPin) == HIGH && digitalRead(eStopClearButtonPin) == HIGH)
  {
    // Serial.println("button pressed!");
    digitalWrite(eStopLedPin, 0);
    curMode = show;
    scanTrackForInitialValues();
  }
}

void activateEStop(int faultKey)
{ /// int faultKey

  dispatchReady = false;
  digitalWrite(dispatchReadyLEDPin, dispatchReady);

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
  // Serial.println("Setting the speed to 0");
  digitalWrite(eStopLedPin, 1);
  curMode = eStop;
  // Reset show mode
  time = 0;
}

// Will only allow the next coaster to go when an input is given
void dispatchMode()
{
  lcdControllerInstance.updateUpperString("Dispatch Mode");

  station_motor.updateDirection(1); // HIGH
  lift_motor.updateDirection(1);
  brake_motor.updateDirection(1);

  int dispatchButton = !digitalRead(dispatchButtonPin);

  digitalWrite(stationOCLedPin, stationOC);
  digitalWrite(layoutOCLedPin, layoutOC);
  digitalWrite(breakRunOCLedPin, brakeRunOC);
  digitalWrite(liftOCLedPin, liftOC);

  ISRLift();
  ISRBrake();
  ISRPreBrake();
  ISRStation();
  ISRClearedPoint();
  // Serial.println(layout_OC);

  int StationSensor = not digitalRead(stationSensorPin);
  int liftSensor = not digitalRead(liftSensorPin);
  int brakeSensor = not digitalRead(brakeSensorPin);
  int preBrakeSensor = not digitalRead(preBrakeSensorPin);

  if (stationOC == false && StationSensor == HIGH)
  { // Stn trig unexp -> staton triggered unexpectidly
    activateEStop(1);
  }
  else if ((StationSensor == HIGH) && (liftOC == 0) && !(dispatchButton == 1) && clearedPoint) // will turn on dispatch LED
  {
    if (!dispatchReady)
    {
      dispatchReady = true;
      digitalWrite(dispatchReadyLEDPin, dispatchReady);
    }
  }
  else if ((StationSensor == HIGH) && (liftOC == 0) && (dispatchButton == 1) && clearedPoint)
  {
    while (digitalRead(dispatchButtonPin) == 1)
    {
      Serial.println(dispatchButton);
    }

    lcdControllerInstance.updateDispatch();
    Serial.println("button pressed here");        // FOR REAL APPLICATION use analogWrite()
    station_motor.updateMotorSpeed(stationSpeed); // FOR REAL APPLICATION use analogWrite()
    lift_motor.updateMotorSpeed(liftSpeed);
    liftOC = true;
    dispatchReady = false;
    digitalWrite(dispatchReadyLEDPin, dispatchReady);
  }
  if (liftSensor == HIGH)
  {
    if (liftOC == false)
    {                   // If lift sensor is triggered but lift is not activated estop will activate
      activateEStop(2); // lft trig unexp
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
    { // If the brake sensor is triggered when brake is not occupied estop will trigger
      Serial.println("break run not occupied");
      activateEStop(3); // brk trig unex
    }
    else if (stationOC == false)
    { // FOR REAL APPLICATION use analogWrite()
      station_motor.updateMotorSpeed(stationSpeed);
      brake_motor.updateMotorSpeed(brakeSpeed);
      // Serial.println("Setting the speed");
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

// Will continue to run for a given amount of time with no action needed
void showMode()
{
  lcdControllerInstance.updateUpperString("Show Mode");

  // if the the button pin is pressed the trains will continue to run for 1 minute
  int dispatchButton = not digitalRead(dispatchButtonPin);
  if (dispatchButton == 1)
  {
    Serial.println("Starting timer");
    time = millis(); // when pressed the time is set to the current time
    Serial.println(time);
  }

  if (time != -0 && millis() < time + 60000) // 60000
  {
    run = true; // as long as the recorder time + a minute is greater than the current time the train will run
    dispatchReady = false;
    digitalWrite(dispatchReadyLEDPin, dispatchReady);
  }
  else
  {
    if (!dispatchReady)
    {
      dispatchReady = true;
      digitalWrite(dispatchReadyLEDPin, dispatchReady);
    }
    run = false;
  }

  station_motor.updateDirection(1); // HIGH
  lift_motor.updateDirection(1);
  brake_motor.updateDirection(1);

  digitalWrite(stationOCLedPin, stationOC);
  digitalWrite(layoutOCLedPin, layoutOC);
  digitalWrite(breakRunOCLedPin, brakeRunOC);
  digitalWrite(liftOCLedPin, liftOC);

  ISRLift();
  ISRBrake();
  ISRPreBrake();
  ISRStation();
  ISRClearedPoint();
  // Serial.println(layout_OC);

  int StationSensor = not digitalRead(stationSensorPin);
  int liftSensor = not digitalRead(liftSensorPin);
  int brakeSensor = not digitalRead(brakeSensorPin);
  int preBrakeSensor = not digitalRead(preBrakeSensorPin);

  // Serial.println("Printing");

  if (stationOC == false && StationSensor == HIGH)
  { // Stn trig unexp -> staton triggered unexpectidly
    activateEStop(1);
  }

  else if ((StationSensor == HIGH) && liftOC == 0 && run && clearedPoint)
  {
    lcdControllerInstance.updateDispatch();
    Serial.println("button pressed here");        // FOR REAL APPLICATION use analogWrite()
    station_motor.updateMotorSpeed(stationSpeed); // FOR REAL APPLICATION use analogWrite()
    lift_motor.updateMotorSpeed(liftSpeed);
    liftOC = true;
  }
  if (liftSensor == HIGH)
  {
    if (liftOC == false)
    {                   // If lift sensor is triggered but lift is not activated estop will activate
      activateEStop(2); // lft trig unexp
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
    { // If the brake sensor is triggered when brake is not occupied estop will trigger
      Serial.println("break run not occupied");
      activateEStop(3); // brk trig unex
    }
    else if (stationOC == false)
    { // FOR REAL APPLICATION use analogWrite()
      station_motor.updateMotorSpeed(stationSpeed);
      brake_motor.updateMotorSpeed(brakeSpeed);
      // Serial.println("Setting the speed");
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

// Will turn on specific motors on comand
void maintenanceMode()
{

  lcdControllerInstance.updateUpperString("Maintenance Mode");
  if (dispatchReady == true)
  {
    dispatchReady = false;
    digitalWrite(dispatchReadyLEDPin, dispatchReady);
  }

  if (stationSwitch.getState() == left)
  {
    station_motor.updateMotorSpeed(maintenanceSpeed);
    station_motor.updateDirection(1);
  }
  else if (stationSwitch.getState() == right)
  {
    station_motor.updateMotorSpeed(maintenanceSpeed);
    station_motor.updateDirection(0);
  }
  else
  {
    station_motor.updateMotorSpeed(0);
  }

  if (brakeRunSwitch.getState() == left)
  {
    Serial.println("Left");
    brake_motor.updateMotorSpeed(maintenanceSpeed);
    brake_motor.updateDirection(1);
  }
  else if (brakeRunSwitch.getState() == right)
  {
    Serial.println("Right");
    brake_motor.updateMotorSpeed(maintenanceSpeed);
    brake_motor.updateDirection(0);
  }
  else
  {
    brake_motor.updateMotorSpeed(0);
  }
  if (liftSwitch.getState() == left)
  {
    lift_motor.updateMotorSpeed(maintenanceSpeed);
    lift_motor.updateDirection(1);
  }
  else if (liftSwitch.getState() == right)
  {
    lift_motor.updateMotorSpeed(maintenanceSpeed);
    lift_motor.updateDirection(0);
  }
  else
  {
    lift_motor.updateMotorSpeed(0);
  }
}

// Config initial occupy values based on sensor input
void scanTrackForInitialValues()
{
  int stationSensor = not digitalRead(stationSensorPin);
  int liftSensor = not digitalRead(liftSensorPin);
  int brakeSensor = not digitalRead(brakeSensorPin);

  stationOC = false;
  brakeRunOC = false;
  liftOC = false;
  Serial.println("lift false heerrre e");
  layoutOC = false;
  clearedPoint = true;

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

    Serial.println("lift treeeeueeee heeerrrrree");
    Serial.println("At lift");
  }
  digitalWrite(stationOCLedPin, stationOC);
  digitalWrite(layoutOCLedPin, layoutOC);
  digitalWrite(breakRunOCLedPin, brakeRunOC);
  digitalWrite(liftOCLedPin, liftOC);

  lcdControllerInstance.setUpDispatchString();
}
