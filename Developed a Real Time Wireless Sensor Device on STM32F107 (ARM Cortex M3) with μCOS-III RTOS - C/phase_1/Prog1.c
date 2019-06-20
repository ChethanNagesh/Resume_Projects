/*File Prog1.c */
/*Chethan Nagesh Pal id:01741616*/

#include "PktParser.h"
#include <stdio.h>
#include <string.h>
#include "includes.h"
#pragma pack(one)
#include "Error.h"
CPU_VOID AppMain();                     //Function prototypes
CPU_VOID PayloadInformation();

/*Constant Definitions*/

#define Destination_Address     1
#define BaudRate                9600 

/*Masking is done to massk the unwanted bits of the particular byte*/

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
#define IdMax                   10
#define IdSize                  10
#define Messagetype_Value       0x01
#define Zero                    0

/*Payload structure */
typedef struct
{
  CPU_INT08U       payloadLen;
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

Payload payloadBfr;                                /*Payload Buffer*/   

CPU_INT32U main ()
{
BSP_IntDisAll();                                /*  Disable all interrupts    */
BSP_Init();                                   /* Initialize BSP functions     */
BSP_Ser_Init(BaudRate);                      /* Initialize the RS232 interface*/

AppMain();                                  /*calling the Appmain function function*/ 
return Zero;
}

CPU_VOID AppMain()                           /*Appmain function defination      */
{
for(;;)
  {
   ParsePkt(&payloadBfr);                      /*sending the address of the payloadBfr*/
  if (payloadBfr.dstAddr!=Desitnation_Address)/*checksing if destination address is =1*/
  {
    PacketError(DestinationAddressErr);
  }
  else                                       /*if yes then execute*/
  {
  PayloadInformation();
  }
}
}

CPU_VOID PayloadInformation()
{
  /*local variable initialization*/
 CPU_INT32U       Date_TimeLeastsbyte=Zero; 
 CPU_INT32U       Date_TimeThirdsbyte=Zero; 
 CPU_INT32U       Date_TimeSecondsbyte=Zero; 
 CPU_INT32U       Date_TimeMostsbyte=Zero;
 CPU_INT08U       IdVal=Zero;
 
 /*creating enum to hold  a series of value equal to message type value of the payload*/
 typedef enum{Temperature_Message=Messagetype_Value,Pressure_Message,Humidity_Message,Wind_Message,
              Radiation_Message,DateTime_Message,Precipitation_Message,Id_Message}MessageType;

  switch (payloadBfr.msgType)
  {
    /*Printing the Temeprature Message along with  its source node*/
  case Temperature_Message:
    BSP_Ser_Printf("\nSource Node %d: Temperature Message \n",payloadBfr.srcAddr);
    BSP_Ser_Printf("\tTemperature = %d\n",payloadBfr.dataPart.temp);
    break;
     
    /*Printing the Pressure  Message along with its its source node*/
  case Pressure_Message:
    BSP_Ser_Printf("\nSOURCE NODE %d:PRESSURE MESSAGE\n",payloadBfr.srcAddr);
    BSP_Ser_Printf("\tBarometric Pressure = %d\n",((payloadBfr.dataPart.pres >> Shift8Bits) & Mask8UpperBits)|((payloadBfr.dataPart.pres << Shift8Bits)&Mask8LowerBits));
      break;
      
   /*Printing the Humidity  Message along with its source node*/ 
  case Humidity_Message:
  BSP_Ser_Printf("\nSOURCE NODE %d: HUMIDITY MESSAGE\n",payloadBfr.srcAddr);
  BSP_Ser_Printf("\tDew Point = %d Humidity = %d\n",payloadBfr.dataPart.hum.dewPt,payloadBfr.dataPart.hum.hum);
      break;
      
  /*Printing the Wind Message Message along with  its source node*/    
  case Wind_Message :
  BSP_Ser_Printf("\nSOURCE NODE %d: WIND MESSAGE= \n", payloadBfr.srcAddr);
  BSP_Ser_Printf("\t Speed  = %d%d%d.%d Wind Direction= %d\n",((payloadBfr.dataPart.wind.speed[Msb_array_zero] >> Shift4Bits) & Mask4UpperBits),   
                (payloadBfr.dataPart.wind.speed[Msb_array_zero] & Mask4UpperBits),
                ((payloadBfr.dataPart.wind.speed[Lsb_array_one] >> Shift4Bits) & Mask4UpperBits),   
                (payloadBfr.dataPart.wind.speed[Lsb_array_one] & Mask4UpperBits),
                 payloadBfr.dataPart.wind.dir);
      break;
      
  /*Printing the Radiation intensity Message along with  its source node*/ 
  case Radiation_Message :
  BSP_Ser_Printf("\nSOURCE NODE %d: SOLAR RADIATION MESSAGE\n",payloadBfr.srcAddr);
  BSP_Ser_Printf("\tSolar Radiation Intensity = %d\n",((payloadBfr.dataPart.rad >> Shift8Bits) & Mask8UpperBits)
               |((payloadBfr.dataPart.pres << Shift8Bits)&Mask8LowerBits));
      break;
      
  /*Printing the Date Time Stamp Message along with its source node*/     
  case DateTime_Message:
  BSP_Ser_Printf("\nSOURCE NODE %d: DATE/TIME STAMP MESSAGE \n",payloadBfr.srcAddr);
                Date_TimeLeastsbyte = ((payloadBfr.dataPart.dateTime >> Shift24Bits) & Mask24UpperBits); 
                Date_TimeThirdsbyte = ((payloadBfr.dataPart.dateTime >> Shift8Bits) & Mask16Upper8LowerBits);
                Date_TimeSecondsbyte= ((payloadBfr.dataPart.dateTime <<Shift8Bits)& Mask8upper16LowerBits);
                Date_TimeMostsbyte= ((payloadBfr.dataPart.dateTime <<Shift24Bits)&Mask24LowerBits);
                payloadBfr.dataPart.dateTime = ( Date_TimeMostsbyte|  Date_TimeSecondsbyte |Date_TimeThirdsbyte|Date_TimeLeastsbyte );
               
                BSP_Ser_Printf("\tDate Time Stamp = %lu/%lu/%lu %lu:%lu\n",((payloadBfr.dataPart.dateTime & ForMonth_Mask)>>ForMonth_Shift),
                (payloadBfr.dataPart.dateTime &   ForDay_Mask),((payloadBfr.dataPart.dateTime&ForYear_Mask) >>  ForYear_Shift),    
               ((payloadBfr.dataPart.dateTime &ForHours_Mask ) >>  ForHour_shift ),
               ((payloadBfr.dataPart.dateTime & ForMinutes_Mask) >>  ForMinute_Shift));
      break;
      
  /*Printing the Precipitation Message along with its source node*/ 
  case Precipitation_Message :
    
  BSP_Ser_Printf("\nSOURCE NODE %d: PRECIPITATION MESSAGE\n",payloadBfr.srcAddr);
  BSP_Ser_Printf("\tPrecipitation Depth = %d%d.%d%d\n",((payloadBfr.dataPart.depth[Msb_array_zero] >> Shift4Bits) & Mask4UpperBits),   
               (payloadBfr.dataPart.depth[Msb_array_zero] & Mask4UpperBits),
               ((payloadBfr.dataPart.depth[Lsb_array_one] >> Shift4Bits) & Mask4UpperBits),   
               (payloadBfr.dataPart.depth[Lsb_array_one] & Mask4UpperBits));
      break;
 /*Printing the Id Message Message along with  its source node*/      
case Id_Message :
  BSP_Ser_Printf("\nSOURCE NODE %d: SENSOR ID MESSAGE\n",payloadBfr.srcAddr);
  BSP_Ser_Printf("\tNode ID =");
      for(IdVal = IdIndex; IdVal < IdMax ;IdVal++)
      {
        BSP_Ser_Printf("%c",payloadBfr.dataPart.id[IdVal]);
        if(payloadBfr.dataPart.id[IdVal]=='\n')
        { 
            break;
        }
      }
      BSP_Ser_Printf("\n");
 break;
 /*Printing the if no message type matches */ 
 default : 
    PacketError(UnknownMessageErr);
      break;
  
}   
                     
}      
