import socket
print("We're in tcp client...");

#the server name and port client wishes to access
server_name = 'localhost'
server_port = 12000
#create a TCP client socket
#Set up a TCP connection
#connection_socket will be assigned to this client on the server side

#some work

#send the message  to the udp server
while True:
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((server_name, server_port))



    msg = input("Enter a string to test if it is alphanumeric: ");

    client_socket.send(msg.encode())

    #return values from the server
    msg = client_socket.recv(1024)
    print("Client Received",msg.decode())
    client_socket.close()
    
##next steps. Command server will send instructions. It does this by reading the text files. 

#messages PA(Pink Alien, curr_cord),T(Next-coord terrain),GA(Green Alien curr-coord)