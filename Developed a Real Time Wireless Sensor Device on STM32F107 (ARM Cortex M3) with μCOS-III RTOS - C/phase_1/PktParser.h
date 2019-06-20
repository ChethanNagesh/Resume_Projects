/*PktParser.h*/
/*Chethan Nagesh Pal id:01741616*/
#ifndef PktParser_H
#define PktParser_H
#define HeaderLength         4
#define Destination_Address  1
#define Preamble1            0x03
#define Preamble2            0xEF
#define Preamble3            0xAF
#define HeaderLength         4
#define PacketLength         8
#define checkSumbyte         0x00
#define zero                 0
#define one                  1
#include "includes.h"

/*function defination*/
CPU_VOID ParsePkt(CPU_VOID *payloadbfr);
/*creating a enum type of initial value 00*/
typedef enum {P1,P2,P3,L,D,CheckSm,ER}ParserState;



#endif
