void setup() 
{
  // put your setup code here, to run once:
  bool station_OC;
  bool lift_OC;
  bool layout_OC;
  attachInterrupt(digitalPinToInerrupt(pin)/*lift sensor*/, ISRLift, falling);
  attachInterrupt(digitalPinToInerrupt(pin)/*break sensor*/, ISRBreak, falling);
  attachInterrupt(digitalPinToInerrupt(pin)/*station sensor*/, ISRStation, falling);
  attachInterrupt(digitalPinToInerrupt(pin)/*preBreak sensor*/, ISRPreBreak, rising);
}

void loop() 
{
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
void ISRLift()
{
  lift_OC = false;
  layout_OC = true;
}
void ISRBreak()
{
  layout_OC = false;
  station_OC = true;
}
void ISRStation()
{
  station_OC = false;
  lift_OC = true;
}
void ISRPreBreak()
{
  //BreakMotor = true;
}
