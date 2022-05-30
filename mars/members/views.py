#the newly written files. 
from django.shortcuts import render
from .models import map_info
from django.db import connection
from django.db.models import Q
from django.db.models import Count


# Create your views here.
def members_list_(request):

    posts = map_info.objects.all().values('map_id','date_time','map_size')
    # posts = pathHistory.objects.distinct('map_id')

    # test = posts.only('map_id')
    print("hehe",posts)
    # print(posts.query)
    # print(connection.queries)
    print(posts.values)
    return render(request, 'output.html',{'posts':posts})
