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
from members.models import live_database, map_info

import hashlib

no = 9


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

    img = []

    ali = [[1]*9 for i in range(9)]
    db_length = len(live_database.objects.all())
    traversed = {}
    if db_length >0:
        
        # print("DATABASE LENGTH >1 ")
        sel_val = live_database.objects.all().values()
        for i in range(0, len(sel_val)):
            labeled_tile = sel_val[i]["tile_num"]
            info_rec = sel_val[i]["tile_info"]
            if info_rec == "T":
                traversed[labeled_tile] = 7
            elif info_rec == "PA":
                traversed[labeled_tile] = 0
            elif info_rec == "GA":
                traversed[labeled_tile] = 3
            elif info_rec == "BA":
                traversed[labeled_tile] = 4
            elif info_rec == "RA":
                traversed[labeled_tile] = 5
            elif info_rec == "OA":
                traversed[labeled_tile] = 6
        filt_cond = live_database.objects.get(last_visited=1)
        tmp = filt_cond.tile_num
        print(tmp)
        for key in traversed:#this for loop will be used to create the map for the aliens. 
            if traversed[key] == 0:
                storer = int(key)
                # print("debug",storer)
                # print("failing",int(storer-(storer%10))/10)
                x_coord = (storer-(storer%10))/10
                ali[int(x_coord)][storer%10] = 0
        storer = int(tmp)
        x_pos = (storer-(storer%10))/10
        ali[int(x_pos)][storer%10] = 2



    
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
    password = request.POST['last']
    if hashlib.sha256(password.encode('utf-8')).hexdigest() == 'ef797c8118f02dfb649607dd5d3f8c7623048c9c063d532cc95c5ed7a898a64f':
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
    traversed = {}

    

            


    if db_length >0:
        print("DATABASE LENGTH >1 ")
        sel_val = live_database.objects.all().values()
        
    
        for i in range (10,69):
            for j in range(10,69):
                key = str(i)+str(j)
                tile = live_database.objects.filter(tile_num=key).values()
                if len(tile) > 0:
                    database[i][j] = tile.tile_info

    




        for i in range(0,len(sel_val)):
            labeled_tile = sel_val[i]["tile_num"]
            info_rec = sel_val[i]["tile_info"]
            print (info_rec)
            if info_rec == "T":
                traversed[labeled_tile] = 7
            elif info_rec == "PA":
                traversed[labeled_tile] = 0
            elif info_rec == "GA":
                traversed[labeled_tile] = 3
            elif info_rec == "BA":
                traversed[labeled_tile] = 4
            elif info_rec == "RA":
                traversed[labeled_tile] = 5
            elif info_rec == "OA":
                traversed[labeled_tile] = 6
        filt_cond = live_database.objects.get(last_visited=1)
        tmp = filt_cond.tile_num
        print(tmp)
        for key in traversed:#this for loop will be used to create the map for the aliens. 
            if traversed[key] == 0:
                storer = int(key)
                # print("debug",storer)
                # print("failing",int(storer-(storer%10))/10)
                x_coord = (storer-(storer%10))/10
                ali[int(x_coord)][storer%10] = 0
            elif traversed[key] == 3:
                storer = int(key)
                # print("debug",storer)
                # print("failing",int(storer-(storer%10))/10)
                x_coord = (storer-(storer%10))/10
                ali[int(x_coord)][storer%10] = 3
            elif traversed[key] == 4:
                storer = int(key)
                # print("debug",storer)
                # print("failing",int(storer-(storer%10))/10)
                x_coord = (storer-(storer%10))/10
                ali[int(x_coord)][storer%10] = 4
            elif traversed[key] == 5:
                storer = int(key)
                # print("debug",storer)
                # print("failing",int(storer-(storer%10))/10)
                x_coord = (storer-(storer%10))/10
                ali[int(x_coord)][storer%10] = 5
            elif traversed[key] == 6:
                storer = int(key)
                # print("debug",storer)
                # print("failing",int(storer-(storer%10))/10)
                x_coord = (storer-(storer%10))/10
                ali[int(x_coord)][storer%10] = 6
            elif traversed[key] == 7:
                storer = int(key)
                # print("debug",storer)
                # print("failing",int(storer-(storer%10))/10)
                x_coord = (storer-(storer%10))/10
                ali[int(x_coord)][storer%10] = 7
        storer = int(tmp)
        x_pos = (storer-(storer%10))/10
        ali[int(x_pos)][storer%10] = 2

    
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
    
    if request.method == 'POST':

        direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
        direction_path = direction_path.replace("\\","/")

        angle = request.POST["angle"]

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

    return render(request, 'blog/about.html', context)

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

