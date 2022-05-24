from django.db import models
from django.forms import ModelForm

# Create your models here.
class Address(models.Model):
    country = models.CharField(null=True, blank=True, max_length=100)
    state = models.CharField(null=True, blank=True, max_length=100)
    def __str__(self):
        return '{} {}'.format(self.country, self.state)