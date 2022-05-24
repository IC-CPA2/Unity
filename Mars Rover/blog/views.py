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

grid = [0 for i in range(0,9)]
for i in range(0,9):
    grid[i] = i

# print (grid)

def base(request):
    return redirect('/home')

def home(request):
    context = {
        'posts': posts
    }
    return render(request, 'blog/home.html', context)

def about(request):
    default = """<canvas id="myCanvas" width="50" height="50" style="border:1px solid #000000;"></canvas>"""
    ret = "<center>"
    for i in range(no):
        for j in range(no):
            ret = ret + default
            ret = ret + "&nbsp"
        ret = ret + "<br>"
    ret = ret + "</center>"
    return HttpResponse(ret)
    # return render(request, 'blog/about.html')

def login(request):
    password = request.POST['last']
    global no
    no = int(password)
    return redirect('/about')

def form(request):

    return redirect('/about')