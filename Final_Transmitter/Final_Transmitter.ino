/*
Two types of communication
--SERIAL: uses a data line and a clock line
---

--Parallel: Uses a pin to transmit each individual bit according to a clock cycle
--- can communicate 8 bits simultaneously, but needs 8 pins

SERIAL PERPHERAL INTERFACE (SPI)

WHAT IS SPI AND HOW DOES IT WORK?
-SPI is a common protocol used to communicate with lots of different desvices
--primary used in SD card readers, RFID card readers, and 2.4 GHz wireless trancievers
-SPI is unique as it can continously stream data to a device and is not limited by data lengths of bits or bytes '0/1' or '100101"
= SPI ustilizes a Master-Slave configuration with one controlling device and multiple other responding devices

-SPI at minumum needs 4 pins
--MOSI: Master out Slave IN = SEND
--MISO: Master in Slave Out = RECIVE
--SCLK: Clock
-int-SS/CS: Slave Select/ Chip Select

1. clock signal and select pin is pulled low
2. data starts with most significant bit
3. slave responds
*/

//Pins for nRF24l01

//Power pins
 // Red = power
 // black/ grey = ground
 
 // These Pins are connect to any digital pin.
    //they are used for setting the module in standby or active mode, as well as for switching between transmit or command mode
    //  CE = 7
    //  CSE = 8

// SPI pins. Specific to mega
    //  MOSI  = 51
    // SCLK = 52
    //  MISO = 50

/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */

/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Monitor to change each node's behavior.
 */
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

// Let these addresses be used for the pair
uint8_t address[][6] = { "1Node", "2Node" };
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit
bool radioNumber = 0;  // 0 uses address[0] to transmit, '1Node' in this case


// this Steering struct is our payload, containing integers for the x cords of the joysticks
struct Steering{
  int x1;
  int x2;
};

struct Steering Control; // makes an object out of the Steering struct called 'Control'

void setup() {

   pinMode(A0, INPUT); //x on left joystick
   pinMode(A2, INPUT);// x on right joystick.

  
  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  Serial.println((int)radioNumber);


  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit the Joystick Struct object
  radio.setPayloadSize(sizeof(Control));  

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0



  // additional setup specific to the node's role

    radio.stopListening();  // put radio in TX mode


  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data

}  // setup



void loop() {
    //set the x values in the Joystick Struct object to read the analog values of the IRL joysticks connected to the pins
    Control.x1 = analogRead(A0); //Left stick
    Control.x2 = analogRead(A2); // Right stick

    unsigned long start_timer = micros();                // start the timer
    bool report = radio.write(&Control, sizeof(Control));  // transmit & save the report
    unsigned long end_timer = micros();                  // end the timer

    if (report) {
      Serial.print(F("Transmission successful! "));  // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);  // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.print(Control.x1);  // print payload sent
      Serial.print(" : ");
      Serial.println(Control.x2);
    
    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }

  



   
 
}  // loop
