import sqlite3 
import os
from members.models import map_info,all_info,live_database
from django.core.management.base import BaseCommand
count = 81
curr_sq = "44"#programmed by default
head_angle = 0

class Command(BaseCommand):
    help = 'import booms'
    def add_arguments(self, parser):
        pass
    def handle(self,*args, **options):
        traversed = {}
        len_a = len(all_info.objects.all())
        print("DEBUGGING NESTS")
        if len_a >=0:
            sel_val = all_info.objects.all().values()
            for i in range(0,len(sel_val)):
                my_test = sel_val[i]["id"]
                traversed[my_test] = i

            print(sel_val)
            print("TRAVERSED",traversed)
