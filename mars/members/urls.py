from django.urls import path
from . import views

app_name = "members"

urlpatterns = [
    path('',views.members_list_,name="members_data"),
]