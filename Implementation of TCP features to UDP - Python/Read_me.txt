

Chethan Nagesh Pal (ChethanNagesh_Pal@student.uml.edu)

	


File:
server.py:source file of server
client.py:source file of client
ADDITION.py:addition api source file 

steps

A.Double click on the sourece file ,it will open black window choose your option of recover or non recovery on both server and client, run the program 
B.if you scroll through the window ,you can see all the 4 errors that we created and how the system is recovering .we are able to recover from almost all the error.
C.In this current code that we have submitted has 1% error of all 4 types of error
D.In Non-Recovery ,whenever there is a ACK error on client end .Client is retransmitting ,but server is ignoring and moving forward.Not waitng for the right packet to arrive 
  D.1 Hence when transaction complete you could see ,server has got only 478KB but transmitted is 501KB
1.Please run in Python version:2.7.15.
2.Once the connection is enabled .if you want to re-run the program ,please change the port number to avoid error no:10048]  
3.Please run the server source code first ,and then run the client code
4.Sneding Image "image_t" should be in the same directory of the source file
5.Please change the "image_received_38" to diffrent name(as shown in line below)in server source code .if you want to run the program again ,because once a file is created with some name ,it cannnot be created with the same name again
  

6.Once you make necsesary changes in the code,press F5 or run in IDE

7.Refrence number of both server and client are independent of each other