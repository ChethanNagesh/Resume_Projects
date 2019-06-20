/*BfrPair.c*/
/*Chethan Nagesh PAl id:01741616*/

#include "includes.h"
#include "BfrPair.h"

/*initialize both buffer of the buffer pair */
void BfrPairInit (BfrPair *bfrPair,CPU_INT08U *bfr0Space ,CPU_INT08U *bfr1Space,CPU_INT16U size)
{
  bfrPair->putBfrNum=0;
  BfrInit(&bfrPair->buffers[0],bfr0Space,size);
  BfrInit(&bfrPair->buffers[1],bfr1Space,size);
}
/*reset the put buffer*/
void putBfrReset(BfrPair *bfrPair)
 {
BfrReset((PutBfrAddr(bfrPair)));
 }
/*obtain the address of the put buffer's buffer data space*/ 
void  *PutBfrAddr(BfrPair *bfrPair)
 {

 return (&bfrPair->buffers[bfrPair->putBfrNum]);
 }
/*obtain the address of the  get buffer's buffer data space*/
void *GetBfrAddr(BfrPair *bfrPair)
{
  return(&bfrPair->buffers[(1-bfrPair->putBfrNum)]);
  
}

/*test whether or not the putbuffer is closed */
CPU_BOOLEAN PutBfrClosed(BfrPair *bfrPair)
{
  return (BfrClosed(PutBfrAddr(bfrPair)));
}

/*Test whether or not the getbuffer if closed */  
CPU_BOOLEAN GetBfrClosed(BfrPair *bfrPair)
{
 return (BfrClosed(GetBfrAddr(bfrPair)));
 }

/*Mark the putbuffer closed*/
void ClosePutBfr(BfrPair *bfrPair)
 {
BfrClose(PutBfrAddr(bfrPair));
}
/*Mark the getbuffer open*/
void OpenGetBfr(BfrPair *bfrPair)
 {
 BfrOpen(GetBfrAddr(bfrPair));
 }
/*Adding a byte to a buffer at position putindex abd increment putindex by 1,if the buffer beccomes
full mark it closed */
 CPU_INT16S PutBfrAddByte(BfrPair *bfrPair,CPU_INT16S byte)
 {
   return(BfrAddByte(PutBfrAddr(bfrPair),byte));
 }

/*Returns the byte from position getindex and increments getindex by 1.marks the
 buffer as open if the buffer is empty mark it open*/
CPU_INT16S GetBfrRemByte(BfrPair *bfrPair)
{
return(BfrRemoveByte(GetBfrAddr(bfrPair)));
}

/*Test whether or not the buffer pair is ready to be swapped .It is ready if the put buffer is cloed 
and the get buffer is open*/
 CPU_BOOLEAN BfrPairSwappable(BfrPair *bfrPair)
{ 
  if (PutBfrClosed(bfrPair))
  
  {
    if (!GetBfrClosed(bfrPair))
      return TRUE;
  }
  return FALSE;
}
/*swap the put buffer and get buffer and reset the putbuffer */
void BfrPairSwap(BfrPair *bfrPair)
{
  bfrPair->putBfrNum=1-bfrPair->putBfrNum;
  putBfrReset(bfrPair);

}
