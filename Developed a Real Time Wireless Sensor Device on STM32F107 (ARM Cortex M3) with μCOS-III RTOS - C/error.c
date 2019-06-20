/*Error.c*/
/*Chethan Nagesh Pal id:01741616*/
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "error.h"
#include "includes.h"
#include "Payload.h"

CPU_VOID ErrorType(CPU_INT08S errorNumber)
{
  switch (errorNumber)
  {
    /*printing preamble byte1 error*/
  case Preamble1Err:
    SerPrintf("\a **Error: Bad Preamble byte 1 error**\n");
      break;
   /*printing preamble byte2 error*/   
  case Preamble2Err:
      
      SerPrintf("\a **Error: Bad Preamble  byte 2 error**\n");
      break;
   /*printing preamble byte3 error*/
  case Preamble3Err:
    
      SerPrintf("\a **Error: Bad Preamble  byte 3 error**\n");
      break;
  /*printing packet length error*/
  case PacketlengthErr:
    SerPrintf("\a  **ERROR:Bad PacketSize** \n");
      break;
  
case CheckSumErr:
    
       SerPrintf("\a **ERROR: Checksum Error**\n");
  }
}