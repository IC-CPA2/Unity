#python3 manage.py serv
#python3 test.py
#python3 dumb_client.py
#python3 manage.py updatemodels

import socket
import os
import time
print("We're in tcp client...");

#the server name and port client wishes to access
server_name = 'localhost'
server_port = 12000


def char_to_ang(msg_inp):
  if msg_inp == "f":
    msg_inp = "0"
  elif msg_inp == "d":
    msg_inp = "45"
  elif msg_inp == "r":
    msg_inp = "90"
  elif msg_inp == "a":
    msg_inp = "135"
  elif msg_inp == "b":
    msg_inp = "180"
  elif msg_inp == "x":
    msg_inp = "225"
  elif msg_inp == "l":
    msg_inp == "270"
  elif msg_inp == "y":
    msg_inp = "315"
  return str(msg_inp)




#send the message  to the udp server
while True:
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((server_name, server_port))

    new_msg = input("Rover Sends Coordinates: ")

    client_socket.send(new_msg.encode())
    msg = client_socket.recv(1024)
    msg = msg.decode()
    msg = char_to_ang(msg)
    print("Client angle change received : ",msg)

    client_socket.close()


    inp_param = input("<Receive Values from Field of View>")




