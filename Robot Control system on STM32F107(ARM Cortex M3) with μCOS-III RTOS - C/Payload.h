// YOU SUPPLY THIS FILE.
#ifndef Payload_H
#define Payload_H
#include "PktParser.h"
#include "includes.h"
#include "BfrPair.h"

#define PayloadBfrSize 16


extern BfrPair payloadBfrPair;
//Semaphores
extern OS_SEM openPayloadBfrs;
extern OS_SEM closedPayloadBfrs;


//---------Function Prototypes------
CPU_VOID CreatePayloadTask(CPU_VOID);
CPU_VOID PayloadTask(CPU_VOID *data);
CPU_VOID SerPrintf(CPU_CHAR *format, ...);

#endif