struct MotorController
{
private:
    int speed;        // (1-255)  //The speed of the motors
    bool direction;   // Which way the motor is spinning
    int pinNumber;    // Which pin the Motor is located on
    int directionPin; //Which pin the direction is controlled at

public:
    void initMotor(int InputPinNumber, int InputDirectionPin)
    {
        pinMode(InputPinNumber, OUTPUT);
        digitalWrite(InputDirectionPin, HIGH);
        pinNumber = InputPinNumber;
        speed = 1;     // defaults the speed at 1
        direction = 1; //defaults the direction to 1
    }
    void updateMotorSpeed(int newSpeed)
    {
        if (newSpeed >= 0 && newSpeed <= 255)
        {
            speed = newSpeed;
            analogWrite(pinNumber, speed);
        }
    }
    void stopMotor()
    {
        speed = 0;
        analogWrite(pinNumber, speed);
    }
    void updateDirection(bool newDirection)
    {
        if (newDirection == true)
        {
            direction = 1;
            digitalWrite(directionPin, HIGH);
        }
        else
        {
            direction = 0;
            digitalWrite(directionPin, LOW);
        }
    }
};
