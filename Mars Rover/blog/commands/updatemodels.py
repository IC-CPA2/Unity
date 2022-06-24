# import sqlite3
from members.models import map_info
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
        test_var = map_info(map_id=9,map_size="9x9")
        print("Before change map_info:",map_info.objects.all().values())
        test_var.save()
        print("after change:",map_info.objects.all().values())



# class map_info(models.Model):
#     map_id = models.CharField(max_length=255,primary_key=True)
#     map_size = models.CharField(max_length=50)
#     date_time = models.DateTimeField(default=timezone.now)
