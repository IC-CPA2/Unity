#python3 manage.py serv
#python3 test.py
#python3 dumb_client.py
#python3 manage.py updatemodels

import socket
import time
print("We're in tcp client...");

#the server name and port client wishes to access
server_name = 'localhost'
server_port = 12000
#some work

#send the message  to the udp server
while True:
    f = open("angles.txt","r")
    for x in f:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((server_name, server_port))
        msg = client_socket.recv(1024)
        print("Client Received Angle :",msg.decode())
        time.sleep(1.1)
        msg = str(x)
        client_socket.send(msg.encode())
        client_socket.close()



    #return values from the server
    
##next steps. Command server will send instructions. It does this by reading the text files. 

#messages PA(Pink Alien, curr_cord),T(Next-coord terrain),GA(Green Alien curr-coord)




#python3 manage.py serv
#python3 test.py
#python3 dumb_client.py
#python3 manage.py updatemodels