from django.db import models
from django.utils import timezone

# Create your models here.
class map_info(models.Model):
    map_id = models.AutoField(primary_key=True)
    map_name = models.CharField(max_length=50)
    user_map_name = models.CharField(max_length=50)
    map_size = models.CharField(max_length=50)
    date_time = models.DateTimeField(default=timezone.now)
    def _str_(self):
        return self.map_id

class all_info(models.Model):
    map_id = models.ForeignKey('map_info',on_delete=models.CASCADE)
    path = models.CharField(max_length=600)#designed to store the whole long path. 
    tile_number = models.CharField(max_length=50)
    tile_info = models.CharField(max_length=50)
    
    # @property
    # def maptilenum(self):
    #      return "%s + %s" % ( self.map_id.map_id, self.tile_number )

class live_database(models.Model):
    ##Attributes: Current Coordinates, Past-visited, Order-Visited.
    #Could maybe try and stop early and render map accordingly.
    tile_num = models.CharField(max_length=50, primary_key=True)
    tile_info = models.CharField(max_length=100)
    #tile info denotes Alien, not Alien etc. 
    last_visited = models.BooleanField()