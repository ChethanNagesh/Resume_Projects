/*Payload.c*/
/*Chethan Nagesh PAl id:01741616*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "assert.h"
#include "includes.h"

#include "SerIODriver.h"
#include "Payload.h"

#define Mask8UpperBits          0x00FF
#define Mask8LowerBits          0XFF00
#define Mask4UpperBits          0x0F
#define Mask16Upper8LowerBits   0x0000FF00   
#define Mask8upper16LowerBits   0x00FF0000
#define Mask24LowerBits         0XFF000000
#define Mask24UpperBits         0x000000FF
/*shifting is done to move the bits to correct position in the byte*/
#define Shift8Bits              8
#define Shift4Bits              4
#define Shift24Bits             24
#define ForMonth_Shift          5
#define ForMonth_Mask           0x000001E0 
#define ForMinute_Shift         21                
#define ForMinutes_Mask         0x7F00000 
#define ForYear_Shift           9 
#define ForYear_Mask            0x001FFE00
#define ForHour_shift           27
#define ForHours_Mask           0xF8000000 
#define ForDay_Mask             0x0000001F
#define Msb_array_zero          0
#define Lsb_array_one           1
#define array_size              2
#define index_id                0 
#define IdMaximum               10 
#define Desitnation_Address     1
#define IdIndex                 0

#define IdSize                  10
#define Messagetype_Value       1

#define	Payload_STK_SIZE     256 
#define PayloadPrio 5
#define Timeout 0	  // Timeout for semaphore wait
#define SpeedLength 2 /* wind speed array length */
#define DepthLength 2
#define DataLength 10 /* id array length */
#define Idindex 0 /* id array start */
#define IdMax 10
#define BaudRate 9600 
#define PAYLOAD_STK_SIZE 256





static OS_TCB PayloadTCB;
 static CPU_STK	PayloadStk[PAYLOAD_STK_SIZE];
/*Payload structure */
 

typedef struct
{
  CPU_INT08S       payloadLen;
  CPU_INT08U       dstAddr;
  CPU_INT08U       srcAddr;
  CPU_INT08U       msgType;
  union
  {
    CPU_INT08S     temp;
    CPU_INT16U     pres;
     struct
    {
      CPU_INT08S   dewPt;
      CPU_INT08U   hum;
    }hum;
    struct
    {
      CPU_INT08U   speed[array_size];
      CPU_INT16U   dir;
    }wind;
    CPU_INT16U     rad;
    CPU_INT32U     dateTime;
    CPU_INT08U     depth[array_size];
    CPU_INT08U     id[IdSize];
    }dataPart;
}Payload;

 
BfrPair payloadBfrPair;
 static CPU_INT08U PBfr0Space[payloadBfrSize]; 
 static CPU_INT08U PBfr1Space[payloadBfrSize];


OS_SEM openPayloadBfrs; 
OS_SEM closedPayloadBfrs;


void PayloadInit(BfrPair *pBfrPair)
{
  // Create and initialize payload buffer pair.
  BfrPairInit(pBfrPair,PBfr0Space,PBfr1Space,payloadBfrSize);
}
//create payload task
CPU_VOID CreatePayloadTask(CPU_VOID)
{
  OS_ERR osErr;
  OSTaskCreate(&PayloadTCB,                             
               "PayloadTask",         
               PayloadTask,                 
               NULL,                    
               PayloadPrio,            
               &PayloadStk[0],         
               PAYLOAD_STK_SIZE / 10,  
               PAYLOAD_STK_SIZE,       
               0,                       
               0,                       
               NULL,                    
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &osErr);
  
  assert(osErr == OS_ERR_NONE);
  //Creates openPayloadBfrs
  OSSemCreate(&openPayloadBfrs,"Open payloadBfrs",2,&osErr);
  assert(osErr==OS_ERR_NONE);
  //Creates closedPayloadBfrs
  OSSemCreate(&closedPayloadBfrs,"Closed payloadBfrs",0,&osErr);
  assert(osErr==OS_ERR_NONE);
  PayloadInit(&payloadBfrPair);
 // BfrPairInit(&payloadBfrPair,PBfr0Space,PBfr1Space,PayloadBfrSize);
}
/*---------PayloadTask()-------------
Purpose : Takes the payload and prints the appropiate message */
CPU_VOID PayloadTask(CPU_VOID *data)
{
   /*local variable initialization*/
 CPU_INT32U       Date_TimeLeastsbyte=0; 
 CPU_INT32U       Date_TimeThirdsbyte=0; 
 CPU_INT32U       Date_TimeSecondsbyte=0; 
 CPU_INT32U       Date_TimeMostsbyte=0;
 CPU_INT08U       IdVal=0;
 
 /*creating enum to hold  a series of value equal to message type value of the payload*/
 typedef enum{Temperature_Message=Messagetype_Value,Pressure_Message,Humidity_Message,Wind_Message,
              Radiation_Message,DateTime_Message,Precipitation_Message,Id_Message}MessageType;
 Payload *payloadBfr;
     
  for(;;)
  {
     OS_ERR osErr;
     /*Payload task pends on this semaphore waiting for the availability of closed put buffer and swap if available*/
      OSSemPend(&closedPayloadBfrs,Timeout,OS_OPT_PEND_BLOCKING,NULL,&osErr);
      assert(osErr==OS_ERR_NONE);
      if(BfrPairSwappable(&payloadBfrPair))
      BfrPairSwap(&payloadBfrPair);
      /*payload task post to this semaphore stating that new open get buffer is available and ready to be filled */
      OpenGetBfr(&payloadBfrPair);
     OSSemPost(&openPayloadBfrs,OS_OPT_POST_1,&osErr); //Post 1 to the semaphore
     assert(osErr==OS_ERR_NONE);
     payloadBfr = (Payload*)GetBfrAddr(&payloadBfrPair);
 
     if(payloadBfr->payloadLen < 0)
      {
        ErrorType(payloadBfr->payloadLen);//Prints the Error message
      }
      else
      {  
       
        if(payloadBfr->dstAddr==1) //Checks if destination address 1
        {
          
          switch(payloadBfr->msgType)
          {
          /*Printing the Temeprature Message along with  its source node*/
  case Temperature_Message:
    SerPrintf("\nSource Node %d: Temperature Message \n \tTemperature = %d\n",payloadBfr->srcAddr,payloadBfr->dataPart.temp);
    break;
     
    /*Printing the Pressure  Message along with its its source node*/
  case Pressure_Message:
    SerPrintf("\nSOURCE NODE %d:PRESSURE MESSAGE\n\tBarometric Pressure = %d\n",payloadBfr->srcAddr,(((payloadBfr->dataPart.pres >> Shift8Bits) & Mask8UpperBits)|((payloadBfr->dataPart.pres << Shift8Bits)&Mask8LowerBits)));
      break;
      
   /*Printing the Humidity  Message along with its source node*/ 
  case Humidity_Message:
  SerPrintf("\nSOURCE NODE %d: HUMIDITY MESSAGE\n\tDew Point = %d Humidity = %d\n",payloadBfr->srcAddr,payloadBfr->dataPart.hum.dewPt,payloadBfr->dataPart.hum.hum);
      break;
      
  /*Printing the Wind Message Message along with  its source node*/    
  case Wind_Message :
  SerPrintf("\nSOURCE NODE %d: WIND MESSAGE= \n\t Speed  = %d%d%d.%d Wind Direction= %d\n" , payloadBfr->srcAddr,((payloadBfr->dataPart.wind.speed[Msb_array_zero] >> Shift4Bits) & Mask4UpperBits),   
                (payloadBfr->dataPart.wind.speed[Msb_array_zero] & Mask4UpperBits),
                ((payloadBfr->dataPart.wind.speed[Lsb_array_one] >> Shift4Bits) & Mask4UpperBits),   
                (payloadBfr->dataPart.wind.speed[Lsb_array_one] & Mask4UpperBits),
                 payloadBfr->dataPart.wind.dir);
      break;
      
  /*Printing the Radiation intensity Message along with  its source node*/ 
  case Radiation_Message :
  SerPrintf("\nSOURCE NODE %d: SOLAR RADIATION MESSAGE\n\tSolar Radiation Intensity = %d\n",payloadBfr->srcAddr,((payloadBfr->dataPart.rad >> Shift8Bits) & Mask8UpperBits)
               |((payloadBfr->dataPart.pres << Shift8Bits)&Mask8LowerBits));
      break;
      
  /*Printing the Date Time Stamp Message along with its source node*/     
  case DateTime_Message:

                Date_TimeLeastsbyte = ((payloadBfr->dataPart.dateTime >> Shift24Bits) & Mask24UpperBits); 
                Date_TimeThirdsbyte = ((payloadBfr->dataPart.dateTime >> Shift8Bits) & Mask16Upper8LowerBits);
                Date_TimeSecondsbyte= ((payloadBfr->dataPart.dateTime <<Shift8Bits)& Mask8upper16LowerBits);
                Date_TimeMostsbyte= ((payloadBfr->dataPart.dateTime <<Shift24Bits)&Mask24LowerBits);
                payloadBfr->dataPart.dateTime = ( Date_TimeMostsbyte|  Date_TimeSecondsbyte |Date_TimeThirdsbyte|Date_TimeLeastsbyte );
               
                SerPrintf("\nSOURCE NODE %d: DATE/TIME STAMP MESSAGE \n\tDate Time Stamp = %lu/%lu/%lu %lu:%lu\n",payloadBfr->srcAddr,((payloadBfr->dataPart.dateTime & ForMonth_Mask)>>ForMonth_Shift),
                (payloadBfr->dataPart.dateTime &   ForDay_Mask),((payloadBfr->dataPart.dateTime&ForYear_Mask) >>  ForYear_Shift),    
               ((payloadBfr->dataPart.dateTime &ForHours_Mask ) >>  ForHour_shift ),
               ((payloadBfr->dataPart.dateTime & ForMinutes_Mask) >>  ForMinute_Shift));
      break;
      
  /*Printing the Precipitation Message along with its source node*/ 
  case Precipitation_Message :

  SerPrintf("\nSOURCE NODE %d: PRECIPITATION MESSAGE\n\tPrecipitation Depth = %d%d.%d%d\n",payloadBfr->srcAddr,((payloadBfr->dataPart.depth[Msb_array_zero] >> Shift4Bits) & Mask4UpperBits),   
               (payloadBfr->dataPart.depth[Msb_array_zero] & Mask4UpperBits),
               ((payloadBfr->dataPart.depth[Lsb_array_one] >> Shift4Bits) & Mask4UpperBits),   
               (payloadBfr->dataPart.depth[Lsb_array_one] & Mask4UpperBits));
      break;
 /*Printing the Id Message Message along with  its source node*/      
case Id_Message :
  SerPrintf("\nSOURCE NODE %d: SENSOR ID MESSAGE\n\tNode ID =",payloadBfr->srcAddr);

      for(IdVal = IdIndex; IdVal < IdMax ;IdVal++)
      {
        SerPrintf("%c",payloadBfr->dataPart.id[IdVal]);
        if(payloadBfr->dataPart.id[IdVal]=='\n')
        { 
            break;
        }
      }
      SerPrintf("\n");
      
 break;
   default : 
            SerPrintf("\a***ERROR: Unknown Message Type\n");
            break; 
            }
          }
        
  
  else
        {
          SerPrintf("\a***INFO: NOT MY ADDRESS\n");
        }
      }
      } 
      
    
  
}
