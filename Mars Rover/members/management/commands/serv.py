#python3 manage.py serv
#python3 test.py
#python3 dumb_client.py
#python3 manage.py updatemodels
from socket import *
import socket
from xml.dom.expatbuilder import parseString
#server side  encoding. 
#this acts as a persistent TCP. 
#s = socket(family=AF_INET, type=SOCK_DGRAM)
from members.models import map_info,all_info,live_database
import os

#coordinates red as RENDERED. Reads Y coordinates before X coordinates. 
from django.core.management.base import BaseCommand
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
if len(live_database.objects.all()) == 0:
    curr_sq = "33"
else:
    query = live_database.objects.get(last_visited=1)
    curr_sq = query.tile_num 
class Command(BaseCommand):
    help = 'import booms'
    def add_arguments(self, parser):
        pass

def choose_next(ang_inp,curr_sq):
    ##assuming 9x9 grid starting with index 0. 
    #should work provide values 
    """
    0 1 2 3 4 5 6 7 8
    | | | | | | | | | 0
    | | | | | | | | | 1
    | | | | | | | | | 2
    | | | | | | | | | 3
    """
    end_x = 8
    end_y = 8 
    # end_coords = int(str(end_x)+str(end_y))
    #temproary fix
    new_val = int(curr_sq)
    if ang_inp == "0" and (new_val)>=10:
        #movement condition. 
        # new_val -= 1
        new_val -= 10
    if ang_inp == "45" and (new_val>end_x) and (new_val%10)<end_x:
        # new_val += 9
        new_val -= 9
    elif ang_inp == "90" and (new_val%10)<end_x:
        # new_val += 10
        new_val += 1

    elif ang_inp == "135" and (new_val%10)<end_x and(new_val)<end_y*10:
        # new_val += 11
        new_val += 11

    elif ang_inp == "180" and (new_val<end_y*10):
        # new_val += 1
        new_val += 10
    elif ang_inp == "225" and (new_val%10)>0 and (new_val<end_y*10):#complete if conditions
        # new_val -= 9
        new_val += 9

    elif ang_inp == "270" and (new_val%10>0):
        # new_val -= 10
        new_val -= 1

    elif ang_inp == "315" and (new_val>=end_x) and (new_val%10)>=1:
        # new_val -= 11 
        new_val -= 11
    else:
        print("No Operation/ Special Condition.")

    return str(new_val)
    #based on existing head angles decide how to maneuveur rover. 

# #could have server trying to push to database. 
s.bind(('0.0.0.0', 12000))
s.listen(1)
flg_msg = 0
try:
    while True:
        print("rendering database with values: ",live_database.objects.all().values(),"\n curr sq is: ",curr_sq)
        conn,address = s.accept()   
        iterator = len(live_database.objects.all())

        if iterator == 0:
            insert_vals = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
            insert_vals.save()
        curr_dir = os.getcwd()
        file_path = curr_dir+"\\blog\\text_files\\distance.txt"
        file_path = file_path.replace("\\","/")
        f = open(file_path,"r")
        #reading angle changes. 
        # dist = int(f.readline())
        print("reading angle to send to CONTROL")
        head_angle = int(f.readline())#reads second line containing angle field. 
        f.close()
        head_angle = str(head_angle)

        ##TCP protocols to send the values back will be used. 
        print("stuff in database",live_database.objects.all().values())
        observed_tile = choose_next(head_angle,curr_sq)
        print("Sending this Message: ",observed_tile)
        cmsg = str(head_angle+","+curr_sq+","+observed_tile)
        #THE HEAD ANGLE SHOULD BE WRITTEN INTO THE TABLE.

        # cmsg = cmsg.decode();
        conn.send(cmsg.encode())                
        # print("Maintaining connection")
        content = conn.recvfrom(32)[0]
        content = content.decode()
        if len(content) >= 4:
            break
        else:
            print("Client Sent",content)
        if content == "T":
            curr_sq = observed_tile
            old_last_sq = live_database.objects.get(last_visited=1)
            old_last_sq.last_visited = 0
            old_last_sq.save()
            new_sq = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
            new_sq.save()
        elif content == "PA":
            new_sq = live_database(tile_num=observed_tile,tile_info="PA",last_visited=0)
            #cannot visit new thingies. 
            new_sq.save()
        elif content == "GA":
            new_sq = live_database(tile_num=observed_tile,tile_info="GA",last_visited=0)
            #cannot visit new thingies. 
            new_sq.save()
        elif content == "BA":
            new_sq = live_database(tile_num=observed_tile,tile_info="BA",last_visited=0)
            #cannot visit new thingies. 
            new_sq.save()
        elif content == "RA":
            new_sq = live_database(tile_num=observed_tile,tile_info="RA",last_visited=0)
            #cannot visit new thingies. 
            new_sq.save()
        elif content == "OA":
            new_sq = live_database(tile_num=observed_tile,tile_info="OA",last_visited=0)
            #cannot visit new thingies. 
            new_sq.save()
        elif content == "HHH":
            break
        
        print("Change Angle Etc. For Next Cycle")


       #s.close()
except KeyboardInterrupt:
    s.shutdown(1)
    print("keyboRD SHUT")
    s.close()


def basic_query():
    print(all_info.objects.all())
basic_query()
#Server-Side Experiences: Store entries in database. Also maintain dictionary of visited unvisited. 
#Render the image.txt file accordingly. 






