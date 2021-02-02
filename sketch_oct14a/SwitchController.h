enum SwitchState {
  left,
  right,
  center
};

struct Switch  {
  private:
    int leftPin;
    int rightPin;

  public:
    Switch(int leftPin, int rightPin) {
      pinMode(leftPin, INPUT);
      pinMode(rightPin, INPUT);
      this->leftPin = leftPin;
      this->rightPin = rightPin;
    }

    SwitchState getState() {
      if (digitalRead(leftPin) == HIGH) {
        return left;
      } else if (digitalRead(rightPin) == HIGH) {
        return right;
      } else {
        return center;
      }
    }
};
