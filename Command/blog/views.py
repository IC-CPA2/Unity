from audioop import minmax
from datetime import datetime
from tkinter import Y
from django.shortcuts import render
from django.shortcuts import redirect
from django.shortcuts import HttpResponse
from django.template import loader
from django.views.generic import ListView, CreateView, UpdateView
from .models import map_info
from django.db import connection
from django.db.models import Q
from django.db.models import Count
import os
from django.utils import timezone
from members.models import live_database, map_info, all_info

import hashlib

no = 9
const = 1
counter = 0
batteryLvl = 100


def reduce (hundred):
  x = 0
  y = 0
  for i in range(len(hundred)):
    for j in range(len(hundred[i])):
      if hundred[i][j] == 2:
        x = i
        y = j

  output = [[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0]]

  startx = x-4
  starty = y-4

  for k in range(9):
    for l in range(9):
      if (k + startx < len(hundred) and l + starty < len(hundred)):
        output[k][l] = hundred[startx+k][starty+l]

  return output


def base(request):
    return redirect('/home')

def home(request):
    context = { }
    return render(request, 'blog/home.html', context)

def about(request):
    print("ABOUT")

    curr_dir = os.getcwd()

    global counter
    global batteryLvl 
    counter += 1

    print ("Timer:", counter)

    mode_path = curr_dir+"\\blog\\text_files\\mode.txt"
    mode_path = mode_path.replace("\\","/")

    direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
    direction_path = direction_path.replace("\\","/")

    alien_path = curr_dir+"\\blog\\text_files\\alien.txt"
    alien_path = alien_path.replace("\\","/")

    modes_path = curr_dir+"\\blog\\text_files\\mode.txt"
    modes_path = modes_path.replace("\\","/")

    if counter > 20:
        batteryLvl = batteryLvl - 1
        counter = 0

    mo=""
    if 'mode' in request.GET:
        modes = request.GET["mode"]
        mo = modes
        m = open(mode_path, "w")

        mod = ""
        if modes == "Manual":
            mod = "M"
        elif modes == "Autonomous":
            mod = "A"

        m.write(mod)
        m.close()
    
    if 'angle' in request.POST:
        angle = request.POST["angle"]
        print ("ANGLE",angle)
        dis_path = curr_dir+"\\blog\\text_files\\distance.txt"
        dis_path = dis_path.replace("\\","/")
        f = open (dis_path, "w")
        f.write(angle)
        f.close()

    img = []
    database = [[1]*71 for i in range(71)]
    ali = [[1]*9 for i in range(9)]
    rover_pos = 0
    db_length = len(live_database.objects.all())

    if db_length > 0:
        sel_val = live_database.objects.all().values()

        if len(database) == 0:
            print("START")
            for i in range (10,71):
                for j in range(10,71):
                    key = str(i)+str(j)
                    tile = live_database.objects.filter(tile_num=key).values()
                    filt_cond = live_database.objects.get(last_visited=1)
                    rover_pos = filt_cond.tile_num

                    if len(tile) > 0:
                        tile = live_database.objects.get(tile_num=key)
                        # print (tile.tile_info)
                        info = tile.tile_info
                        if (info == "T"):
                            database[i][j] = 7
                        elif (info == "PA"):
                            database[i][j] = 0
                        elif (info == "GA"):
                            database[i][j] = 3
                        elif (info == "BA"):
                            database[i][j] = 4
                        elif (info == "RA"):
                            database[i][j] = 5
                        elif (info == "OA"):
                            database[i][j] = 6
                        elif (info == "YA"):
                            database[i][j] = 10
                        elif (info == "DGA"):
                            database[i][j] = 8
                        elif (info == "DBA"):
                            database[i][j] = 9
                        elif (info == "F"):
                            database[i][j] = 11
                        elif (info == "W"):
                            database[i][j] = 12

                    if key == rover_pos:
                        database[i][j] = 2
            ali = reduce(database)   

        else:
            print("AFTER")
            filthy = live_database.objects.filter(last_visited=1)
            if len(filthy)!=0:
                filt_cond = live_database.objects.get(last_visited=1)
                rover_pos = filt_cond.tile_num
                rover_posy = str(rover_pos)[0:2]
                rover_posy = int(rover_posy)
                rover_posx = str(rover_pos)[2:4]
                rover_posx = int(rover_posx)
                for i in range (rover_posy-7, rover_posy+7):
                    for j in range (rover_posx-7, rover_posx+7):  
                        key = str(i)+str(j)
                        tile = live_database.objects.filter(tile_num=key).values()
                        rover_pos = filt_cond.tile_num

                        if len(tile) > 0:
                            tile = live_database.objects.get(tile_num=key)
                            info = tile.tile_info
                            if (info == "T"):
                                database[i][j] = 7
                            elif (info == "PA"):
                                database[i][j] = 0
                            elif (info == "GA"):
                                database[i][j] = 3
                            elif (info == "BA"):
                                database[i][j] = 4
                            elif (info == "RA"):
                                database[i][j] = 5
                            elif (info == "OA"):
                                database[i][j] = 6
                            elif (info == "YA"):
                                database[i][j] = 10
                            elif (info == "DGA"):
                                database[i][j] = 8
                            elif (info == "DBA"):
                                database[i][j] = 9
                            elif (info == "F"):
                                database[i][j] = 11
                            elif (info == "W"):
                                database[i][j] = 12
                        if key == rover_pos:
                            database[i][j] = 2
                ali = reduce(database)                     


    direc = open(direction_path, "r")
    heading = direc.read()

    if os.path.getsize(direction_path): 
        print("HERE:", heading)
    else:
        heading = "0"

    # alie = open(alien_path, "r")
    alienlog = []

    # for i in alie:
    #     alienlog.append(i)

    aliensquery = live_database.objects.filter(~Q(tile_info='T'), ~Q(tile_info='U'), ~Q(tile_info='R')).values_list()

    for i in aliensquery:
        tile_num = i[0]
        y = -(int(tile_num[:2]) - 40)
        x = int(tile_num[2:]) - 40
        tile_num = str(x) + ', ' + str(y)
        tile_info = i[1]
        if tile_info == 'BA':
            tile_info = 'Blue Alien'
        elif tile_info == 'YA':
            tile_info = 'Yellow Alien'
        elif tile_info == 'GA':
            tile_info = 'Green Alien'
        elif tile_info == 'RA':
            tile_info = 'Red Alien'
        elif tile_info == 'PA':
            tile_info = 'Pink Alien'
        elif tile_info == 'DGA':
            tile_info = 'Dark Green Alien'
        elif tile_info == 'DBA':
            tile_info = 'Dark Blue Alien'
        elif tile_info == 'W':
            tile_info = 'Building'
        elif tile_info == 'F':
            tile_info = 'Underground Infrastructure'

        display = tile_info + ": " + tile_num
        if display not in alienlog:
            alienlog.append(display)

    
    m = open(modes_path, "r")
    modss = m.readline()
    if modss== "M":
        mo = "Manual"
    elif modss == "A":
        mo = "Autonomous"

    context = {
        'counter': ['1','2','3','4','5','6','7','8','9'],
        'aliens': ali,
        'headings': int(heading),
        'wifis': os.system("ping -c 1 google.com")==0,
        'options': ['Manual', 'Autonomous'],
        'picked': mo,
        'alienlogs': alienlog,
        'battery': str(100-batteryLvl) + "%",
        'charging': False
    } 

    return render(request, 'blog/about.html', context)

def login(request):
    password = request.POST['psw']
    username = request.POST['uname']
    if username == 'ccl19' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '08c368c45b3e8d0c6ddc111a564f05dac269f1b1623ac4989b94b8d577d85d19':
        return redirect('/about')
    elif username == 'yscamy' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '65123d9463c1fa5535e014bfaf9c551481b1b4d227b587b917ca9f05c8edc778':
        return redirect('/about')
    elif username == 'ms3120' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '4191a83a68daf3b0a0e997342f79f8c38a68553e5cd255830552ba302efccecd':
        return redirect('/about')
    elif username == 'hjj120' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8':
        return redirect('/about')
    elif username == 'jjo20' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '1906bc7c801f03c41551b06e2fd406e8f471787c51357e8731ec61dd599f04c8':
        return redirect('/about')
    elif username == 'milu' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '9ab83c29298c6edeefe322684cdb4e81c1175eb41463c1d6d62c5e1acc7b0c6c':
        return redirect('/about')
    elif username == 'kelvin' and hashlib.sha256(password.encode('utf-8')).hexdigest() == 'e45a1e24db27c12e9a60a38e56ade21c2f37e74adcd13959d01f898c27cae891':
        return redirect('/about')
    else:
        return redirect('/')

def form(request):
    return redirect('/about')

#takes distance and sends to back end I think. 
#the file edited is distance.txt
def distance(request):
    print("DISTANCE")
    curr_dir = os.getcwd()
    
    modes_path = curr_dir+"\\blog\\text_files\\mode.txt"
    modes_path = modes_path.replace("\\","/")

    pang_path = curr_dir+"\\blog\\text_files\\procang.txt"
    pang_path = pang_path.replace("\\","/")

    alien_path = curr_dir+"\\blog\\text_files\\alien.txt"
    alien_path = alien_path.replace("\\","/")

    img = []
    ali = [[1]*9 for i in range(9)]
    db_length = len(live_database.objects.all())
    database = [[1]*71 for i in range(71)]
    rover_pos = 0

    if db_length > 0:
        sel_val = live_database.objects.all().values()
        
        if len(database) == 0:
            print("START")
            for i in range (10,71):
                for j in range(10,71):
                    key = str(i)+str(j)
                    
                    tile = live_database.objects.filter(tile_num=key).values()
                    filthy = live_database.objects.filter(last_visited=1)
                    if len(filthy) != 0:
                        filt_cond = live_database.objects.get(last_visited=1)
                        rover_pos = filt_cond.tile_num


                    if len(tile) > 0:
                        tile = live_database.objects.get(tile_num=key)
                        # print (tile.tile_info)
                        info = tile.tile_info
                        if (info == "T"):
                            database[i][j] = 7
                        elif (info == "PA"):
                            database[i][j] = 0
                        elif (info == "GA"):
                            database[i][j] = 3
                        elif (info == "BA"):
                            database[i][j] = 4
                        elif (info == "RA"):
                            database[i][j] = 5
                        elif (info == "OA"):
                            database[i][j] = 6
                        elif (info == "YA"):
                            database[i][j] = 10
                        elif (info == "DGA"):
                            database[i][j] = 8
                        elif (info == "DBA"):
                            database[i][j] = 9
                        elif (info == "F"):
                            database[i][j] = 11
                        elif (info == "W"):
                            database[i][j] = 12
                    if key == rover_pos:
                        database[i][j] = 2
            ali = reduce(database)   

        else:
            print("AFTER")
            filthy = live_database.objects.filter(last_visited=1)
            if len(filthy) == 1:
                    
                filt_cond = live_database.objects.get(last_visited=1)
                rover_pos = filt_cond.tile_num
                rover_posy = str(rover_pos)[0:2]
                rover_posy = int(rover_posy)
                rover_posx = str(rover_pos)[2:4]
                rover_posx = int(rover_posx)

                for i in range (rover_posy-7, rover_posy+7):
                    for j in range (rover_posx-7, rover_posx+7):  
                        key = str(i)+str(j)
                        # print(key)
                        tile = live_database.objects.filter(tile_num=key).values()
                        # filt_cond = live_database.objects.get(last_visited=1)
                        rover_pos = filt_cond.tile_num

                        if len(tile) > 0:
                            tile = live_database.objects.get(tile_num=key)
                            # print (tile.tile_info)
                            info = tile.tile_info
                            if (info == "T"):
                                database[i][j] = 7
                            elif (info == "PA"):
                                database[i][j] = 0
                            elif (info == "GA"):
                                database[i][j] = 3
                            elif (info == "BA"):
                                database[i][j] = 4
                            elif (info == "RA"):
                                database[i][j] = 5
                            elif (info == "OA"):
                                database[i][j] = 6
                            elif (info == "YA"):
                                database[i][j] = 10
                            elif (info == "DGA"):
                                database[i][j] = 8
                            elif (info == "DBA"):
                                database[i][j] = 9
                            elif (info == "F"):
                                database[i][j] = 11
                            elif (info == "W"):
                                database[i][j] = 12
                        if key == rover_pos:
                            database[i][j] = 2
                ali = reduce(database)        
    

    if 'reset' in request.GET:
        yes = request.GET["reset"]
        if yes == "1":
            live_database.objects.filter(last_visited=0).delete()
    
    if 'mode' in request.GET:
        modes = request.GET["mode"]
        m = open(mode_path, "w")

        mod = ""
        if modes == "Manual":
            mod = "M"
        elif modes == "Autonomous":
            mod = "A"

        m.write(mod)
        m.close()

    if 'angle' in request.POST:
        angle = request.POST["angle"]
        f = open (pang_path, "w")
        f.write(angle)
        f.close()

    if 'map_name' in request.POST: 
        name = request.POST['map_name']
        print("map_name:", name)
        
        maxx = 0
        maxy = 0
        minx = 100
        miny = 100
        tile_numarr =[]

        tile = live_database.objects.values_list('tile_num')
        for i in tile:
            y = str(i)[2:4]
            y = int(y)
            x = str(i)[4:6]
            x = int(x)
            tile_numarr.append(str(y)+str(x))
            

            # print (y,x)

            if x > maxx:
                maxx = x
            if y > maxy:
                maxy = y
            if x < minx:
                minx = x
            if y < miny:
                miny = y
        
        sizex = maxx-minx+1
        sizey = maxy-miny+1

        size = str(sizex) + "x" + str(sizey)
        
        unique = map_info.objects.filter(user_map_name=str(name))

        if len(unique) == 0:
            new = map_info(map_name=name,map_size=size,user_map_name=str(name))
            new.save()
            mapid = new.map_id
                    
        else:
            rawname = name 
            name += " (" + str(len(unique)) + ")"
            new = map_info(map_name=name,map_size=size,user_map_name=rawname)
            new.save()
            mapid = new.map_id

        last_vis = live_database.objects.get(last_visited=1)  
        
        for i in range(miny,maxy+1):
            for j in range(minx,maxx+1):
                key = str(i)+str(j)
                if key in tile_numarr:
                    tile = live_database.objects.get(tile_num=key)
                    if key == last_vis.tile_num: 
                        new_tile = all_info(tile_number=key,tile_info="R",map_id_id=mapid)
                    else:
                        new_tile = all_info(tile_number=key,tile_info=tile.tile_info,map_id_id=mapid)
                else:
                    new_tile = all_info(tile_number=key,tile_info="U",map_id_id=mapid)

                new_tile.save()   


    pang = open(pang_path, "r")
    heading = pang.read()
    if os.path.getsize(pang_path): 
        print("heading from direction:", heading)
    else:
        heading = "0"
    
    alienlog = {}
    alie = open(alien_path, "r")
    aliensquery = live_database.objects.filter(~Q(tile_info='T'), ~Q(tile_info='U'), ~Q(tile_info='R'))

    for i in aliensquery:
        alienlog[i.tile_num] = i.tile_info
    
    mo=""
    if 'mode' in request.GET:
        print("MODES")
        modes = request.GET["mode"]
        mo = modes
        m = open(mode_path, "w")

        mod = ""
        if modes == "Manual":
            mod = "M"
        elif modes == "Autonomous":
            mod = "A"

        m.write(mod)
        m.close()
        
    m = open(modes_path, "r")

    if m.read() == "M":
        mo = "Manual"
    elif m.read() == "A":
        mo = "Autonomous"

    context = {
        'counter': ['1','2','3','4','5','6','7','8','9'],
        'aliens': ali,
        'headings': int(heading),
        'options': ['Manual', 'Autonomous'],
        'picked': mo,
        'alienlogs': alienlog
    } 

    return redirect ('/about', context)