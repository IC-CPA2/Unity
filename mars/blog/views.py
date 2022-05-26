from django.shortcuts import render
from django.shortcuts import redirect
from django.shortcuts import HttpResponse
from django.template import loader
from django.views.generic import ListView, CreateView, UpdateView
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
    f = open("bat.txt", "r")
    batteryLvl = f.readline() + "%"
    f.close()

    img = []

    ali = [[0]*9 for i in range(9)]
    fi = open("image.txt", "r")
    val = fi.readline()
    val1 = val.split(";")
    for i in range(len(val1)):
        value = val1[i].split(",")
        # img.append(value)
        for j in range(len(value)):
            ali[i][j] = int(value[j])

    context = {
        'counter': ['1', '2', '3', '4', '5', '6', '7', '8', '9'], 
        'battery': batteryLvl,
        'aliens': ali
    }
    return render(request, 'blog/about.html', context)

def login(request):
    password = request.POST['last']
    if hashlib.sha256(password.encode('utf-8')).hexdigest() == 'ef797c8118f02dfb649607dd5d3f8c7623048c9c063d532cc95c5ed7a898a64f':
        return redirect('/about')
    else:
        return redirect('/')

def form(request):

    return redirect('/about')

