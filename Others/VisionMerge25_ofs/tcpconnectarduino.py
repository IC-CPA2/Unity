from re import A
from socket import *
import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 12001))
s.listen(1)

memo = {'bd': 'W', 'r': 'RA', 'y': 'YA', 'p': 'PA',
        'b': 'BA', 'dg': 'DGA', 'lg': 'LGA', 'nb': 'TA'}

arr = []

while True:
    conn, address = s.accept()
    content = conn.recvfrom(100)[0]
    if len(content) == 0:
        print("empty")
    else:
        print(content)
    msg = content.decode('UTF-8')
    info = msg.split(";")
    f = open("on_serv.txt", "w")
    if(len(info) == 6):
        f.write(str(int(float(info[3])))+','+str(int(float(info[4])//7)) +
                ";T1;T2;T3;"+memo[info[0]]+";"+str(int(float(info[5]))))
        arr.append(str(int(float(info[3])))+','+str(int(float(info[4])//7)) +
                   ";T1;T2;T3;"+memo[info[0]]+";"+str(int(float(info[5]))))
    f.close()

    while True:
        x = input("Enter A to Move Rover: ")
        conn.send(x.encode())
        time.sleep(1)
        break
