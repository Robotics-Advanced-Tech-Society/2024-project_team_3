#include <ArduinoBLE.h>

BLEService ratService("0000ffe0-0000-1000-8000-00805f9b34fb"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("0000ffe1-0000-1000-8000-00805f9b34fb", BLERead | BLEWrite | BLEWriteWithoutResponse);

// pin to use for the LED
const int ledPin = LED_BUILTIN; 
// Motor A connections
int enA = 9;
int in1 = 8;
int in2 = 7;
// Motor B connections
int enB = 3;
int in3 = 5;
int in4 = 4;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);
  // Set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);

  // Turn off motors - Initial state
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
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

byte currentCommand = 0; // 0 = stop

void loop() {
  BLEDevice device = BLE.central();

  if (device) {
    Serial.print("Connected to device: ");
    Serial.println(device.address());

    while (device.connected()) {
      if (switchCharacteristic.written()) {
        byte receivedNum;
        switchCharacteristic.readValue(&receivedNum, 1);

        // Convert ASCII to number
        receivedNum = receivedNum - '0';

        Serial.print("Received command: ");
        Serial.println(receivedNum);

        currentCommand = receivedNum; // Update global state
      }

      // Motor control based on currentCommand
      handleMovement(currentCommand);
    }
    Serial.print(F("Disconnected from device: "));
    Serial.println(device.address());
  }
}

void handleMovement(byte command) {
  switch (command) {
    case 0: // Stop
      stopMotors();
      break;
    case 1: // Forward
      digitalWrite(ledPin, HIGH);
      moveForward();
      break;
    case 2: // Backward
      digitalWrite(ledPin, HIGH);
      moveBackward();
      break;
    case 3: // Turn left
    	digitalWrite(ledPin, HIGH);
      turnLeft();
      break;
    case 4: // Turn right
	    digitalWrite(ledPin, HIGH);
      turnRight();
      break;
    default:
      digitalWrite(ledPin, LOW);
      stopMotors();
      break;
  }
}

// Movement functions:
void moveForward() {
  analogWrite(enA, 200);
  analogWrite(enB, 200);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void moveBackward() {
  analogWrite(enA, 200);
  analogWrite(enB, 200);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnLeft() {
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnRight() {
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
