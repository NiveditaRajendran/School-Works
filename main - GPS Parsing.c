/*******************************************************************************
  * File Name          : main.c
  * Description        : Takes a GPS string through the serial port, parse it
  * 					 and extract each information from the string and perform the
  * 					 following tasks.
  * 					 1. Use atoi() to convert numeric ascii fields to integers
  * 					 	 (with the exception of altitude) and print out hh mm and ss.s
  * 					 	 separately. Similarly print out dd and mmm and mmmm separately
  * 					 	 for lat and long.
  *						 2. Convert altitude to a float using atof() and the checksum to a
  *						 	byte using strtoul() and add to your printout.
  *						 3. Use a push button to cycle through the different information and
  *						 	display it on your LCD screen
  *
  * Author:              Nivedita Rajendran
  * Date:                2018/08/09
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "debounce.h"
#include "HD44780.h"

/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static const int16_t nextPbPin = 0; 	//setting the pin assigned to each pb
static const int16_t exitPbPin  = 1;	//don't use pin 2 as it's connected to VCP TX
enum pushButton {sentenceID = 1, UTCTime, latitude, latitudeDirection,
	longitude, longitudeDirection, positionFix, satellitesUsed, HDOP,
										// HDOP - Horizontal Dilution Of Precision
	altitude, altitudeUnits, geoIDSeperation, geoIDUnits, DGPSAge,checksum };
#define recBufferSize 128 // size of the receive buffer

/* This structure holds different data present in a GPS string.
 */
struct gps
{
	char* sentenceID;
	char* UTCTime;
	char* latitude;
	char* latitudeDirection;	// measured w.r.t Equator
	char* longitude;
	char* longitudeDirection;	// measured w.r.t Greenwich England
	char* positionFix;
	char* satellitesUsed;
	char* HDOP;					// HDOP - Horizontal Dilution Of Precision
	char* altitude;
	char* altitudeUnits;
	char* geoIDSeperation;
	char* geoIDUnits;
	char* DGPSAge;
	char* checksum;

};
struct gps GPS;

/* This structure holds UTC Time data
 */
struct time
{
	int16_t hour;
	int16_t minute;
	int16_t second;
	int16_t decimalOfSecond;

};
struct time TIME;

/* This structure holds Latitude data
 */
struct latitude
{
	int16_t degree;
	int16_t minute;
	int16_t decimalOfMinute;

};
struct latitude LATITUDE;

/* This structure holds Longitude data
 */
struct longitude
{
	int16_t degree;
	int16_t minute;
	int16_t decimalOfMinute;

};
struct longitude LONGITUDE;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void HD44780_Init();

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

// FUNCTION      : waitForPBRelease
// DESCRIPTION   : Loops until the PB that is currently
//				 : pressed and at a logic low
//				 : is released. Release is debounced
// PARAMETERS    : pin and port
// RETURNS       : nothing
void waitForPBRelease(const int16_t pin, const char port)
{
	while (deBounceReadPin(pin, port, 10) == 0)
	{
		//do nothing wait for key press to be released
	}
}

// FUNCTION      : displayNextExit
// DESCRIPTION   : It displays  1:Next Data
//				   2:Exit Any Time
// PARAMETERS    : Nothing
// RETURNS       : Nothing
void displayNextExit()
{
	char stringBuffer[16] = { 0 };
	HD44780_ClrScr();
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "1:Next Data");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "2:Exit Any Time");
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : startUpLCDSplashScreen()
// DESCRIPTION   : displays Debit Demo for 2s
//                 on line 1 of the display and
//				 : Disappears
// PARAMETERS    : None
// RETURNS       : nothing
void startUpLCDSplashScreen(void)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0);
	snprintf(stringBuffer, 16, "   GPS Receive");
	HD44780_PutStr(stringBuffer);
	HAL_Delay(2000);
	HD44780_ClrScr();

	displayNextExit();
}

//  FUNCTION      : pushButtonInit
//   DESCRIPTION   : Calls deBounceInit to initialize ports that
//                   will have pushbutton on them to be inputs.
//                   Note: Don't use PA2 as it is connected to VCP TX and you'll
//                   lose printf output ability.
//   PARAMETERS    : None
//   RETURNS       : nothing
void pushButtonInit()
{
	deBounceInit(nextPbPin, 'A', 1); 		//1 = pullup resistor enabled
	deBounceInit(exitPbPin, 'A', 1); 		//1 = pullup resistor enabled
}

// FUNCTION      : displaySentenceID
// DESCRIPTION   : It displays the sentence ID of the GPS string
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displaySentenceID(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Sentence ID ");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->sentenceID + 1); // 1 is added to remove the '$' sign
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : display UTC Time
// DESCRIPTION   : It displays the UTC Time of the GPS string
// PARAMETERS    : TIME - structure variable pointer for time
// RETURNS       : Nothing
void displayUTCTime(struct time* TIME)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "UTC Time");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%i:%i:%i.%02i", TIME->hour, TIME->minute,
			TIME->second, TIME->decimalOfSecond);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayLatitude
// DESCRIPTION   : It displays the Latitude of the GPS string
//				   as degree minute decimal_degree format
// PARAMETERS    : LATITUDE - structure variable pointer for latitude
// RETURNS       : Nothing
void displayLatitude(struct latitude* LATITUDE)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Latitude");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%i%c%i.%04i'", LATITUDE->degree, 223,
			LATITUDE->minute, LATITUDE->decimalOfMinute);
						  // 223  is the ASCII value for degree symbol
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayLatitudeDirection
// DESCRIPTION   : It displays the direction of latitude measured
//				   w.r.t Equator
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayLatitudeDirection(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Latitude Dir");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->latitudeDirection);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayLongitude
// DESCRIPTION   : It displays the Longitude of the GPS string
// PARAMETERS    : LONGITUDE - structure variable pointer for longitude
// RETURNS       : Nothing
void displayLongitude(struct longitude* LONGITUDE)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Longitude");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%i%c%i.%04i'", LONGITUDE->degree, 223,
			LONGITUDE->minute, LONGITUDE->decimalOfMinute);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayLongitudeDirection
// DESCRIPTION   : It displays the direction of longitude
//				   measured w.r.t Greenwich England
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayLongitudeDirection(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Longitude Dir");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->longitudeDirection);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayPositionFix
// DESCRIPTION   : It displays the position fix of the GPS string
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayPositionFix(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Position Fix");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->positionFix);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displaySatelliteUsed
// DESCRIPTION   : It displays the number ofSatellite Used
//				   from the GPS string
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displaySatelliteUsed(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to second line first position
	snprintf(stringBuffer, 16, "Satellites Used");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->satellitesUsed);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayHDOP
// DESCRIPTION   : It displays the HDOP of the GPS string
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayHDOP(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to second line first position
	snprintf(stringBuffer, 16, "HDOP");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->HDOP);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayAltitude
// DESCRIPTION   : It displays the Altitude of the GPS string
// PARAMETERS    : altitudeInDouble - Value of altitude
// RETURNS       : Nothing
void displayAltitude(double altitudeInDouble)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Altitude");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%lf", altitudeInDouble);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayAltitudeUnits
// DESCRIPTION   : It displays the Unit of Altitude
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayAltitudeUnits(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Altitude Unit");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->altitudeUnits);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayGeoIDSeperation
// DESCRIPTION   : It displays the Geo ID from the GPS string
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayGeoIDSeperation(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "GEO ID");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->geoIDSeperation);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayGeoIDUnits
// DESCRIPTION   : It displays the unit of geo ID
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayGeoIDUnits(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "GEO ID Unit");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->geoIDUnits);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayDGPS
// DESCRIPTION   : It displays the DGPS Age of the gps
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : Nothing
void displayDGPSAge(struct gps* GPS)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "DGPS Age");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%s", GPS->DGPSAge);
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : displayChecksum
// DESCRIPTION   : It displays the Checksum of the GPS string
// PARAMETERS    : checksumInDecimal - Decimal converted Checksum value
// RETURNS       : Nothing
void displayChecksum(unsigned long int checksumInDecimal)
{
	char stringBuffer[16] = { 0 };
	HD44780_GotoXY(0, 0); // move to First line first position
	snprintf(stringBuffer, 16, "Checksum ");
	HD44780_PutStr(stringBuffer);
	HD44780_GotoXY(0, 1); // move to second line first position
	snprintf(stringBuffer, 16, "%x", checksumInDecimal);
						  // 1 is added to remove the '*' character
	HD44780_PutStr(stringBuffer);
}

// FUNCTION      : parsingGPS
// DESCRIPTION   : Takes the string received from serial port and
// 				   uses strtok() function to parse through the string
// PARAMETERS    : GPS - structure variable pointer for gps
//				   recBuffer[recBuffersize] - the string received from
//				   	serial port
//				   delimComma - Contains the delimiter for strtok() as Comma
// RETURNS       : Noting
void parsingGPS(struct gps* GPS, char recBuffer[recBufferSize],
		const char* delimComma)
{
	GPS->sentenceID = strtok(recBuffer, delimComma);
	GPS->UTCTime = strtok(NULL, delimComma);
	GPS->latitude = strtok(NULL, delimComma);
	GPS->latitudeDirection = strtok(NULL, delimComma);
	GPS->longitude = strtok(NULL, delimComma);
	GPS->longitudeDirection = strtok(NULL, delimComma);
	GPS->positionFix = strtok(NULL, delimComma);
	GPS->satellitesUsed = strtok(NULL, delimComma);
	GPS->HDOP = strtok(NULL, delimComma);
	GPS->altitude = strtok(NULL, delimComma);
	GPS->altitudeUnits = strtok(NULL, delimComma);
	GPS->geoIDSeperation = strtok(NULL, delimComma);
	GPS->geoIDUnits = strtok(NULL, delimComma);
	GPS->DGPSAge = strtok(NULL, delimComma);
	GPS->checksum = strtok(NULL, delimComma);

	printf("Sentence ID %s\r\n", GPS->sentenceID);
	printf("UTC Time %s\r\n", GPS->UTCTime);
	printf("latitude %s\r\n", GPS->latitude);
	printf("Latitude measured WRT %s\r\n", GPS->latitudeDirection);
	printf("longitude %s\r\n", GPS->longitude);
	printf("longitude WRT Greenwich England %s \r\n", GPS->longitudeDirection);
	printf("Position Fix %s \r\n", GPS->positionFix);
	printf("Satellites used %s\r\n", GPS->satellitesUsed);
	printf("HDOP %s\r\n", GPS->HDOP);
	printf("Altitude %s \r\n", GPS->altitude);
	printf("Altitude unit %s \r\n", GPS->altitudeUnits);
	printf("GeoID Seperation %s \r\n", GPS->geoIDSeperation);
	printf("GeoID Unit %s \r\n", GPS->geoIDUnits);
	printf("DGPS age %s \r\n", GPS->DGPSAge);
	printf("Checksum %s\r\n", GPS->checksum);
}

// FUNCTION      : convertLatitudeStrToInt
// DESCRIPTION   : converts the latitude from string to integer
//				   and splits it into degree minute and decimal degree
// PARAMETERS    : GPS - structure variable pointer for gps
//				   delimDot - Contains the delimiter for strtok() as Dot
//				   LATITUDE - structure variable pointer for latitude
// RETURNS       : Nothing
void convertLatitudeStrToInt(struct gps* GPS, const char* delimDot,
		struct latitude* LATITUDE)
{
	int32_t degreeAndMinute = 0;
	char* degreeAndMinuteLatitude = NULL;
	char* decimalDegreeLatitude = NULL;
	char copyLatitude[sizeof(GPS->latitude)] = "0";

	strcpy(copyLatitude, GPS->latitude);

	degreeAndMinuteLatitude = strtok(copyLatitude, delimDot);
	decimalDegreeLatitude = strtok(NULL, delimDot);

	degreeAndMinute = atoi(degreeAndMinuteLatitude);
	LATITUDE->decimalOfMinute = atoi(decimalDegreeLatitude);
	LATITUDE->degree = degreeAndMinute / 100;
	LATITUDE->minute = degreeAndMinute % 100;
	printf("latitude %2i degree %2i.%04i minute %s\r\n", LATITUDE->degree,
			LATITUDE->minute, LATITUDE->decimalOfMinute,
			GPS->latitudeDirection);
}

// FUNCTION      : convertLongitudeStrToInt
// DESCRIPTION   : converts the longitude from string to integer
//				   and splits it into degree minute and decimal degree
// PARAMETERS    : GPS - structure variable pointer for gps
//				   delimDot - Contains the delimiter for strtok() as Dot
//				   LONGITUDE - structure variable pointer for longitude
// RETURNS       : Nothing
void convertLongitudeStrToInt(struct gps* GPS, const char* delimDot,
		struct longitude* LONGITUDE)
{
	int32_t degreeAndMinute = 0;
	char* degreeAndMinuteLongitude = NULL;
	char* decimalDegreeLongitude = NULL;
	char copyLongitude[sizeof(GPS->longitude)] = "0";

	strcpy(copyLongitude, GPS->longitude);
	degreeAndMinuteLongitude = strtok(copyLongitude, delimDot);
	decimalDegreeLongitude = strtok(NULL, delimDot);
	degreeAndMinute = atoi(degreeAndMinuteLongitude);
	LONGITUDE->decimalOfMinute = atoi(decimalDegreeLongitude);
	LONGITUDE->degree = degreeAndMinute / 100;
	LONGITUDE->minute = degreeAndMinute % 100;
	printf("longitude %3i degree %2i.%04i minute %s\r\n", LONGITUDE->degree,
			LONGITUDE->minute, LONGITUDE->decimalOfMinute,
			GPS->longitudeDirection);
}

// FUNCTION      : convertAltitudeStrToDouble
// DESCRIPTION   : converts the altitude in string to Double
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       : altitudeInDouble - returns the converted altitude in Double
double convertAltitudeStrToDouble(struct gps* GPS)
{
	double altitudeInDouble = 0;
	/* upon running this code I noticed that atof() is not working properly.
	 * it was spitting out some random float value. When searching in google
	 * I found that I have to include stdlib.h for atof() to work properly
	 * ref:https://stackoverflow.com/questions/7708309/atof-not-working
	 */
	altitudeInDouble = atof(GPS->altitude);
	printf("Altitude %.3lf %s\r\n", altitudeInDouble, GPS->altitudeUnits);
	return altitudeInDouble;
}

// FUNCTION      : convertTimeStrToInt
// DESCRIPTION   : converts the UTC time from string to integer
//				   and splits it into degree minute and decimal degree
// PARAMETERS    : GPS - structure variable pointer for gps
//				   delimDot - Contains the delimiter for strtok() as Dot
//				   TIME - structure variable pointer for time
// RETURNS       : Nothing
void convertTimeStrToInt(struct gps* GPS, const char* delimDot,
		struct time* TIME)
{
	int32_t hourMinSecInInt = 0;
	int32_t temp = 0;
	char* hourMinSec = 0;
	char* secDecimal = 0;
	char copyUTCTime[sizeof(GPS->UTCTime)] = "0";

	strcpy(copyUTCTime, GPS->UTCTime);
	hourMinSec = strtok(copyUTCTime, delimDot);
	secDecimal = strtok(NULL, delimDot);
	hourMinSecInInt = atoi(hourMinSec);
	TIME->decimalOfSecond = atoi(secDecimal);
	TIME->second = hourMinSecInInt % 100;
	temp = hourMinSecInInt / 100;
	TIME->minute = temp % 100;
	TIME->hour = temp / 100;
	printf("Time %i Hr %i min %i.%02i sec\r\n", TIME->hour, TIME->minute,
			TIME->second, TIME->decimalOfSecond);
}

// FUNCTION      : convertChecksumStrToDecimal
// DESCRIPTION   : It converts the checksum in String to Decimal
//				   since checksum is in hex, we use strtoul()
// PARAMETERS    : GPS - structure variable pointer for gps
// RETURNS       :checksumInDecimal - decimal converted checksum
int convertChecksumStrToDecimal(struct gps* GPS)
{
	unsigned long int checksumInDecimal = 0;

	checksumInDecimal = strtoul(GPS->checksum + 1, NULL, 16);
	// 1 is added to remove the '*' character
	printf("Checksum %x in decimal %i \r\n", checksumInDecimal,
			checksumInDecimal);
	return checksumInDecimal;
}

/* USER CODE END 0 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_TIM1_Init();

	/* USER CODE BEGIN 2 */
	printf("Ready to Receive GPS string\r\n");
	HD44780_Init();
	/* setup Port A bits 0,1,2 and 3, i.e.: PA0-PA3 for input */
	pushButtonInit();
	startUpLCDSplashScreen();

	int8_t infoNumber = 1;

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		/* recChar will hold a single char received on the serial port*/
		char recChar = 0;
		/* recBuffer will have characters that are received on the serial
		 * port copied into it*/
		static char recBuffer[recBufferSize] = {0};	//static so that the previous
													//values of recBuffer aren't reset each
													//time we get here

		/* recBufferIndex is used to keep track of where we are in recBuffer.  */
		static int8_t recBufferIndex = 0;	//and again static so we don't reset our
											//value each time we get here
		static int8_t sentenceReceived = 0;	//flag that indicates a complete sentence has
											//been received. static for the same reasons
											//above
		const char *delimComma = ",";       // since each data from GPS is seperated by
											//comma operator
		const char* delimDot = ".";			// since the latitude decimal place is denoted by a Dot
											// we can split it into two and use atoi() to convert
		double altitudeInDouble = 0;
		unsigned long int checksumInDecimal = 0;

		/* result will be checked to be ok or a timeout telling us whether
		* the char should be copied over to recBuffer, or if comm should be reset */
		HAL_StatusTypeDef result = 0;

		/*check for a char on the serial port with a timeout value */
		result = HAL_UART_Receive(&huart2, &recChar,1,10000);   //will wait up to 10s for
																// a char and then timeout
		if(result == HAL_OK)							//then we have rec'd a char
		{
			if(recChar == '$')							//STX! so let's start storing char in the recBuffer
			{
				recBufferIndex = 0;						//$ is the first char in a sentence.
				recBuffer[recBufferIndex] = recChar;	//so we've got to store it at the
														//start of our recBuffer
				recBufferIndex++;						//and inc the index so the next char
														//is stored in the next position in
														//the buffer
			}
			else if(recBufferIndex > 0)					//then we're receiving the rest of the sentence
			{
				recBuffer[recBufferIndex] = recChar;	//so place the rec'd char in the buffer
				if(recBufferIndex <recBufferSize)		//and increment the index as long as
				{										//we don't exceed the size of the buffer
					recBufferIndex++;
				}
				else
				{
					recBufferIndex = 0;					//if we're we've exceeded the size of our
					printf("recBuffer Overflow\r\n");	//buffer so let's say so
				}
			}
			if(recChar == '\r')							//if we get a cr then we know we're at the
			{											//end of our sentence
				recBuffer[recBufferIndex] = '\0';		//put a null instead of cr in the buffer
														//'cause that's what strings need
				recBufferIndex = 0;						//and reset the buffer index
				sentenceReceived = 1;					//and set the flag 'cause we've got a
														//complete sentence now.
			}
		}
		else if(result == HAL_TIMEOUT)
		{
			printf("serial port timeout\r\n");			//let's say so
			recBufferIndex = 0;							//and start over
		}

		if(sentenceReceived)
		{
			/* your code goes here to parse the string inside recBuffer */
			/*$GPGGA,014729.10,4303.5753,N,08019.0810,W,1,6,1.761,214.682,M,0,M,0,*5D
			 * this is the example GPS string used for testing
			 */
			printf("recBuffer: %s\r\n", recBuffer);

			parsingGPS(&GPS, recBuffer, delimComma);

			convertLatitudeStrToInt(&GPS, delimDot,	&LATITUDE);

			convertLongitudeStrToInt(&GPS, delimDot, &LONGITUDE);

			altitudeInDouble = convertAltitudeStrToDouble(&GPS);

			convertTimeStrToInt(&GPS, delimDot, &TIME);

			checksumInDecimal = convertChecksumStrToDecimal(&GPS);

			while(infoNumber<16)
			{
				if(deBounceReadPin(nextPbPin, 'A', 50) == 0)
				{
					++infoNumber;
				}

				if(deBounceReadPin(exitPbPin, 'A', 50) == 0)
				{
					infoNumber = 1;
					displayNextExit();
					break;
				}

				HD44780_ClrScr();

				switch(infoNumber)
				{
				case sentenceID:
					displaySentenceID(&GPS);
					break;

				case UTCTime:
					displayUTCTime(&TIME);
					break;

				case latitude:
					displayLatitude(&LATITUDE);
					break;

				case latitudeDirection:
					displayLatitudeDirection(&GPS);
					break;

				case longitude:
					displayLongitude(&LONGITUDE);
					break;

				case longitudeDirection:
					displayLongitudeDirection(&GPS);
					break;

				case positionFix:
					displayPositionFix(&GPS);
					break;

				case satellitesUsed:
					displaySatelliteUsed(&GPS);
					break;

				case HDOP:
					displayHDOP(&GPS);
					break;

				case altitude:
					displayAltitude(altitudeInDouble);
					break;

				case altitudeUnits:
					displayAltitudeUnits(&GPS);
					break;

				case geoIDSeperation:
					displayGeoIDSeperation(&GPS);
					break;

				case geoIDUnits:
					displayGeoIDUnits(&GPS);
					break;

				case DGPSAge:
					displayDGPSAge(&GPS);
					break;

				case checksum:
					displayChecksum(checksumInDecimal);
					break;

				default:
					infoNumber = 1;
					break;
				}
			}
			sentenceReceived = 0;				//as we've finished processing the sentence
		}

	} //closing brace for while(1)
	/* USER CODE END 3 */
}//closing brace for main

/** System Clock Configuration **/
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 16;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV16;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2
			| RCC_PERIPHCLK_ADC;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
	PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
	PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
	PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
	PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
	PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	/**Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 9090;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 4045;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	/* adding this as ST Tech Support said PWM should be stopped before
	 * calling HAL_TIM_PWM_ConfigChannel and I've been getting flakey start-up
	 * i.e.: sometime PWM starts up, other times the line remains stuck high.
	 **************************************/
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	/*************************************/
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_TIM_MspPostInit(&htim1);

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

	/*Configure GPIO pin : PB3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while (1)
	{
	}
	/* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
