/*error.h
Chethan Nagesh Pal
id:01741616*/

#ifndef error_H
#define error_H

#include "Payload.h"
//-------Error Codes--------
#define Preamble1Err -1  
#define Preamble2Err -2  
#define Preamble3Err -3
#define CheckSumErr -4
#define PacketlengthErr -5

//-------------Function Prototypes-----
void ErrorType(CPU_INT08S errorNumber);

#endif