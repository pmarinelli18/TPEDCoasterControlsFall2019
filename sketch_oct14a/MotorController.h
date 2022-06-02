struct MotorController
{
private:
    int speed; // (1-255)  //The speed of the motors
    bool fwd;  // Which way the motor is spinning
    bool bwd;
    int fwdSpeedPinNumber; // Which pin the Motor is located on
    int bwdSpeedPinNumber; // Which pin the Motor is located on
    bool direction;
    //    int fwdDirectionPin; //Which pin the direction is controlled at
    //    int bwdDirectionPin; //Which pin the direction is controlled at

public:
    void initMotor(int FWDSpeedPinNumber, int BWDSpeedPinNumber)
    {
        pinMode(FWDSpeedPinNumber, OUTPUT);
        pinMode(BWDSpeedPinNumber, OUTPUT);

        fwdSpeedPinNumber = FWDSpeedPinNumber;
        bwdSpeedPinNumber = BWDSpeedPinNumber;

        direction = 1; // defaults the direction to 1
        fwd = 1;
        bwd = 0;
        speed = 0;

        digitalWrite(fwdSpeedPinNumber, LOW);
        digitalWrite(bwdSpeedPinNumber, LOW);
    }
    void updateMotorSpeed(int newSpeed)
    {
        // Serial.println(speed);//,upperString

        if (newSpeed >= 0 && newSpeed <= 255)
        {
            speed = newSpeed;

            digitalWrite(fwdSpeedPinNumber, speed);
            digitalWrite(bwdSpeedPinNumber, speed);
        }

        // Needed for LEDs only

        // Serial.println(newSpeed);

        if (newSpeed == 0)
        {
            digitalWrite(fwdSpeedPinNumber, LOW);
            digitalWrite(bwdSpeedPinNumber, LOW);
        }
        else
        {
            if (direction == 1)
            {
                digitalWrite(fwdSpeedPinNumber, speed);
                digitalWrite(bwdSpeedPinNumber, LOW);
            }
            else
            {
                digitalWrite(fwdSpeedPinNumber, LOW);
                digitalWrite(bwdSpeedPinNumber, speed);
            }
        }
    }
    void stopMotor()
    {
        speed = 0;
        // analogWrite(pinNumber, speed);
        digitalWrite(fwdSpeedPinNumber, LOW);
        digitalWrite(bwdSpeedPinNumber, LOW);
    }
    void updateDirection(bool newDirection)
    {
        if (newDirection == true)
        {
            direction = 1;
            digitalWrite(fwdSpeedPinNumber, speed);
            digitalWrite(bwdSpeedPinNumber, LOW);
        }
        else
        {
            direction = 0;
            digitalWrite(fwdSpeedPinNumber, LOW);
            digitalWrite(bwdSpeedPinNumber, speed);
        }
    }
};
