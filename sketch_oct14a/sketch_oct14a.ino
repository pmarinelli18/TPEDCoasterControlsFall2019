void setup() 
{
  int StationPin = number;
  int liftPin = number;
  int preBreakPin = number;
  int breakPin = number;

  
  // put your setup code here, to run once:
  bool station_OC;
  bool breakRun_OC;
  bool layout_OC;
  int StationSensor = 0;
  int liftSensor = 0;
  int breakSensor =0;
  

  attachInterrupt(digitalPinToInerrupt(pin)/*station sensor*/, ISRStationFall, falling);
  attachInterrupt(digitalPinToInerrupt(pin)/*preBreak sensor*/, ISRPreBreakRise, rising);
  attachInterrupt(digitalPinToInerrupt(pin)/*preBreak sensor*/, ISRPreBreakFall, falling);
  attachInterrupt(digitalPinToInerrupt(pin)/*break sensor*/, ISRBreakFall, falling);
  attachInterrupt(digitalPinToInerrupt(pin)/*lift sensor*/, ISRLiftFall, falling);
}

void loop() 
{
  StationSensor = digitalRead(stationPin);
  liftSensor = digitalRead(liftPin);
  breakSensor = digitalRead(breakPin);
  if(stationSensor =HIGH)
  {
    // stop station motor
    if(layout_OC = false)
    {
      //start station motor
      //start lift motor
    }
  }
      if(liftSensor =HIGH)
  {
    if(break_OC = true)
      //stop lift motor
    else
      //start lift motor
  }
    if(breakSensor =HIGH)
  {
    // stop break motor
    if(station_OC = false)
      //start break motor
  }
    if(breakSensor =HIGH)
  {
    // stop break motor
    if(station_OC = false)
    {
      //start station motor
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
  //stop station motor
}
void ISRPreBreakRise() //entering breakrun
{
  //start break motor
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
  //stop break motor
}
void ISRLiftFall() // leaving station
{
  //stop lift motor
}
