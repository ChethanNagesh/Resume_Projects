/*Error.c*/
/*Chethan Nagesh Pal id:01741616*/
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "Error.h"
#include "includes.h"
#include "PktParser.h"

CPU_VOID PacketError(CPU_INT08U Error)
{
  
  switch (Error)
  {
    /*printing preamble byte1 error*/
  case Preamble1Err:
    
      BSP_Ser_Printf("\a **Error: Bad Preamble byte 1 error**\n");
      break;
   /*printing preamble byte2 error*/   
  case Preamble2Err:
      
      BSP_Ser_Printf("\a **Error: Bad Preamble  byte 2 error**\n");
      break;
   /*printing preamble byte3 error*/
  case Preamble3Err:
    
      BSP_Ser_Printf("\a **Error: Bad Preamble  byte 3 error**\n");
      break;
  /*printing packet length error*/
  case PacketlengthErr:
    
      BSP_Ser_Printf("\a  **ERROR:Bad PacketSize** \n");
      break;
   /*printing destination address error*/
  case DestinationAddressErr:
    
      BSP_Ser_Printf("\a  **INFO: NOT MY ADDRESS** \n");
      break;         
  /*printing unkown message  error*/  
  case UnknownMessageErr:
   
      BSP_Ser_Printf("\a **ERROR: Unknown Message Type** \n");
      break;
    /*printing checksum erro*/
  case ChecksumErr:
    
       BSP_Ser_Printf("\a **ERROR: Checksum Error**\n");
  }
}