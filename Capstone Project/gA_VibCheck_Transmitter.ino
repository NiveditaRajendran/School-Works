
#include <SPI.h>

//Radio Head Library:
#include <RH_RF95.h>

// SparkFun ADXL345 Accelerometer Library
#include <SparkFun_ADXL345.h>

ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

RH_RF95 rf95(4, 3); // RFM9x Chip Select and Interrupt Pins

int LED = 13; //Status LED is on pin 13

int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received

float frequency = 921.2; //Broadcast frequency

void setup()
{
  pinMode(LED, OUTPUT);

  Serial.begin(9600);
 // Serial.println("RFM Client!");

  //Initialize the Radio.
  if (rf95.init() == false) {
    //Serial.println("Radio Init Failed - Freezing");
    while (1);
  }
  else {
    //An LED inidicator to let us know radio initialization has completed.
 //   Serial.println("Transmitter up!");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  // Set frequency
  rf95.setFrequency(frequency);

  rf95.setTxPower(14, false); // Sets the transmission frequency

  // Power on the ADXL345
  adxl.powerOn();

  adxl.setRangeSetting(2);           // Give the range settings 2g,4g,8g,16g

  adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1

  adxl.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)

  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)

  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment

  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment


  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(1);
}


void loop()
{

  int x, y, z;

  // Read the accelerometer values and store them in variables declared above x,y,z
  adxl.readAccel(&x, &y, &z);         
 //delay(1);
  // Stores all axis value to a single array to transmit
  uint8_t toSend[] = {z};
  // Sends Data through LoRa
  rf95.send(toSend, sizeof(toSend));
  rf95.waitPacketSent(); // Waits till the transmission is completed

}
