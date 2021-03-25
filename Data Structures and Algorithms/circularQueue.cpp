/*******************************************************************************
* File Name          : circularQueue.cpp
* Description        : Circular Queue assignment skeleton starting point
*
* Author             : Allan Smith / Nivedita Rajendran        
* Date               : Jun 27, 2017
* Updated            : Oct 31, 2018
* Submitted          : Nov 25, 2018
******************************************************************************
*/

#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>

DWORD WINAPI myReadThreadFunction(LPVOID lpParam);                                   // function used by read from queue thread
unsigned int putToCircularQueue(char *ptrInputBuffer, unsigned int bufferLength);    // circular queue function to add data to queue
unsigned int getFromCircularQueue(char *ptrOutputBuffer, unsigned int bufferLength); // circular queue function to remove data from queue

#define BUFFER_SIZE         200                // local buffer used for sending or receiving data to/from queue in main and worker thread

#define INPUT_TIMEOUT_MS    5000               // dequeue every 5 seconds

#define CIRCULAR_QUEUE_SIZE 10                // size of the circular queue

// data structure used to keep track of circular queue (DO NOT CHANGE THIS STRUCTURE)
struct myQueueStruct {
	char *ptrBuffer = NULL;                    // pointer to start of the circular queue buffer
	volatile unsigned int queueCount = 0;      // number of characters currently in cirucular queue
	volatile char *ptrCircularHead = NULL;     // location where data is added to queue
	volatile char *ptrCircularTail = NULL;     // loation where data is removed from queue
};

struct myQueueStruct myQueue;                  // create an instance of the circular queue data structure
static unsigned int cs = 0;                    // Flag for protecting the Critical section

// DO NOT CHANGE THE CODE IN MAIN
int main()
{
	HANDLE  hThread;            // handle to thread
	char    inputBuffer[BUFFER_SIZE];

	myQueue.ptrBuffer = (char *)calloc(CIRCULAR_QUEUE_SIZE, sizeof( char ));
	if (myQueue.ptrBuffer == NULL)
	{
		printf("Error unable to allocate memory for buffer\n");
		exit(-1);
	}

	myQueue.ptrCircularHead = myQueue.ptrCircularTail = myQueue.ptrBuffer;

	// create a thread that will consume the data we type in to demonstrate dequeing the data
	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		myReadThreadFunction,   // thread function name
		NULL,                   // argument to thread function (not being used)
		0,                      // use default creation flags 
		NULL);                  // returns the thread identifier (not being used)

	// get a string from the console and queue it to circular queue
	while (1)
	{
		scanf_s("%s", inputBuffer, BUFFER_SIZE);    // get data from console

		// put the data into the circular queue but check if an error (marked by queue function returning 0) occurred
		if (putToCircularQueue(inputBuffer, (unsigned int)strlen(inputBuffer)) == 0)
			printf("Error queuing data\n");
	}
    return 0;
}

// FUNCTION      : putToCircularQueue
// DESCRIPTION   :
//   Put the supplied data into a circular queue
// PARAMETERS    :
//   ptrInputBuffer - a pointer to the buffer to be put into the queue
//   bufferLength   - how many characters are in the buffer being put into the queue
//
// RETURNS       :
//   Number of characters successfully queued (0 on failure)

unsigned int putToCircularQueue( char *ptrInputBuffer, unsigned int bufferLength )
{
	unsigned int numberOfCharsQueued = 0;  // number of characters placed in circular queue

	// add code to implement the adding to the circular queue using the data structures in myQueueStruct
	// and the passed in pointer and data to be added count
	unsigned int j = 0;
	while (j < bufferLength)
	{
		/* checks if the queue is full */
		if (myQueue.ptrCircularTail == (myQueue.ptrCircularHead - 1) || 
			(myQueue.ptrBuffer == myQueue.ptrCircularHead && 
			(myQueue.ptrBuffer + CIRCULAR_QUEUE_SIZE) == myQueue.ptrCircularTail))
		{
			printf("Queue is FULL \n");
			exit(-1);
		}
		/* Checks if tail reached the end of buffer and wraps it back to the front */
		if (myQueue.ptrCircularTail > (myQueue.ptrBuffer + CIRCULAR_QUEUE_SIZE))  
		{                                                                        
			myQueue.ptrCircularTail = myQueue.ptrBuffer;
		}
		/* Protectiong the Critical section */
		if (cs == 1)
			while (cs);
		
			cs = 1;

			*myQueue.ptrCircularTail = *ptrInputBuffer; // Copies value to the end of queue
			myQueue.ptrCircularTail++;
			*ptrInputBuffer++;
			numberOfCharsQueued++;

			cs = 0;
		/* Critical section ends */
		j++;

	}

	return numberOfCharsQueued;
}

// FUNCTION      : getFromCircularQueue
// DESCRIPTION   :
//   Read as much data as there is room for from the circular queue
// PARAMETERS    :
//   ptrOutputBuffer - a pointer to the buffer to place the data read from queue
//   bufferLength    - maxiumum number of characters that can be read (IE the size of the output buffer)
//
// RETURNS       :
//   Number of characters successfully queued (0 if none are read)

unsigned int getFromCircularQueue(char *ptrOutputBuffer, unsigned int bufferLength)
{
	unsigned int readCount = 0;
	// add code to implement the removing from the circular queue using the data structures in myQueueStruct
	// and place the data into the ptrOutputBuffer up to a maximum number of bytes specified by bufferLength
	// return the count of the bytes that was dequeued
	while (myQueue.ptrCircularHead != myQueue.ptrCircularTail)
	{
		/* Checks if the queue is empty */
		if (myQueue.ptrCircularHead == myQueue.ptrCircularTail)
		{
			printf("Queue is EMPTY\n");
			return 0;
		}
		/* Checks if the head has eached the end of queue adn wraps it around */
		if (myQueue.ptrCircularHead > (myQueue.ptrBuffer + CIRCULAR_QUEUE_SIZE))
		{
			myQueue.ptrCircularHead = myQueue.ptrBuffer;
		}
		/* Protecting the critical section */
		if (cs == 1)
			while (cs);
		
			cs = 1;

			*ptrOutputBuffer = *myQueue.ptrCircularHead; // Copies the first element of the queue to the output buffer
			myQueue.ptrCircularHead++;
			*ptrOutputBuffer++;
			readCount++;

			cs = 0;
		/* critical section ends */
	}
	return readCount;
}

// FUNCTION      : myReadThreadFunction
// DESCRIPTION   :
//   A seperate thread from the main program that will independently check for data in queue and print it out
//   DO NOT CHANGE THIS CODE!
// PARAMETERS    :
//   lpParam - arguement passed into thread (not used in this example)
//
// RETURNS       :
//   Will never return so no effective return value

DWORD WINAPI myReadThreadFunction(LPVOID lpParam)
{
	char readBuffer[BUFFER_SIZE] = " ";     // local buffer to put the data into when reading from queue and print
	unsigned int  readCount = 0;      // the number of characters read from queue

	while (1)
	{
		Sleep(INPUT_TIMEOUT_MS);      // wait for some data to be received
		char readBuffer[BUFFER_SIZE] = " ";     // local buffer to put the data into when reading from queue and print
		// check if data is available and if so print it out
		readCount = getFromCircularQueue(readBuffer, BUFFER_SIZE);
		if (readCount != 0)           // check for reads that did not get any data
			printf("UNQUEUE: %s\n", readBuffer);
	}
	return 0;    // will never reach here
}