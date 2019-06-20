
/*SerIoDriver.h*/
/*Chethan Nagesh PAl id:01741616*/

 #include <stdio.h>
 #include "includes.h"
 #include "BfrPair.h"
 #include "Buffer.h"
 #ifndef SerIODriver_H
 #define SerIODriver_H

 //------------Function Prototypes-------------
/*Create and Initialize iBfr and oBfr*/
void InitSerIO(void);
/*Put the byte into oBfrPair put buffer*/ 
 CPU_INT16S PutByte(CPU_INT16S txChar);
/*Get the byte from iBfrPair get buffer*/ 
CPU_INT16S GetByte(void);
/*Execute ServiceRx() and ServiceTx()*/
void SerialISR(void);
#endif