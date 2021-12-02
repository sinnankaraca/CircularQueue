/*******************************************************************************
* File Name          : circularQueueV2021.cpp
* Description        : Simple example of an implemenation of a circular Queue
*					   Memory allocation is for 10 digits.
*					   Any strings under 10 digits can be entered.
*                     
*
* Author:              PROG8130 / Sinan Karaca 8743013
* Date:                Nov 02 12 2021
******************************************************************************
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

DWORD WINAPI myReadThreadFunction(LPVOID lpParam);                                   // function used by read from queue thread
unsigned int putToCircularQueue(char* ptrInputBuffer, unsigned int bufferLength);    // circular queue function to add data to queue
unsigned int getFromCircularQueue(char* ptrOutputBuffer, unsigned int bufferLength); // circular queue function to remove data from queue

#define BUFFER_SIZE         200                // local buffer used for sending or receiving data to/from queue in main and worker thread

#define INPUT_TIMEOUT_MS    5000               // dequeue every 5 seconds

#define CIRCULAR_QUEUE_SIZE 10                 // size of the circular queue

// data structure used to keep track of circular queue
struct myQueueStruct {
	char* ptrBuffer = NULL;                    // pointer to start of the circular queue buffer
	volatile unsigned int queueCount = 0;      // number of characters currently in cirucular queue
	volatile char* ptrCircularHead = NULL;     // location where data is added to queue
	volatile char* ptrCircularTail = NULL;     // loation where data is removed from queue
};

struct myQueueStruct myQueue;                  // create an instance of the circular queue data structure

int main()
{
	HANDLE  hThread;            // handle to thread
	char    inputBuffer[BUFFER_SIZE];

	myQueue.ptrBuffer = (char*)calloc(CIRCULAR_QUEUE_SIZE, sizeof(char));
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

	printf("length of circular queue is %d\n", CIRCULAR_QUEUE_SIZE);
	// get a string from the console and queue it to circular queue
	while (1)
	{
		scanf_s("%199s", inputBuffer, BUFFER_SIZE);     // get data from console (note this is the input from console not the circular queue yet)
		inputBuffer[BUFFER_SIZE - 1] = '\0';			// ensure the read string has a nul char on end of string

		// put the data into the circular queue but check if an error (marked by queue function returning 0) occurred
		if (putToCircularQueue(inputBuffer, strlen(inputBuffer)) == 0)
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

unsigned int putToCircularQueue(char* ptrInputBuffer, unsigned int bufferLength)
{
	unsigned int numberOfCharsQueued = 0;  // number of characters placed in circular queue

	// add code to implement the adding to the circular queue using the data structures in myQueueStruct
	// and the passed in pointer and data to be added count

	// check if buffer length is above 10, if it is not give an error to the console.
	if (bufferLength < 10) {

		// Get the character inside input buffer individually.
		for (int tempCount = 0; tempCount < bufferLength; tempCount++ ) {


			// Check if the circular head is in the end of allocated memory
			// if it is increment the pointer value for one byte.
			if (myQueue.ptrCircularHead == (char*)myQueue.ptrBuffer + 10) {
				myQueue.ptrCircularHead = myQueue.ptrBuffer;

			}

			// Check if queue is full
			// If it is full delete all the data and print error to the console
			if (myQueue.ptrCircularHead[0] != '\0') {
				for (int i = 0; i <= 10; i++) {
					myQueue.ptrBuffer[i] = '\0';
				}

				return printf("Queue is full.");

			}
			
			// Assing the data to head pointer
			myQueue.ptrCircularHead[0] = ptrInputBuffer[tempCount];

			// Increment the head pointer after data loaded
			myQueue.ptrCircularHead = (char*)myQueue.ptrCircularHead + 1;
			numberOfCharsQueued = numberOfCharsQueued + 1;

		}
	}
	else {

		return printf("Please type max 10 characters.");
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

unsigned int getFromCircularQueue(char* ptrOutputBuffer, unsigned int bufferLength)
{
	// Inital value for dequeued data
	int tempQueueCount = 0;

	// add code to implement the removing from the circular queue using the data structures in myQueueStruct
	// and place the data into the ptrOutputBuffer up to a maximum number of bytes specified by bufferLength
	// return the count of the bytes that was dequeued (char*)


	// Check if tail is equal to head.
	// Whenever they are equal it means all data is popped.
	while (myQueue.ptrCircularTail != myQueue.ptrCircularHead) {

		// Check if the circular tail is in the end of allocated memory
		// if it is increment the pointer value for one byte. 
		if (myQueue.ptrCircularTail == (char*)myQueue.ptrBuffer + 10) {
			myQueue.ptrCircularTail = myQueue.ptrBuffer;
		}

		// Assing the data from memory to output buffer array
		ptrOutputBuffer[tempQueueCount] = myQueue.ptrCircularTail[0];

		// Add '\0' to delete data inside the memory
		myQueue.ptrCircularTail[0] = '\0';

		// Increment the head pointer after data loaded
		myQueue.ptrCircularTail = (char*)myQueue.ptrCircularTail + 1;
		tempQueueCount = tempQueueCount + 1;

	}

	// Add '\0' to end of string.
	ptrOutputBuffer[tempQueueCount] = '\0';
	
	return tempQueueCount;
}

// FUNCTION      : myReadThreadFunction
// DESCRIPTION   :
//   A seperate thread from the main program that will independently check for data in queue and print it out
// PARAMETERS    :
//   lpParam - arguement passed into thread (not used in this example)
//
// RETURNS       :
//   Will never return so no effective return value

DWORD WINAPI myReadThreadFunction(LPVOID lpParam)
{
	char readBuffer[BUFFER_SIZE];     // local buffer to put the data into when reading from queue and print
	unsigned int  readCount = 0;      // the number of characters read from queue

	while (1)
	{
		Sleep(INPUT_TIMEOUT_MS);      // wait for some data to be received

		// check if data is available and if so print it out
		readCount = getFromCircularQueue(readBuffer, BUFFER_SIZE);
		if (readCount != 0)           // check for reads that did not get any data
			printf("UNQUEUE: %s\n", readBuffer);
	}
	return 0;    // will never reach here
}
