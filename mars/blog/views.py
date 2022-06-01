from django.shortcuts import render
from django.shortcuts import redirect
from django.shortcuts import HttpResponse
from django.template import loader
from django.views.generic import ListView, CreateView, UpdateView
from .models import map_info
from django.db import connection
from django.db.models import Q
from django.db.models import Count

import hashlib

no = 9

posts = [
    {
        'author': 'CoreyMS',
    },
    {
        'author': 'Jane Doe',
    },
    {
        'author': 'Jane Doe',
    },
    {
        'author': 'Jane Doe',
    },
    {
        'author': 'Jane Doe',
    },
    {
        'author': 'Jane Doe',
    },
    {
        'author': 'Jane Doe',
    },
    {
        'author': 'Jane Doe',
    },
    {
        'author': 'Jane Doe',
    }
]

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
    # ret = "<center>"
    # for i in range(9):
    #     for j in range(9):
    #         ret = ret + default
    #         ret = ret + "&nbsp"
    #     ret = ret + "<br>"
    # ret = ret + "</center>"
    # return HttpResponse(ret)
    f = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/bat.txt", "r")
    batteryLvl = f.readline() + "%"
    f.close()

    img = []

    ali = [[0]*9 for i in range(9)]
    fi = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/image.txt", "r")
    val = fi.readline()
    fi.close()
    val1 = val.split(";")
    print (val1)
    for i in range(len(val1)):
        value = val1[i].split(",")
        # img.append(value)
        for j in range(len(value)):
            ali[i][j] = int(value[j])
    print (ali)

    directionFile = []
    direction = []
    direc = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/direction.txt", "r")
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

    w = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/wifi.txt", "r")
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

def distance(request):
    f = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/bat.txt", "r")
    batteryLvl = f.readline() + "%"
    f.close()

    img = []

    ali = [[0]*9 for i in range(9)]
    fi = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/image.txt", "r")
    val = fi.readline()
    fi.close()
    val1 = val.split(";")
    for i in range(len(val1)):
        value = val1[i].split(",")
        # img.append(value)
        for j in range(len(value)):
            ali[i][j] = int(value[j])
    

    w = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/wifi.txt", "r")
    wifi = w.readline()
    w.close()
    
    if request.method == 'POST':
        distance = request.POST["dist"]
        angle = request.POST["angle"]
        f = open ("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/distance.txt", "w")
        f.write(distance+ "\n" + angle)
        f.close()

        direc = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/direction.txt", "a")
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
    direc = open("/Users/charmainelouie/Documents/Imperial/Year 2/Summer Project/Unity/direction.txt", "r")
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