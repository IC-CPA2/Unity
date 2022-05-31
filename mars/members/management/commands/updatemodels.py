# import sqlite3 
from members.models import map_info,all_info
from django.core.management.base import BaseCommand
### Run this executable to insert data into the database. 
## The database will change after running these series of commands.
# Allows for viewing of before and after.  

class Command(BaseCommand):
    help = 'import booms'
    def add_arguments(self, parser):
        pass

    def handle(self, *args, **options):
        #pass in every single element in sequence from this. 
        iterator = len(map_info.objects.all())
        test_var = map_info.objects.all().filter(map_id=iterator)
        print("CAN THIS DEBUG",type(test_var))
        print("Additionally",test_var)

        # select_var = test_var[0]
        print("further debugging",len(test_var))
        if len(test_var)==0:
            ins_ent = map_info(map_size="9x9")
            print("entering IF conditional")
            update_entry = all_info(tile_info="0",tile_number=12,path=(11))
            ins_ent.save()
            print("IF DEBUG",map_info.objects.all())


            #this conditional can be used if the query is null to insert a value into the datbase
        else:
            sel_var = test_var[0]
            # 1 for terrain, 2 for rover and 0 for alien
            update_entry = all_info(map_id=sel_var,tile_info="2",tile_number=99,path=(12))
            update_table = map_info(map_id=sel_var.map_id,map_size="9x9")
            #get a file reading protocol to push all values into these fields.
            update_entry.save()
            update_table.save()
            print("entering ELSE block")
            print("ELSE DEBUG",all_info.objects.all())
            #this will be used if the query works succesfully and will get what I want. 
            
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


# class map_info(models.Model):
#     map_id = models.CharField(max_length=255,primary_key=True)
#     map_size = models.CharField(max_length=50)
#     date_time = models.DateTimeField(default=timezone.now)
