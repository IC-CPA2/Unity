import subprocess

ip = input("Enter IP Address: ")

subprocess.run("python3 manage.py runserver "+ ip +":8000 & python3 arduino_tcp.py & python3 edit_files.py & python3 manage.py nests", shell=True)

# # ip = input("Enter IP Address:")
# import os
# os.system("python3 manage.py runserver && arduino_tcp.py && putho")