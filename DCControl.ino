// Pins
const int EN_PINS[2] = {}; 
const int DIR1_PINS[2] = {};
const int DIR2_PINS[2] = {};

int controls[2]; // stores x values of controler (place holder)

void setup(){
    for (int i = 0; i < 2; i++){
        pinMode(EN_PINS[i], OUTPUT);
        pinMode(DIR1_PINS[i], OUTPUT);
        pinMode(DIR2_PINS[i], OUTPUT);

        digitalWrite(DIR1_PINS[i], LOW);
        digitalWrite(DIR2_PINS[i], LOW);
    }
}

void loop() {
    for (int i = 0; i < 2; i++) {
        if (controls[i] > 0) controlMotor(i, 0);
        else if (controls[i] < 0) controlMotor(i, 1);
        else analogWrite(EN_PINS[i], 0);
    }
}

void controlMotor(int i, int dir){    
    digitalWrite(DIR1_PINS[i], dir? HIGH: LOW);
    digitalWrite(DIR2_PINS[i], dir? LOW: HIGH);
    analogWrite(EN_PINS[i], abs(controls[i]) * 255);
}