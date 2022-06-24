from django.urls import include, path
from django.contrib.staticfiles.urls import staticfiles_urlpatterns
from . import views

urlpatterns = [
    path('', views.home, name='blog-home'),
    path('about/', views.about, name='blog-about'),
    path('login/',views.login, name='blog-about'),
    path('form/',views.form, name='blog-about'),
    path('about/distance', views.distance, name='blog-about'),
]

urlpatterns += staticfiles_urlpatterns()