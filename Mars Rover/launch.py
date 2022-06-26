import subprocess

subprocess.run("python3 manage.py runserver 146.169.160.85:8000 & python3 arduino_tcp.py & python3 edit_files.py & python3 manage.py nests", shell=True)

# # ip = input("Enter IP Address:")
# import os
# os.system("python3 manage.py runserver && arduino_tcp.py && putho")