#!/usr/bin/env bash

python3 manage.py runserver $1:8000 &
python3 manage.py nests &
python3 arduino_tcp.py &
python3 edit_files.py



