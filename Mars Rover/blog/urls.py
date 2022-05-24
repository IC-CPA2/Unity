from django.urls import include, path
from . import views

urlpatterns = [
    path('', views.home, name='blog-home'),
    path('home/', views.about, name='blog-about'),
    path('login/',views.login, name='blog-about'),
    path('form/',views.form, name='blog-about'),
    path('', views.base, name='blog-home')

]