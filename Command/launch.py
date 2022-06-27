import subprocess

ip = input("Enter IP Address: ")

subprocess.run("python3 manage.py runserver "+ ip +":8000 & python3 arduino_tcp.py & python3 edit_files.py & python3 manage.py nests", shell=True)
# subprocess.run("python3 '.\Mars Rover\manage.py' runserver  "+ ip +":8000 & python3 '.\Mars Rover\arduino_tcp.py' & python3 '.\Mars Rover\edit_files.py' & python3 '.\Mars Rover\manage.py' nests", shell=True)
#python3 '.\Mars Rover\self_test.py'
# # ip = input("Enter IP Address:")
# import os
# os.system("python3 manage.py runserver && arduino_tcp.py && putho")