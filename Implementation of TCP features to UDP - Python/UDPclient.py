import socket                              #importing all the libraries
import os
from ADDITION import add                   #importing the user built library 
import math
import threading

from threading import Timer
from time import sleep
selection=input("choose your option :1 for recovery ,2 for non-recovery ")# we are giving an option to choose the tye of mechanism to work
class RepeatedTimer(object): # we are creating a repeated timer class which gives us the option of start stop and cancel the timer
    def __init__(self, interval, function, *args, **kwargs): 
        self._timer     = None
        self.interval   = interval
        self.function   = function
        self.args       = args
        self.kwargs     = kwargs
        self.is_running = False
        self.start()

    def _run(self):
        self.is_running = False
        self.start()
        self.function(*self.args, **self.kwargs)

    def start(self):
        if not self.is_running:
            self._timer = Timer(self.interval, self._run)
            self._timer.start()
            self.is_running = True

    def stop(self):
        self._timer.cancel()
        self.is_running = False

Refrence=0
ServerName="Localhost"                     #since we are transmitting on same ip ,we are just filling with Localhost character
PortNumber=5007                        #assining the port number 
ClientSocket=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)#creating client UDP packet
address=(ServerName,PortNumber)            # binding the ip and portnumber  
buff=1024                                  #limiting the datatransmit to 1KB
chk=0                                      #intializing the chk variable 

fle=open("image_t.jpg","rb")               #opening the file
buffer=bytearray(os.path.getsize("image_t.jpg"))#using the library bytearray to get the bytes in array format
#print''+str(buffer)
fle.readinto(buffer)                       #read into all the information contained in the image 
length_of_buffer=len(buffer)               #get the lenght of the buffer
print length_of_buffer

#perfroming to get the empty bytes and left justifying all the bytes

number_Packets=(int)(length_of_buffer/buff)+1 #keeping the value of the result to integer
emptyBytes=(number_Packets*buff -length_of_buffer)
print emptyBytes
if(emptyBytes!=0):
        filler=buffer.ljust(emptyBytes+length_of_buffer,'0')#left justifying the bytes and appending the empty space with zero
        New_Length=len(filler)
        print New_Length
        buffer=filler
        
#intializing the indexs that point towards the required data

first_index=0
second_index=2  # keeping the raange to 2,so that it fetches 2 bytes of data in one iteration


pck0_0=0        #Packet_1 range
pck0_1=1024

pck1_0=1024     #Packet_2 range
pck1_1=2048

pck2_0=2048     #Packet_3 range
pck2_1=3072

pck3_0=3072      #Packet_4 range
pck3_1=4096

pck4_0=4096      #Packet_5 range
pck4_1=5120

ACK=str(0)      #initializing it to 0


               #Initializing all the variable that are used in the program
check=0
seqNo=0
first_iteration=1
FSM=1          #Initilzing the FSM (finite state machine to state 2)
z2=1
Refrence=0
Nextseq=1
base=1
window_size=3  #Flags for the transmission of the packets
flag_1=0;
flag_2=0
flag_3=0
flag_4=0
flag_5=0
flag_6=0
flag_1_1=1
flag_2_2=1
check_1=0
check_2=0
check_3=0
check_4=0
check_5=0
buf=0
packets_state=1
checksum=0
err=78
err_1=52
#function to calculate checksum

def checksum_Packets():
        global first_index
        global second_index
        global buf
        global packets_state
        global checksum
        iteration=0
        sum_all_Bites=0
        sum_all_Bites=bin(sum_all_Bites)  #typecasting it to binary

        checksum=0
        checksum=bin(checksum)            #typecasting it to binary

        while(iteration<=510):            # looping it 510 times ,we are fetching 1024 bytes of data in this loop in 50%in data1 and 50 % in data2
                data_1=buffer[first_index:second_index]   #fetches 16 bits at a time
                two_bytes=''.join(format(x,'b').zfill(8) for x in bytearray(data_1))
                buf=buf+1
                #print'index'
                
                first_index=first_index+2 #incrementing it by 2 position two fecth next 16 bits
                second_index=second_index+2
                
                
                

                data_2=buffer[first_index:second_index]
                two_bytes_2=''.join(format(x,'b').zfill(8) for x in bytearray(data_2))
                sum_one_iteration=add(two_bytes,two_bytes_2)

                first_index=first_index+2   #incrementing it by 2 position two fecth next 16 bits
                second_index=second_index+2
               
                iteration=iteration+2
                #print ""+str(iteration)
                sum_all_Bites=add(sum_one_iteration,sum_all_Bites)
      
        checksum=''.join([bin(~0)[3:] if x == '0' else bin(~1)[4:] for x in sum_all_Bites])
        if(checksum=='111'): #Ending the transmission of the packet if there is no data to transmit

            
            print"All data have been sent "
            print"closing the socket"
           
            flag_1=1
            flag_2=1
            flag_3=1
            flag_4=1
            flag_5=1
            #print ''
            
            fle.close
            ClientSocket.close()
            
                                                   
        
        return checksum


#Function to send first packet

def SendPacket_1():
    global pck0_0  #telling the compiler that these varibale as been defined in global section
    global pck0_1
    global packets_state
    global selection
    checksum=0
    checksum=checksum_Packets()
    data=buffer[pck0_0:pck0_1]
    data=str(data) #typecasting it string type
    packetdata=[data,checksum,'1'] #appending it with sequence number
    packetdata=''.join(packetdata) #joing all the bits
    
    print "PACKET[1] is Sent"
    #print "Checksum of Sender="+checksum
    packets_state=1
    ClientSocket.sendto(packetdata,address)#tranitting the data to server
    if(selection==1 or selection==2): # if selection is equal to 1, start timer (repeat for all selection lines)
                rt_1.start()
    pck0_0=pck0_0+5120 #jumping to fetch next KBs of data
    pck0_1=pck0_1+5120
    return checksum

#function to send second packet

def SendPacket_2():
    global pck1_0                #telling the compiler that these varibale as been defined in global section
    global pck1_1
    global packets_state
    global checksum
    global selection
    checksum=0
    checksum=checksum_Packets()
    data=buffer[pck1_0:pck1_1]
    data=str(data)                #typecasting it string type
    packetdata=[data,checksum,'2']#appending it with sequence number
    packetdata=''.join(packetdata)#joing all the bits
    
    print "PACKET[2] is Sent"
    #print "Checksum of Sender="+checksum
    packets_state=2
    ClientSocket.sendto(packetdata,address)#tranitting the data to server
    if(selection==1 or selection==2):
                rt_2.start()
 
    pck1_0=pck1_0+5120           #jumping to fecth next KBs of data
    pck1_1=pck1_1+5120
    return checksum

def SendPacket_3():
    global pck2_0                #telling the compiler that these varibale as been defined in global section
    global pck2_1
    global packets_state
    global checksum
    global selection            
    checksum=0
    checksum=checksum_Packets()
    data=buffer[pck2_0:pck2_1]
   
    data=str(data)                #typecasting it string type
    packetdata=[data,checksum,'3']#appending it with sequence number
    packetdata=''.join(packetdata)#joing all the bits
    
    print "PACKET[3] is Sent"
    #print "Checksum of Sender="+checksum
    packets_state=3
    ClientSocket.sendto(packetdata,address)#tranitting the data to server
    if(selection==1 or selection==2):
                rt_3.start()
    pck2_0=pck2_0+5120           #jumping to fecth next KBs of data
    pck2_1=pck2_1+5120
    
    return checksum

def SendPacket_4():
    global pck3_0                #telling the compiler that these varibale as been defined in global section
    global pck3_1
    global packets_state
    global checksum
    global selection
    checksum=0
    checksum=checksum_Packets()
    data=buffer[pck3_0:pck3_1]
    data=str(data)                #typecasting it string type
    packetdata=[data,checksum,'4']#appending it with sequence number
    packetdata=''.join(packetdata)#joing all the bits
   
    print "PACKET[4] is Sent"
    #print "Checksum of Sender="+checksum
    packets_state=4
    ClientSocket.sendto(packetdata,address)#tranitting the data to server
    if(selection==1 or selection==2):
                rt_4.start()
  
    pck3_0=pck3_0+5120           #jumping to fecth next KBs of data
    pck3_1=pck3_1+5120


    return checksum

def SendPacket_5():
    global pck4_0                #telling the compiler that these varibale as been defined in global section
    global pck4_1
    global packets_state
    global checksum
    global selection
    checksum=0
    checksum=checksum_Packets()
    data=buffer[pck4_0:pck4_1]
    data=str(data)                #typecasting it string type
    packetdata=[data,checksum,'5']#appending it with sequence number
    packetdata=''.join(packetdata)#joing all the bits
    
    print "PACKET[5] is Sent"
    #print "Checksum of Sender="+checksum
    packets_state=5 
    ClientSocket.sendto(packetdata,address)
    if (selection==1 or selection==2):
                rt_5.start()
    
    pck4_0=pck4_0+5120           #jumping to fecth next KBs of data
    pck4_1=pck4_1+5120
    
    return checksum


 
#function for sending packets to the server and waiting for the Acknowledge from server

def Packets():
        
        global pck0_0        #intializing all the variables that is used in this function
        global pck0_1
        global pck1_0
        global pck1_1
        global ACK
        global seqNo
        global ab
        global first_index
        global second_index
        global Refrence
        global FSM
        global z2
        global first_iteration
        global Nextseq
        global Refrence
        global base
        global window_size
        global flag_1
        global flag_2
        global flag_3
        global flag_4
        global flag_5
        global flag_6
        global check_1
        global check_2
        global check_3
        global check_4
        global check_4
        global check_5
        global flag_1_1
        global flag_2_2
        global check
        global No_of_kilobytes
        global selection
        global err
        global err_1
        global Refrence
        #No_of_kilobytes=No_of_kilobytes+1
        #print"base"+str(base)
        
                
        if (base==6): #resetting the flags after every 5 packets have been sent
            Nextseq=1
            base=1
            flag_1=0
            flag_2=0
            flag_3=0
            flag_4=0
            flag_5=0
        if(Nextseq<(base+window_size)):#if next sequence is less than the sum of base+window size then proceed
           
            if (flag_1==0): #if flag1==0 then send packet 1
                check_1=SendPacket_1()
                flag_1=1
                #print Nextseq
                Nextseq=Nextseq+1
                return
            elif(flag_2==0): #if flag2==0 then send packet 2
                check_2=SendPacket_2()
                flag_2=1
                #print Nextseq
                Nextseq=Nextseq+1
                return
            elif(flag_3==0): #if flag3==0 then send packet 3
                check_3=SendPacket_3()
                flag_3=1
                #print Nextseq
                Nextseq=Nextseq+1
                return
            elif(flag_4==0): #if flag4==0 then send packet 4
                check_4=SendPacket_4()
                flag_4=1
                #print Nextseq
                Nextseq=Nextseq+1 #increment next sequence number
                return
            elif(flag_5==0): #if flag5==0 then send packet 5
                check_5=SendPacket_5()
                flag_5=1
                Nextseq=Nextseq+1
                return
            elif(flag_1_1==0): #if base of window is flag4 and flag5 then transmit flag1_1
                check_1=SendPacket_1()
                flag_1_1=1
                #print Nextseq
                Nextseq=Nextseq+1
                return
            elif(flag_2_2==0): #if base of window is flag5 and flag1_1 then transmit flag2_2
                check_2=SendPacket_2()
                flag_2_2=1
                #print Nextseq
                Nextseq=Nextseq+1
                return
                
               
          
                
               
      
                   
        returnpacket,serverAddress=ClientSocket.recvfrom(1041) #receiving the packet and parsing the acknowledgement bit
        ACK=returnpacket[0:1]
        print Refrence
        print"ACK at receiver="+ACK
        if (Refrence==err_1 and err_1<53):
            ACK=returnpacket[0:0]
            err_1=err_1+1
            print "ACk bit loss"
        if (Refrence==err and err<80):
            ACK=returnpacket[0:4]
            err=err+1
            print "ACk bit error"
            
       
        seqNo=returnpacket[1:2] #parsing the sequence bit from the received bytes from the  server
        chk=returnpacket[2:18]  #parsing 16 bit checksum from the received bytes from the server
        #print "Receiver Checksum ="+chk
        print ""
        if (FSM==1): #waiting for the arrival of packet 1, repeat it for all
            if ((ACK=='1')&(chk==check_1)&(seqNo=='1')):
              
                FSM=2 #moving to next FSm if packet 1 is received successfully
                base=base+1 #incrementing the base index
              
                print "ACK 1 is received"
                rt_4.stop() #stopping the timers
                rt_5.stop()
                rt_1.stop()
                
                #Packets()
                #print''
                return
            
               
        elif(FSM==2):
            if((ACK=='2')&(chk==check_2)&(seqNo=='2')):
                FSM=3
                base=base+1
                print "ACK 2 is received"
                rt_5.stop()
                rt_1.stop()
                rt_2.stop()
                
                return
           
            
        elif(FSM==3):
            if((ACK=='3')&(chk==check_3)&(seqNo=='3')):
                FSM=4
                base=base+1
                
                print "ACK 3 is received"
                rt_2.stop()
                rt_1.stop()
                rt_3.stop()
                
                
                return
            
                         
        elif(FSM==4):
            if((ACK=='4')&(chk==check_4)&(seqNo=='4')):
                FSM=5
                base=base+1
                
                print "ACK 4 is received"
                rt_2.stop()
                rt_3.stop()
                rt_4.stop()
                
                
                return
                         
        elif(FSM==5):
            if((ACK=='5')&(chk==check_5)&(seqNo=='5')):
                FSM=1
                
                base=base+1
                
                print "ACK 5 is received"
                rt_4.stop()
                rt_3.stop()
                rt_5.stop()
                
               
                return
       

def fg(): #if there is a timeout for any packet we come to this function location
        global FSM
        global pck0_0        #intializing all the variables that is used in this function
        global pck0_1
        global pck1_0
        global pck1_1
        global ACK
        global seqNo
        global ab
        global first_index
        global second_index
        global Refrence
        global FSM
        global z2
        global first_iteration
        global pck2_0
        global pck2_1
        global Nextseq
        global base
        global pck3_0
        global pck3_1
        global flag_1
        global flag_2
        global flag_3
        global flag_4
        global flag_5
        global pck4_0
        global pck4_1
        global pck5_0
        global pck5_1
        global check
        global checksum
        global No_of_kilobytes
        global packets_state
        global flag_1_1
        global flag_2_2
        print"TIMEOUT "
        rt_1.stop()
        rt_2.stop()
        rt_3.stop()
        rt_4.stop()
        rt_5.stop()
        
    
		
        if(FSM==1): #if we lose packet 1 this particular if condition is executed
            rt_1.stop()
            rt_2.stop()
            rt_3.stop()
            rt_4.stop()
            rt_5.stop()
            
            if (packets_state==1):                # if only packet 1 had been transmitted
                pck0_0=pck0_0-5120                #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck0_1=pck0_1-5120
                
                first_index=first_index-1024 
                second_index=second_index-1024
            elif(packets_state==2):                # if both packet1 and packet 2 have been transmitted
                pck0_0=pck0_0-5120                #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck0_1=pck0_1-5120
                pck1_0=pck1_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck1_1=pck1_1-5120
                
               
                first_index=first_index-2048 
                second_index=second_index-2048
            elif(packets_state==3):               #all three packets in the window have been transmitted
                pck0_0=pck0_0-5120                #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck0_1=pck0_1-5120
                pck1_0=pck1_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck1_1=pck1_1-5120
                pck2_0=pck2_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck2_1=pck2_1-5120
               
                first_index=first_index-3072 
                second_index=second_index-3072
                
                
            
            base=6     #resetting the flags 
            Packets()   #calling the packet function again
            return
        elif(FSM==2):      #If we lose packet2 we execute this condition
            rt_1.stop()    #stopping all the timers
            rt_2.stop()
            rt_3.stop()
            rt_4.stop()
            rt_5.stop()
            
          
           
            base=2       #resetting the flags
            Nextseq=2
            flag_2=0
            flag_3=0
            flag_4=0
            flag_5=0
            if (packets_state==2): #if only packet 2 had been transmitted execute this condition
                pck1_0=pck1_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck1_1=pck1_1-5120
                
                first_index=first_index-1024 
                second_index=second_index-1024
            elif(packets_state==3):              #if packet2 and packet3 had been transmitted execute this statement
                
                pck1_0=pck1_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck1_1=pck1_1-5120
                pck2_0=pck2_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck2_1=pck2_1-5120
                
                first_index=first_index-2048 
                second_index=second_index-2048
            elif(packets_state==4):
                pck1_0=pck1_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck1_1=pck1_1-5120
                pck2_0=pck2_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck2_1=pck2_1-5120
                pck3_0=pck3_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck3_1=pck3_1-5120
                first_index=first_index-3072 
                second_index=second_index-3072
            Packets()
            return
        elif(FSM==3):
            rt_1.stop()
            rt_2.stop()
            rt_3.stop()
            rt_4.stop()
            rt_5.stop()
      
            
            base=3
            Nextseq=3
            
            flag_3=0
            flag_4=0
            flag_5=0
            if (packets_state==3):
                
                pck2_0=pck2_0-5120             #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck2_1=pck2_1-5120
                
                first_index=first_index-1024 
                second_index=second_index-1024
            elif(packets_state==4):
                
                pck2_0=pck2_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck2_1=pck2_1-5120
                pck3_0=pck3_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck3_1=pck3_1-5120
                first_index=first_index-2048 
                second_index=second_index-2048
            elif(packets_state==5):
                
                pck2_0=pck2_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck2_1=pck2_1-5120
                pck3_0=pck3_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck3_1=pck3_1-5120
                pck4_0=pck4_0-5120
                pck4_1=pck4_1-5120
                first_index=first_index-3072 
                second_index=second_index-3072
            #No_of_kilobytes=0
            Packets()
            return
        elif(FSM==4):
            rt_1.stop()
            rt_2.stop()
            rt_3.stop()
            rt_4.stop()
            rt_5.stop()
      
         
            base=4
            Nextseq=4
            rt_4.stop()
            flag_4=0
            flag_5=0
            
            if (packets_state==4):
                flag_4=0
                pck3_0=pck3_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck3_1=pck3_1-5120
                first_index=first_index-1024 
                second_index=second_index-1024
            elif(packets_state==5):
                flag_4=0
                flag_5=0
                pck3_0=pck3_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck3_1=pck3_1-5120
                pck4_0=pck4_0-5120
                pck4_1=pck4_1-5120
                first_index=first_index-2048 
                second_index=second_index-2048
            elif(packets_state==1):
                flag_4=0
                flag_5=0
                flag_1_1=0
                pck3_0=pck3_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck3_1=pck3_1-5120
                pck4_0=pck4_0-5120
                pck4_1=pck4_1-5120
                pck0_0=pck0_0-5120                #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck0_1=pck0_1-5120
                first_index=first_index-3072 
                second_index=second_index-3072
            Packets()
            return
        elif(FSM==5):
            rt_1.stop()
            rt_2.stop()
            rt_3.stop()
            rt_4.stop()
            rt_5.stop()
            base=5
            Nextseq=5
            flag_5=0
            
            if (packets_state==5):
                
                pck4_0=pck4_0-5120
                pck4_1=pck4_1-5120
                
                first_index=first_index-1024 
                second_index=second_index-1024
            elif(packets_state==1):
                
                pck4_0=pck4_0-5120
                pck4_1=pck4_1-5120
                pck0_0=pck0_0-5120                #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck0_1=pck0_1-5120
                flag_1_1=0
                
                first_index=first_index-2048 
                second_index=second_index-2048
            elif(packets_state==2):
                flag_1_1=0
                flag_2_2=0
                pck4_0=pck4_0-5120
                pck4_1=pck4_1-5120
                pck0_0=pck0_0-5120                #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck0_1=pck0_1-5120
                pck1_0=pck1_0-5120               #if there was an error in the ACK bit then we are decrementing the indexs and sending back the packets
                pck1_1=pck1_1-5120
                first_index=first_index-3072 
                second_index=second_index-3072
            Packets()
            return

print ("starting...")  #initialising the timers
rt_1=RepeatedTimer(0.3,fg)
rt_1.stop()

rt_2=RepeatedTimer(0.4,fg)
rt_2.stop()

rt_3=RepeatedTimer(0.5,fg)
rt_3.stop()

rt_4=RepeatedTimer(0.6,fg)
rt_4.stop()

rt_5=RepeatedTimer(0.7,fg)
rt_5.stop()

buffer_1=1022
if (selection==2):
    buffer_1=999
    
while(buffer_1):
    Packets()
    buffer_1=buffer_1-1
    Refrence=Refrence+1
    print ''
    print "Refrence "+str(Refrence)
    print"buffer "+ str(buffer_1)

print ""
print"All the Packets have been sent"
rt_1.stop()
rt_2.stop()
rt_3.stop()
rt_4.stop()
rt_5.stop()

fle.close                                               #closing the  file and socket
ClientSocket.close()                                       
        


























        
        
