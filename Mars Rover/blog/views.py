from django.shortcuts import render
from django.shortcuts import redirect
import hashlib

posts = [
    {
        'author': 'CoreyMS',
        'title': 'Blog Post 1',
        'content': 'First post content',
        'date_posted': 'August 27, 2018'
    },
    {
        'author': 'Jane Doe',
        'title': 'Blog Post 2',
        'content': 'Second post content',
        'date_posted': 'August 28, 2018'
    }
]
def base(request):
    return redirect('/home')

def home(request):
    context = {
        'posts': posts
    }
    return render(request, 'blog/home.html', context)


def about(request):
    default = """<canvas id="myCanvas" width="50" height="50" style="border:1px solid #000000;"></canvas>"""
    return render(request, 'blog/about.html', {'title': 'About'})

def login(request):
    password = request.POST['last']
    if hashlib.sha256(password.encode('utf-8')).hexdigest() == 'ef797c8118f02dfb649607dd5d3f8c7623048c9c063d532cc95c5ed7a898a64f':
        return redirect('/about')
    else:
        return redirect('/')
