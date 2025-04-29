#include <ArduinoBLE.h>
#include <IRremote.h>
#include <Servo.h>

Servo gun_servo; // servo

BLEService ratService("0000ffe0-0000-1000-8000-00805f9b34fb"); // bluetooth® low energy LED service
BLEByteCharacteristic switchCharacteristic("0000ffe1-0000-1000-8000-00805f9b34fb", BLERead | BLEWrite | BLEWriteWithoutResponse); // bluetooth® low energy LED switch characteristic - custom 128-bit UUID, read and writable by central

// pins for transmitter/reciever
int pin_send = 11;
int pin_rec = 12;

// servo position angle
int pos = 0;

// laser program setup
int hitCount = 0;
unsigned long lastIrSendTime = 0;     // for timing IR sending
const unsigned long irSendInterval = 1000;
unsigned long lastHitTime = 0;        // last valid hit timestamp
const unsigned long hitCooldown = 5000;       

// pin for LED
const int ledPin = LED_BUILTIN; 

// motor A pins
int enA = 9;
int in1 = 8;
int in2 = 7;
// motor B pins
int enB = 3;
int in3 = 5;
int in4 = 4;

byte currentCommand = 0; // 0 = stop


void setup(){
  Serial.begin(9600);
  while(!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);
  // set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);

  // turn off motors - initial state
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);  

  IrSender.begin(pin_send); // initializes IR sender
  IrReceiver.begin(pin_rec); // Iiitializes the IR receiver object

  // begin initialization
  if(!BLE.begin()){
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while(1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Death Destruction Bot #00");
  BLE.setAdvertisedService(ratService);

  // add the characteristic to the service
  ratService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ratService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Dating Start!");
}


void loop(){
  BLEDevice device = BLE.central();

  if (device){
    Serial.print("Connected to device: ");
    Serial.println(device.address());

    while(device.connected()){
      // BLE command receive
      if(switchCharacteristic.written()){
        byte receivedNum;
        switchCharacteristic.readValue(&receivedNum, 1);

        receivedNum = receivedNum - '0';  // ASCII to integer
        Serial.print("Command: ");
        Serial.println(receivedNum);

        currentCommand = receivedNum; // update movement
      }

      // RC car movement
      handleMovement(currentCommand);

      // IR sending (every 1 second)
      if (millis() - lastIrSendTime >= irSendInterval){
        IrSender.sendNEC(0x025, 0x025, 0); // Send IR
        Serial.println("    marco");
        lastIrSendTime = millis(); // reset timer
      }

      // IR receiving
      if(IrReceiver.decode()){
        Serial.println("    polo");

        // check if received data matches 0x025
        if (IrReceiver.decodedIRData.command == 0x025) {
          unsigned long currentTime = millis();

          if (currentTime - lastHitTime >= hitCooldown) {
            hitCount++;
            lastHitTime = currentTime;

            Serial.print("OUCH! Total Hits: ");
            Serial.println(hitCount);

            if (hitCount > 2) {
              TERMINATE();
            }
          }
        }
        IrReceiver.resume(); // ready to receive next signal
      }
    }

    Serial.print(F("Disconnected from device: "));
    Serial.println(device.address());
  }
}




void handleMovement(byte command){
  switch(command){
    case 0: // stop
      stopMotors();
      break;
    case 1: // forward
      digitalWrite(ledPin, HIGH);
      moveForward();
      break;
    case 2: // backward
      digitalWrite(ledPin, HIGH);
      moveBackward();
      break;
    case 3: // turn left
    	digitalWrite(ledPin, HIGH);
      turnLeft();
      break;
    case 4: // turn right
	    digitalWrite(ledPin, HIGH);
      turnRight();
      break;
    case 5: // turn gun left
      digitalWrite(ledPin, HIGH);
      turngunLeft();
      break;
    case 6: // turn gun left
      digitalWrite(ledPin, HIGH);
      turngunRight();
      break;
    default:
      digitalWrite(ledPin, LOW);
      stopMotors();
      break;
  }
}



// movement functions:
void moveForward(){
  analogWrite(enA, 200);
  analogWrite(enB, 200);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void moveBackward(){
  analogWrite(enA, 200);
  analogWrite(enB, 200);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnLeft(){
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnRight(){
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turngunLeft(){
  gun_servo.attach(2);
  gun_servo.write(180);
  delay(200);
  gun_servo.detach();
}

void turngunRight(){
  gun_servo.attach(2);
  gun_servo.write(-180);
  delay(200);
  gun_servo.detach();
}

void stopMotors(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void TERMINATE(){
  Serial.println("\n Game Over :(");

  // stop IR communication
  IrReceiver.stop(); // Correct way to stop IR receiving
  // IrSender does not need explicit disabling, just stop using it

  // turn off the motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // turn off the LED
  digitalWrite(ledPin, LOW);

  // stop BLE
  BLE.stopAdvertise();
  BLE.disconnect();
  BLE.end();

  while(1){
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }
}
