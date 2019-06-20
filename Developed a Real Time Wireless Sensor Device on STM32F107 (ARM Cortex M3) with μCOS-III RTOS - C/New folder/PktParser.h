
/*PktParser.h*/
/*Chethan Nagesh PAl id:01741616*/
#ifndef PktParser_H
#define PktParser_H

/*=============== P k t P a r s e r . h ===============*/

/*
BY:	George Cheney
		EECE472 / EECE572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
This is the interface to the module PktParser.c.

CHANGES
03-02-2017  - Created for Spring 2017
*/

#include "includes.h"
#include "Payload.h"
#include "BfrPair.h"
#include "SerIODriver.h"
#include "error.h"
#include "assert.h"


typedef struct
{
  CPU_INT08S  payloadLen;
  CPU_INT08U  data[1];
}PktBfr;

//----- f u n c t i o n    p r o t o t y p e s -----

CPU_VOID CreateParserTask(CPU_VOID);
CPU_VOID ParsePkt(CPU_VOID *data);


#endif

