from django import forms
from blog.widgets import RangeInput

class NameForm(forms.Form):
    distance = forms.FloatField()
    angle = forms.IntegerField(widget=RangeInput)