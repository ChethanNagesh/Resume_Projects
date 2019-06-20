import socket
import io
import os
import sys
from ADDITION import add
       
ServerName = "localhost"              #Server name ,since we are receiving  on same ip ,we are just filling with Localhost charachter
PortNumber = 5007                    #assinginng the port number 
Refrence=0;                           #intializing the refrence variable
ServerSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)#assinging the address family (IPV4) and Socket type(UDP)
ServerSocket.bind((ServerName,PortNumber))#binding the server socket
selection=input("choose your option :1 for recovery,2 for non-recovery ")
print "Server ready to receive"
buffer = bytearray()
fle = open("image_received_36.jpg","wb")
start=0
packet_size = 1041                       #includes data, checksum, seqno.

FSM_r=1                                  #initialiazing the FSM state 
err=1
packetdata=0
err_1=100



def datasum(data):                        #Calculating sum all th data that we reecived in a packet 
    buffer = bytearray(data)              #typecasting all the data into array
    length = len(buffer)
    first_index = 0                       #initializing all the varaible 
    second_index = 2
    iteration = 0
    sum_all_Bites = 0
    sum_all_Bites = bin(sum_all_Bites)
    
    checksum = 0
    checksum = bin(checksum)
    while(iteration<=510):                       # looping it 512 times ,we are fetching 1024 bytes of data in this loop in 50%in data1 and 50 % in data2,2 extra becasue of all the header in it 
        data1 = buffer[first_index:second_index]  #fetches 16 bits at a time
        two_bytes = ''.join(format(x, 'b').zfill(8) for x in bytearray(data1))#joining all the bytes 
        first_index = first_index+2              #incrementing it by 2 position two fecth next 16 bits
        second_index = second_index+2
        
        data2 = buffer[first_index:second_index]
        two_bytes_2 = ''.join(format(y, 'b').zfill(8) for y in bytearray(data2))
        sum_one_bites = add(two_bytes, two_bytes_2)
        first_index = first_index+2             #incrementing it by 2 position two fecth next 16 bits
        second_index = second_index+2
        
        
        sum_all_Bites = add(sum_one_bites, sum_all_Bites)   #Sum of data only
        iteration = iteration+2
    return sum_all_Bites




'''Function to Extract packet from sender and again sending it with ACK and checksum'''
def extracting():
    global packetdata                        #telling the compiler that these varibale as been defined in global section
    global FSM_r
    global Refrence
    global No_of_kilobytes
    global selection
    global err
    global start
    global err_1
    packetdata,clientAddress = ServerSocket.recvfrom(packet_size)#waitng for the packet from the sender end 
    
    #print clientAddress
    data = packetdata[0:1024]               #pasring the data of 1 KB
    Chk = packetdata[1024:1040]             #parsing the checksum
    Refrence=Refrence+1                      
    print ""
    print Refrence
    #data loss error
    if (Refrence==err_1<101):
        err_1=err_1+1
        data=packetdata[0:1022] 
        print "data loss error"
        
     
          
        
    datasumvalue = datasum(data)            #sending it to the function to calculate all the sum of the data that we received 
    received_packet = add(Chk, datasumvalue)#adding the checksum we received and sum of all data to get "1111111111111111"
    received_packet = str(received_packet)
    SeqNo = packetdata[1040:1041]#parsing the seqno
    #data bit error
    if (Refrence==err and err<2):
        err=err+1
        SeqNo=3
        print "data bit error"
    
   
    if(FSM_r==1):                                #checking whether we recieved packet 1
        if((SeqNo=='1') & (received_packet=='1111111111111111')):
            print "PACKET [1] is RECIEVED"
            print "CHECKSUM  = " + Chk
            print "LOCAL SUM OF DATA= " + datasumvalue
            print "Sum of CHK and LOCAL = " + received_packet
            fle.write(data)                                        #writting the data to the file 
            backpacket = ['1', '1', Chk]                           #telling the client we received right data which had seq 0,and cheksum you sent
            backpacket = ''.join(backpacket)
            FSM_r=2                                                #telling the FSM to jump to next level FSM 2
            print "Sending ACK 1"                                      
            ServerSocket.sendto(backpacket, clientAddress)         #send the packet to client 
            return
        
        else:                                           #if our selction is non recovery, jump to next state ignoring missing packets
            if (selection==2):
                FSM_r=2
                backpacket = ['1', '1', Chk]                           #telling the client we received right data which had seq 0,and cheksum you sent
                backpacket = ''.join(backpacket)
                print"Im ignoring errors "
                ServerSocket.sendto(backpacket, clientAddress)
                return
            if (SeqNo=='2' or SeqNo=='3'):               #correcting its FSM, go back to the beginning of the window
                FSM_r=1
            elif(SeqNo=='5'):
                FSM_r=5
            elif(SeqNo=='4'):                            #go back to the right FSM and wait for the right packet ot arrive
                FSM_r=4
            else:
                FSM_r=1                                 #stay in FSM 1
            print"received_packet"+str(received_packet)
                                                             #telling the FSM to stay in same level of FSM1
            No_of_kilobytes=No_of_kilobytes+1                        #since we asking the  client to send the data again ,we are incrementing the No_of Kilobytes
            print"Im here because of wrong ACK 1 bit recived by Sender "
            backpacket = ['5', '1', Chk]                             #telling client we recived a seq 1 instead of seq 0
            backpacket = ''.join(backpacket)
            ServerSocket.sendto(backpacket, clientAddress)
            return
    
   
    elif(FSM_r==2):                 #checking whether we recieved packet 1
        print received_packet
        if((SeqNo=='2') & (received_packet=='1111111111111111')):
            print "PACKET [2] is RECIEVED"
            print "CHECKSUM  = " + Chk
            print "LOCAL SUM OF DATA=" + datasumvalue
            print "Sum of CHK and LOCAL = " + received_packet
            fle.write(data)                                         #writting the data to the file 
            backpacket = ['2', '2', Chk]                            #telling the client we received right data which had seq 0,and cheksum you sent
            backpacket = ''.join(backpacket)
            print "Sending ACK 2"
            FSM_r=3                                                 #telling the FSM to jump to next level FSM 1
            ServerSocket.sendto(backpacket, clientAddress)
            return
        else:
            if (selection==2):                                  #if our selction is non recovery, jump to next state ignoring missing packets
                FSM_r=3
                backpacket = ['2', '2', Chk]                           #telling the client we received right data which had seq 0,and cheksum you sent
                backpacket = ''.join(backpacket)
                print"Im ignoring errors "
                ServerSocket.sendto(backpacket, clientAddress)
                return
            if (SeqNo=='3' or SeqNo=='4'):                      #correcting its FSM, go back to the beginning of the window
                FSM_r=2
            elif(SeqNo=='1'):
                FSM_r=1
            elif(SeqNo=='5'):                                   #go back to the right FSM and wait for the right packet ot arrive
                FMS_r=5
            else:
                FSM_r=2                                         #stay in FSM 2
            print"received_packet"+str(received_packet)
                                                           #telling the FSM to stay in same level of FSM1
            No_of_kilobytes=No_of_kilobytes+1                        #since we asking the  client to send the data again ,we are incrementing the No_of Kilobytes
            print"Im sending duplicate ACK 2 bit recived by Sender "
            backpacket = ['1', '2', Chk]                             #telling client we recived a seq 1 instead of seq 0
            backpacket = ''.join(backpacket)
            ServerSocket.sendto(backpacket, clientAddress)
            return
    
            
    elif(FSM_r==3):                                                 #checking whether we recieved packet
        if((SeqNo=='3') & (received_packet=='1111111111111111')):
            print "PACKET [3] is RECIEVED"
            print "CHECKSUM  = " + Chk
            print "LOCAL SUM OF DATA=" + datasumvalue
            print "Sum of CHK and LOCAL = " + received_packet
            fle.write(data)                                         #writting the data to the file 
            backpacket = ['3', '3', Chk]                            #telling the client we received right data which had seq 0,and cheksum you sent
            backpacket = ''.join(backpacket)
            print "Sending ACK 3"
            FSM_r=4                                                 #telling the FSM to jump to next level FSM 1
            ServerSocket.sendto(backpacket, clientAddress)
            return
        else:
            if (selection==2):                                      #if our selction is non recovery, jump to next state ignoring missing packets
                FSM_r=4
                backpacket = ['3', '3', Chk]                           #telling the client we received right data which had seq 0,and cheksum you sent
                backpacket = ''.join(backpacket)
                print"Im ignoring errors "
                ServerSocket.sendto(backpacket, clientAddress)
                return
            if (SeqNo=='4' or SeqNo=='5'):                          #correcting its FSM, go back to the beginning of the window
                FSM_r=3
            elif(SeqNo=='2'):
                FSM_r=2
            elif(SeqNo=='1'):                                       #go back to the right FSM and wait for the right packet ot arrive
                FSM_r=1
            else:
                FSM_r=3                                             #stay in FSM 3
            print"received_packet"+str(received_packet)
                                                             #telling the FSM to stay in same level of FSM1
            No_of_kilobytes=No_of_kilobytes+1                        #since we asking the  client to send the data again ,we are incrementing the No_of Kilobytes
            print"Im sending duplicate ACk 3 bit recived by Sender "
            backpacket = ['2', '3', Chk]                             #telling client we recived a seq 1 instead of seq 0
            backpacket = ''.join(backpacket)
            ServerSocket.sendto(backpacket, clientAddress)
            return
    
        
    elif(FSM_r==4):                                                 #checking whether we recieved packet
        print"received_packet"+str(received_packet)
        if((SeqNo=='4') & (received_packet=='1111111111111111')):
            print "PACKET [4] is RECIEVED"
            print "CHECKSUM  = " + Chk
            print "LOCAL SUM OF DATA=" + datasumvalue
            print "Sum of CHK and LOCAL = " + received_packet
            fle.write(data)                                         #writting the data to the file 
            backpacket = ['4', '4', Chk]                            #telling the client we received right data which had seq 0,and cheksum you sent
            backpacket = ''.join(backpacket)
            print "Sending ACK 4"
            FSM_r=5                                                 #telling the FSM to jump to next level FSM 1
            ServerSocket.sendto(backpacket, clientAddress)
            return
        else:
            if (selection==2):                                      #if our selction is non recovery, jump to next state ignoring missing packets
                FSM_r=5
                backpacket = ['4', '4', Chk]                           #telling the client we received right data which had seq 0,and cheksum you sent
                backpacket = ''.join(backpacket)
                print"Im ignoring errors "
                ServerSocket.sendto(backpacket, clientAddress)
                return
            if (SeqNo=='5' or SeqNo=='1'):                          #correcting its FSM, go back to the beginning of the window
                FSM_r=4
            elif(SeqNo=='3'):
                FSM_r=3
            elif(SeqNo=='2'):                                       #go back to the right FSM and wait for the right packet ot arrive
                FSM_r=2
            else:
                FSM_r=4                                                 #telling the FSM to stay in same level of FSM4
            No_of_kilobytes=No_of_kilobytes+1
            print "CHECKSUM  = " + Chk
            print"Im here because of wrong ACK 4 bit recived by Sender "
            backpacket = ['3', '4', Chk]                             #telling client we recived a seq 1 instead of seq 0
            backpacket = ''.join(backpacket)
            ServerSocket.sendto(backpacket, clientAddress)
            return
    
    elif(FSM_r==5):                                                 #checking whether we recieved packet
        if((SeqNo=='5') & (received_packet=='1111111111111111')):
            print "PACKET [5] is RECIEVED"
            print "CHECKSUM  = " + Chk
            print "LOCAL SUM OF DATA=" + datasumvalue
            print "Sum of CHK and LOCAL = " + received_packet
            fle.write(data)                                         #writting the data to the file 
            backpacket = ['5', '5', Chk]                            #telling the client we received right data which had seq 0,and cheksum you sent
            backpacket = ''.join(backpacket)
            print "Sending ACK 5"
            FSM_r=1                                                #telling the FSM to jump to next level FSM 1
            ServerSocket.sendto(backpacket, clientAddress)
            return
        else:
            if (selection==2):                                      #if our selction is non recovery, jump to next state ignoring missing packets
                FSM_r=1
                backpacket = ['5', '5', Chk]                           #telling the client we received right data which had seq 0,and cheksum you sent
                backpacket = ''.join(backpacket)
                print"Im ignoring errors "
                ServerSocket.sendto(backpacket, clientAddress)
                return
            if (SeqNo=='1' or SeqNo=='2' ):                          #correcting its FSM, go back to the beginning of the window
                FSM_r=5
            elif(SeqNo=='4'):
                FSM_r=4
            elif(SeqNo=='3'):                                       #go back to the right FSM and wait for the right packet ot arrive
                FSM_r=3
          
            else:
                FSM_r=5                                                 #telling the FSM to stay in same level of FSM
            No_of_kilobytes=No_of_kilobytes+1                        #since we asking the  client to send the data again ,we are incrementing the No_of Kilobytes
            print"Im here because of wrong ACK 5 bit recived by Sender "
            backpacket = ['4', '5', Chk]                             #telling client we recived a seq 1 instead of seq 0
            backpacket = ''.join(backpacket)
            ServerSocket.sendto(backpacket, clientAddress)
            return
        
   
if(selection==2):
    print "Non-Recovery"
    No_of_kilobytes=530
No_of_kilobytes =501 
while(No_of_kilobytes):
    No_of_kilobytes=No_of_kilobytes-1
    extracting()                                                  #extractig the packets
    #No_of_kilobytes=No_of_kilobytes-1
    print"No_of_kilobytes "+ str(No_of_kilobytes)
   
       

fle.close()#closing the  file
serverSocket.close()
print "File Downloaded successfully"

