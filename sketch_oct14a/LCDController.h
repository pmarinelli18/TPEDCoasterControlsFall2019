struct LCDController  {
  private:
    String upperString = "";
    String lowerString = "";
    int lifeTimeCycle = EEPROM.read(0);
    int hourlyDispatch = 0;
    int lastHour = 0;
  public:
    void initLCDController(String initUpperString, String initLowerString) {
      upperString = initUpperString;
      lowerString = initLowerString;
    }
    void updateUpperString(String newUpperString) {
      if (upperString.compareTo(newUpperString) != 0) {
        upperString = newUpperString;
        Serial.print("Upper Line: ");//,upperString
        Serial.println(newUpperString);
      }
    }
    void updateLowerString(String newLowerString) {
      if (lowerString.compareTo(newLowerString) != 0) {
        lowerString = newLowerString;
        Serial.print("Lower Line: "); //, lowerString
        Serial.println(newLowerString);
        //update LCD screen here
      }
    }

    void setUpDispatchString() {
      char LCDText[17] = "DPH:000 LTD:0000";
      if (hourlyDispatch >= 100) {
        LCDText[4] = int((hourlyDispatch / 100)) % 10 + '0';
      }
      if (hourlyDispatch >= 10) {
        LCDText[5] = int((hourlyDispatch / 10)) % 10 + '0';
      }
      LCDText[6] = int((hourlyDispatch / 1)) % 10 + '0';

      if (lifeTimeCycle >= 1000) {
        LCDText[12] = int(lifeTimeCycle / 1000) % 10 + '0';
      }
      if (lifeTimeCycle >= 100) {
        LCDText[13] = int(lifeTimeCycle / 100) % 10 + '0';
      }
      if (lifeTimeCycle >= 10) {
        LCDText[14] = int(lifeTimeCycle / 10) % 10 + '0';
      }
      LCDText[15] = int(lifeTimeCycle / 1) % 10 + '0';
      updateLowerString(LCDText);
    }
    void updateDispatch() {
      lifeTimeCycle += 1;
      EEPROM.write(0, lifeTimeCycle);
      if ( (int)millis() - (lastHour + 3600000) >= 0) { //3000
        hourlyDispatch = 1;
        lastHour = millis();
      }
      else {
        hourlyDispatch += 1;
      }
      setUpDispatchString();
    }
};