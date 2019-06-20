/*SerIoDriver.c*/
/*Chethan Nagesh PAl id:01741616*/

#include "Includes.h"
#include "SerIODriver.h"

#include "assert.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define USART_RXNE  0x20  // Rx not Empty Status Bit
#define USART_TXE   0x80  // Tx Empty Status Bit
#define USART_RXNEIE  0x20  // Rx Not Empty Interrupt Mask
#define USART_TXEIE   0x80  // Tx Empty Interrupt Mask
#define SETENA1 (*((CPU_INT32U*)0xE000E104)) // For Enabling IRQ38 
#define USART2ENA 0x00000040
#define Timeout 0
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

// If not already defined, use the default buffer size of 4.
#ifndef BfrSize
#define BfrSize 4
#endif

static OS_SEM	openOBfrs;	 
static OS_SEM	closedIBfrs;	
// Allocate the input buffer pair.
static BfrPair iBfrPair;
static CPU_INT08U iBfr0Space[BfrSize];
static CPU_INT08U iBfr1Space[BfrSize];

// Allocate the output buffer pair.
static BfrPair oBfrPair;
static CPU_INT08U oBfr0Space[BfrSize];
static CPU_INT08U oBfr1Space[BfrSize];

/*Initialize the RS232 I/O driver*/
CPU_VOID InitSerIO(CPU_VOID)
{
  OS_ERR      err;                                              /* -- Semaphore error code */
  BfrPairInit(&iBfrPair, iBfr0Space, iBfr1Space, BfrSize);      /*Initializing  the input buffer pair*/
  BfrPairInit(&oBfrPair, oBfr0Space, oBfr1Space, BfrSize);      /*Initializing the output buffer pair*/
  OSSemCreate(&openOBfrs, "Empty Bfrs", 1, &err);               /*Creating openObfrs semaphore*/
  assert(err == OS_ERR_NONE);
  OSSemCreate(&closedIBfrs, "Full Buffers", 0, &err);           /*Creating closedIBfrs semaphore*/
  assert(err == OS_ERR_NONE);
  USART2->CR1 |= USART_TXEIE;                                   /*Also unmasking TX and RX interrupt and enable IRQ38*/
  USART2->CR1 |= USART_RXNEIE;
  SETENA1 = USART2ENA;
 }
                                    /*Putbyte*/
/*Putbyte pends on the openOBfrs Semaphore ,waiting for the availability of the open OBfrPair,if available then it swaps the two buffers*/
/*if oBfrPair put buffer is not full,write one byte into the buffer ,unmask the TX interrupt and return txChar as the return value*/
CPU_INT16S PutByte(CPU_INT16S c)
{
  OS_ERR      osErr;

  if (PutBfrClosed(&oBfrPair))
    {
    OSSemPend(&openOBfrs, 0, OS_OPT_PEND_BLOCKING, NULL, &osErr);
    assert(osErr==OS_ERR_NONE);
    BfrPairSwap(&oBfrPair);
    }  
   CPU_INT16S Char= PutBfrAddByte(&oBfrPair, c);
   USART2->CR1 |= USART_TXEIE;

   return Char;
}
                                   /*serviceTx*/
/*serviceTx posts to the OpenOBfrs Sempaphore to signal Putbyte that a new open OBfrPair get buffer is avaialble and ready to be swappped*/
/*If TXE=1 and the oBfrPair get buffer is not empty ,then output one byte to the  UART TX and return*/
CPU_VOID ServiceTx(CPU_VOID)
{
  OS_ERR      osErr;
 
  if (!(USART2->SR & USART_TXE))
    return;
  
  if (!GetBfrClosed(&oBfrPair))
    {
    USART2->CR1 &= ~USART_TXEIE;
    return; 
    }
   
  USART2->DR =GetBfrRemByte(&oBfrPair);

  if (!GetBfrClosed(&oBfrPair))
    {
    OSSemPost(&openOBfrs, OS_OPT_POST_1, &osErr);
    assert(osErr==OS_ERR_NONE);
    }
}

                                      /*GetByte*/
/*Getbyte pends on closedIBfrs semaphore ,waiting for the availability of closed put buffer IBfPair ,if avaliable then it swaps two buffers*/
/*if the iBfrpair get buffer  is not empty ,unmaks the RX interrupt ,remove and return the next byte from the buffer*/

CPU_INT16S GetByte(CPU_VOID)
{
  OS_ERR      osErr;

  CPU_INT16S c;
  
  if (!GetBfrClosed(&iBfrPair))
  {
   OSSemPend(&closedIBfrs,Timeout,OS_OPT_PEND_BLOCKING, NULL, &osErr);	
   assert(osErr==OS_ERR_NONE);
   BfrPairSwap(&iBfrPair);
  }		
  c =GetBfrRemByte(&iBfrPair);
  USART2->CR1 |= USART_RXNEIE;
  return c;
}  
                                        /*ServiceRx*/
/*ServiceRx posts to ClosedIBfrs Sempahore to signal Getbyte that a new closed iBfrpair putbuffer is available and ready to be swapped*/
 /*------------ServiceRx()-----------
If RXNE is 1 and the IBfrPair put buffer is not full ,then read a byte from the UART RX and add it to the put buffer */
CPU_VOID ServiceRx(CPU_VOID)
{
  OS_ERR      osErr;

  if (!(USART2->SR & USART_RXNE))
    return;

  if (PutBfrClosed(&iBfrPair))
    {
    USART2->CR1 &= ~USART_RXNEIE;
    return;
    }
    
  PutBfrAddByte(&iBfrPair,USART2->DR);

  if (PutBfrClosed(&iBfrPair))
    {
    OSSemPost(&closedIBfrs, OS_OPT_POST_1, &osErr);
    assert(osErr==OS_ERR_NONE);
    }

}

/*--------------- S e r P r i n t f ( ) ---------------

PURPOSE:
Provides functionality of standard C printf( ) function printing
to the RS232 Tx rather than to the screen.

INPUT PARAMETERS:
format  - Standard printf format string
...     - Zero or more parameters giving values to print according
          to the "format" format string
*/
 
CPU_VOID SerPrintf(CPU_CHAR *format, ...)
{
#define PrintfBfrSize 81
  
  CPU_CHAR  buffer[PrintfBfrSize];  //  SerPrintf( ) output string buffer
  va_list   vArgs;                  //  Used to interpret "format" parameter

  CPU_CHAR *ptr;                    // Pointer to output string

  // Generate formatted output in "buffer"
  va_start(vArgs, format);
  vsnprintf((char *)buffer, PrintfBfrSize, (char const *)format, vArgs);
  va_end(vArgs);

  // Output the SerPrintf( ) buffer
  for (ptr=buffer; *ptr!='\0';)
    PutByte(*ptr++);   
  
}

/*--------------- E x t I n t e r r u p t ( ) ---------------*/

/*
PURPOSE
External interrupt exception handler. 
Handle rx and tx interrupts.
*/
CPU_VOID SerialISR(CPU_VOID)
{
  /* Disable interrupts. */
  CPU_SR_ALLOC();
  OS_CRITICAL_ENTER();  
  
	/* Tell kernel we're in an ISR. */
	OSIntEnter();

  /* Enable interrupts. */
  OS_CRITICAL_EXIT();
	
  // Service rx interrupt.
  ServiceRx();

  // Service tx interrupt.
  ServiceTx();

	/* Give the O/S a chance to swap tasks. */
	OSIntExit ();
        return;
}
