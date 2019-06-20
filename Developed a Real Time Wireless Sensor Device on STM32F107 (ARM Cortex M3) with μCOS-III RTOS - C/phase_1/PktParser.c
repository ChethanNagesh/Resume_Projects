/*PktParser.c*/
/*Chethan Nagesh Pal id:01741616*/

#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "PktParser.h"
#include "Error.h"
#include <cpu.h>

/*Packet strcuture datatype*/
typedef struct
{
  CPU_INT08U payloadLen;
  CPU_INT08U data[one];
  
}PktBfr;

CPU_VOID ParsePkt(CPU_VOID *payloadBfr)
{
  ParserState parseState = P1;                         /*current parser state*/ 
  CPU_INT08U c;                                       /*Getting next byte from file*/
  CPU_INT08U i=zero;                                  /*initialization of local variable*/
  CPU_INT08U checkSum =zero;                           
  PktBfr *pktBfr =payloadBfr;                        /*Making payloadBfr look like packet buffer*/
  for (;;)
  {
    c=GetByte();                                    /*getting next byte from the pakcet*/
    switch (parseState)
    {
  case P1:
     if (c==Preamble1)                               /*checking if the byte we got is equal to the Preambl1*/
      {   
        parseState=P2;
        checkSum=c;                                  /*putting the byte to checksum in order to xor with upcoing bytes*/
      }     
      else 
      {
        parseState=ER;                               /*changing parstate equal to error*/
        PacketError(Preamble1Err);                   /*calling the  error function*/
      }
      break;
  case P2:
    if (c==Preamble2)                                
      {
        parseState=P3;
        checkSum=checkSum^c;                          /* xoring the previous byte with the current byte and storing it*/
      } 
      else
      {
        parseState=ER;                               
        PacketError(Preamble2Err);                   
      }
      break;
  case P3:
    if (c==Preamble3)
      {
        parseState=L;
        checkSum=checkSum^c;                         
      }
      else 
      {
        parseState=ER;                              
        PacketError(Preamble3Err);
      }
      break;
  case L:
      pktBfr->payloadLen=c-HeaderLength;
      parseState=D;
      checkSum=checkSum^c;                         
      if (c<PacketLength)                          
      {
        PacketError(PacketlengthErr);
        parseState=ER;                             
      }
      break;
 case D:
    pktBfr->data[i++]=c;                          /*storing the coming bytes into the defined structure of payload */
    checkSum=checkSum^c;                          
    if (i>=pktBfr->payloadLen-one)                
    {
      parseState=CheckSm;
    }
    break;
 case CheckSm:
    checkSum=checkSum^c;                         
    if (checkSum==checkSumbyte)                  /*checking whether entire checksum byte is eqaul to zero*/
    {
      parseState=P1;
      return;                                   /*returning to the calling function*/
    }
     else
     {
       PacketError(ChecksumErr);
       parseState=ER;                           
       checkSum=zero;
       i=zero;
     }
    break;
      
    case ER:
     while (c==Preamble1)                   
     {
       c=GetByte();                          /*if yes get the next byte */
     if (c==Preamble2)                      
      {
        parseState=P3;                        /*changing  parsestate to P3*/
        checkSum=(Preamble1^Preamble2);       /*calculating the preamble1 and preamble 2 checksum*/
     }
        else
          break;
       }  
     break;
  }
 }
}

