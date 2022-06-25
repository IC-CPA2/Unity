from re import A
from socket import *
import socket
import time
import math 

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 12000))
s.listen(1)

def ex_ang():
    curr_dir = os.getcwd()
    serv_path = curr_dir+"\\on_serv.txt"
    serv_path = serv_path.replace("\\","/")
    f = open (serv_path, "r")
    line = f.readline()
    splitstr = line.split(";")
    return splitstr

memo = {'bd':'W','r':'RA','y':'YA','p':'PA','b':'BA','dg':'DGA','lg':'GA','nb':'TA'}
curr_time = time.time()
while True:
    conn,address = s.accept()
    content = conn.recvfrom(100)[0]
    if len(content) == 0:
        print("empty")
    else:
        print("received content",content)
    msg = content.decode('UTF-8')
    info = msg.split(";")
    f = open("on_serv.txt","w")
    if(len(info)==6):
        f.write(str(int(float(info[3])))+','+"T1;"+str(int(float(info[4])))+";T2;T3;"+memo[info[0]]+"x;"+str(int(float(info[5]))))
    else:

        f.write("0,0;U1;U2;U3;U4;0")
    f.close()
    while True:
        x = "A"
        conn.send(x.encode())
        time.sleep(1)
        break






