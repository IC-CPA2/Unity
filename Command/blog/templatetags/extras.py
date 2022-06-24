from django import template

register = template.Library()

@register.filter
def index(indexable, i):
    inti = int(i)
    return indexable[inti-1]