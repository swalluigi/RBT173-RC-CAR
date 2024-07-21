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
--SS/CS: Slave Select/ Chip Select

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
    // CE = 7
    // CSE = 8

// SPI pins. Specific to mega
    //MOSI  = 51
    //SCLK = 52
    //MISO = 50

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
bool radioNumber = 1;  //1 uses address[1] to transmit which is '2NODE"


// this Steering struct is our payload, containing integers for the x cords of the joysticks
struct Steering{
  int x1;
  int x2;
};

//initializes pin numbers for the Right motor connected below the H-bridge
int rBackward = 6;
int rForward = 5;

//initializes pin numbers for the Left motor connected above the H-Bridge
int lBackward = 10;
int lForward = 9;

//integers for the right motor values
int rUP;
int rDOWN;

//integers for left motor values
int lUP;
int lDOWN;

struct Steering Control; // makes an object out of the Steering struct called 'Control'
void setup() {
  // Initializes the Pins for the Right motor (remember, up is 1023, down is 0)
  pinMode(rForward , INPUT);
  pinMode(rBackward, INPUT);

  // Initializes the Pins for the Right motor (remember, up is 0, down is 1023)
  pinMode(lForward, INPUT);
  pinMode(lBackward, INPUT);
  
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

;

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(Control));  // float datatype occupies 4 bytes


  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1


    radio.startListening();  // put radio in RX mode
  

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data

}  // setup

void loop() {

  
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
      radio.read(&Control, bytes);             // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);  // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);  // print the pipe number
      Serial.print(F(": "));
      Serial.print(Control.x1);  // print out value of right analog stick
      Serial.print(" : ");
      Serial.println(Control.x2);// print value of left analog stick

     // map(value, fromLow, fromHigh, toLow, toHigh)
     //These are functions that Map the Low and High values from the Joystick (0-1023), and translates them to the Low and High balues of the Motors(0-255)
     //These integers with the map functions help make it so the speed of the motors are dependant on how much you push the Joystick

      lUP = map(Control.x1, 518, 1023, 0, 255);
      lDOWN =map(Control.x1, 518, 0, 0, 255); 

      rUP = map(Control.x2, 518, 0, 0, 255);
      rDOWN =map(Control.x2, 518, 1023, 0, 255); 


      

      //left motor

      // If the left Joystick's analog values is >= 523, move the left motor forward and set its speed to lUP. else, set it to zero
      if (Control.x1 >= 523){
        analogWrite(lForward, lUP);
      }
      else{
        analogWrite(lForward, 0);
      }
      // If the left Joystick's analog values is <= 517, move the Left motor backwards and set its speed to lDOWN. else, set it to zero
      if (Control.x1 <= 517){
        analogWrite(lBackward, lDOWN);
      }
      else{
        analogWrite(lBackward, 0);
      }


      //Right motor

      // If the Right Joystick's analog values is <= 509,  move the Right motor forward and set its speed to rUP. else, set it to zero
      if (Control.x2 <= 509){
        analogWrite(rForward, rUP);
      }
      else{
        analogWrite(rForward, 0);
      }
       // If the Right Joystick's analog values is >=515,  move the left motor forward and set its speed to rDOWN. else, set it to zero
        if (Control.x2 >= 515){
        analogWrite(rBackward, rDOWN);
      }
      else{
        analogWrite(rBackward, 0);
      }


    }

}  // loop
