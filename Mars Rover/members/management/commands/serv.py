#python3 manage.py serv
#python3 test.py
#python3 dumb_client.py
#python3 manage.py updatemodels
from hashlib import new
from socket import *
import socket
import numpy as np
import math
import time
import os

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

def storer():
  color = ['PA','OA','RA','GA','YA','DGA','DBA','BA','W']
  alien_storer = {}
  for i in color:
    sel = live_database.objects.filter(tile_info=i)
    alien_storer[i]=len(sel)
    if i == 'W':
      alien_storer[i] = 0

  return alien_storer


def square_mapper(curr_coords,angle):
  #curr_coords is likely an string input. 
  all_vals = mult_mat(angle)#multiply matrix and get all combinations
  #this does a matrix multiplication to get all angles.
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

def ang_to_char(msg_inp):
  if msg_inp == "0":
    msg_inp = "f"
  elif msg_inp == "45":
    msg_inp = "d"
  elif msg_inp == "90":
    msg_inp = "r"
  elif msg_inp == "135":
    msg_inp = "a"
  elif msg_inp == "180":
    msg_inp = "b"
  elif msg_inp == "225":
    msg_inp = "x"
  elif msg_inp == "270":
    msg_inp == "l"
  elif msg_inp == "315":
    msg_inp = "y"
  return msg_inp



# #could have server trying to push to database. 
s.bind(('0.0.0.0', 12000))
s.listen(1)
counter = 0

# test_funct = input("please input operation mode")
sel_work_dir = os.getcwd()
file_acc_path = sel_work_dir+"\\blog\\text_files\\mode.txt"
file_acc_path = file_acc_path.replace("\\","/")
my_f = open(file_acc_path,"r")
test_funct = str(my_f.readline())
test_funct.replace(" ","")#replace whitespace with nothing
deb_co = 0
try:
    while True:
        # print("rendering database with values: ",live_database.objects.all().values(),"\n curr sq is: ",curr_sq)
        conn,address = s.accept()   
        iterator = len(live_database.objects.all())
        alien_storer = {}
        # sel_pink = live_database.objects.filter(tile_info="PA")
        # sel_blue = live_database.objects.filter(tile_info="BA")
        # sel_red = live_database.objects.filter(tile_info="RA")
        # sel_green = live_database.objects.filter(tile_info="GA")
        # sel_orange = live_database.objects.filter(tile_info="OA")
        # sel_yellow = live_database.objects.filter(tile_info="YA")
        # sel_darkblue = live_database.objects.filter(tile_info="DBA")
        # sel_darkgreen = live_database.objects.filter(tile_info="DGA")
        # sel_wall = live_database.objects.filter(tile_info="W")
        # alien_storer['PA']=len(sel_pink)
        # alien_storer['OA']=len(sel_orange)
        # alien_storer['RA']=len(sel_red)
        # alien_storer['GA']=len(sel_green)
        # alien_storer['YA']=len(sel_yellow)
        # alien_storer['DGA']=len(sel_darkgreen)
        # alien_storer['DBA']=len(sel_darkblue)
        # alien_storer['BA']=len(sel_blue) 
        # alien_storer['W']=0# we select aliens. 

        alien_storer = storer()

        print("Aliens by Colour:",alien_storer)   

        if iterator == 0:
          insert_vals = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
          insert_vals.save()
        if test_funct == "M":
          curr_dir = os.getcwd()
          file_path = curr_dir+"\\blog\\text_files\\direction.txt"
          file_path = file_path.replace("\\","/")

          f_op2 = open(file_path,"r")
          ang_change = int(f_op2.readline())#reads line containing angle field.
          f_op2.close()
          f_op2 = open(file_path,"w")
          f_op2.write("0")
          f_op2.close()
          # ang_change = input("enter angle change lol: ") 
          # if str(ang_change)!=0:
          #   f.write(ang_change)



          # if counter == 0:
          #     head_angle = str(ang_change)
          #     counter+=1
          # else:
          #     ang_change = int(ang_change)
          #     head_angle = str((int(head_angle)+ang_change)%360)      
          cmsg = str(ang_change) #sending angle to server
          cmsg = ang_to_char(cmsg) #convert from an angle to character.



          print("Angle Facing",cmsg)

          conn.send(cmsg.encode())   
          # print("Maintaining connection")
          content = conn.recvfrom(32)[0]
          content = content.decode()
          if content=="XX":
              print("if condition entered",content)
              break
          else:
              print("Client Sent",content)
          all_info = content.split(";") ##gives array like ['0,1';'PA1';'T2';'T3';'T4']
          coords = all_info[0].split(",") #gives x and y coordinates
          curr_sq = 4040+int(coords[0])+(100*int(coords[1]))
          #send back as current values the X,Y values. 
          temp_dict = {}
          temp_dict[1] = all_info[1][:-1]
          temp_dict[2] = all_info[2][:-1]
          temp_dict[3] = all_info[3][:-1]
          temp_dict[4] = all_info[4][:-1]
          temp_dict[5] = all_info[5]
          head_ang = temp_dict[5]
          observed_tile = square_mapper(curr_sq,int(head_ang))
  
          f = open(file_path,"w")
          f.write(head_ang) #overwrite to 0 as a result. 
          f.close()           


          print("debugging",temp_dict)

          new_squares = square_mapper(curr_sq,int(head_angle))
          old_last_sq = live_database.objects.get(last_visited=1)
          old_last_sq.last_visited = 0
          old_last_sq.save()
          checker = live_database.objects.filter(tile_num=curr_sq)
          if len(checker) == 0:# to restore the current squares and rendering based on database values. 
            new_sq = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
            new_sq.save()#apply the new last square.
          else:
            sel_vals = live_database.objects.get(tile_num=curr_sq)
            sel_vals.last_visited=1
            sel_vals.save()
          for i in range(0,4):
              #loop from 1-4 inclusive.
            see_entries = live_database.objects.filter(tile_num=new_squares[i]) #checks whether value is present inside
            ali_info = temp_dict[i+1]
            if len(see_entries)==0:#check if an entry exists in the given tile. 
              if ali_info == "T" or ali_info == "U" or ali_info=="W":# if terrain or unknown in the slot. 
                make_new_tile = live_database(tile_num=new_squares[i],tile_info=temp_dict[i+1],last_visited=0)
                make_new_tile.save()
              else:
                #check if not a terrain or Unknown thingy.
                if alien_storer[ali_info] == 0:#for aliens we can just save them if not there before. 
                  first_ali = live_database(tile_num=new_squares[i],tile_info=temp_dict[i+1],last_visited=0)
                  first_ali.save() #there does not need to be any new vals inserted.
                  alien_storer[ali_info] += 1
                  if ali_info == "W":
                    alien_storer[ali_info] = 0
                  
              # we can select tile info from dictionary

            else: #think about the new conditions to insert a new alien onto the screen . 
              if ali_info != "T" and ali_info != "U":#this is the case something is written in the tile. 
                ##get and label as a new tile, no previouosly written information. 
                check_sq = live_database.objects.get(tile_num=new_squares[i])
                check_info = check_sq.tile_info

                if alien_storer[ali_info] == 0:#check if an alien not in tile. 
                  #we check if this is 0 and need to like reprocess. 
                  if check_info == "T" or check_info=="U":
                    alien_ins = live_database(tile_num=new_squares[i],tile_info=ali_info,last_visited=0)
                    alien_ins.save()
                    alien_storer[ali_info] += 1#just in case 0. 

                

        else:
          # print("BOTTOM LOOP")
          cmsg = "A"
          conn.send(cmsg.encode())  
          fan = False              
          # print("Maintaining connection")
          content = conn.recvfrom(64)[0]
          content = content.decode()
          print("Checking content",content)
          print("content length", len(content))

          if len(content)<=4:
              print("checking IF condition")
              break
          else:
              print("see errors")
              pass
            
          all_info = content.split(";") ##gives array like ['0,1';'PA1';'T2';'T3';'T4']
          if len(all_info) >= 6:
            if len(all_info) == 7:
              fan = True

            print("line 239")
            coords = all_info[0].split(",") #gives x and y coordinates
            curr_sq = 4040+int(coords[0])+(100*int(coords[1]))
            #send back as current values the X,Y values. 
            temp_dict = {}#dictionary for the information sent back by TCP server. 
            temp_dict[1] = all_info[1][:-1]
            temp_dict[2] = all_info[2][:-1]
            temp_dict[3] = all_info[3][:-1]
            temp_dict[4] = all_info[4][:-1]
            temp_dict[5] = all_info[5]



            # print("debugging",temp_dict)
            angle = temp_dict[5]
            curr_dir = os.getcwd()
            direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
            direction_path = direction_path.replace("\\","/")
            direc = open(direction_path, "w")

            direc.write(angle)


            # print("debug: ",angle)
            print(temp_dict[5])
            print(curr_sq)

            new_squares = square_mapper(curr_sq,int(angle))
            ##This obtains the new mapping with a set of arrays for new squares. 
            old_last_sq = live_database.objects.get(last_visited=1)
            old_last_sq.last_visited = 0
            old_last_sq.save()
            print("run code")
            checker = live_database.objects.filter(tile_num=curr_sq)
            if len(checker) == 0:
              if fan:
                new_sq = live_database(tile_num=curr_sq,tile_info="F",last_visited=1)
              else:
                new_sq = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
              new_sq.save()#apply the new last square.
            else:
              sel_vals = live_database.objects.get(tile_num=curr_sq)
              if fan:
                sel_vals.tile_info="F"
              sel_vals.last_visited=1
              sel_vals.save()

            for i in range(0,4):
                #loop from 1-4 inclusive.
              check_db = live_database.objects.filter(tile_num=new_squares[i]) #checks whether value is present inside
              ali_info = temp_dict[i+1]
              # print("I val: ",i," aliens information: ",ali_info)
              if len(check_db)==0:#check if an entry exists in the given tile. 
                if ali_info == "T" or ali_info == "U":# if terrain or unknown in the slot. 
                  make_new_tile = live_database(tile_num=new_squares[i],tile_info=temp_dict[i+1],last_visited=0)
                  make_new_tile.save()
                
                else:
                  print("entering bottom crap")
                  #check if not a terrain or Unknown thingy.
                  if alien_storer[ali_info] == 0:#for aliens we can just save them if not there before. 
                    first_ali = live_database(tile_num=new_squares[i],tile_info=temp_dict[i+1],last_visited=0)
                    first_ali.save() #there does not need to be any new vals inserted.
                    alien_storer[ali_info] += 1#not labelled as terrain etc. prevents multiple insertions my guess is. 
                    if ali_info == "W":
                      alien_storer[ali_info] = 0
                  elif ali_info == "W":
                    print("entering bottom if conditions")
                    wallquery = live_database.objects.filter(tile_num=new_squares[i])
                    if len(wallquery)!=0:
                      wall = live_database.objects.get(tile_num=new_squares[i])
                      wall.tile_info="W"
                      wall.save()
                    else:
                      first_ali = live_database(tile_num=new_squares[i],tile_info=temp_dict[i+1],last_visited=0)
                      first_ali.save()
                # we can select tile info from dictionary

              else: #think about the new conditions to insert a new alien onto the screen
                if ali_info != "T" and ali_info != "U":#this is the case something is written in the tile. 
                  ##get and label as a new tile, no previouosly written information. 
                  check_sq = live_database.objects.get(tile_num=new_squares[i])
                  check_info = check_sq.tile_info

                  if alien_storer[ali_info] == 0:#check if an alien is rendered.
                    print("line 356 entered")
 
                    #we check if this is 0 and need to like reprocess. 
                    if check_info == "T" or check_info=="U":
                      alien_ins = live_database(tile_num=new_squares[i],tile_info=ali_info,last_visited=0)
                      alien_ins.save()
                      alien_storer[ali_info] += 1#just in case 0. 
                
        
          else:
            print("entering else condition")
            pass



              

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
