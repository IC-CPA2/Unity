# Generated by Django 4.0.4 on 2022-05-31 10:41

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('members', '0002_all_info'),
    ]

    operations = [
        migrations.AlterField(
            model_name='map_info',
            name='map_id',
            field=models.AutoField(primary_key=True, serialize=False),
        ),
    ]
