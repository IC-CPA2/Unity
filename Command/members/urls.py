from django.urls import path
from . import views
from django.contrib.staticfiles.urls import staticfiles_urlpatterns

app_name = "members"

urlpatterns = [
    path('',views.members_list_,name="members_data"),
    path('search', views.query, name="search")
]

urlpatterns += staticfiles_urlpatterns()