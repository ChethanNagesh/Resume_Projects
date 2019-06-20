/*=============== P k t P a r s e r . c ===============*/

/*
BY:	George Cheney
		EECE472 / EECE572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell 
*/

/*
CHANGES
03-02-2017  - Created for Spring 2017
*/

#include "PktParser.h"
#include "BfrPair.h"
#include "includes.h"
#include "error.h"
#include "SerIODriver.h"
//----- c o n s t a n t    d e f i n i t i o n s -----

#define ParserPrio 6 // Parser task priority
#define HeaderLength         4
#define Preamble1            0x03
#define Preamble2            0xEF
#define Preamble3            0xAF
#define PacketLength         8
#define checkSumbyte         0

#include "includes.h"
#define Timeout 0
/* Size of the Process task stack */
#define	PARSER_STK_SIZE     256 

/*----- g l o b a l s -----*/

// Process Task Control Block
 static OS_TCB parserTCB;

/* Stack space for Process task stack */
static CPU_STK parserStk[PARSER_STK_SIZE];
void ErrorState(PktBfr *pktBfr,BfrPair *payloadBfrPair,CPU_INT16S errorcode);


extern BfrPair payloadBfrPair;
extern CPU_INT08U PBfr0Space[payloadBfrSize]; 
extern CPU_INT08U PBfr1Space[payloadBfrSize];
 


/*----- C r e a t e P a r s e r T a s k ( ) -----

PURPOSE
Create and initialize the Parser Task.
*/
CPU_VOID CreateParserTask(CPU_VOID)
{
	OS_ERR		osErr; /* -- OS Error code */

	/* Create Parser task. */	
  OSTaskCreate(&parserTCB,
               "Processing Task",
               ParsePkt, 
               NULL,
               ParserPrio,
               &parserStk[0],
               PARSER_STK_SIZE / 10,
               PARSER_STK_SIZE,
               0,
               0,
               NULL,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &osErr);

  assert(osErr == OS_ERR_NONE);
}
typedef enum {P1,P2,P3,L,D,CheckSm,ER}ParserState;

CPU_VOID ParsePkt(CPU_VOID *data)
{
  ParserState parseState = P1;                         /*current parser state*/ 
  CPU_INT08U c;                                       /*Getting next byte from file*/
  CPU_INT08U i=0;                                  /*initialization of local variable*/
  CPU_INT08U checkSum=0 ;
  PktBfr *pktBfr;

  for(;;)
  {
  
   if(PutBfrClosed(&payloadBfrPair))
    {
      OS_ERR osErr;
      /*parser task pends on this semaphore ,waiting for the availability of the  new open get buffer ,if available then swap buffer */
      OSSemPend(&openPayloadBfrs,Timeout,OS_OPT_PEND_BLOCKING,NULL,&osErr);
      assert(osErr==OS_ERR_NONE);
      if(BfrPairSwappable(&payloadBfrPair))
        BfrPairSwap(&payloadBfrPair);
    }
      pktBfr = (PktBfr*)PutBfrAddr(&payloadBfrPair); /*typecasting the addreess of payloadBfr Putbuffer to pktbfr */
      c = GetByte();
    
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
        parseState=ER;                                               /*changing parstate equal to error*/
        ErrorState(pktBfr,&payloadBfrPair,Preamble1Err);             /*calling the  error function*/
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
       ErrorState(pktBfr,&payloadBfrPair,Preamble2Err);                   
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
       ErrorState(pktBfr,&payloadBfrPair,Preamble3Err);
      }
      break;
  case L:
      pktBfr->payloadLen=c-HeaderLength;
      parseState=D;
      i=0;
      checkSum=checkSum^c;                         
      if (c<PacketLength)                          
      {
        ErrorState(pktBfr,&payloadBfrPair,PacketlengthErr);
        parseState=ER;                             
      }
      break;
 case D:
    pktBfr->data[i++]=c;                          /*storing the coming bytes into the defined structure of payload */
    checkSum=checkSum^c;                          
    if (i>=pktBfr->payloadLen-1)                
    {
      parseState=CheckSm;
    }
    break;
 case CheckSm:
    checkSum=checkSum^c;                         
    if (checkSum==checkSumbyte)                  /*checking whether entire checksum byte is eqaul to zero*/
    {
      parseState=P1;
      OS_ERR osErr;
      ClosePutBfr(&payloadBfrPair);
      OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1,&osErr); /*parser task post to this semaphore to signal payloadtask that a new closed put buffer is filled and ready to be swapped */
      assert(osErr==OS_ERR_NONE);
     }
     else
     {
       ErrorState(pktBfr,&payloadBfrPair,CheckSumErr);
       parseState=ER;                           
       checkSum=0;
       i=0;
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
  

/*------------ErrorState()---------
//Purpose:Posts the semaphore closedpayloadBfrs and gets the error code*/
CPU_VOID ErrorState(PktBfr *pktBfr,BfrPair *payloadBfrPair,CPU_INT16S errorCode)
{
  OS_ERR osErr;
  pktBfr->payloadLen=errorCode;
  ClosePutBfr(payloadBfrPair);
  OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1, &osErr);
  assert(osErr == OS_ERR_NONE);
}
