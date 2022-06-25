from members.models import map_info,all_info,live_database
from hashlib import new
from xml.dom.expatbuilder import parseString
from django.core.management.base import BaseCommand
import os
import numpy as np
import math
import time
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

def storer():
  color = ['PA','OA','RA','GA','YA','DGA','DBA','BA','W']
  alien_storer = {}
  for i in color:
    sel = live_database.objects.filter(tile_info=i)
    alien_storer[i]=len(sel)
    if i == 'W':
      alien_storer[i] = 0
  return alien_storer
  
def square_mapper(curr_coords,angle):#array of coordinates from field of view.
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
  return arr#gives array of coordinates.
   
class Command(BaseCommand):
    help = 'import booms'
    def add_arguments(self, parser):
        pass
    def handle(self,*args, **options):
        print("reached here")
        curr_sq = "4040"
        if len(live_database.objects.all()) == 0:
            curr_sq = "4040"
        else:
            print("in else loop")
            sel_filter = live_database.objects.filter(last_visited=1)
            if len(sel_filter)!=0:
                query = live_database.objects.get(last_visited=1)
                curr_sq = query.tile_num 
        while True:
            time.sleep(1.5)
            iterator = len(live_database.objects.all())
            if iterator == 0:
                insert_vals = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
                insert_vals.save()
            alien_storer = {}
            # print("in this line")
            alien_storer = storer()
            nav_curr_dir = os.getcwd()
            my_file = open(nav_curr_dir+"/on_serv.txt","r") #read the string from the text file. 
            content = my_file.readline()#can write and read from the file. 
            if content == "XX":
                break
            my_file.close()
            # print("check content",content)
            # print("BOTTOM LOOP")
            all_info = content.split(";") ##gives array like ['0,1';'PA1';'T2';'T3';'T4']
            fan = False
            if len(all_info) >= 6:
                if len(all_info) == 7:
                    fan = True
            coords = all_info[0].split(",") #gives x and y coordinates
            curr_sq = 4040+int(coords[0])+(100*int(coords[1]))
            #send back as current values the X,Y values. 
            temp_dict = {}#dictionary for the information sent back by TCP server. 
            temp_dict[1] = all_info[1][:-1]
            temp_dict[2] = all_info[2][:-1]
            temp_dict[3] = all_info[3][:-1]
            temp_dict[4] = all_info[4][:-1]
            temp_dict[5] = all_info[5]
            print("content is",content)
            angle = temp_dict[5]
            h_ang = int(angle)
            while h_ang<0:
                h_ang += 360#add 360 if receiving negative angles constantyly. 
            curr_dir = os.getcwd()
            direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
            direction_path = direction_path.replace("\\","/")
            direc = open(direction_path, "w")
            direc.write(str(h_ang))#writing head angle to current thing. 
            new_squares = square_mapper(curr_sq,h_ang)
            ##This obtains the new mapping with a set of arrays for new squares. 
            check_if_in = live_database.objects.filter(last_visited=1)
            if len(check_if_in)!=0:
                # print("thing is getting overwritten")
                old_last_sq = live_database.objects.get(last_visited=1)
                old_last_sq.last_visited = 0
                old_last_sq.save()
            checker = live_database.objects.filter(tile_num=curr_sq)
            if len(checker) == 0:#seeing if the current square in DB.
                if fan:
                    new_sq = live_database(tile_num=curr_sq,tile_info="F",last_visited=1)
                else:
                    # print("should happen here.")
                    new_sq = live_database(tile_num=curr_sq,tile_info="T",last_visited=1)
                    new_sq.save()#apply the new last square.
            else:
                sel_vals = live_database.objects.get(tile_num=curr_sq)
                if fan:
                    sel_vals.tile_info="F"
                    sel_vals.last_visited=1
                    sel_vals.save()
                else:
                  pres_info = live_database.objects.get(tile_num=curr_sq)
                  pres_info.last_visited=1
                  pres_info.save()
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
                        print("checking ins_conds")
                        #check if not a terrain or Unknown thingy.
                        if alien_storer[ali_info] == 0:#for aliens we can just save them if not there before. 
                            print("check alien entering condition!!")
                            print("double confo 161!!")
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
                                print("verify ali_inf: ",ali_info)
                                first_ali = live_database(tile_num=new_squares[i],tile_info=temp_dict[i+1],last_visited=0)
                                first_ali.save()
                # we can select tile info from dictionary
                else: #think about the new conditions to insert a new alien onto the screen
                    print("entering else conditions")
                    if ali_info != "T" and ali_info != "U":#this is the case something is written in the tile. 
                        ##get and label as a new tile, no previouosly written information. 
                        check_sq = live_database.objects.get(tile_num=new_squares[i])
                        check_info = check_sq.tile_info
                        if alien_storer[ali_info] == 0:#check if an alien is rendered.
                            #we check if this is 0 and need to like reprocess. 
                            if check_info == "T" or check_info=="U":
                                alien_ins = live_database(tile_num=new_squares[i],tile_info=ali_info,last_visited=0)
                                alien_ins.save()
                                alien_storer[ali_info] += 1#just in case 0. 
                # print("loop edge")

