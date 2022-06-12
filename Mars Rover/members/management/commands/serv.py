#python3 manage.py serv
#python3 test.py
#python3 dumb_client.py
#python3 manage.py updatemodels
from socket import *
import socket
import numpy as np
import math

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
    curr_sq = "4040"
else:
    query = live_database.objects.get(last_visited=1)
    curr_sq = query.tile_num 

class Command(BaseCommand):
    help = 'import booms'
    def add_arguments(self, parser):
        pass

def mult_mat(angle):#executes matrix multiplicaitons 2x2 by 2x1.
  coords_left = np.array([[-1],
              [2]])
  coords_top = np.array([[0],
                         [2]])
  coords_right = np.array([[1],
               [2]])
  coords_up = np.array([[0],
            [1]])

  print("ANGLE RECEIVED: ",angle)
  deg_to_r = (angle*math.pi)/180
  rotation_mat = np.array([[math.cos(deg_to_r), math.sin(deg_to_r)],[-math.sin(deg_to_r), math.cos(deg_to_r)]])  
  top_lef = np.matmul(rotation_mat,coords_left)
  top_two = np.matmul(rotation_mat,coords_top)
  top_rig = np.matmul(rotation_mat,coords_right)
  one_up = np.matmul(rotation_mat,coords_up)
  return np.concatenate((top_lef,top_two,top_rig,one_up),axis=1)
# need some more base cases for which stuff I need to calculate. 

def square_mapper(curr_coords,angle):
  #curr_coords is likely an string input. 
  all_vals = mult_mat(angle)#multiply matrix and get all combinations
  coords_int = int(curr_coords)
  left_trans = coords_int-(int(round(all_vals[1][0],0)*100))+(int(round(all_vals[0][0],0)))#adds y and x
  centre_top = coords_int-(int(round(all_vals[1][1],0))*100)+(int(round(all_vals[0][1],0)))
  rig_top = coords_int-(int(round(all_vals[1][2],0)*100))+(int(round(all_vals[0][2],0)))
  up_one = coords_int-(int(round(all_vals[1][3],0)*100))+(int(round(all_vals[0][3],0)))
  arr = []
  if left_trans-int(left_trans)<0.5:
    arr.append(str(int(left_trans)))
  else:
    arr.append(str(int(left_trans+1)))
  
  if centre_top-int(centre_top)<0.5:
    arr.append(str(int(centre_top)))
  else:
    arr.append(str(int(centre_top+1)))

  if rig_top-int(rig_top)<0.5:
    arr.append(str(int(rig_top)))
  else:
    arr.append(str(int(rig_top+1)))

  if up_one-int(up_one)<0.5:
    arr.append(str(int(up_one)))
  else:
    arr.append(str(int(up_one+1)))
  # arr.append(str(left_trans))
  # arr.append(str(centre_top))
  # arr.append(str(rig_top))
  # arr.append(str(up_one))
  return arr#gives array of coordinates. 


# def mult_mat(angle):#executes matrix multiplicaitons 2x2 by 2x1.
#   coords_left = np.array([[-1],
#               [2]])
#   coords_top = np.array([[0],
#                          [2]])
#   coords_right = np.array([[1],
#                [2]])
#   coords_up = np.array([[0],
#             [1]])

#   print("ANGLE RECEIVED: ",angle)
#   deg_to_r = (angle*math.pi)/180
#   rotation_mat = np.array([[math.cos(deg_to_r), math.sin(deg_to_r)],[-math.sin(deg_to_r), math.cos(deg_to_r)]])  
#   top_lef = np.matmul(rotation_mat,coords_left)
#   top_two = np.matmul(rotation_mat,coords_top)
#   top_rig = np.matmul(rotation_mat,coords_right)
#   one_up = np.matmul(rotation_mat,coords_up)
#   return np.concatenate((top_lef,top_two,top_rig,one_up),axis=1)
# # need some more base cases for which stuff I need to calculate. 

# def square_mapper(curr_coords,angle):
#   #curr_coords is likely an string input. 
#   all_vals = mult_mat(angle)#multiply matrix and get all combinations
#   coords_int = int(curr_coords)
#   left_trans = coords_int-(int(all_vals[1][0])*100)+(all_vals[0][0])#adds y and x
#   centre_top = coords_int-(int(all_vals[1][1])*100)+(all_vals[0][1])
#   rig_top = coords_int-(int(all_vals[1][2])*100)+(all_vals[0][2])
#   up_one = coords_int-(int(all_vals[1][3])*100)+(all_vals[0][3])
#   arr = []
#   if left_trans-int(left_trans)<0.5:
#     arr.append(str(int(left_trans)))
#   else:
#     arr.append(str(int(left_trans+1)))
  
#   if centre_top-int(centre_top)<0.5:
#     arr.append(str(int(centre_top)))
#   else:
#     arr.append(str(int(centre_top+1)))

#   if rig_top-int(rig_top)<0.5:
#     arr.append(str(int(rig_top)))
#   else:
#     arr.append(str(int(rig_top+1)))

#   if up_one-int(up_one)<0.5:
#     arr.append(str(int(up_one)))
#   else:
#     arr.append(str(int(up_one+1)))
#   # arr.append(str(left_trans))
#   # arr.append(str(centre_top))
#   # arr.append(str(rig_top))
#   # arr.append(str(up_one))
#   return arr#gives array of coordinates. 



# #could have server trying to push to database. 
s.bind(('0.0.0.0', 12000))
s.listen(1)
counter = 0
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
        ang_change = int(f.readline())#reads second line containing angle field. 
        f.close()   
        if counter == 0:
            head_angle = str(ang_change)
            counter+=1
        else:
            head_angle = str((int(head_angle)+ang_change)%360)

        ##TCP protocols to send the values back will be used. 
        print("stuff in database",live_database.objects.all().values())
    
        observed_tile = square_mapper(curr_sq,int(head_angle))
        print(observed_tile)
        cmsg = str(head_angle)
        print("Angle Facing",cmsg)

        #THE HEAD ANGLE SHOULD BE WRITTEN INTO THE TABLE.

        # cmsg = cmsg.decode();
        conn.send(cmsg.encode())                
        # print("Maintaining connection")
        content = conn.recvfrom(32)[0]
        content = content.decode()
        if content=="XX":
            break
        else:
            print("Client Sent",content)
        all_info = content.split(";") ##gives array like ['0,1','PA1','T2','T3','T4']
        coords = all_info[0].split(",") #gives x and y coordinates
        curr_sq = 4040+int(coords[0])+(100*int(coords[1]))
        #send back as current values the X,Y values. 
        temp_dict = {}
        temp_dict[1] = all_info[1][:-1]
        temp_dict[2] = all_info[2][:-1]
        temp_dict[3] = all_info[3][:-1]
        temp_dict[4] = all_info[4][:-1]

        new_squares = square_mapper(curr_sq,int(head_angle))
        ##This obtains the new mapping with a set of arrays for new squares. 
        old_last_sq = live_database.objects.get(last_visited=1)
        old_last_sq.last_visited = 0
        old_last_sq.save()
        new_sq = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
        new_sq.save()#apply the new last square.

        for i in range(0,4):
            #loop from 1-4 inclusive. 
            make_new_tile = live_database(tile_num=new_squares[i],tile_info=temp_dict[i+1],last_visited=0)
            make_new_tile.save()
        
        # if content == "T":
        #     curr_sq = observed_tile
        #     old_last_sq = live_database.objects.get(last_visited=1)
        #     old_last_sq.last_visited = 0
        #     old_last_sq.save()
        #     new_sq = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
        #     new_sq.save()
        # elif content == "PA":
        #     # new_sq = live_database(tile_num=observed_tile,tile_info="PA",last_visited=0)
        #     #cannot visit new thingies. 
        #     new_sq = live_database.objects.filter(tile_num=observed_tile)
        #     if len(new_sq) == 0:
        #         two_sq = live_database(tile_num=observed_tile,tile_info="PA",last_visited=0)
        #         two_sq.save()
        #     else:
        #         new_sq = live_database.objects.get(tile_num=observed_tile)
        #         new_sq.tile_info = "PA"
        #         new_sq.save()

        # elif content == "GA":
        #     new_sq = live_database.objects.filter(tile_num=observed_tile)
        #     #cannot visit new thingies. 
        #     if len(new_sq) == 0:
        #         two_sq = live_database(tile_num=observed_tile,tile_info="GA",last_visited=0)
        #         two_sq.save()
        #     else:
        #         new_sq = live_database.objects.get(tile_num=observed_tile)
        #         new_sq.tile_info = "GA"
        #         new_sq.save()
        # elif content == "BA":
        #     new_sq = live_database.objects.filter(tile_num=observed_tile)
        #     if len(new_sq) == 0:
        #         two_sq = live_database(tile_num=observed_tile,tile_info="BA",last_visited=0)
        #         two_sq.save()
        #     else:
        #         new_sq = live_database.objects.get(tile_num=observed_tile)
        #         new_sq.tile_info = "BA"
        #         new_sq.save()
        #     #cannot visit new thingies. 
        # elif content == "RA":
        #     new_sq = live_database.objects.filter(tile_num=observed_tile)
        #     if len(new_sq) == 0:
        #         two_sq = live_database(tile_num=observed_tile,tile_info="RA",last_visited=0)
        #         two_sq.save()
        #     else:
        #         new_sq = live_database.objects.get(tile_num=observed_tile)
        #         new_sq.tile_info = "RA"
        #         new_sq.save()
        #     #cannot visit new thingies. 
        # elif content == "OA":
        #     new_sq = live_database.objects.filter(tile_num=observed_tile)
        #     #cannot visit new thingies. 
        #     if len(new_sq) == 0:
        #         two_sq = live_database(tile_num=observed_tile,tile_info="OA",last_visited=0)
        #         two_sq.save()
        #     else:
        #         new_sq = live_database.objects.get(tile_num=observed_tile)
        #         new_sq.tile_info = "OA"
        #         new_sq.save()
        # elif content == "HHH":
        #     break
        
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




# def choose_next(ang_inp,curr_sq):
#     ##assuming 9x9 grid starting with index 0. 
#     #should work provide values 
#     """
#     0 1 2 3 4 5 6 7 8
#     | | | | | | | | | 0
#     | | | | | | | | | 1
#     | | | | | | | | | 2
#     | | | | | | | | | 3
#     """
#     new_val = int(curr_sq)
#     new_val = large_grid(ang_inp,new_val)
#     return str(new_val)
#     #based on existing head angles decide how to maneuveur rover. 


# def large_grid(angle,curr_tile):
#   #takes in a 4 string input. The maximum map size assuming rover is 15cm 
#   #We know this to be a total of about 15m which should be more than adequate.
#   #scale factors can be adjusted accordingly. New maps and terrains can be handled thereafter. 
#       new_tile = curr_tile
#       if angle == "0" and curr_tile>=1100:
#         #movement condition. 
#         new_tile -= 100
#       elif angle == "45" and curr_tile%100<69 and curr_tile>=1110:
#           new_tile -= 99
#       elif angle == "90":
#         if curr_tile %100<69:
#           new_tile = curr_tile+1
#         else:
#           new_tile = curr_tile
#       elif angle == "135" and new_tile <6900 and new_tile%100<69:
#         new_tile = curr_tile+101
#       elif angle == "180" and new_tile<6900:
#         new_tile = curr_tile+100
#       elif angle == "225":
#       #complete if conditions
#         new_tile = curr_tile+99
#       elif angle == "270" and curr_tile%100>11:
#         new_tile -= 1        
#       elif angle == "315":
#         new_tile -= 101
#       else:
#         new_tile = curr_tile
#       return str(new_tile)



