enum SwitchState {
  left,
  right,
  center
};

struct SwitchController  {
  private:
    int leftPin;
    int rightPin;

  public:
    SwitchController(int leftPin, int rightPin) {
      pinMode(leftPin, INPUT_PULLUP);
      pinMode(rightPin, INPUT_PULLUP);
      this->leftPin = leftPin;
      this->rightPin = rightPin;
    }

    SwitchState getState() {
      if (digitalRead(leftPin) == HIGH && digitalRead(rightPin) == LOW) {
        return left;
      } else if (digitalRead(rightPin) == HIGH && digitalRead(leftPin) == LOW) {
        return right;
      } else {
        return center;
      }
    }
};
