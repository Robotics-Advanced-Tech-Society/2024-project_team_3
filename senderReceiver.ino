#include <IRremote.h> // >v3.0.0
                                            
#define PIN_SEND 2
#define PIN_RECV 3

void setup()  
{  
  IrSender.begin(PIN_SEND); // Initializes IR sender

  Serial.begin(9600); //initialize serial connection to print on the Serial Monitor of the Arduino IDE
  IrReceiver.begin(PIN_RECV); // Initializes the IR receiver object
}  
                               
void loop()  
{  
  IrSender.sendNEC(0x025, 0x025, 0); // the address 0x0102 with the command 0x34 is sent 
  delay(1000); // wait for one second

  if (IrReceiver.decode()) { 
    Serial.println("Received something...");    
    IrReceiver.printIRResultShort(&Serial); // Prints a summary of the received data
    Serial.println();
    IrReceiver.resume(); // Important, enables to receive the next IR signal
  }  
}  