/*
   Authors: Lisler Thomson
            Nivedita Rajendran
            Shadab Saiyed
   Description: This Code implements the Receiver end at the Elevator. It takes the Vibration/acccelerometer reading from LoRa and Transmits to Raspberry Pi over Serial USB
   Revision : C
   Date: 23.07.2019
*/

#include <SPI.h>

//Radio Head Library: 
#include <RH_RF95.h>

RH_RF95 rf95(12, 6);

int LED = 13; //Status LED on pin 13

int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received
float frequency = 921.2;
int data = 0;

void setup()
{
  pinMode(LED, OUTPUT);

  // INitialise SerialUSB
  SerialUSB.begin(9600);
  while(!SerialUSB);

  //Initialize the Radio. 
  if (rf95.init() == false){
    while (1);
  }
  else{
  // An LED indicator to let us know radio initialization has completed.
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  rf95.setFrequency(frequency); 

}

void loop()
{
  if (rf95.available()){
    // Should be a message for us now
    int8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf)-1;

    if (rf95.recv((uint8_t*)buf, &len)){
      digitalWrite(LED, HIGH); //Turn on status LED
      timeSinceLastPacket = millis(); //Timestamp this packet
      
      SerialUSB.println(buf[0]*1000/256);
    }
    
  else {
    SerialUSB.println(data);
  }
  }
  
  //Turn off status LED if we haven't received a packet after 1s
  if(millis() - timeSinceLastPacket > 1000){
    digitalWrite(LED, LOW); //Turn off status LED
    timeSinceLastPacket = millis(); //Don't write LED but every 1s
  }
}
