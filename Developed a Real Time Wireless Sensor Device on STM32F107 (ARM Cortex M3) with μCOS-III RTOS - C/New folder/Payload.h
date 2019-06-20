/*Payload.h*/
/*Chethan Nagesh PAl id:01741616*/
#ifndef Payload_H
#define Payload_H
#include "PktParser.h"
#include "includes.h"
#include "BfrPair.h"

#define payloadBfrSize 14
extern BfrPair payloadBfrPair;
extern OS_SEM openPayloadBfrs;
extern OS_SEM closedPayloadBfrs;
//---------Function Prototypes------
CPU_VOID CreatePayloadTask(CPU_VOID);
CPU_VOID PayloadTask(CPU_VOID *data);
CPU_VOID SerPrintf(CPU_CHAR *format, ...);

#endif