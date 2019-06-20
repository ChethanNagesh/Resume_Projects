/*Error.h*/
/*Chethan Nagesh Pal id:01741616*/
#ifndef Error_H
#define Error_H
#include "includes.h"
/*creating a enum type of initial value 00*/
typedef enum {Preamble1Err,Preamble2Err,Preamble3Err,PacketlengthErr,DestinationAddressErr,UnknownMessageErr,ChecksumErr} errorstate;
/*function defination*/
CPU_VOID PacketError(CPU_INT08U Error);

#endif