#!/usr/bin/env bash

python3 manage.py runserver $1:8001 &
python3 arduino_tcp.py &
python3 edit_files.py &
python3 manage.py nests





