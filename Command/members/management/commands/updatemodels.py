#python3 manage.py serv
#python3 test.py
#python3 dumb_client.py
#python3 manage.py updatemodels
import sqlite3 
import os
from members.models import map_info,all_info,live_database
from django.core.management.base import BaseCommand
## Run this executable to insert data into the database. 
# The database will change after running these series of commands.
count = 81
curr_sq = "44"#programmed by default
head_angle = 0
#modify distance.txt to change the head_angle value. 
#by default rover moves up/forward
#assume rover will move by Newton's first law. 
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
    end_x, end_y = 8 
    end_coords = int(str(end_x)+str(end_y))
    new_val = int(curr_sq)
    if ang_inp == "0" and (new_val%10)>=1:
        new_val -= 1
    if ang_inp == "45" and (new_val%10)<end_y and (new_val-9)<=end_coords:
        new_val += 9
    elif ang_inp == "90" and (new_val<=end_coords-10):
        new_val += 10
    elif ang_inp == "135" and (new_val%10)<end_y and (new_val<=end_coords-11):
        new_val += 11
    elif ang_inp == "180" and (new_val%10)<end_y:
        new_val += 1
    elif ang_inp == "225" and (new_val>=10) and (new_val%10)<8:#complete if conditions
        new_val -= 9
    elif ang_inp == "270" and (new_val>=10):
        new_val -= 10
    elif ang_inp == "315" and (new_val>=11) and (new_val%10)>=1:
        new_val -= 11 
    return new_val
    

class Command(BaseCommand):
    help = 'import booms'
    def add_arguments(self, parser):
        pass
    def handle(self,*args, **options):
        #made some modifications to handle the iterator.
        iterator = len(live_database.objects.all())
        live_database.objects.all().delete()
        # map_info.objects.all().delete()
        # all_info.objects.all().delete()

        s = live_database(tile_num="4040",tile_info="R",last_visited=1)
        s.save()
        s = live_database(tile_num="4041",tile_info="PA",last_visited=0)
        s.save()
        print("AFTER",live_database.objects.all().values())
    


        # test_var = map_info.objects.all().filter(map_id=iterator)
        # print("CAN THIS DEBUG",type(test_var))
        # print("Additionally",test_var)

        # select_var = test_var[0]
        # print("further debugging",len(test_var))
        # if len(test_var)==0:
        #     ins_ent = map_info(map_size="9x9")
        #     print("entering IF conditional")
        #     update_entry = all_info(tile_info="0",tile_number=12,path=(11))
        #     ins_ent.save()
        #     print("IF DEBUG",map_info.objects.all())


        #     this conditional can be used if the query is null to insert a value into the datbase
        # else:
        #     sel_var = test_var[0]
        #     # 1 for terrain, 2 for rover and 0 for alien
        #     update_entry = all_info(map_id=sel_var,tile_info="2",tile_number=99,path=(12))
        #     update_table = map_info(map_id=sel_var.map_id,map_size="9x9")
        #     #get a file reading protocol to push all values into these fields.
        #     update_entry.save()
        #     update_table.save()
        #     print("entering ELSE block")
        #     print("ELSE DEBUG",all_info.objects.all())
        #     this will be used if the query works succesfully and will get what I want. 
            
        # update_entry = all_info(map_id=select_var,tile_info="X",tile_number=11,path=())
        # update_entry.save()


        # test_var = map_info(map_id=3,map_size="8x8")
        # print("Before change map_info:",map_info.objects.all().values())
        # test_var.save()
        # print("after change:",map_info.objects.all().values())


# class Command(BaseCommand):
#     help = 'import booms'
#     def add_arguments(self, parser):
#         pass

#     def handle(self, *args, **options):
#         #pass in every single element in sequence from this. 
  
#         test_val = map_info.objects.all().filter(map_id=3)
#         selector = test_val[0]
#         upload_vals = experiment(new_attr=selector,sq_info=22)

#         upload_vals.save()
#         # print("after change:",upload_vals.objects.all().values())




