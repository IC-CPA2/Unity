from socket import *
import socket
#server side  encoding. 
#s = socket(family=AF_INET, type=SOCK_DGRAM)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

s.bind(('0.0.0.0', 12000))
s.listen(1)
try:
    while True:
        conn,address = s.accept()
        # while True:
            
        #     #print("hi")
        #     #bytesAddressPair = s.recvfrom(32)


        content = conn.recvfrom(32)[0]
            #content = bytesAddressPair[0]
            #address = bytesAddressPair[1]
        if len(content) == 0:
            break
            
        else:
            print("debug",content)
  
        #client.sendto(str.encode("hello"), address)
        # conn.sendto(str.encode("hello"),address)
        cmsg = "tryingfrom"
        # cmsg = cmsg.decode();
        conn.send(cmsg.encode())
                
        print("Maintaining connection")
        #s.close()

except KeyboardInterrupt:
    s.shutdown(1)
    print("kwyboRD SHUT")
    s.close()
# import socket
# print("We're in tcp server...");

# #select a server port
# server_port = 12000
# #create a UDP socket
# welcome_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# #bind the server to the localhost at port server_port
# welcome_socket.bind(('localhost',server_port))

# #extra for tcp socket:
# welcome_socket.listen(1)

# #ready message
# print('Server running on port ', server_port)

# #Now the loop that actually listens from clients
# while True:
#     connection_socket, caddr = welcome_socket.accept()
#     #notice recv and send instead of recvto and sendto
#     cmsg = connection_socket.recv(1024)  	
#     cmsg = cmsg.decode()
#     if(cmsg.isalnum() == False):
#         cmsg = "Not alphanumeric.";
#     else:
#         cmsg = "Alphanumeric";
#     connection_socket.send(cmsg.encode())

