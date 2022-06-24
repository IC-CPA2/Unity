from django.db import models
from django.utils import timezone


# Create your models here.
class map_info(models.Model):
    map_name = models.CharField(max_length=100)
    map_id = models.IntegerField(primary_key=True)
    map_size = models.CharField(max_length=50)
    date_time = models.DateTimeField(default=timezone.now)
    def _str_(self):
        return self.map_id

# class all_info(models.Model):
#     map_id = models.ForeignKey('map_info.map_id',on_delete=models.CASCADE,)
#     map_size = models.ForeignKey('map_info.map_size',on_delete=models.CASCADE,)
#     path = models.CharField(max_length=600)#designed to store the whole long path. 
#     tile_number = models.IntegerField()
#     tile_info = models.CharField(max_length=50)


    