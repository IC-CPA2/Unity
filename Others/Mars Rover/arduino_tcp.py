from re import A
from socket import *
import socket
import time
import math 
import os

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

def angleAdjust(angle):
  if angle % 90 == 0:
    return angle
  else:
    modul = angle % 90
    if modul <= 45:
      return angle - modul
    else:
      oppmod = 90 - modul
      return angle + oppmod

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
    f = open("on_serv.txt","r")
    prev_x = float((f.read().split(";")[0]).split(",")[0])
    f = open("on_serv.txt","w")
    if(len(info)==6):
        f.write(str(-int(float(info[4])))+','+str(int(float(info[3])))+";T1;T2;T3;"+memo[info[0]]+"x;"+str(angleAdjust(int(float(info[5])))))

        # if int(float(info[5]))==0:
        #     if(int(float(info[3]))>prev_x):
        #         f.write(str(int(float(info[3])))+','+str(int(float(info[4])))+";"+memo[info[0]]+"x;T2;T3;T4;"+str(angleAdjust(int(float(info[5])))))
        #     else:
        #         f.write(str(int(float(info[3])))+','+str(int(float(info[4])))+";T1;T2;"+memo[info[0]]+"x;T4;"+str(angleAdjust(int(float(info[5])))))
        # else:
        #     if(int(float(info[3]))>prev_x):
        #         f.write(str(int(float(info[3])))+','+str(int(float(info[4])))+";T1;T2;"+memo[info[0]]+"x;T4;"+str(angleAdjust(int(float(info[5])))))
            # else:
            #     f.write(str(int(float(info[3])))+','+str(int(float(info[4])))+";"+memo[info[0]]+"x;T2;T3;T4;"+str(angleAdjust(int(float(info[5])))))
    elif (len(info)==4):
        f.write(str(-int(float(info[2])))+","+str(int(float(info[1])))+";T1;T2;T3;T4;"+str(angleAdjust(int(float(info[3])))))

        # f.write("0,0;U1;U2;U3;U4;0")
    f.close()
    
    while True:
        x = "A"
        conn.send(x.encode())
        time.sleep(1)
        break






