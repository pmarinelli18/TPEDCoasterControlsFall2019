//Location of pin input and output
#include <EEPROM.h>

#include "LCDController.h"

int StationPin = A5;
int station_motor = 2;
int stationDirection = 1;

int liftPin = A4;

int lift_motor = 4;
int liftDirection = 1;

int preBrakePin = A2;
int brakePin = A3;
int brake_run_motor = 5;
int brakeDirection = 1;
int eStopTripLED = 6;

int buttonPin = 11;
int keyPin = 10;

int switchShow = 13;
int switchMaintenance = 12;
int dispatchLED = 1;
int showLED = 1;
int maintenanceLED = 1;

int switchStationForward =   1; //NEED TO add correct pins
int switchStationBackward =  1;

int switchBrakeForward =   1;
int switchBrakeBackward =  1;

int switchLiftForward =   1;
int switchLiftBackward =  1;
// Initializes variables at what they should be at the beginning
bool stationOC = false;
bool liftOC = false;
bool layoutOC = false;
bool brakeRunOC = false;

int lastStationState = 0;
int lastLiftState = 0;
int lastPrebrakeState = 0;
int lastBrakeState = 0;
//int dispatchButton = 0;
int eStopPin = 9;
int tripEStopButton = 8;

int stationSpeed = 1;
int liftSpeed = 1; //liftMotor
int brakeSpeed = 1;

int maintenanceSpeed = 5;

int time = -1;
bool run = true;

enum mode {
	maintenance,
	dispatch,
	show,
	eStop
};
struct LCDController lcdControllerInstance;

mode curMode = show;

void setup() {
	Serial.begin(9600);
	
	//initlize lcd
	lcdControllerInstance.initLCDController("Welcome", "");
	
	//sets up LEDs as temp for the motors
	//]attachInterrupt(digitalPinToInterrupt(StationPin)/*station sensor*/, ISRStation, CHANGE);
	//attachInterrupt(digitalPinToInterrupt(preBrakePin)/*preBrake sensor*/, ISRPreBrake, CHANGE);
	//attachInterrupt(digitalPinToInterrupt(brakePin)/*brake sensor*/, ISRBrake, CHANGE);
	//attachInterrupt(digitalPinToInterrupt(liftPin)/*lift sensor*/, ISRLift, CHANGE);
	pinMode(StationPin, INPUT);
	pinMode(liftPin, INPUT);
	pinMode(preBrakePin, INPUT);
	pinMode(brakePin, INPUT);
	pinMode(buttonPin, INPUT);
	pinMode(eStopPin, INPUT);
	pinMode(eStopTripLED, OUTPUT);
	pinMode(tripEStopButton, INPUT);
	
	pinMode(7, OUTPUT);  //stationOC led
	//pinMode(8, OUTPUT);  //layoutOC led
	//pinMode(9, OUTPUT);  //brakeRunOC led
	pinMode(3, OUTPUT);  // led
	
	
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
	
	pinMode(dispatchLED, OUTPUT);
	pinMode(showLED, OUTPUT);
	pinMode(maintenanceLED, OUTPUT);
	
	scanTrackForInitialValues();
	
}
void loop() {
	int keyValue = digitalRead(keyPin);
	
	
	if (digitalRead(tripEStopButton) == HIGH) {
		activateEStop(0);
	}
	
	if (curMode == eStop) {
		eStopMode();
	}
	//If the key is inserted
	else if (keyValue == 1) {
		lcdControllerInstance.setUpDispatchString();
		curMode = show;
		if (digitalRead(switchShow) == HIGH) {
			showMode();
			digitalWrite(showLED, HIGH);
			digitalWrite(maintenanceLED, LOW);
			digitalWrite(dispatchLED, LOW);
		}
		//maintenance mode
		else if  (digitalRead(switchMaintenance) == HIGH) {
			curMode = maintenance;
			maintenanceMode();
			digitalWrite(showLED, LOW);
			digitalWrite(maintenanceLED, HIGH);
			digitalWrite(dispatchLED, LOW);
		}
		//Dispatch mode
		else {
			curMode = dispatch;
			dispatchMode();
			digitalWrite(showLED, LOW);
			digitalWrite(maintenanceLED, LOW);
			digitalWrite(dispatchLED, HIGH);
		}
	}
	
	//If the key is not inserted
	else {
		if (curMode == show ) {
			showMode();
			digitalWrite(showLED, HIGH);
			digitalWrite(maintenanceLED, LOW);
			digitalWrite(dispatchLED, LOW);
		}
		else {
			dispatchMode();
			digitalWrite(showLED, LOW);
			digitalWrite(maintenanceLED, LOW);
			digitalWrite(dispatchLED, HIGH);
		}
		
		if (curMode == show && digitalRead(switchShow) != HIGH) {
			lcdControllerInstance.updateLowerString("Insert Key");
		}
		else if (curMode == dispatch && (digitalRead(switchShow) == HIGH || digitalRead(switchMaintenance) == HIGH)) {
			lcdControllerInstance.updateLowerString("Insert Key");
		}
		else if (curMode == maintenance && digitalRead(switchMaintenance) == HIGH) {
			lcdControllerInstance.updateLowerString("Insert Key");
		}
		else if (curMode != eStop) {
			lcdControllerInstance.setUpDispatchString();
		}
	}
}

void ISRStation() { // station
	int sensorState = not digitalRead(StationPin);
	if (curMode != eStop) {
		
		if (sensorState == 1 && lastStationState == 0) {
			delay(100);
			Serial.println("entering Station");
			analogWrite(station_motor, 0);
		}
		if (sensorState == 0 && lastStationState == 1) {
			delay(100);
			Serial.println("leaving Station");
			stationOC = false;
			analogWrite(station_motor, 0);
			
		}
		lastStationState = sensorState;
	}
}

void ISRPreBrake() //entering brakerun
{
	int sensorState = not digitalRead(preBrakePin);
	if (curMode != eStop) {
		if (sensorState == 1 && lastPrebrakeState == 0) {
			delay(100);
			Serial.println("entering pre-brake-run");
			digitalWrite(brake_run_motor, brakeSpeed); //FOR REAL APPLICATION use analogWrite()
			brakeRunOC = true;
		}
		if (sensorState == 0 && lastPrebrakeState == 1) {
			delay(100);
			Serial.println("leaving pre-brakerun...off layout");
			layoutOC = false;
		}
		lastPrebrakeState = sensorState;
	}
}

void ISRBrake() //leaving brakerun
{
	int sensorState = not digitalRead(brakePin);
	if (curMode != eStop) {
		if (sensorState == 1 && lastBrakeState == 0) {
			delay(100);
			Serial.println("at Brakerun");
		}
		if (sensorState == 0 && lastBrakeState == 1) {
			delay(100);
			Serial.println("left brakerun");
			stationOC = true;
			brakeRunOC = false;
			digitalWrite(brake_run_motor, 0); //FOR REAL APPLICATION use analogWrite()
			
		}
		lastBrakeState = sensorState;
	}
}

void ISRLift() // leaving lift
{
	int sensorState = not digitalRead(liftPin);
	if (curMode != eStop) {
		if (sensorState == 1 && lastLiftState == 0) {
			delay(100);
			Serial.println("entering top of Lift");
		}
		if (sensorState == 0 && lastLiftState == 1) {
			delay(100);
			liftOC = false;
			layoutOC = true;
			Serial.println("leaving lift hill");
			analogWrite(lift_motor, 0);
		}
		lastLiftState = sensorState;
	}
}

void eStopMode() {
	//Serial.println("estopped");
	
	if (digitalRead(keyPin) == HIGH && digitalRead(eStopPin) == HIGH ) {
		//Serial.println("button pressed!");
		digitalWrite(eStopTripLED, 0);
		curMode = show;
		scanTrackForInitialValues();
	}
}

void activateEStop(int faultKey) { ///int faultKey
	lcdControllerInstance.updateUpperString("Fault:");
	switch (faultKey) {
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
	digitalWrite(station_motor, 0);
	digitalWrite(lift_motor, 0);
	digitalWrite(brake_run_motor, 0);
	digitalWrite(eStopTripLED, 1);
	curMode = eStop;
}

//Will only allow the next coaster to go when an input is given
void dispatchMode()
{
	lcdControllerInstance.updateUpperString("Dispatch Mode");
	

	digitalWrite(stationDirection, HIGH);
	digitalWrite(liftDirection, HIGH);
	digitalWrite(brakeDirection, HIGH);
	
	int dispatchButton = digitalRead(buttonPin);
	
	digitalWrite(7, stationOC); //print out layout OC to LED
	//digitalWrite(8, layoutOC); //layout_OC
	//digitalWrite(9, brakeRunOC);
	digitalWrite(3, liftOC);
	
	ISRLift();
	ISRBrake();
	ISRPreBrake();
	ISRStation();
	//Serial.println(layout_OC);
	
	int StationSensor = not digitalRead(StationPin);
	int liftSensor = not digitalRead(liftPin);
	int brakeSensor = not digitalRead(brakePin);
	int preBrakeSensor = not digitalRead(preBrakePin);
	
	
	if (stationOC == false && StationSensor == HIGH) { //Stn trig unexp -> staton triggered unexpectidly
		activateEStop(1);
	}
	else if ((StationSensor == HIGH) && (liftOC == 0) && (dispatchButton == 1)) {
		while (digitalRead(buttonPin) == 1);
		
		lcdControllerInstance.updateDispatch();
		Serial.println("button pressed here");
		digitalWrite(station_motor, stationSpeed); //FOR REAL APPLICATION use analogWrite()
		digitalWrite(lift_motor, liftSpeed); //FOR REAL APPLICATION use analogWrite()
		liftOC = true;
	}
	if (liftSensor == HIGH) {
		if (liftOC == false) { //If lift sensor is triggered but lift is not activated estop will activate
			activateEStop(2);   //lft trig unexp
		}
		else if (brakeRunOC == true || layoutOC == true) {
			digitalWrite(lift_motor, 0); //FOR REAL APPLICATION use analogWrite()
		}
		else {
			digitalWrite(lift_motor, liftSpeed); //FOR REAL APPLICATION use analogWrite()
		}
	}
	
	if (brakeSensor == HIGH) {
		if (brakeRunOC == false) { //If the brake sensor is triggered when brake is not occupied estop will trigger
			Serial.println("break run not occupied");
			activateEStop(3);   // brk trig unex
		}
		else if (stationOC == false) {
			digitalWrite(station_motor, stationSpeed); //FOR REAL APPLICATION use analogWrite()
			digitalWrite(brake_run_motor, brakeSpeed);//FOR REAL APPLICATION use analogWrite()
			//start brake motor
		}
		else {
			digitalWrite(brake_run_motor, 0); //FOR REAL APPLICATION use analogWrite()
		}
	}
	if (preBrakeSensor == HIGH && layoutOC == false) {
		activateEStop(4);
	}
}




//Will continue to run for a given amount of time with no action needed
void showMode()
{
	lcdControllerInstance.updateUpperString("Show Mode");
	
	digitalWrite(stationDirection, HIGH);
	digitalWrite(liftDirection, HIGH);
	digitalWrite(brakeDirection, HIGH);
	
	
	//if the the button pin is pressed the trains will continue to run for 1 minute
	int dispatchButton = digitalRead(buttonPin);
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
	
	digitalWrite(7, stationOC); //print out layout OC to LED
	//digitalWrite(8, layoutOC); //layout_OC
	//digitalWrite(9, brakeRunOC);
	digitalWrite(3, liftOC);
	
	
	ISRLift();
	ISRBrake();
	ISRPreBrake();
	ISRStation();
	
	
	
	int StationSensor = not digitalRead(StationPin);
	int liftSensor = not digitalRead(liftPin);
	int brakeSensor = not digitalRead(brakePin);
	int preBrakeSensor = not digitalRead(preBrakePin);
	
	//Serial.println("Printing");
	
	if (stationOC == false && StationSensor == HIGH) { //if station sensor is triggered, lift not occupied, and run is true but station is not activated estop wil activate
		activateEStop(1);
	}
	else if ((StationSensor == HIGH) && liftOC == 0 && run) {
		Serial.println("dispatch triggered");
		digitalWrite(station_motor, stationSpeed); //FOR REAL APPLICATION use analogWrite()
		digitalWrite(lift_motor, liftSpeed); //FOR REAL APPLICATION use analogWrite()
		liftOC = true;
	}
	
	if (liftSensor == HIGH) {
		if (liftOC == false) { //If lift sensor is triggered but lift is not activated estop will activate
			activateEStop(2);
		}
		else if (brakeRunOC == true || layoutOC == true) {
			digitalWrite(lift_motor, 0);//FOR REAL APPLICATION use analogWrite()
		}
		else {
			digitalWrite(lift_motor, liftSpeed); //FOR REAL APPLICATION use analogWrite()
		}
	}
	
	if (brakeSensor == HIGH) {
		if (brakeRunOC == false) { //If brake sensor is triggered but brake is not occupied eStop will activte
			activateEStop(3);
		}
		else if (stationOC == false) {
			digitalWrite(station_motor, stationSpeed);//FOR REAL APPLICATION use analogWrite()
			digitalWrite(brake_run_motor, brakeSpeed);//FOR REAL APPLICATION use analogWrite()
			//start brake motor
		}
		else {
			digitalWrite(brake_run_motor, 0);//FOR REAL APPLICATION use analogWrite()
		}
	}
	if (preBrakeSensor == HIGH && layoutOC == false) {
		activateEStop(4);
	}
}


//Will turn on specific motors on comand
void maintenanceMode()
{
	
	lcdControllerInstance.updateUpperString("Maintenance Mode");
	
	
	if (digitalRead(switchStationForward) == HIGH ) {
		digitalWrite(stationDirection, HIGH);
		analogWrite(station_motor, maintenanceSpeed);
	}
	else if  (digitalRead(switchStationBackward) == HIGH) {
		digitalWrite(stationDirection, LOW);
		analogWrite(station_motor, maintenanceSpeed);
	}
	if (digitalRead(switchBrakeForward) == HIGH ) {
		
		digitalWrite(brakeDirection, HIGH);
		analogWrite(brake_run_motor, maintenanceSpeed);
	}
	else if  (digitalRead(switchBrakeBackward) == HIGH) {
		
		digitalWrite(brakeDirection, LOW);
		analogWrite(brake_run_motor, maintenanceSpeed);
	}
	if (digitalRead(switchLiftForward) == HIGH ) {

		digitalWrite(liftDirection, HIGH); //sets direction of station motor
		analogWrite(lift_motor, maintenanceSpeed);
	}
	else if  (digitalRead(switchLiftBackward) == HIGH) {
		digitalWrite(liftDirection, LOW); //sets direction of station motor
		analogWrite(lift_motor, maintenanceSpeed);
	}
}

//Config initial occupy values based on sensor input
void scanTrackForInitialValues() {
	int stationSensor = not digitalRead(StationPin);
	int liftSensor = not digitalRead(liftPin);
	int brakeSensor = not digitalRead(brakePin);
	
	stationOC = false;
	brakeRunOC = false;
	liftOC = false;
	layoutOC = false;
	
	if (stationSensor == HIGH) {
		stationOC = true;
		Serial.println("At station");
	}
	
	if (brakeSensor == HIGH) {
		brakeRunOC = true;
		Serial.println("At brake");
		
	}
	
	if (liftSensor == HIGH) {
		liftOC = true;
		Serial.println("At lift");
		
	}
	digitalWrite(7, stationOC); //print out layout OC to LED
	//digitalWrite(8, layoutOC); //layout_OC
	//digitalWrite(9, brakeRunOC);
	
	lcdControllerInstance.setUpDispatchString();
	
}

