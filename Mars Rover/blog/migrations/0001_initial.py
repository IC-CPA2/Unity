# Generated by Django 4.0.4 on 2022-06-25 10:27

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='map_info',
            fields=[
                ('map_name', models.CharField(max_length=100)),
                ('map_id', models.IntegerField(primary_key=True, serialize=False)),
                ('map_size', models.CharField(max_length=50)),
                ('date_time', models.DateTimeField(default=django.utils.timezone.now)),
            ],
        ),
    ]
