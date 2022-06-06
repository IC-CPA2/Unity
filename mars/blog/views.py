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

import hashlib

no = 9

modes = ["Slow", "Fast"]

def base(request):
    return redirect('/home')

def home(request):
    context = {
        'posts': posts
    }
    return render(request, 'blog/home.html', context)

def about(request):
    default = """<canvas id="myCanvas" width="50" height="50" style="border:1px solid #000000;"></canvas>"""

    curr_dir = os.getcwd()
    file_path = curr_dir+"\\blog\\text_files\\bat.txt"
    file_path = file_path.replace("\\","/")
    f = open(file_path, "r")
    batteryLvl = f.readline() + "%"
    #reads battery levels. 
    f.close()

    img = []

    ali = [[0]*9 for i in range(9)]
    #edit value of ali at each position to get the current position. 
    file_path = curr_dir+"\\blog\\text_files\\image.txt"
    file_path = file_path.replace("\\","/")
    fi = open(file_path, "r")
    val = fi.readline()
    fi.close()
    val1 = val.split(";")
    #reads in sequence the vlues of this thing. 
    print (val1)
    curr_pos = 0
    for i in range(len(val1)):
        #renders the values
        value = val1[i].split(",")
        # img.append(value)
        for j in range(len(value)):
            ali[i][j] = int(value[j])
            if int(value[j]) == 2:
                curr_pos = str(i)+str(j)# in array coordinates will be 44
    print ("let's find curr_pos",curr_pos)

    directionFile = []
    direction = []
    file_path = curr_dir+"\\blog\\text_files\\direction.txt"
    file_path = file_path.replace("\\","/")
    direc = open(file_path, "r")
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
    w = open(file_path, "r")
    wifi = w.readline()
    w.close()

    context = {
        'counter': ['1', '2', '3', '4', '5', '6', '7', '8', '9'], 
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
    file_path = curr_dir+"\\blog\\text_files\\bat.txt"
    file_path = file_path.replace("\\","/")

    f = open(file_path, "r")
    batteryLvl = f.readline() + "%"
    f.close()

    img = []

    ali = [[0]*9 for i in range(9)]
    file_path = curr_dir+"\\blog\\text_files\\image.txt"
    file_path = file_path.replace("\\","/")
    fi = open(file_path, "r")
    val = fi.readline()
    fi.close()
    val1 = val.split(";")
    for i in range(len(val1)):
        value = val1[i].split(",")
        # img.append(value)
        for j in range(len(value)):
            ali[i][j] = int(value[j])
    
    file_path = curr_dir+"\\blog\\text_files\\wifi.txt"
    file_path = file_path.replace("\\","/")
    w = open(file_path, "r")
    wifi = w.readline()
    print(wifi)
    w.close()
    
    if request.method == 'POST':
        distance = request.POST["dist"]
        direction_path = curr_dir+"\\blog\\text_files\\direction.txt"
        direction_path = direction_path.replace("\\","/")

        dis_path = curr_dir+"\\blog\\text_files\\distance.txt"
        dis_path = dis_path.replace("\\","/")

        angle = request.POST["angle"]
        f = open (dis_path, "w")
        f.write(distance+ "\n" + angle)
        f.close()

        direc = open(direction_path, "a")
        if angle == "0":
            direc.write("\n" + "U" + distance)
        elif angle == "90":
            direc.write("\n" + "R" + distance)
        elif angle == "180":
            direc.write("\n" + "D" + distance)
        elif angle == "270":
            direc.write("\n" + "L" + distance)
        distance = 0
        angle = 0
        direc.close()
        

    directionFile = []
    direction = []
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

    context = {
        'counter': ['1', '2', '3', '4', '5', '6', '7', '8', '9'], 
        'battery': batteryLvl,
        'aliens': ali,
        'directions': direction,
        'wifis': wifi
    } 

    return render(request, 'blog/distance.html', context)