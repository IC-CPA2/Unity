from django.urls import include, path
from . import views

urlpatterns = [
    path('', views.home, name='blog-home'),
    path('about/', views.about, name='blog-about'),
    path('login/',views.login, name='blog-about'),
    path('form/',views.form, name='blog-about')
]