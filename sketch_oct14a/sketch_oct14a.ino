void setup() 
{
  int StationPin = number;
  int liftPin = number;
  int preBreakPin = number;
  int breakPin = number;

  int station_motor = number;
  int lift_motor = number;
  int break_run_motor = number;
  
  // put your setup code here, to run once:
  bool station_OC;
  bool breakRun_OC;
  bool layout_OC;
  int StationSensor = 0;
  int liftSensor = 0;
  int breakSensor =0;

  //sets up LEDs as temp for the motors

  attachInterrupt(digitalPinToInerrupt(StationPin)/*station sensor*/, ISRStationFall, falling);
  attachInterrupt(digitalPinToInerrupt(preBreakPin)/*preBreak sensor*/, ISRPreBreakRise, rising);
  attachInterrupt(digitalPinToInerrupt(preBreakPin)/*preBreak sensor*/, ISRPreBreakFall, falling);
  attachInterrupt(digitalPinToInerrupt(breakPin)/*break sensor*/, ISRBreakFall, falling);
  attachInterrupt(digitalPinToInerrupt(liftPin)/*lift sensor*/, ISRLiftFall, falling);

  digitalWrite(station_motor, LOW);
  digitalWrite(lift_motor, LOW);
  digitalWrite(break_run_motor, LOW);

}

void loop() 
{
  StationSensor = digitalRead(stationPin);
  liftSensor = digitalRead(liftPin);
  breakSensor = digitalRead(breakPin);
  if(stationSensor =HIGH)
  {
     digitalWrite(station_motor, LOW); // stop station motor
    if(layout_OC = false)
    {
      
       digitalWrite(station_motor, HIGH); //start station motor
       digitalWrite(lift_motor, HIGH); //start lift motor
    }
  }
      if(liftSensor =HIGH)
  {
    if(break_OC = true)
       digitalWrite(lift_motor, LOW); //stop lift motor
    else
       digitalWrite(lift_motor, HIGH);//start lift motor
  }
    if(breakSensor =HIGH)
  {
    // stop break motor
    if(station_OC = false)
      digitalWrite(break_run_motor, HIGH)//start break motor
  }
    if(breakSensor =HIGH)
  {
    digitalWrite(break_run_motor, LOW)// stop break motor
    if(station_OC = false)
    {
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
void ISRStationFall() // leaving station
{
  station_OC = false;
  layout_OC = true;
  digitalWrite(station_motor, LOW); //stop station motor
}
void ISRPreBreakRise() //entering breakrun
{
  digitalWrite(break_run_motor, HIGH)//start break motor
}
void ISRPreBreakFall() //leaving layout
{
  layout_OC =false;
  breakRun_OC = true;
}
void ISRBreakFall() //leaving breakrun
{
  breakRun_OC = false;
  station_OC = true;
  digitalWrite(break_run_motor, LOW)//stop break motor
}
void ISRLiftFall() // leaving station
{
   digitalWrite(lift_motor, LOW);//stop lift motor
}
