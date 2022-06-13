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
    default = """<canvas id="myCanvas" width="50" height="50" style="border:1px solid #000000;"></canvas>"""

    curr_dir = os.getcwd()
    bat_path = curr_dir+"\\blog\\text_files\\bat.txt"
    bat_path = bat_path.replace("\\","/")
    f = open(bat_path, "r")
    batteryLvl = f.readline() + "%"
    #reads battery levels. 
    f.close()

    if 'mode' in request.GET:
        modes = request.GET["mode"]
        mode_path = curr_dir+"\\blog\\text_files\\mode.txt"
        mode_path = mode_path.replace("\\","/")
        m = open(mode_path, "w")

        mod = ""
        if modes == "Manual":
            mod = "M"
        elif modes == "Autonomous":
            mod = "A"

        m.write(mod)
        m.close()


    img = []
    database = [[1]*59 for i in range(59)]

    ali = [[1]*9 for i in range(9)]
    rover_pos = 0
    db_length = len(live_database.objects.all())
    if db_length > 0:
        sel_val = live_database.objects.all().values()

        if len(database) == 0:
            print("START")
            for i in range (10,69):
                for j in range(10,69):
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

                    if key == rover_pos:
                        database[i][j] = 2
            ali = reduce(database)   

        else:
            print("AFTER")
            filt_cond = live_database.objects.get(last_visited=1)
            rover_pos = filt_cond.tile_num
            rover_posx = str(rover_pos)[0:2]
            rover_posx = int(rover_posx)
            rover_posy = str(rover_pos)[2:4]
            rover_posy = int(rover_posy)
            for i in range (rover_posy-7, rover_posy+7):
                for j in range (rover_posx-7, rover_posx+7):  
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

                    if key == rover_pos:
                        database[i][j] = 2
            ali = reduce(database)                     
        # print (ali)


        # for i in range(0, len(sel_val)):
        #     labeled_tile = sel_val[i]["tile_num"]
        #     info_rec = sel_val[i]["tile_info"]
        #     if info_rec == "T":
        #         traversed[labeled_tile] = 7
        #     elif info_rec == "PA":
        #         traversed[labeled_tile] = 0
        #     elif info_rec == "GA":
        #         traversed[labeled_tile] = 3
        #     elif info_rec == "BA":
        #         traversed[labeled_tile] = 4
        #     elif info_rec == "RA":
        #         traversed[labeled_tile] = 5
        #     elif info_rec == "OA":
        #         traversed[labeled_tile] = 6
        # filt_cond = live_database.objects.get(last_visited=1)
        # tmp = filt_cond.tile_num
        # print(tmp)
        # for key in traversed:#this for loop will be used to create the map for the aliens. 
        #     if traversed[key] == 0:
        #         storer = int(key)
        #         # print("debug",storer)
        #         # print("failing",int(storer-(storer%10))/10)
        #         x_coord = (storer-(storer%10))/10
        #         # ali[int(x_coord)][storer%10] = 0
        # # storer = int(tmp)
        # # x_pos = (storer-(storer%10))/10
        # # ali[int(x_pos)][storer%10] = 2

    # #edit value of ali at each position to get the current position. 
    # file_path = curr_dir+"\\blog\\text_files\\image.txt"
    # file_path = file_path.replace("\\","/")
    # fi = open(file_path, "r")
    # val = fi.readline()
    # fi.close()
    # val1 = val.split(";")
    # #reads in sequence the vlues of this thing. 
    # print (val1)
    # curr_pos = 0
    # for i in range(len(val1)):
    #     #renders the values
    #     value = val1[i].split(",")
    #     # img.append(value)
    #     for j in range(len(value)):
    #         ali[i][j] = int(value[j])
    #         if int(value[j]) == 2:
    #             curr_pos = str(i)+str(j)# in array coordinates will be 44
    # print ("let's find curr_pos",curr_pos)

    directionFile = []
    direction = []
    direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
    direction_path = direction_path.replace("\\","/")
    direc = open(direction_path, "r")
    for x in direc:
        directionFile.append(x)
    direc.close()

    for i in directionFile:
        if i[0] == "U":
            direction.append("Forward: " + str(i[1:]))
        elif i[0] == "D":
            direction.append("Backward: " + str(i[1:]))
        elif i[0] == "L":
            direction.append("Left: " + str(i[1:]))
        elif i[0] == "R":
            direction.append("Right: " + str(i[1:]))
   
    wifi_path = curr_dir+"\\blog\\text_files\\wifi.txt"
    wifi_path = wifi_path.replace("\\","/")

    w = open(wifi_path, "r")
    wifi = w.readline()
    w.close()

    context = {
        # 'y': ['90','91', '92', '93', '94', '95', '96', '97', '98', '99','10','11','12','13','14','15','16','17','18','19'], 
        # 'x': ['90','91','92','93','94','95','96','97','98','99','10','11','12'],
        'counter': ['1','2','3','4','5','6','7','8','9'],
        'battery': batteryLvl,
        'aliens': ali,
        'directions': direction,
        'wifis': wifi
    }
    print(request.POST)
    return render(request, 'blog/about.html', context)

def login(request):
    password = request.POST['psw']
    username = request.POST['uname']
    if username == 'ccl19' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '08c368c45b3e8d0c6ddc111a564f05dac269f1b1623ac4989b94b8d577d85d19':
        return redirect('/about')
    elif username == 'yscamy' and hashlib.sha256(password.encode('utf-8')).hexdigest() == '65123d9463c1fa5535e014bfaf9c551481b1b4d227b587b917ca9f05c8edc778':
        return redirect('/about')
    else:
        return redirect('/')

def form(request):
    return redirect('/about')

#takes distance and sends to back end I think. 
#the file edited is distance.txt
def distance(request):

    curr_dir = os.getcwd()
    bat_path = curr_dir+"\\blog\\text_files\\bat.txt"
    bat_path = bat_path.replace("\\","/")

    f = open(bat_path, "r")
    batteryLvl = f.readline() + "%"
    f.close()

    img = []
    ali = [[1]*9 for i in range(9)]
    db_length = len(live_database.objects.all())
    database = [[1]*59 for i in range(59)]
    rover_pos = 0

    if db_length >0:
        # print ("HELLO")
        # print("DATABASE LENGTH >1 ")
        sel_val = live_database.objects.all().values()
        
        if len(database) == 0:
            print("START")
            for i in range (10,69):
                for j in range(10,69):
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
                    if key == rover_pos:
                        database[i][j] = 2
            ali = reduce(database)   

        else:
            print("AFTER")
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

                    if key == rover_pos:
                        database[i][j] = 2
            ali = reduce(database)    
        # print (ali)
        # for i in range(0,len(sel_val)):
        #     labeled_tile = sel_val[i]["tile_num"]
        #     info_rec = sel_val[i]["tile_info"]
        #     print (info_rec)
        #     if info_rec == "T":
        #         traversed[labeled_tile] = 7
        #     elif info_rec == "PA":
        #         traversed[labeled_tile] = 0
        #     elif info_rec == "GA":
        #         traversed[labeled_tile] = 3
        #     elif info_rec == "BA":
        #         traversed[labeled_tile] = 4
        #     elif info_rec == "RA":
        #         traversed[labeled_tile] = 5
        #     elif info_rec == "OA":
        #         traversed[labeled_tile] = 6
        # filt_cond = live_database.objects.get(last_visited=1)
        # tmp = filt_cond.tile_num
        # print(tmp)
        # for key in traversed:#this for loop will be used to create the map for the aliens. 
        #     if traversed[key] == 0:
        #         storer = int(key)
        #         # print("debug",storer)
        #         # print("failing",int(storer-(storer%10))/10)
        #         x_coord = (storer-(storer%10))/10
        #         ali[int(x_coord)][storer%10] = 0
        #     elif traversed[key] == 3:
        #         storer = int(key)
        #         # print("debug",storer)
        #         # print("failing",int(storer-(storer%10))/10)
        #         x_coord = (storer-(storer%10))/10
        #         ali[int(x_coord)][storer%10] = 3
        #     elif traversed[key] == 4:
        #         storer = int(key)
        #         # print("debug",storer)
        #         # print("failing",int(storer-(storer%10))/10)
        #         x_coord = (storer-(storer%10))/10
        #         ali[int(x_coord)][storer%10] = 4
        #     elif traversed[key] == 5:
        #         storer = int(key)
        #         # print("debug",storer)
        #         # print("failing",int(storer-(storer%10))/10)
        #         x_coord = (storer-(storer%10))/10
        #         ali[int(x_coord)][storer%10] = 5
        #     elif traversed[key] == 6:
        #         storer = int(key)
        #         # print("debug",storer)
        #         # print("failing",int(storer-(storer%10))/10)
        #         x_coord = (storer-(storer%10))/10
        #         ali[int(x_coord)][storer%10] = 6
        #     elif traversed[key] == 7:
        #         storer = int(key)
        #         # print("debug",storer)
        #         # print("failing",int(storer-(storer%10))/10)
        #         x_coord = (storer-(storer%10))/10
        #         # ali[int(x_coord)][storer%10] = 7
        # storer = int(tmp)
        # x_pos = (storer-(storer%10))/10
        # ali[int(x_pos)][storer%10] = 2

    
    ## if (rover coord) on the edge
    ## shift to centre (traversed[55] = 2)
    ## use func to get 4 coords around the rover 
    ## add them to the corresponding side of the rover
    ## OR look thru database instead, rmb the prev val
    ## of the rover position and find values 4 coords away
    ## using the keys (tile_num)


    # ali = [[0]*9 for i in range(9)]
    # file_path = curr_dir+"\\blog\\text_files\\image.txt"
    # file_path = file_path.replace("\\","/")
    # fi = open(file_path, "r")
    # val = fi.readline()
    # fi.close()
    # val1 = val.split(";")
    # for i in range(len(val1)):
    #     value = val1[i].split(",")
    #     # img.append(value)
    #     for j in range(len(value)):
    #         ali[i][j] = int(value[j])
    
    wifi_path = curr_dir+"\\blog\\text_files\\wifi.txt"
    wifi_path = wifi_path.replace("\\","/")
    w = open(wifi_path, "r")
    wifi = w.readline()
    w.close()

    if 'reset' in request.GET:
        yes = request.GET["reset"]
        if yes == "1":
            live_database.objects.filter(last_visited=0).delete()
    
    if 'mode' in request.GET:
        modes = request.GET["mode"]
        mode_path = curr_dir+"\\blog\\text_files\\mode.txt"
        mode_path = mode_path.replace("\\","/")
        m = open(mode_path, "w")

        mod = ""
        if modes == "Manual":
            mod = "M"
        elif modes == "Autonomous":
            mod = "A"

        m.write(mod)
        m.close()

    if 'angle' in request.POST:
        direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
        direction_path = direction_path.replace("\\","/")

        angle = request.POST["angle"]
        print (angle)

        dis_path = curr_dir+"\\blog\\text_files\\distance.txt"
        dis_path = dis_path.replace("\\","/")
        f = open (dis_path, "w")
        f.write(angle)
        f.close()

        # direc = open(direction_path, "a")
        # if angle == "0":
        #     direc.write("\n" + "U" + distance)
        # elif angle == "90":
        #     direc.write("\n" + "R" + distance)
        # elif angle == "180":
        #     direc.write("\n" + "D" + distance)
        # elif angle == "270":
        #     direc.write("\n" + "L" + distance)
        # distance = 0
        # angle = 0
        # direc.close()

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
            

            print (y,x)

            if x > maxx:
                maxx = x
            if y > maxy:
                maxy = y
            if x < minx:
                minx = x
            if y < miny:
                miny = y
        
        # print (maxy)
        # print (miny)
        sizex = maxx-minx+1
        sizey = maxy-miny+1
        # print ("size:", sizex, sizey)

        size = str(sizex) + "x" + str(sizey)
        
        unique = map_info.objects.filter(map_name=str(name))

        if len(unique) == 0:
            new = map_info(map_name=name,map_size=size)
            new.save()
            mapid = new.map_id
                    
        else:
            global const
            name += " (" + str(const) + ")"
            const += 1
            # rename = map_info.objects.get(map_name=str(name))
            # rename.map_size = size
            new = map_info(map_name=name,map_size=size)
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
            # print(rename.map_name)
        
            

    directionFile = []
    direction = []
    direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
    direction_path = direction_path.replace("\\","/")
    direc = open(direction_path, "r")
    for x in direc:
        directionFile.append(x)
    direc.close()

    for i in directionFile:
        if i[0] == "U":
            direction.append("Forward: " + str(i[1:]))
        elif i[0] == "D":
            direction.append("Backward: " + str(i[1:]))
        elif i[0] == "L":
            direction.append("Left: " + str(i[1:]))
        elif i[0] == "R":
            direction.append("Right: " + str(i[1:]))

    # image_file =  curr_dir + "\\blog\\text_files\\image.txt"
    # image_file = image_file.replace("\\","/")

    # imagef = open (image_file, "r")

    context = {
        # 'y': ['90','91', '92', '93', '94', '95', '96', '97', '98', '99','10','11','12','13','14','15','16','17','18','19'], 
        # 'x': ['90','91','92','93','94','95','96','97','98','99','10','11','12'],
        'counter': ['1','2','3','4','5','6','7','8','9'],
        'battery': batteryLvl,
        'aliens': ali,
        'directions': direction,
        'wifis': wifi
    } 

    return redirect ('/about', context)
    # return render(request, 'blog/about.html', context)

def ajax (request):
    context = {
        'counter': ['1', '2', '3', '4', '5', '6', '7', '8', '9']
    } 
    # image_dec = """<img id="terrain" src="static/members/media/terrain.jpg" alt="terrain" height="0" width="0"/>"""
    # image_dec += """<img id="question" src="static/blog/media/question.jpg" alt="question" height="0" width="0"/> """

    # printout = ""
    # printout += image_dec
    # printout += """<div>"""
    # for i in (n+1 for n in range(9)):
    #     for j in (n+1 for n in range(9)):
    #         printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
    #                                     style="border:0.5px solid #000000;">
    #                                  </canvas>
    #                                  <script>
    #                                      var c = document.getElementById(\"""" + str(i) + str(j) + """\");
    #                                      var ctx = c.getContext("2d");
    #                                      var img = document.getElementById("terrain");
    #                                      ctx.drawImage(img,0,0,50,50);
    #                                  </script>"""
    #     printout = printout + "<br>"

    # printout = printout + "</div>"

    # return HttpResponse(printout)
    return render(request, 'blog/ajax.html', context)

