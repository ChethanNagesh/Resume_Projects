/*SerIoDriver.c/
/*Chethan Nagesh PAl id:01741616*/

#include "SerIODriver.h"
#ifndef BfrSize
#define BfrSize 4
#endif

#define USART_TXE 0x80 /* Masking to read the TXE bit */
#define USART_RXNE 0x20 /*Masking to read the RXNE bit*/

/*Allocating static space for input buffer pair*/
static BfrPair iBfrPair;
static CPU_INT08U iBfr0Space[BfrSize];
static CPU_INT08U iBfr1Space[BfrSize];

/*Allocating static space for output buffer pair*/
static BfrPair oBfrPair;
static CPU_INT08U oBfr0Space[BfrSize];
static CPU_INT08U oBfr1Space[BfrSize];

/*Initialize the RS232 I/O driver*/
/*Initializing  the input buffer pair*/
/*Initializing the output buffer pair*/
 void InitSerIO(void)
 {
 BfrPairInit(&iBfrPair,iBfr0Space,iBfr1Space,BfrSize);
 BfrPairInit(&oBfrPair,oBfr0Space,oBfr1Space,BfrSize);
 }


 /*PutByte():
if the oBfrPair put buffer is closed and oBfrpair get buffer is not closed, swap the get buffers and put buffers.
if oBfrPair put buffer is not full,write one byte into the buffer ,and return txChar as the return value;
if ,the buffer is full return -1 indicating fialure*/
 
 CPU_INT16S PutByte(CPU_INT16S txChar)
 {
 if(BfrPairSwappable(&oBfrPair))
 {
BfrPairSwap(&oBfrPair);
 }
 if(PutBfrClosed(&oBfrPair))
 {
 return -1;
 }
 return PutBfrAddByte(&oBfrPair, txChar);

 }

 /*ServiceTX():
If TXE=1 and the oBfrPair get buffer is not empty ,then output one byte to the  UART TX and return.if TXE=0 
or if the get buffer is empty ,just return */

 void ServiceTx(void)
 {
 if (!(USART2->SR & USART_TXE)) {
 return;
 }
 if (!GetBfrClosed(&oBfrPair))
 {
 return;
 }
 USART2->DR = (CPU_INT08U)GetBfrRemByte(&oBfrPair);
 }

 /*GetByte():
If the iBfrPair put buffer is closed and the iBfrPair get buffer is not closed swap the get buffer and put buffer.
if the iBfrpair get buffer  is not empty ,remove and return the next byte from the buffer .if the buffer is empty 
return -1 indicating failure */
 CPU_INT16S GetByte(void)
 {
 if(BfrPairSwappable(&iBfrPair))
 { 
 BfrPairSwap(&iBfrPair);
 }
 if(!GetBfrClosed(&iBfrPair))
 return -1;
 return GetBfrRemByte(&iBfrPair);
 }
 

 
 /*------------ServiceRx()-----------
If RXNE is 1 and the IBfrPair put buffer is not full ,then read a byte from the UART RX and add it to the put buffer 
if RXNE =0 or the put buffer is full ,just return */

 void ServiceRx(void)
 {
 if (!(USART2->SR & USART_RXNE))
 {
 return;
 }
 if (PutBfrClosed(&iBfrPair))
 {
 return;
 }
 PutBfrAddByte(&iBfrPair, (CPU_INT16S) USART2->DR);
 }
 
