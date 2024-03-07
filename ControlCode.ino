/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty 2bndy5
 */

/**
 * A simple example of streaming data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Monitor to change each node's behavior.
 */
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//INPUTS
//LEFT JOYSTICK
#define leftXaxis A0
#define leftYaxis A1
#define leftZaxis A2
#define leftJoySwitch 0

//RIGHT JOYSTICK
#define rightXaxis A3
#define rightYaxis A4
#define rightZaxis A5
#define rightJoySwitch 1
        // 42,  //IDENTIFICADOR DE LA TRAMA (ELRECEPTOR SABRÁ QUE DATOS SÓN AL COMPORBAR ÉSTE IDENTIFICADOR

//POTIS
#define pot1 A6
#define pot2 A7

//LEVERS
#define leverSwitch1 6
#define leverSwitch2 7
#define leverSwitch3 10

//REAR BUTTONS
#define buttonL1 2
#define buttonL2 3
#define buttonR1 4
#define buttonR2 5

#define CE_PIN 9
#define CSN_PIN 8
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

// Let these addresses be used for the pair
uint8_t address[][6] = { "1Node", "2Node" };
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit
bool radioNumber;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Used to control whether this node is sending or receiving
bool role = true;  // true = TX node, false = RX node

// For this example, we'll be sending 32 payloads each containing
// 32 bytes of data that looks like ASCII art when printed to the serial
// monitor. The TX node and RX node needs only a single 32 byte buffer.
#define SIZE 32             // this is the maximum for this example. (minimum is 1)
char buffer[SIZE + 1];      // for the RX node
uint8_t counter = 0;        // for counting the number of received payloads
void makePayload(uint8_t);  // prototype to construct a payload dynamically


void setup() {

  buffer[SIZE] = 0;  // add a NULL terminating character (for easy printing)

  Serial.begin(9600);
  //while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  //}
digitalWrite(CE_PIN, HIGH);
digitalWrite(CSN_PIN, LOW);
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  // print example's introductory prompt
  //Serial.println(F("RF24/examples/StreamingData"));

  // To set the radioNumber via the Serial monitor on startup
  // Serial.println(F("Which radio is this? Enter '0' or '1'. Defaults to '0'"));
  // while (!Serial.available()) {
  //   // wait for user input
  // }
  // char input = Serial.parseInt();
  radioNumber = 0;  //input == 1;
  Serial.print(F("radioNumber = "));
  Serial.println((int)radioNumber);

  // role variable is hardcoded to RX behavior, inform the user of this
  //Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit
  radio.setPayloadSize(SIZE);  // default value is the maximum 32 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1

  // additional setup specific to the node's role
//  if (role) {
    radio.stopListening();  // put radio in TX mode
  // } else {
  //   radio.startListening();  // put radio in RX mode
  // }

  // For debugging info
  // printf_begin();  // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails();  // (larger) function that prints human readable data

}  // setup()


void loop() {

  //if (role) {
    // This device is a TX node

    radio.flush_tx();
    uint8_t i = 0;
    uint8_t failures = 0;
    unsigned long start_timer = micros();  // start the timer
    while (i < SIZE) {
      //makePayload(i);  // make the payload
      //HACEMOS LAS LECTURAS DE LOS DIFERENTES PINES

      //LEFT JOYSTICK
      int leftXaxisValue = analogRead(leftXaxis);
      int leftYaxisValue = analogRead(leftYaxis);
      int leftZaxisValue = analogRead(leftZaxis);
      int leftJoySwitchValue = digitalRead(leftJoySwitch);

      //RIGHT JOYSTICK
      int rightXaxisValue = analogRead(rightXaxis);
      int rightYaxisValue = analogRead(rightYaxis);
      int rightZaxisValue = analogRead(rightZaxis);
      int rightJoySwitchValue = digitalRead(rightJoySwitch);

      //POTIS
      int pot1Value = digitalRead(pot1);
      int pot2Value = digitalRead(pot2);

      //LEVERS
      bool leverSwitch1Value = digitalRead(leverSwitch1);
      bool leverSwitch2Value = digitalRead(leverSwitch2);
      bool leverSwitch3Value = digitalRead(leverSwitch3);

      //REAR BUTTONS
      bool buttonL1Value = digitalRead(buttonL1);
      bool buttonL2Value = digitalRead(buttonL2);
      bool buttonR1Value = digitalRead(buttonR1);
      bool buttonR2Value = digitalRead(buttonR2);
      int joysticksValues[] = {
        // 42,  //IDENTIFICADOR DE LA TRAMA (ELRECEPTOR SABRÁ QUE DATOS SÓN AL COMPORBAR ÉSTE IDENTIFICADOR
        leftXaxisValue,
        leftYaxisValue,
        leftZaxisValue,
        leftJoySwitchValue,
        rightXaxisValue,
        rightYaxisValue,
        rightZaxisValue,
        rightJoySwitchValue,
        buttonL1Value,
        buttonL2Value,
        buttonR1Value,
        buttonR2Value
      };
      //int datos[] = { 10, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
      if (!radio.writeFast(&joysticksValues, sizeof(joysticksValues))) {
        failures++;
        radio.reUseTX();
      } else {
        i++;
      }

      if (failures >= 100) {
        // Serial.print(F("Too many failures detected. Aborting at payload "));
        // Serial.println(buffer[0]);
        break;
      }
    }
    unsigned long end_timer = micros();  // end the timer

    Serial.print(F("Time to transmit = "));
    Serial.print(end_timer - start_timer);  // print the timer result
    Serial.print(F(" us with "));
    Serial.print(failures);  // print failures detected
    Serial.println(F(" failures detected"));

   // to make this example readable in the serial monitor
    delay(5);  // slow transmissions down by 1 second
 // }

}  // loop


// void makePayload(uint8_t i) {
//   // Make a single payload based on position in stream.
//   // This example employs function to save memory on certain boards.

//   // let the first character be an identifying alphanumeric prefix
//   // this lets us see which payload didn't get received
//   buffer[0] = i + (i < 26 ? 65 : 71);
//   for (uint8_t j = 0; j < SIZE - 1; ++j) {
//     char chr = j >= (SIZE - 1) / 2 + abs((SIZE - 1) / 2 - i);
//     chr |= j < (SIZE - 1) / 2 - abs((SIZE - 1) / 2 - i);
//     buffer[j + 1] = chr + 48;
//   }
// }
