import os
import math
import time 

def get_file():
    curr_dir = os.getcwd()
    serv_path = curr_dir+"\\on_serv.txt"
    serv_path = serv_path.replace("\\","/")
    return serv_path

    
def ex_ang():
    get_path = get_file()
    f = open (get_path, "r")
    line = f.readline()
    splitstr = line.split(";")
    return splitstr

def retu_home():
    time.sleep(7.5)#sleep 4 secs.                   
    #extracts the current angle. 
    #we will ha+ve an approximation algorithm.        
    file_vals = ex_ang()

    angle = int(file_vals[5])
    coords = file_vals[0].split(",")
    xcoord = int(coords[0])
    ycoord = int(coords[1])

    if (xcoord<25 and xcoord>-25) and (ycoord<25 and ycoord>-25):
        new_y = round(ycoord-math.cos(angle/57.296))
        new_x = round(xcoord+math.sin(angle/57.296))
        open_text = get_file()
        file_op = open(open_text,"w")
        file_op.write(str(new_x)+","+str(new_y)+";"+file_vals[1]+";"
        +file_vals[2]+";"+file_vals[3]+";"+file_vals[4]+";"+file_vals[5])
        file_op.close() #write new values to files.
    pass

while True:
    f = open(get_file(),'r')
    line = f.readline()

    if (os.stat(get_file()).st_size == 0):
        f = open(get_file(),'w')
        f.write("0,0;U1;U2;U3;U4;180")
    # string = ex_ang()
    # angle = string[5]
    # first_angle = angle


    else:
        retu_home()
