# 3;0.96;1;0;1>


from socket import *
import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 12000))
s.listen(1)

while True:
    conn, address = s.accept()
    content = conn.recvfrom(100)[0]
    if len(content) == 0:
        break

    else:
        print(content)
    #f = open("t.txt","r")
    x = ""
    while x != "a":
        #file = open("t.txt","r")
        #x = file.read()
        x = input("Enter: ")
        conn.send(x.encode())
        x = "a"
        conn.send(x.encode())
        time.sleep(1)
